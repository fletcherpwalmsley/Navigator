/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdio>
#include <iostream>

#include "tensorflow/lite/core/interpreter_builder.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

// This is an example that is minimal to read a model
// from disk and perform inference. There is no data being loaded
// that is up to you to add as a user.
//
// NOTE: Do not add any dependencies to this that cannot be built with
// the minimal makefile. This example must remain trivial to build with
// the minimal build tool.
//

// https://blog.conan.io/2023/05/11/tensorflow-lite-cpp-mobile-ml-guide.html
// Usage: minimal <tflite model>

#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }


// Helper function to perform argmax on the output tensor
cv::Mat ApplyArgmax(const float* output_data, int height, int width, int num_classes) {
    // Create a cv::Mat for the mask with single channel
    cv::Mat mask(height, width, CV_8UC1);

    // Iterate through each pixel and apply argmax
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * num_classes;
            // Find the class with the highest probability
            int class_index = std::max_element(output_data + index, output_data + index + num_classes) - (output_data + index);
            // std::cout << class_index << std::endl;
            mask.at<uchar>(y, x) = static_cast<uchar>(class_index * 255);
        }
    }
    return mask;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "minimal <tflite model>\n");
    return 1;
  }
  const char* image_filename = argv[1];
  const char* filename = "../model.tflite";

  // Load model
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile(filename);
  TFLITE_MINIMAL_CHECK(model != nullptr);


  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*model, resolver);
  std::unique_ptr<tflite::Interpreter> interpreter;
  builder(&interpreter);
  TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  // tflite::PrintInterpreterState(interpreter.get());

  // Allocate tensor buffers.
  TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);

  interpreter->SetAllowFp16PrecisionForFp32(true);

  // std::string imagePath = "../image.png";
  cv::Mat import_image = cv::imread(image_filename, cv::IMREAD_COLOR);
  if (import_image.empty()) {
    std::cerr << "Error: Could not open or find the image at " << image_filename << std::endl;
    return -1;
  }

  cv::Mat image;
  cv::cvtColor(import_image, image, cv::COLOR_BGR2RGB);

  cv::Mat resizedImage;
  auto input = interpreter->inputs()[0];
  cv::Size newSize(
    interpreter->tensor(input)->dims->data[1],
    interpreter->tensor(input)->dims->data[2]);
  cv::resize(image, resizedImage, newSize, cv::INTER_NEAREST);
  cv::imwrite("output/resize.jpeg", resizedImage);
  resizedImage.convertTo(resizedImage, CV_32FC2, 1.0 / 255.0);


  // Check tensor shape and type
  if (interpreter->tensor(interpreter->inputs()[0])->type != kTfLiteFloat32) {
      std::cerr << "Error: Expected input tensor to be of type float32!" << std::endl;
      return -1;
  }

  // std::memcpy(interpreter->typed_input_tensor<float>(0), resizedImage.ptr<float>(0), resizedImage.total() * resizedImage.elemSize());
  std::memcpy(interpreter->typed_input_tensor<float>(0), resizedImage.ptr<float>(0), resizedImage.total() * resizedImage.elemSize());

  // Run inference
  TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

  // Read output buffers
  // TODO(user): Insert getting data out code.
  // Note: The buffer of the output tensor with index `i` of type T can
  // be accessed with `T* output = interpreter->typed_output_tensor<T>(i);`

  // Get the output tensor
  int output_tensor_index = interpreter->outputs()[0];
  TfLiteTensor* output_tensor = interpreter->tensor(output_tensor_index);

  // Check tensor type
  if (output_tensor->type != kTfLiteFloat32) {
      std::cerr << "Error: Expected output tensor to be of type float32!" << std::endl;
      return -1;
  }

  // Get dimensions
  const int height = output_tensor->dims->data[1];
  const int width = output_tensor->dims->data[2];
  const int num_classes = output_tensor->dims->data[3];

  // Access the output data
  float* output_data = output_tensor->data.f;

  // Apply argmax to get the mask
  cv::Mat mask = ApplyArgmax(output_data, height, width, num_classes);

  // Save the mask
  cv::imwrite("output/mask.jpeg", mask);
  return 0;
}