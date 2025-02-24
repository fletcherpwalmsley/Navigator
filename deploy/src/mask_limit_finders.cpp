#include "mask_limit_finders.h"

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

auto findHighestPoint(cv::Mat mask) -> cv::Point {
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  if (contours.empty()) {
    return cv::Point(-1, -1);  // Return an invalid point if no contours are found
  }

  cv::Point leftmost = contours[0][0];
  cv::Point rightmost = contours[0][0];
  cv::Point topmost = contours[0][0];
  cv::Point bottommost = contours[0][0];

  for (const auto& contour : contours) {
    for (const auto& point : contour) {
      if (point.x < leftmost.x) {
        leftmost = point;
      }
      if (point.x > rightmost.x) {
        rightmost = point;
      }
      if (point.y < topmost.y) {
        topmost = point;
      }
      if (point.y > bottommost.y) {
        bottommost = point;
      }
    }
  }

  // For demonstration purposes, let's print the points
  //   std::cout << "Leftmost: " << leftmost << std::endl;
  //   std::cout << "Rightmost: " << rightmost << std::endl;
  //   std::cout << "Topmost: " << topmost << std::endl;
  //   std::cout << "Bottommost: " << bottommost << std::endl;

  return topmost;  // Return the topmost point as an example
}