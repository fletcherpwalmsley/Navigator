## Helpful references

### Profiling:
`valgrind --tool=callgrind ./navigator`
Use: `kcachegrind` to view callgrind output

### Video Streaming
GStreamer string:
`gst-launch-1.0 v4l2src ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast ! rtspclientsink location=rtsp://localhost:8554/mystream`
This can be used to stream to a MediaMTX server.
This uses software h264 encoding. Hopefully in the future I can get hardware h264 encoding to work
