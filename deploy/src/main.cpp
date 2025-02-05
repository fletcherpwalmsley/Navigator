#include <cstdio>
#include <cstdlib>  // For setenv
#include <filesystem>
#include <iostream>
#include <memory>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <utility>

#include "cnn_runner.h"
#include "process_video.h"
#include "river_mask_generator.h"

const std::vector<std::string> videoTypes{".mp4", ".avi"};
const std::vector<std::string> imageTypes{".png", ".jpg", ".jpeg"};

std::string getMatType(int type) {
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

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Need video and model to process: navigator <video_path> <model_path>" << std::endl;
    return 1;
  }
  // Set the GST_DEBUG environment variable to a higher level (e.g., 3)
  // setenv("GST_DEBUG", "3", 1);
  std::filesystem::path file_path = argv[1];
  // std::filesystem::path file_path = "../input_video.mp4";
  std::filesystem::path model_path = argv[2];
  // std::filesystem::path model_path = "model.tflite";

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
  std::string video_file_save = "mask_video.avi";

  std::string mediamtx_rtsp =
      "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=2000 speed-preset=fast ! rtspclientsink "
      "location=rtsp://192.168.1.29:8554/river";

  VideoHandler handler(file_path, model_path);
  cv::VideoWriter video(mediamtx_rtsp, cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 5,
                        cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), true);

  handler.setFrameRate(30);
  int numProcessedFrames = 0;

  std::shared_ptr<RiverMaskGenerator> m_generator;
  std::shared_ptr<TFliteRunner> runner;
  runner = std::make_unique<TFliteRunner>(model_path);
  m_generator = std::make_unique<RiverMaskGenerator>(runner);

  cv::Mat colourCorrectFrame;
  cv::Mat mask(cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), CV_8UC1);
  cv::Mat scaledMask(cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), CV_8UC1);
  // cv::Mat colourMask;
  cv::Mat inFrame(cv::Size(handler.getFrameWidth(), handler.getFrameHeight()), CV_8UC3);
  cv::Mat outFrame;
  std::vector<cv::Mat> inputFrameChannels(3);

  while (handler.isDataWaiting()) {
    std::cout << "Processed frame number " << numProcessedFrames++ << "\n";
    inFrame = handler.getCurrentFrame();
    cv::cvtColor(inFrame, colourCorrectFrame, cv::COLOR_BGR2RGB);
    // mask = m_generator->GenerateMask(colourCorrectFrame);
    cv::resize(mask, scaledMask, scaledMask.size(), 0, 0, cv::INTER_LINEAR);

    cv::split(inFrame, inputFrameChannels);
    inputFrameChannels.at(2) += scaledMask;
    cv::merge(inputFrameChannels, outFrame);

    // std::cerr << "colourMask type: " << getMatType(colourMask.type()) << std::endl;
    // std::cerr << "inFrame type: " << getMatType(inFrame.type()) << std::endl;
    // std::cerr << "outFrame type: " << getMatType(outFrame.type()) << std::endl;
    video.write(outFrame);
  }

  // When everything done, release the video capture and write object
  video.release();

  // Save the mask
  // cv::imwrite("mask.jpeg", mask);
  return 0;
}
