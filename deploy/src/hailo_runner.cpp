/**
 * @file hailo_runner.cpp
 * @brief Hailo impylmention of CNN Runner
 * @date 2025-03-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "hailo_runner.hpp"

#include "helpers.hpp"
#include <chrono>
#include <thread>

using hailort::Device;
using hailort::Hef;
using hailort::Expected;
using hailort::make_unexpected;
using hailort::ConfiguredNetworkGroup;
using hailort::VStreamsBuilder;
using hailort::InputVStream;
using hailort::OutputVStream;
using hailort::MemoryView;

HailoRunner::HailoRunner(std::string filename) {
  auto all_devices = Device::scan_pcie();
  auto device_expected = Device::create_pcie(all_devices.value()[0]);

  if (!device_expected) {
    std::cerr << "-E- Failed create_pcie " << device_expected.status() << std::endl;
    exit(1);
  }

  m_device = std::move(device_expected.value());
  auto hef = Hef::create(filename);
  if (!hef) {
    std::cerr << "-E Failed to create hef from file " << filename << " - status: " << hef.status() << std::endl;
    exit(1);
  }

  auto configure_params = hef->create_configure_params(HAILO_STREAM_INTERFACE_PCIE);
  if (!configure_params) {
    std::cerr << "-E- Failed hef->create_configure_params " << configure_params.status() << std::endl;
    exit(1);
  }

  auto network_groups = m_device->configure(hef.value(), configure_params.value());
  if (!network_groups) {
    std::cerr << "-E- Failed device.configure " << network_groups.status() << std::endl;
    exit(1);
  }

  if (1 != network_groups->size()) {
    std::cerr << "Invalid amount of network groups" << std::endl;
    exit(1);
  }

  m_network_group = std::move(network_groups->at(0));

  if (!m_network_group) {
    std::cerr << "-E- Failed to configure network group " << filename << std::endl;
    exit(1);
  }

  auto input_vstream_params = m_network_group->make_input_vstream_params(true, HAILO_FORMAT_TYPE_FLOAT32, HAILO_DEFAULT_VSTREAM_TIMEOUT_MS, HAILO_DEFAULT_VSTREAM_QUEUE_SIZE);
  if (!input_vstream_params){
      std::cerr << "-E- Failed make_input_vstream_params " << input_vstream_params.status() << std::endl;
      exit(input_vstream_params.status());
  }

  auto output_vstream_params = m_network_group->make_output_vstream_params(true, HAILO_FORMAT_TYPE_FLOAT32, HAILO_DEFAULT_VSTREAM_TIMEOUT_MS, HAILO_DEFAULT_VSTREAM_QUEUE_SIZE);
  if (!output_vstream_params){
      std::cerr << "-E- Failed make_output_vstream_params " << output_vstream_params.status() << std::endl;
      exit(output_vstream_params.status());
  }
  auto input_vstreams_expected = VStreamsBuilder::create_input_vstreams(*m_network_group, input_vstream_params.value());
  if (!input_vstreams_expected){
      std::cerr << "-E- Failed create_input_vstreams " << input_vstreams_expected.status() << std::endl;
      exit(input_vstreams_expected.status());
  }
  m_input_vstreams = std::move(input_vstreams_expected.value());

  auto output_vstreams_expected = VStreamsBuilder::create_output_vstreams(*m_network_group, output_vstream_params.value());
  if (!output_vstreams_expected) {
      std::cerr << "-E- Failed creating output: " << output_vstreams_expected.status() << std::endl;
      exit(output_vstreams_expected.status());
  }
  m_output_vstreams = std::move(output_vstreams_expected.value());	

  auto activated_network_group_exp = m_network_group->activate();
  if (!activated_network_group_exp) {
      std::cerr << "-E- Failed activated network group " << activated_network_group_exp.status();
      exit(activated_network_group_exp.status());
  }

  m_activated_network_group = std::move(activated_network_group_exp.value());

  // Print testing
  std::cout << "-I---------------------------------------------------------------------" << std::endl;
  std::cout << "-I- Dir  Name                                                          " << std::endl;
  std::cout << "-I---------------------------------------------------------------------" << std::endl;
  for (auto &value: m_input_vstreams){
      std::cout << "-I- IN:  " << value.get_info().name << std::endl;
  }
  std::cout << "-I---------------------------------------------------------------------" << std::endl;
  for (auto &value: m_output_vstreams){
  std::cout << "-I- OUT: " << value.get_info().name << std::endl;
  }
  std::cout << "-I---------------------------------------------------------------------\n" << std::endl;


}

float*  HailoRunner::DoInference(cv::Mat frame) {
  int height = m_input_vstreams.front().get_info().shape.height;
  int width = m_input_vstreams.front().get_info().shape.width;
  if (frame.rows != height || frame.cols != width) {
    cv::resize(frame, frame, cv::Size(width, height), cv::INTER_AREA);
  }
  frame.convertTo(frame, CV_32FC3, 1.0 / 255.0);
  // int factor = std::is_same<T, uint8_t>::value ? 1 : 4;  // In case we uread_allse float32_t, we have 4 bytes per component
  const int factor = 4;
  int channels = m_input_vstreams.front().get_info().shape.features;
  auto write_status = m_input_vstreams[0].write(MemoryView(frame.data, height * width * channels * factor)); // Writing height * width, 3 channels of uint8
  if (HAILO_SUCCESS != write_status) {
    std::cerr << "-E- Failed to write to input stream " << write_status << std::endl;
    return nullptr;
  }

  m_data = std::make_unique<std::vector<float32_t>>(m_output_vstreams.front().get_frame_size());
  auto read_status = m_output_vstreams[0].read(MemoryView(m_data->data(), m_data->size()));
  if (HAILO_SUCCESS != read_status)
  {
    std::cerr << "-E- Failed to read from output stream " << read_status << std::endl;
    return nullptr;
  }
  return m_data->data();
}

cv::Size HailoRunner::GetInputSize() const {
  return cv::Size(64, 64);
}

int HailoRunner::GetOutputHeight() const {
  if (m_output_vstreams.empty()) {
    std::cerr << "-E- No output vstreams available" << std::endl;
    return 0;
  }
  return m_output_vstreams.front().get_info().shape.height;
}

int HailoRunner::GetOutputWidth() const {
  if (m_output_vstreams.empty()) {
    std::cerr << "-E- No output vstreams available" << std::endl;
    return 0;
  }
  return m_output_vstreams.front().get_info().shape.width;
}

int HailoRunner::GetNumOutputClasses() const {
  return 3;
}
