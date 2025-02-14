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
#include <opencv2/opencv.hpp>
#include <string>
/**
 * @brief null check macro taken from
 * tensorflow/lite/examples/minimal/minimal.cc
 *
 */
#define NULL_CHECK(x)                                        \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

const inline std::string getMatType(int type) {
  std::string r, a;
  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
    case CV_8U:
      r = "8U";
      break;
    case CV_8S:
      r = "8S";
      break;
    case CV_16U:
      r = "16U";
      break;
    case CV_16S:
      r = "16S";
      break;
    case CV_32S:
      r = "32S";
      break;
    case CV_32F:
      r = "32F";
      break;
    case CV_64F:
      r = "64F";
      break;
    default:
      r = "User";
      break;
  }

  a = "C";
  a += (chans + '0');

  return r + a;
}