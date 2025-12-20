# Navigator Cmake file.

cmake_minimum_required(VERSION 3.16)
project(navigator C CXX)

# if(DEFINED ENV{ENABLE_WARNINGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  message(STATUS "Enabled warnings")
# else()
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#   message(STATUS "ALL WARNINGS DISABLED!!")
# endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_CXX_FLAGS_RELEASE "-g")

# Build options
option(USE_TFLITE "Use TensorFlow Lite" OFF)
option(USE_HAILO "Use Hailo" OFF)
option(USE_LIBCAMERA "Use libcamera" OFF)

find_package(Threads)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

if(USE_LIBCAMERA)
  pkg_check_modules(LIBCAMERA REQUIRED libcamera)
  add_definitions(-DUSE_LIBCAMERA)
  include_directories(${LIBCAMERA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
else()
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()


set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


if(USE_TFLITE)
  # Include Tflite Items
  set(TENSORFLOW_SOURCE_DIR "/tensorflow/tensorflow_src" CACHE PATH
    "Directory that contains the TensorFlow project"
  )
  if(NOT TENSORFLOW_SOURCE_DIR)
    get_filename_component(TENSORFLOW_SOURCE_DIR
      "${CMAKE_CURRENT_LIST_DIR}/../../../../"
      ABSOLUTE
    )
  endif()

  # In-code pre-processor definitions
  add_definitions(-DUSE_TFLITE)

  add_subdirectory(
    "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
    "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite"
    EXCLUDE_FROM_ALL
  )

  add_executable(${PROJECT_NAME}
    main.cpp
    tflite_runner.cpp
    river_mask_generator.cpp
    process_video.cpp
    weighted_moving_average.cpp
    mask_limit_finders.cpp
          webserver.cpp
          webserver.h
  )

  target_link_libraries(${PROJECT_NAME}
    tensorflow-lite
    ${OpenCV_LIBS}
  )

elseif(USE_HAILO)

  add_definitions(-DUSE_HAILO)
  find_package(HailoRT REQUIRED)
  add_executable(${PROJECT_NAME}
    main.cpp
    river_mask_generator.cpp
    process_video.cpp
    weighted_moving_average.cpp
    mask_limit_finders.cpp
    hailo_runner.cpp
          webserver.cpp
          webserver.h
  )

  # include_directories(/usr/include ./)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g)

  set(LINK_LIBRARIES
    ${OpenCV_LIBS}
    HailoRT::libhailort
    Threads::Threads
  )
  
  if(USE_LIBCAMERA)
    list(APPEND LINK_LIBRARIES -llccv)
  endif()
  
  target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

else()
  add_executable(${PROJECT_NAME}
    main.cpp
    river_mask_generator.cpp
    process_video.cpp
    weighted_moving_average.cpp
    mask_limit_finders.cpp
          external/crow_websvr.hpp
          webserver.cpp
          webserver.h
  )

  target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
  )
endif()