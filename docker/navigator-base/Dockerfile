# Build the Rust based webrtcsink by itself, as it's develop depends conflict with OpenCV's ones
FROM rust:1-bookworm AS rust_build

RUN apt-get update && apt-get install -y \
    git \
    gstreamer1.0-tools \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-bad \
    libgstreamer-plugins-bad1.0-dev \
    libssl-dev \
    npm

WORKDIR /
RUN git clone https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs.git 
WORKDIR /gst-plugins-rs
RUN cargo install cargo-c  && \
    cargo cbuild --release -p gst-plugin-webrtc --prefix=/usr && \
    cargo cinstall --release -p gst-plugin-webrtc --prefix=/usr

# gst-inspect-1.0 webrtcsink
# Location: /usr/include/gstreamer-1.0/gst/webrtc
# Need to copy out for deploy step

# /gst-plugins-rs/target/x86_64-unknown-linux-gnu/release/libgstrswebrtc.so
# /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstrswebrtc.so

# # Use a base image with a compatible version of CMake and build tools
# FROM debian:bookworm

# # Set environment variables to avoid interactive prompts
# ENV DEBIAN_FRONTEND=noninteractive

# # Install essential packages and dependencies
# RUN apt-get update && apt-get install -y \
#     build-essential \
#     cmake \
#     vim \
#     git \
#     wget \
#     unzip \
#     gdb \
#     libavformat-dev \
#     libavcodec-dev \
#     libavutil-dev \
#     libswscale-dev \
#     gstreamer1.0-tools \
#     gstreamer1.0-plugins-base \
#     gstreamer1.0-plugins-bad \
#     libgstreamer1.0-dev \
#     libgstreamer-plugins-base1.0-dev \
#     && rm -rf /var/lib/apt/lists/*


# # Copy in the webrtc plug-in from before
# COPY --from=rust_build /usr/include/gstreamer-1.0/gst/webrtc /usr/include/gstreamer-1.0/gst/webrtc
# COPY --from=rust_build /gst-plugins-rs/target/x86_64-unknown-linux-gnu/release/libgstrswebrtc.so \
#     /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstrswebrtc.so

# # # Get Opencv and Tensorflow Lite
# # WORKDIR /
# RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.10.0.zip && \
#     unzip opencv.zip && \
#     rm opencv.zip && \
#     mv opencv-4.10.0 opencv_src && \
#     wget -O tensorflow.zip https://github.com/tensorflow/tensorflow/archive/refs/tags/v2.17.0.zip && \
#     unzip tensorflow.zip && \
#     rm tensorflow.zip && \
#     mkdir /tensorflow && \
#     mv tensorflow-2.17.0 /tensorflow/tensorflow_src

# # Create build directory and make OpenCv
# WORKDIR /opencv_src/opencv_build
# RUN cmake .. \
#     -D CMAKE_CXX_STANDARD=17 \
#     -D BUILD_SHARED_LIBS=OFF \
#     -D BUILD_opencv_python2=OFF \
#     -D BUILD_opencv_python3=OFF \
#     -D OPENCV_DNN_OPENCL=OFF \
#     -D BUILD_PROTOBUF=OFF \
#     -D WITH_PROTOBUF=OFF \
#     -D WITH_GSTREAMER=ON \
#     -D WITH_FFMPEG=OFF \
#     && make -j 20 && make install

# # Create build directory and make Tensorflow Lite
# WORKDIR /tensorflow/tflite_build
# RUN cmake ../tensorflow_src/tensorflow/lite \
#     -D CMAKE_CXX_STANDARD=17 && \
#     cmake --build . -j 20 && \
#     cmake --install .