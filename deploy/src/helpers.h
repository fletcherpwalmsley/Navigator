/**
 * @file helpers.h
 * @author Fletcher Walmsley
 * @brief A shared collection of helper functions and macros
 * @date 2024-09-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <opencv2/core/mat.hpp>
/**
 * @brief null ptr check macro taken from 
 * tensorflow/lite/examples/minimal/minimal.cc
 * 
 */
#define NULL_PTR_CHECK(x)                                    \
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
            mask.at<uchar>(y, x) = static_cast<uchar>(class_index * 255);
        }
    }
    return mask;
}