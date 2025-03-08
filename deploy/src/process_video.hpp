#pragma once
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "river_mask_generator.hpp"
#include "weighted_moving_average.hpp"

class VideoHandler {
 public:
  VideoHandler(std::filesystem::path video_path);
  ~VideoHandler() { m_cap.release(); }

  bool isDataWaiting();
  cv::Mat processFrame();
  void setFrameRate(size_t desiredFPS);
  [[nodiscard]] auto getCurrentFrame() const -> const cv::Mat& { return m_currentFrame; }
  [[nodiscard]] auto getFrameWidth() const -> int { return m_frame_width; }
  [[nodiscard]] auto getFrameHeight() const -> int { return m_frame_height; }
  [[nodiscard]] auto getFPS() const -> int { return m_fps; }

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
  std::unique_ptr<WeightedMovingAverage> m_movingAverage;
};