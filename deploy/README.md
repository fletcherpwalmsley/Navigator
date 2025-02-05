## Helpful references

### Profiling:
`valgrind --tool=callgrind ./navigator`
Use: `kcachegrind` to view callgrind output

### Video Streaming
GStreamer string:
`gst-launch-1.0 v4l2src ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast ! rtspclientsink location=rtsp://localhost:8554/mystream`
This can be used to stream to a MediaMTX server.
This uses software h264 encoding. Hopefully in the future I can get hardware h264 encoding to work


### Old GST strings

 AV1 requires gst-plugin-rs for rtpav1pay. But these need to be compiled from source
  If I think I need the savings that AV1 will provide, I can go this route
`std::string mediamtx_av1 = appsrc ! videoconvert ! video/x-raw,format=I420 ! av1enc ! rtpav1pay ! rtspclientsink location=rtsp://localhost:8554/mystream`

`std::string h264_2 = appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtspclientsink location=rtsp://localhost:8554/mystream`

` std::string mediamtx_h264 = appsrc ! queue ! videoconvert ! video/x-raw,format=I420 !    x264enc speed-preset=ultrafast bitrate=600 key-int-max=20 ! video/x-h264,profile=baseline ! rtspclientsink location=rtsp://127.0.0.1:8554/mystream`

`std::string mediamtx_rtp = appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink host=127.0.0.1 port=5000 debug=true`