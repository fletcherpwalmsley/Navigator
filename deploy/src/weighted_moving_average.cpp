#include "weighted_moving_average.hpp"

#include "iostream"
#include "opencv2/imgproc/imgproc.hpp"

auto WeightedMovingAverage::apply(const cv::Mat& newFrame) -> cv::Mat {
  if (!initialized) {
    // Initialize the moving average with the first frame
    newFrame.convertTo(movingAverage, CV_8U);
    initialized = true;
  } else {
    // Update the moving average
    cv::Mat newFrameFloat;
    newFrame.convertTo(newFrameFloat, CV_8U);
    movingAverage = alpha * newFrameFloat + (1.0 - alpha) * movingAverage;
  }

  // Apply high-pass filter
  double beta = 220;
  cv::Mat threshold_result;
  // std::cout << "movingAverage Value " << movingAverage << std::endl;
  cv::threshold(movingAverage, threshold_result, beta, 255, cv::THRESH_TOZERO);

  // Convert the moving average back to the original type
  cv::Mat output_result;
  threshold_result.convertTo(output_result, newFrame.type());

  return output_result;
}