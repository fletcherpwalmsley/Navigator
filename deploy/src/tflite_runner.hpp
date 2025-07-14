#pragma once

#include <opencv2/core/mat.hpp>

#include "cnn_runner.hpp"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model_builder.h"

typedef float float32_t;

class TFliteRunner : public CNNRunner {
 public:
  TFliteRunner(std::string model_path);
  std::unique_ptr<std::vector<float32_t>> DoInference(cv::Mat input_mat);
  cv::Size GetInputSize() const;
  int GetOutputHeight() const;
  int GetOutputWidth() const;
  int GetNumOutputClasses() const;

 private:
  std::unique_ptr<tflite::Interpreter> m_interpreter;
  std::unique_ptr<tflite::FlatBufferModel> m_model;
};