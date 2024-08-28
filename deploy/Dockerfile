# Use a base image with a compatible version of CMake and build tools
FROM ubuntu:24.04

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
    && rm -rf /var/lib/apt/lists/*

# Get Tensorflow-lite 
RUN wget -O tensorflow.zip https://github.com/tensorflow/tensorflow/archive/refs/tags/v2.17.0.zip && \
    unzip tensorflow.zip && \
    rm tensorflow.zip && \
    mv tensorflow-2.17.0 tensorflow_src

# Create build directory and make
WORKDIR /tensorflow_src/tflite_build
RUN cmake ../tensorflow/lite && \
    cmake --build . -j 20 && \
    cmake --install .


# Get Opencv
WORKDIR /
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.10.0.zip && \
    unzip opencv.zip && \
    rm opencv.zip && \
    mv opencv-4.10.0 opencv_src

# Create build directory and make
WORKDIR /opencv_src/opencv_build
RUN cmake .. \
    -DCMAKE_CXX_STANDARD=14 \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=OFF \
    -DOPENCV_DNN_OPENCL=OFF \
    -DWITH_PROTOBUF=OFF \
    -DBUILD_PROTOBUF=OFF
RUN make -j 20 && make install

