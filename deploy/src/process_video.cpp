#include "process_video.hpp"

#include <iostream>

VideoHandler::VideoHandler(std::unique_ptr<LibCameraFacade> pi_cam_facade) {
  m_pi_cam = std::move(pi_cam_facade);

  if (!m_pi_cam) {
    std::cerr << "Failed to initialize pi camera" << std::endl;
  }
  m_frame_width = m_pi_cam->getOptions()->video_width;
  m_frame_height = m_pi_cam->getOptions()->video_height;
  m_fps = m_pi_cam->getOptions()->video_height;
  m_currentFrame = std::make_unique<cv::Mat>(cv::Size(m_frame_width, m_frame_height), CV_8UC3);
  m_pi_cam->startVideo();
}

VideoHandler::VideoHandler(std::unique_ptr<cv::VideoCapture> cv_cap) {
  m_cv_cap = std::move(cv_cap);

  if (!m_cv_cap) {
    std::cerr << "Failed to initialize cv capture" << std::endl;
  }

  if (!m_cv_cap->isOpened()) {
    std::cerr << "Error opening CV VideoCapture file: " << std::endl;
  }

  m_frame_width = m_cv_cap->get(cv::CAP_PROP_FRAME_WIDTH);
  m_frame_height = m_cv_cap->get(cv::CAP_PROP_FRAME_HEIGHT);
  m_fps = static_cast<float>(m_cv_cap->get(cv::CAP_PROP_FPS));
  m_currentFrame = std::make_unique<cv::Mat>(cv::Size(m_frame_width, m_frame_height), CV_8UC3);
}

VideoHandler::~VideoHandler() {
  if (m_pi_cam) {
    m_pi_cam->stopVideo();
  }
  if (m_cv_cap) {
    m_cv_cap->release();
  }
}

std::pair<bool, const cv::Mat&> VideoHandler::getNextFrame() {
  // If we have a video file object, get the frames from that
  if (m_cv_cap) {
    if (m_cv_cap->read(*m_currentFrame)) {
      return {true, *m_currentFrame};
    } else {
      std::cout << "Error retrieving frame from CV VideoCapture - Likely end of File" << std::endl;
      return {false, *m_currentFrame};
    }
  }

  // Otherwise, get it from the pi camera
  if (m_pi_cam) {
    if (m_pi_cam->getVideoFrame(*m_currentFrame, 1000)) {
      return {true, *m_currentFrame};
    } else {
      std::cout << "Error retrieving frame from PiCamera" << std::endl;
      return {false, *m_currentFrame};
    }
  }
  return {false, *m_currentFrame};
}