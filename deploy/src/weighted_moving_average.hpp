#ifndef WEIGHTED_MOVING_AVERAGE_HPP
#define WEIGHTED_MOVING_AVERAGE_HPP
#include <opencv2/core.hpp>

class WeightedMovingAverage {
 public:
  WeightedMovingAverage(double alpha) : alpha(alpha), initialized(false) {}
  auto apply(const cv::Mat& newFrame) -> cv::Mat;

 private:
  double alpha;  // Weight for the new frame
  bool initialized = false;
  cv::Mat movingAverage;
};

#endif  // WEIGHTED_MOVING_AVERAGE_HPP