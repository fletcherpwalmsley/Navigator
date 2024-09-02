#include <cstdio>
#include <iostream>

#include "tensorflow/lite/core/interpreter_builder.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model_builder.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>



int main(int argc, char* argv[]) {
  // if (argc != 2) {
  //   fprintf(stderr, "minimal <tflite model>\n");
  //   return 1;
  // }
  const char* image_filename = argv[1];
  const char* filename = "model.tflite";

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