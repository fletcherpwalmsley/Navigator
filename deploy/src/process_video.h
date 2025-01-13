#pragma once
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "river_mask_generator.h"

class VideoHandler {
 public:
  VideoHandler(std::filesystem::path video_path);
  ~VideoHandler() { m_cap.release(); }

  bool isDataWaiting();
  cv::Mat processFrame();
  void setFrameRate(size_t desiredFPS);

 private:
  cv::VideoCapture m_cap;
  int m_frame_width{0};
  int m_frame_height{0};
  int m_totalNumFrames{0};
  int m_fps{0};
  size_t m_skipFrames{0};
  size_t m_numProcessedFrames{0};
  cv::Mat m_currentFrame;

  std::shared_ptr<RiverMaskGenerator> m_generator;
};