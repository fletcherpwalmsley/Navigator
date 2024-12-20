#include <cstdio>
#include <iostream>
#include <memory>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <utility>

#include "cnn_runner.h"
#include "river_mask_generator.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "navigator video_path\n");
    return 1;
  }
  const char* video_filename = argv[1];

  // std::string image_filename = "image.png";
  // cv::Mat import_image = cv::imread(image_filename, cv::IMREAD_COLOR);
  // if (import_image.empty()) {
  //   std::cerr << "Error: Could not open or find the image at " << image_filename << std::endl;
  //   return -1;
  // }

  // std::cout << cv::getBuildInformation();
  cv::VideoCapture cap(video_filename);
  if (!cap.isOpened()) {
    std::cout << "Error opening video file: " << video_filename << std::endl;
    return -1;
  }

  // Default resolutions of the frame are obtained.The default resolutions are system dependent.
  int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  int number_of_frames = cap.get(cv::CAP_PROP_FRAME_COUNT);
  int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS)) / 10;

  // Setup mask network
  std::shared_ptr<CNNRunner> runner = std::make_unique<TFliteRunner>("../../../training/output/model.tflite");
  RiverMaskGenerator river_mask_generator(runner);

  cv::VideoWriter video("mask_video.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30,
                        cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), false);

  int frame_num = 0;
  int next_frame_num = frame_num + fps;
  while (1) {
    cv::Mat frame;
    cv::Mat mask;
    cv::Mat colour_correct_image;
    while (frame_num++ < next_frame_num) {
      cap >> frame;
    }
    next_frame_num = frame_num + fps;
    if (frame.empty()) {
      break;
    }
    cv::cvtColor(frame, colour_correct_image, cv::COLOR_BGR2RGB);
    mask = river_mask_generator.GenerateMask(colour_correct_image);

    cv::Mat resizedFrame;
    cv::Mat grayScaleFrame;
    cv::Mat outFrame;
    cv::resize(frame, resizedFrame, mask.size(), cv::INTER_NEAREST);
    cv::cvtColor(resizedFrame, grayScaleFrame, cv::COLOR_BGR2GRAY);
    cv::addWeighted(grayScaleFrame, 0.6, mask, 0.4, 0, outFrame);

    video.write(outFrame);
    std::cout << "Processed frame number " << frame_num << " of " << number_of_frames << std::endl;
  }

  // When everything done, release the video capture and write object
  cap.release();
  video.release();

  // Save the mask
  // cv::imwrite("mask.jpeg", mask);
  return 0;
}