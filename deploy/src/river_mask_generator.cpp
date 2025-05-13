/**
 * @file river_mask_generator.cpp
 * @brief Creates a mask outline the river from a given image
 * @date 2024-09-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "river_mask_generator.hpp"

#include <memory>
#include <opencv2/imgproc.hpp>

// Helper function to perform argmax on the output tensor (TFLite). Give in the form of a float pointer
cv::Mat ApplyArgmax(const float* output_data, const int height, const int width, const int num_classes) {
  // Create a cv::Mat for the mask with single channel
  cv::Mat mask(height, width, CV_8UC1);

  // Iterate through each pixel and apply argmax
  //  std::cout << "New mask" << std::endl;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int index = (y * width + x) * num_classes;
      //      std::cout << "x: " << x << " y: " << y << " - " << *(output_data + index) << std::endl;
      // Find the class with the highest probability
      volatile int class_index =
          std::max_element(output_data + index, output_data + index + num_classes) - (output_data + index);
      std::cout << class_index;
      mask.at<uchar>(y, x) = static_cast<uchar>(class_index * 255);
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  return mask;
}

cv::Mat ApplyArgmax(std::unique_ptr<std::vector<float>> logits, int height, int width, int num_classes) {
  cv::Mat output(height, width, CV_8UC1, cv::Scalar(0));
  cv::Mat input(height, width, CV_32FC3, logits->data());

  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      int index = (r * width + c) * num_classes;
      float* pixel = input.ptr<float>(0, 0);
      // std::cout << "R: " << r << " C: " << c << " Value[0]: " << int(*pixel) <<" Value[1]: " << int(*(pixel + 1)) <<
      // "\n";
      int class_index = std::max_element(pixel + index, pixel + index + num_classes) - (pixel + index);
      // std::cout << class_index;
      output.at<uchar>(r, c) = static_cast<uchar>(class_index * 255);
      // if (int(*pixel) >= int(*(pixel + 1))) {
      //     output.at<cv::Vec3f>(r,c) = cv::Vec3f(220., 20.,  60. ); // red
      // } else {
      //     output.at<cv::Vec3f>(r,c) = cv::Vec3f(0.,   0.,   230.); // green
      // }
    }
  }
  return output;
}

cv::Mat RiverMaskGenerator::GenerateMask(cv::Mat input_mat) {
  cv::Mat resizedImage;
  cv::resize(input_mat, resizedImage, m_runner->GetInputSize(), cv::INTER_NEAREST);
  resizedImage.convertTo(resizedImage, CV_32FC3, 1.0 / 255.0);
  auto output_data = m_runner->DoInference(resizedImage);
  return ApplyArgmax(std::move(output_data), m_runner->GetOutputHeight(), m_runner->GetOutputWidth(),
                     m_runner->GetNumOutputClasses());
}
