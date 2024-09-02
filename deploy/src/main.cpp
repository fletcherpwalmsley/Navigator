#include <cstdio>
#include <iostream>
#include <memory>
#include <utility>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "cnn_runner.h"
#include "river_mask_generator.h"


int main(int argc, char* argv[]) {
  // if (argc != 2) {
  //   fprintf(stderr, "minimal <tflite model>\n");
  //   return 1;
  // }
  // const char* image_filename = argv[1];

  std::string image_filename = "image.png";
  cv::Mat import_image = cv::imread(image_filename, cv::IMREAD_COLOR);
  if (import_image.empty()) {
    std::cerr << "Error: Could not open or find the image at " << image_filename << std::endl;
    return -1;
  }

  cv::Mat image;
  cv::Mat mask;
  cv::cvtColor(import_image, image, cv::COLOR_BGR2RGB);

  std::shared_ptr<CNNRunner> runner = std::make_unique<TFliteRunner>("/workspaces/Navigator/training/output/model.tflite");
  RiverMaskGenerator river_mask_generator(runner);

  mask = river_mask_generator.GenerateMask(image);
  // Save the mask
  cv::imwrite("mask.jpeg", mask);
  return 0;
}