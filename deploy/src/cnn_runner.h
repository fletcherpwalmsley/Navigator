/**
 * @file cnn_runner.h
 * @author Fletcher Walmsley
 * @brief Handles the creation and life cycle of tflite objects
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <iostream>
#include <opencv2/core/mat.hpp>

#include "tensorflow/lite/interpreter.h"


class CNNRunner {
public:
    virtual float* DoInference(cv::Mat& input_mat) = 0;
    // For when you need to know what size your cv:Mat must be
    virtual cv::Size GetInputSize() const = 0;
    // For when you need to know what you're getting out
    virtual int GetOutputHeight() const = 0;
    virtual int GetOutputWidth() const = 0;
    virtual int GetNumOutputClasses() const = 0;
};

class TFliteRunner : public CNNRunner {
public:
    TFliteRunner(std::string model_path);
    float* DoInference(cv::Mat& input_mat);
    cv::Size GetInputSize() const;
    int GetOutputHeight() const;
    int GetOutputWidth() const;
    int GetNumOutputClasses() const;

private:
    std::unique_ptr<tflite::Interpreter> m_interpreter;
 };