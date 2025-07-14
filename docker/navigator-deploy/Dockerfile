# Use Navigator base which has all needed dependencies
FROM ghcr.io/fletcherpwalmsley/navigator-base:latest AS build

# Compile navigator app
RUN apt-get update && apt-get install -y \
    curl
COPY deploy/src /app/
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake -DUSE_TFLITE=ON -DUSE_HAILO=OFF -DUSE_LIBCAMERA=OFF -DCMAKE_BUILD_TYPE=Release .. && make -j 2
# No need to get the model, as TFlite is disabled
RUN curl -L -o /model.tar.gz \ 
    https://www.kaggle.com/api/v1/models/fletcherpw/river-segment/tfLite/default/2/download
RUN tar xf model.tar.gz


# Create a slim container for deployment
FROM debian:bookworm-slim
# Install runtime deps
# Install essential packages and dependencies
# At some point, update the base container to remove FFMPEG
RUN apt-get update && apt-get install -y \
    libgstreamer1.0-0 \
    libgstreamer-plugins-base1.0-0 \
    gstreamer1.0-rtsp \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /app/build/navigator /bin/navigator/
COPY --from=build model.tar.gz /bin/navigator/model.tar.gz

# Set the entrypoint to a shell to allow for interactive use, if needed
ENTRYPOINT ["/bin/bash"]

# Required to be run from base dir
# docker build -t build -f deploy/docker/navigator-deploy/Dockerfile .
