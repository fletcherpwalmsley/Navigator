cmake_minimum_required(VERSION 3.16)
project(rigger C CXX)

# if(DEFINED ENV{ENABLE_WARNINGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  message(STATUS "Enabled warnings")
# else()
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#   message(STATUS "ALL WARNINGS DISABLED!!")
# endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 20)

find_package(MAVLink REQUIRED)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(rigger_server
  server.c
)

add_executable(rigger_client
  client.c
)

target_link_libraries(rigger_server
  PRIVATE
  MAVLink::mavlink
)

target_link_libraries(rigger_client
  PRIVATE
  MAVLink::mavlink
)
