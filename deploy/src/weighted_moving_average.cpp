#include "weighted_moving_average.hpp"

auto WeightedMovingAverage::apply(const cv::Mat& newFrame) -> cv::Mat {
  if (!initialized) {
    // Initialize the moving average with the first frame
    newFrame.convertTo(movingAverage, CV_32F);
    initialized = true;
  } else {
    // Update the moving average
    cv::Mat newFrameFloat;
    newFrame.convertTo(newFrameFloat, CV_32F);
    movingAverage = alpha * newFrameFloat + (1.0 - alpha) * movingAverage;
  }

  // Convert the moving average back to the original type
  cv::Mat result;
  movingAverage.convertTo(result, newFrame.type());
  return result;
}