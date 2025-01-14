#include "process_video.h"

VideoHandler::VideoHandler(std::filesystem::path video_path, std::filesystem::path model_path) {
  m_cap.open(video_path);
  if (!m_cap.isOpened()) {
    std::cerr << "Error opening video file: " << video_path << std::endl;
  }

  m_frame_width = m_cap.get(cv::CAP_PROP_FRAME_WIDTH);
  m_frame_height = m_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  m_totalNumFrames = m_cap.get(cv::CAP_PROP_FRAME_COUNT);
  m_fps = static_cast<int>(m_cap.get(cv::CAP_PROP_FPS));

  std::shared_ptr<CNNRunner> runner = std::make_unique<TFliteRunner>(model_path);
  m_generator = std::make_unique<RiverMaskGenerator>(std::move(runner));
}

void VideoHandler::setFrameRate(size_t desiredFPS) {
  m_skipFrames = round(m_fps / desiredFPS);
  // m_skipFrames = 0;
  std::cout << "Using: " << m_skipFrames << " skip frames\n";
}

bool VideoHandler::isDataWaiting() {
  size_t i = 0;
  m_cap.grab();
  while (i++ <= m_skipFrames) {
    if (!m_cap.retrieve(m_currentFrame)) {
      return false;
    }
  }
  return m_cap.retrieve(m_currentFrame);
}

cv::Mat VideoHandler::processFrame() {
  cv::Mat colour_correct_image;
  cv::Mat resizedFrame;
  cv::Mat grayScaleFrame;
  cv::Mat outFrame;
  cv::Mat mask;

  cv::cvtColor(m_currentFrame, colour_correct_image, cv::COLOR_BGR2RGB);
  mask = m_generator->GenerateMask(colour_correct_image);

  cv::resize(m_currentFrame, resizedFrame, mask.size(), cv::INTER_NEAREST);
  cv::cvtColor(resizedFrame, grayScaleFrame, cv::COLOR_BGR2GRAY);
  cv::addWeighted(grayScaleFrame, 0.6, mask, 0.4, 0, outFrame);
  m_numProcessedFrames++;
  if (m_skipFrames != 0) {
    std::cout << "Processed frame number " << m_numProcessedFrames << " of " << m_totalNumFrames / m_skipFrames << "\n";
  } else {
    std::cout << "Processed frame number " << m_numProcessedFrames << " of " << m_totalNumFrames << "\n";
  }

  return outFrame;
}