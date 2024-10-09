# Use a base image with a compatible version of CMake and build tools
FROM debian:bookworm

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install essential packages and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    vim \
    git \
    wget \
    unzip \
    gdb \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libswscale-dev \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-rtsp \
    && rm -rf /var/lib/apt/lists/*

# Get Opencv and Tensorflow Lite
WORKDIR /
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.10.0.zip && \
    unzip opencv.zip && \
    rm opencv.zip && \
    mv opencv-4.10.0 opencv_src && \
    wget -O tensorflow.zip https://github.com/tensorflow/tensorflow/archive/refs/tags/v2.17.0.zip && \
    unzip tensorflow.zip && \
    rm tensorflow.zip && \
    mkdir /tensorflow && \
    mv tensorflow-2.17.0 /tensorflow/tensorflow_src

# Create build directory and make OpenCv
WORKDIR /opencv_src/opencv_build
RUN cmake .. \
    -D CMAKE_CXX_STANDARD=17 \
    -D BUILD_SHARED_LIBS=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_opencv_python3=OFF \
    -D OPENCV_DNN_OPENCL=OFF \
    -D BUILD_PROTOBUF=OFF \
    -D WITH_PROTOBUF=OFF \
    -D WITH_GSTREAMER=ON \
    -D WITH_FFMPEG=OFF \
    && make -j 20 && make install

# Create build directory and make Tensorflow Lite
WORKDIR /tensorflow/tflite_build
RUN cmake ../tensorflow_src/tensorflow/lite \
    -D CMAKE_CXX_STANDARD=17 && \
    cmake --build . -j 20 && \
    cmake --install .