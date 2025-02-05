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
