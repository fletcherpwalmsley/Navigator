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
    NULL_PTR_CHECK(model != nullptr);

    // Build model
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    _interpreter = tflite::Interpreter interpreter;
    builder(&_interpreter);
    NULL_PTR_CHECK(_interpreter != nullptr);

    // Allocate tensor buffers.
    NULL_PTR_CHECK(_interpreter->AllocateTensors() == kTfLiteOk);

    _interpreter->SetAllowFp16PrecisionForFp32(true);

}

TFliteHandler::DoInference(cv::Mat input_mat)
{
}
