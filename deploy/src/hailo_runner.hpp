#pragma once

#include <opencv2/core/mat.hpp>

#include "cnn_runner.hpp"
#include "hailo/hailort.hpp"

class HailoRunner : public CNNRunner {
 public:
  HailoRunner(std::string model_path);
  float* DoInference(cv::Mat input_mat);
  cv::Size GetInputSize() const;
  int GetOutputHeight() const;
  int GetOutputWidth() const;
  int GetNumOutputClasses() const;

 private:
 std::unique_ptr<hailort::Device> m_device;
 std::shared_ptr<hailort::ConfiguredNetworkGroup>  m_network_group;
 std::vector<hailort::InputVStream> m_input_vstreams;
 std::vector<hailort::OutputVStream> m_output_vstreams;
 std::unique_ptr<hailort::ActivatedNetworkGroup> m_activated_network_group;
 // This class keeps ownership of the data, primalry to allow
 // for a raw pointer to be returned
 std::unique_ptr<std::vector<float32_t>> m_data;
}; 
