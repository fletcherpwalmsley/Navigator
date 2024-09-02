#include "tflite_handler.h"

#include "tensorflow/lite/model_builder.h"
#include "helpers.h"
/**
 * @file tflite_handler.cpp
 * @brief tflite_hander implementation
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

TFliteHandler::TFliteHandler(std::string move)
{
    // Load model
    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(filename);
    NULL_CHECK(model != nullptr);

    // Build model
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    _interpreter = tflite::Interpreter interpreter;
    builder(&_interpreter);
    NULL_CHECK(_interpreter != nullptr);

    // Allocate tensor buffers.
    NULL_CHECK(_interpreter->AllocateTensors() == kTfLiteOk);

    _interpreter->SetAllowFp16PrecisionForFp32(true);

    // Check tensor input shape and type
    if (interpreter->tensor(interpreter->inputs()[0])->type != kTfLiteFloat32) {
        std::cerr << "Error: Expected input tensor to be of type float32!" << std::endl;
        exit(1);
  }
}

TFliteHandler::DoInference(cv::Mat& input_mat)
{
    std::memcpy(interpreter->typed_input_tensor<float>(0), resizedImage.ptr<float>(0), resizedImage.total() * resizedImage.elemSize());
    NULL_CHECK(interpreter->Invoke() == kTfLiteOk);

    // Get the output tensor
    int output_tensor_index = interpreter->outputs()[0];
    std::unique_ptr<TfLiteTensor> output_tensor = interpreter->tensor(output_tensor_index);
    // Check tensor output type
    if (output_tensor->type != kTfLiteFloat32) {
        std::cerr << "Error: Expected output tensor to be of type float32!" << std::endl;
        exit(1);
  }
}
