/**
 * @file river_mask_generator.h
 * @author Fletcher Walmsley
 * @brief Creates a mask outline the river from a given image
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <memory>
#include <utility>
#include <opencv2/core/mat.hpp>

#include "helpers.h"
#include "cnn_runner.h"

class RiverMaskGenerator {
private:
    std::shared_ptr<CNNRunner> m_runner;
public:
    RiverMaskGenerator(std::shared_ptr<CNNRunner> runner)
    : m_runner(runner) {
        NULL_CHECK(m_runner);
    }
    cv::Mat GenerateMask(cv::Mat input_mat);

private:
    int VerifyRunner();
};