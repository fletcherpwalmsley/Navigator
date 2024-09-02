# Navigator Cmake file.

cmake_minimum_required(VERSION 3.16)
project(navigator C CXX)

# Include Tflite and OpenCV Items
set(TENSORFLOW_SOURCE_DIR "/tensorflow/tensorflow_src" CACHE PATH
  "Directory that contains the TensorFlow project"
)
if(NOT TENSORFLOW_SOURCE_DIR)
  get_filename_component(TENSORFLOW_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../../../../"
    ABSOLUTE
  )
endif()

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite"
  EXCLUDE_FROM_ALL
)

set(CMAKE_CXX_STANDARD 17)

target_compile_options(navigator PRIVATE -O3)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(navigator PRIVATE -O0)


add_executable(navigator
  main.cpp
  tflite_handler.cpp
)
target_link_libraries(navigator
  tensorflow-lite
  ${OpenCV_LIBS}
)