#include <atomic>  // For atomic flag
#include <chrono>
#include <csignal>  // For signal handling
#include <cstdio>
#include <cstdlib>  // For setenv
#include <ctime>
#include <filesystem>
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <utility>

#include "cnn_runner.hpp"
#include "helpers.hpp"
#include "libcamera_factory.hpp"
#include "mask_limit_finders.h"
#include "process_video.hpp"
#include "river_mask_generator.hpp"
#include "weighted_moving_average.hpp"

#ifdef USE_TFLITE
#include "tflite_runner.hpp"
#endif

#ifdef USE_HAILO
#include "hailo_runner.hpp"
#endif

// Global variables for signal handling
std::atomic<bool> running(true);
cv::VideoWriter* global_video_ptr = nullptr;
cv::VideoWriter* global_video_ptr2 = nullptr;

// Signal handler function
void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received. Cleaning up resources..." << std::endl;
  running = false;

  // Release video if it exists
  if (global_video_ptr != nullptr) {
    global_video_ptr->release();
    std::cout << "Video resources released." << std::endl;
  }

  // Release video if it exists
  if (global_video_ptr2 != nullptr) {
    global_video_ptr2->release();
    std::cout << "Video2 resources released." << std::endl;
  }

  // Exit after a short delay to allow cleanup
  std::exit(signum);
}

int main(int argc, char* argv[]) {
  // Register signal handlers
  std::signal(SIGINT, signalHandler);   // Handle Ctrl+C
  std::signal(SIGTERM, signalHandler);  // Handle termination request

  // Set the GST_DEBUG environment variable to a higher level (e.g., 3)
  // setenv("GST_DEBUG", "3", 1);

  std::filesystem::path model_path = "../model.hef";

  std::unique_ptr<VideoHandler> video_handler;

  if (argc == 2) {
    // If we are passed a file path, check that it is valid and use CV VideoCapture to open it
    std::filesystem::path input_video_path = argv[1];
    std::cout << input_video_path;
    if (!input_video_path.has_extension() &&
        (input_video_path.extension() == ".avi" || input_video_path.extension() == ".mp4")) {
      std::cerr << "Error: Video file must be an avi or mp4" << std::endl;
      return -1;
    }
    // If we didn't fail the above checks, we should be good to go!
    auto video_capture = std::make_unique<cv::VideoCapture>(input_video_path);
    video_handler = std::make_unique<VideoHandler>(std::move(video_capture));

    // Otherwise, use libcamera to open the default camera
  } else {
    // Set pi_cam
    int32_t height = 432;
    int32_t width = 768;
    float fps = 15;

    auto pi_cam_facade = std::make_unique<LibCameraFacade>();
    pi_cam_facade->getOptions()->video_width = width;
    pi_cam_facade->getOptions()->video_height = height;
    pi_cam_facade->getOptions()->framerate = fps;
    pi_cam_facade->getOptions()->verbose = true;
    video_handler = std::make_unique<VideoHandler>(std::move(pi_cam_facade));
  }

  std::string video_file_save = "/home/autoboat/autoboat_sailing_videos/";
  video_file_save += uuid::generate_uuid_v4();
  video_file_save += ".avi";

  std::cout << video_file_save;

  std::string mediamtx_rtsp =
      "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=2000 speed-preset=medium byte-stream=false "
      "key-int-max=20 bframes=0 aud=true ! rtspclientsink "
      "location=rtsp://localhost:8554/river";

  cv::VideoWriter video(mediamtx_rtsp, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                        video_handler->getFPS(),
                        cv::Size(cv::Size(video_handler->getFrameWidth(), video_handler->getFrameHeight())), true);

  cv::VideoWriter video2(video_file_save, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                         video_handler->getFPS(),
                         cv::Size(cv::Size(video_handler->getFrameWidth(), video_handler->getFrameHeight())), true);

  // Store video writer pointer in global variable for signal handler
  global_video_ptr = &video;
  global_video_ptr2 = &video2;

#ifdef USE_TFLITE
  std::shared_ptr<RiverMaskGenerator> m_generator;
  std::shared_ptr<TFliteRunner> runner;
  runner = std::make_unique<TFliteRunner>(model_path);
  m_generator = std::make_unique<RiverMaskGenerator>(runner);
  cv::Mat mask(cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), CV_8UC1);
#elif USE_HAILO
  std::shared_ptr<RiverMaskGenerator> m_generator;
  std::shared_ptr<HailoRunner> runner;
  runner = std::make_unique<HailoRunner>(model_path);
  m_generator = std::make_unique<RiverMaskGenerator>(runner);
  // Dynamically doing this gets a segfault. Likely need to pass a value through the network first
  cv::Mat mask(cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), CV_8UC1);
#else
  cv::Mat mask(cv::Size(64, 64), CV_8UC1);
#endif

  cv::Mat colourCorrectFrame;
  cv::Mat scaledMask(cv::Size(video_handler->getFrameWidth(), video_handler->getFrameHeight()), CV_8UC1);

  cv::Mat outFrame;
  std::vector<cv::Mat> inputFrameChannels(3);
  WeightedMovingAverage wma(0.35);

  std::cout << "Starting video loop\n";
  while (running) {
    auto inFrame = video_handler->getNextFrame();
    video2.write(inFrame.second);
    cv::cvtColor(inFrame.second, colourCorrectFrame, cv::COLOR_BGR2RGB);
#if defined(USE_TFLITE) || defined(USE_HAILO)
    auto output_data = m_generator->GenerateMask(colourCorrectFrame);
    // Apply Closing morphology to the mask
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(output_data, output_data, cv::MORPH_CLOSE, kernel);

    mask = wma.apply(output_data);
#endif
    cv::resize(mask, scaledMask, scaledMask.size(), 0, 0, cv::INTER_LINEAR);
    cv::split(inFrame.second, inputFrameChannels);
    inputFrameChannels.at(2) += scaledMask;
    cv::merge(inputFrameChannels, outFrame);

    cv::circle(outFrame, findHighestPoint(scaledMask), 5, cv::Scalar(0, 255, 255), -1);
    video.write(outFrame);
  }

  global_video_ptr = nullptr;   // Reset the pointer after releasing
  global_video_ptr2 = nullptr;  // Reset the pointer after releasing

  return 0;
}
