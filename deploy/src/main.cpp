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
int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Need video and model to process: navigator <video_path> <model_path>" << std::endl;
    return 1;
  }
  // Set the GST_DEBUG environment variable to a higher level (e.g., 3)
  setenv("GST_DEBUG", "3", 1);
  std::filesystem::path file_path = argv[1];
  std::filesystem::path model_path = argv[2];

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
  std::unique_ptr<CNNRunner> runner = std::make_unique<TFliteRunner>(model_path);

  std::string mediamtx_h264 =
      "appsrc ! queue ! videoconvert ! video/x-raw,format=I420 ! x264enc speed-preset=ultrafast bitrate=600 "
      "key-int-max=20 ! "
      "video/x-h264,profile=baseline ! rtspclientsink location=rtsp://127.0.0.1:8554/mystream";

  std::string h264_2 =
      "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtspclientsink "
      "location=rtsp://localhost:8554/mystream";

  std::string mediamtx_rtp =
      "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink "
      "host=127.0.0.1 port=5000 debug=true";

  // AV1 requires gst-plugin-rs for rtpav1pay. But these need to be compiled from source
  // If I think I need the savings that AV1 will provide, I can go this route
  // std::string mediamtx_av1 =
  //     "appsrc ! videoconvert ! video/x-raw,format=I420 ! av1enc ! rtpav1pay ! rtspclientsink "
  //     "location=rtsp://localhost:8554/mystream";

  // cv::VideoWriter video(mediamtx_h264, cv::CAP_GSTREAMER, 10.0,
  //                       cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), false);

  cv::VideoWriter video("mask_video.h264", cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('H', '2', '6', '4'), 5,
                        cv::Size(runner->GetOutputWidth(), runner->GetOutputHeight()), false);

  VideoHandler handler(file_path, model_path);
  handler.setFrameRate(5);
  while (handler.isDataWaiting()) {
    video.write(handler.processFrame());
  }

  // When everything done, release the video capture and write object
  video.release();

  // Save the mask
  // cv::imwrite("mask.jpeg", mask);
  return 0;
}
