/**
 * @file tflite_handler.h
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


class TFliteHandler {
    public:
    TFliteHandler(std::string std::move(model_path));
    DoInference(cv::Mat input_mat);

    private:
    std::unique_ptr<tflite::Interpreter> _interpreter;
 }
