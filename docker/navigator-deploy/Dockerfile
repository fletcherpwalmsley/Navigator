# Use Navigator base which has all needed dependencies
FROM ghcr.io/fletcherpwalmsley/navigator-base:latest AS build

# Compile navigator app
COPY deploy/src /app/
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake .. && make -j 10

# Create a slim container for deployment
FROM debian:bookworm-slim
# Install runtime deps
# Install essential packages and dependencies
RUN apt-get update && apt-get install -y \
    libgstreamer1.0-0 \
    libgstreamer-plugins-base1.0-0 \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /app/build/navigator /bin/navigator/
COPY training/output/model.tflite /bin/navigator/

# Set the entrypoint to a shell to allow for interactive use, if needed
ENTRYPOINT ["/bin/bash"]

# Required to be run from base dir
# docker build -t build -f deploy/docker/navigator-deploy/Dockerfile .
