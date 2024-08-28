# Use Navigator base which has all needed dependencies
FROM ghcr.io/fletcherpwalmsley/navigator-base:latest AS build

# Compile navigator app
COPY deploy/src /app/
RUN mkdir /app/build
WORKDIR /app/build
RUN cmake .. && make -j 20

# Create a slim container for deployment
FROM debian:bookworm-slim
COPY --from=build /app/build/minimal /bin/navigator/

# Set the entrypoint to a shell to allow for interactive use, if needed
ENTRYPOINT ["/bin/bash"]

# Required to be run from base dir
# docker build -t build -f deploy/docker/navigator-deploy/Dockerfile .