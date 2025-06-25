#pragma once
#include <memory>
#include <opencv2/opencv.hpp>

#ifdef USE_LIBCAMERA
// As of now, OpenCV doesn't support libcamera.
// So use the LCCV library to bridge this gap.
// The .so needs to be manually built and installed
// See https://github.com/kbarni/LCCV
#include <lccv.hpp>
#endif

struct Options {
  int video_width{768};
  int video_height{432};
  float framerate{15.0f};
  bool verbose{false};
};

class LibCameraFacade {
 public:
  LibCameraFacade() {
#ifdef USE_LIBCAMERA
    m_pi_cam = std::make_unique<lccv::PiCamera>();
    if (!m_pi_cam) {
      throw std::runtime_error("Failed to initialize PiCamera");
    }
#endif
    options = std::make_unique<Options>();
  };
  ~LibCameraFacade() {
#ifdef USE_LIBCAMERA
    if (m_pi_cam) {
      m_pi_cam->stopVideo();
    }
#endif
  };

  // Delete copy constructor and copy assignment operator
  LibCameraFacade(const LibCameraFacade&) = delete;
  LibCameraFacade& operator=(const LibCameraFacade&) = delete;
  // Delete move constructor and move assignment operator
  LibCameraFacade(LibCameraFacade&&) = delete;
  LibCameraFacade& operator=(LibCameraFacade&&) = delete;

  // Implement currently used lccv methods
  // When lccv is not compiled in, dummy methods are used instead
  auto startVideo() const -> bool {
#ifdef USE_LIBCAMERA
    return m_pi_cam->startVideo();
#else
    return 0;
#endif
  };

  auto stopVideo() const -> void {
#ifdef USE_LIBCAMERA
    if (m_pi_cam) {
      m_pi_cam->stopVideo();
#endif
    };

    auto getVideoFrame(cv::Mat & frame, int timeout_ms) const -> bool {
#ifdef USE_LIBCAMERA
      if (m_pi_cam) {
        return m_pi_cam->getVideoFrame(frame, timeout_ms);
      }
#endif
      (void)frame;
      (void)timeout_ms;
      return true;
    };

#ifdef USE_LIBCAMERA
    typedef decltype(m_pi_cam->options) options;
#else
  std::unique_ptr<Options> options;
#endif

   private:
#ifdef USE_LIBCAMERA
    std::unique_ptr<lccv::PiCamera> m_pi_cam;
#endif
  };