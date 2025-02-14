/**
 * @file river_mask_generator.cpp
 * @brief Creates a mask outline the river from a given image
 * @date 2024-09-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "river_mask_generator.hpp"

#include <opencv2/imgproc.hpp>

// Helper function to perform argmax on the output tensor
cv::Mat ApplyArgmax(const float* output_data, const int height, const int width, const int num_classes) {
  // Create a cv::Mat for the mask with single channel
  cv::Mat mask(height, width, CV_8UC1);

  // Iterate through each pixel and apply argmax
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int index = (y * width + x) * num_classes;
      // Find the class with the highest probability
      int class_index =
          std::max_element(output_data + index, output_data + index + num_classes) - (output_data + index);
      mask.at<uchar>(y, x) = static_cast<uchar>(class_index * 255);
    }
  }
  return mask;
}

cv::Mat RiverMaskGenerator::GenerateMask(cv::Mat input_mat) {
  cv::Mat resizedImage;
  cv::resize(input_mat, resizedImage, m_runner->GetInputSize(), cv::INTER_NEAREST);
  resizedImage.convertTo(resizedImage, CV_32FC2, 1.0 / 255.0);
  float* output_data = m_runner->DoInference(resizedImage);
  return ApplyArgmax(output_data, m_runner->GetOutputHeight(), m_runner->GetOutputWidth(),
                     m_runner->GetNumOutputClasses());
}
