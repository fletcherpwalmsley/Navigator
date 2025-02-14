/**
 * @file cnn_running.cpp
 * @brief Different implementations for running neural networks
 * @date 2024-09-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "cnn_runner.hpp"

#include "helpers.hpp"
#include "tensorflow/lite/core/interpreter_builder.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/optional_debug_tools.h"

TFliteRunner::TFliteRunner(std::string filename) {
  // Load model
  m_model = tflite::FlatBufferModel::BuildFromFile(filename.c_str());
  NULL_CHECK(m_model != nullptr);

  // Build model
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*m_model, resolver);
  builder.SetNumThreads(4);
  builder(&m_interpreter);
  NULL_CHECK(m_interpreter != nullptr);

  // Allocate tensor buffers.
  NULL_CHECK(m_interpreter->AllocateTensors() == kTfLiteOk);

  m_interpreter->SetAllowFp16PrecisionForFp32(true);

  // Check tensor input shape and type
  if (m_interpreter->tensor(m_interpreter->inputs()[0])->type != kTfLiteFloat32) {
    std::cerr << "Error: Expected input tensor to be of type float32!" << std::endl;
    exit(1);
  }
}

float* TFliteRunner::DoInference(cv::Mat input_mat) {
  std::memcpy(m_interpreter->typed_input_tensor<float>(0), input_mat.ptr<float>(0),
              input_mat.total() * input_mat.elemSize());
  NULL_CHECK(m_interpreter != nullptr);
  NULL_CHECK(m_interpreter->Invoke() == kTfLiteOk);

  // Get the output tensor
  int output_tensor_index = m_interpreter->outputs()[0];
  TfLiteTensor* output_tensor = m_interpreter->tensor(output_tensor_index);
  // Check tensor output type
  if (output_tensor->type != kTfLiteFloat32) {
    std::cerr << "Error: Expected output tensor to be of type float32!" << std::endl;
    exit(1);
  }
  return output_tensor->data.f;
}

cv::Size TFliteRunner::GetInputSize() const {
  NULL_CHECK(m_interpreter);
  auto input = m_interpreter->inputs()[0];
  return cv::Size(m_interpreter->tensor(input)->dims->data[1], m_interpreter->tensor(input)->dims->data[2]);
}

int TFliteRunner::GetOutputHeight() const {
  NULL_CHECK(m_interpreter);
  TfLiteTensor* output_tensor = m_interpreter->tensor(m_interpreter->outputs()[0]);
  return output_tensor->dims->data[1];
}

int TFliteRunner::GetOutputWidth() const {
  NULL_CHECK(m_interpreter);
  TfLiteTensor* output_tensor = m_interpreter->tensor(m_interpreter->outputs()[0]);
  return output_tensor->dims->data[2];
}

int TFliteRunner::GetNumOutputClasses() const {
  NULL_CHECK(m_interpreter);
  TfLiteTensor* output_tensor = m_interpreter->tensor(m_interpreter->outputs()[0]);
  return output_tensor->dims->data[3];
}
