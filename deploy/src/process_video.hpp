#pragma once
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "libcamera_factory.hpp"

class VideoHandler {
 public:
  explicit VideoHandler(std::unique_ptr<LibCameraFacade> pi_cam_facade);
  explicit VideoHandler(std::unique_ptr<cv::VideoCapture> cv_cap);
  ~VideoHandler();  // Destructor

  // Delete copy constructor and copy assignment operator
  VideoHandler(const VideoHandler&) = delete;
  VideoHandler& operator=(const VideoHandler&) = delete;

  // Delete move constructor and move assignment operator
  VideoHandler(VideoHandler&&) = delete;
  VideoHandler& operator=(VideoHandler&&) = delete;

  [[nodiscard]] auto getFrameWidth() const -> int { return m_frame_width; }
  [[nodiscard]] auto getFrameHeight() const -> int { return m_frame_height; }
  [[nodiscard]] auto getFPS() const -> int { return m_fps; }

  [[nodiscard]] std::pair<bool, const cv::Mat&> getNextFrame();

 private:
  std::unique_ptr<cv::VideoCapture> m_cv_cap;
  std::unique_ptr<LibCameraFacade> m_pi_cam;
  std::unique_ptr<cv::Mat> m_currentFrame;
  int m_frame_width{0};
  int m_frame_height{0};
  float m_fps{0};
  size_t m_numProcessedFrames{0};
};
