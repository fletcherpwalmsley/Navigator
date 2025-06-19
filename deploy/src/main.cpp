#include <atomic>   // For atomic flag
#include <csignal>  // For signal handling
#include <cstdio>
#include <cstdlib>  // For setenv
#include <filesystem>
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <utility>
#include <chrono>
#include <ctime>

// As of now, OpenCV doesn't support libcamera.
// So use the LCCV library to bridge this gap.
// The .so needs to be manually built and installed
// See https://github.com/kbarni/LCCV
#include <lccv.hpp>

#include "cnn_runner.hpp"
#include "helpers.hpp"
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

const std::vector<std::string> videoTypes{".mp4", ".avi"};
const std::vector<std::string> imageTypes{".png", ".jpg", ".jpeg"};

int main(int argc, char* argv[]) {
  // Register signal handlers
  std::signal(SIGINT, signalHandler);   // Handle Ctrl+C
  std::signal(SIGTERM, signalHandler);  // Handle termination request

  // if (argc != 3) {
  //   std::cerr << "Need video and model to process: navigator <video_path> <model_path>" << std::endl;
  //   return 1;
  // }
  // Set the GST_DEBUG environment variable to a higher level (e.g., 3)
  // setenv("GST_DEBUG", "3", 1);
  // std::filesystem::path file_path = argv[1];
  std::filesystem::path file_path = "../input_video.mp4";
  // std::filesystem::path model_path = argv[2];
  std::filesystem::path model_path = "../model.hef";

  if (!file_path.has_extension()) {
    std::cerr << "Error: File path must have an extension" << std::endl;
    return -1;
  } else if (std::find(videoTypes.begin(), videoTypes.end(), file_path.extension()) == videoTypes.end()) {
    std::cout << "File is of type " << file_path.extension() << " processing as a video \n";
  } else if (std::find(imageTypes.begin(), imageTypes.end(), file_path.extension()) == imageTypes.end()) {
    std::cout << "File is of type " << file_path.extension() << " processing as an image \n";
  } else {
    std::cerr << "Error: File type not supported" << std::endl;
    return -1;
  }

  // std::string image_filename = "image.png";
  // cv::Mat import_image = cv::imread(image_filename, cv::IMREAD_COLOR);
  // if (import_image.empty()) {
  //   std::cerr << "Error: Could not open or find the image at " << image_filename << std::endl;
  //   return -1;
  // }

  // Default resolutions of the frame are obtained.The default resolutions are system dependent.

  // Setup mask network

  // Set pi_cam
  int32_t height = 432;
  int32_t width = 768;
  int32_t fps = 15;
  cv::Mat inFrame(cv::Size(width, height), CV_8UC3);
  cv::Mat scaledMask(cv::Size(width, height), CV_8UC1);
  lccv::PiCamera pi_cam;
  pi_cam.options->video_width=width;
  pi_cam.options->video_height=height;
  pi_cam.options->framerate=fps;
  pi_cam.options->verbose=true;

  pi_cam.startVideo();

  // std::string video_file_save = "mask_video.avi";
  std::string video_file_save = "/home/autoboat/autoboat_sailing_videos/";
  video_file_save += uuid::generate_uuid_v4();
  video_file_save += ".avi";

  std::cout << video_file_save;

  std::string mediamtx_rtsp =
      "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=2000 speed-preset=medium byte-stream=false "
      "key-int-max=20 bframes=0 aud=true ! rtspclientsink "
      "location=rtsp://localhost:8554/river";

  // VideoHandler handler(file_path);
    // cv::VideoWriter video(video_file_save, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 30,
    //                       cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), true);
  cv::VideoWriter video2(video_file_save, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps,
                      cv::Size(cv::Size(width, height)), true);
    cv::VideoWriter video(mediamtx_rtsp, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps,
                            cv::Size(cv::Size(width, height)), true);

  // Store video writer pointer in global variable for signal handler
  global_video_ptr = &video;
  global_video_ptr2 = &video2;

  // handler.setFrameRate(30);
  int numProcessedFrames = 0;

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
// cv::Mat mask(cv::Size(64, 64), CV_8UC1);
#else
  cv::Mat mask(cv::Size(64, 64), CV_8UC1);
#endif

  cv::Mat colourCorrectFrame;
  // cv::Mat scaledMask(cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), CV_8UC1);
  //cv::Mat inFrame(cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), CV_8UC3);
  cv::Mat outFrame;
  std::vector<cv::Mat> inputFrameChannels(3);
  WeightedMovingAverage wma(0.35);

  // while (handler.isDataWaiting() && running) {
  std::cout << "Starting video video\n";
  while (running) {
    // std::cout << "Processing frame number " << numProcessedFrames++ << "\n";
    // inFrame = handler.getCurrentFrame();
    pi_cam.getVideoFrame(inFrame,10000);
    video2.write(inFrame);
    cv::cvtColor(inFrame, colourCorrectFrame, cv::COLOR_BGR2RGB);
#if defined(USE_TFLITE) || defined(USE_HAILO)
    auto output_data = m_generator->GenerateMask(colourCorrectFrame);
    // std::cout << output_data << std::endl;
    mask = wma.apply(output_data);
#endif
    cv::resize(mask, scaledMask, scaledMask.size(), 0, 0, cv::INTER_LINEAR);
    cv::split(inFrame, inputFrameChannels);
    inputFrameChannels.at(2) += scaledMask;
    cv::merge(inputFrameChannels, outFrame);

    // std::cerr << "inFrame type: " << getMatType(inFrame.type()) << std::endl;
    // std::cerr << "outFrame type: " << getMatType(outFrame.type()) << std::endl;
    cv::circle(outFrame, findHighestPoint(scaledMask), 5, cv::Scalar(0, 255, 255), -1);
    video.write(outFrame);
  }

  // When everything done, release the video capture and write object
  // video.release();
  pi_cam.stopVideo();
  global_video_ptr = nullptr;  // Reset the pointer after releasing
  global_video_ptr2 = nullptr;  // Reset the pointer after releasing

  // Save the mask
  // cv::imwrite("mask.jpeg", mask);
  return 0;
}
