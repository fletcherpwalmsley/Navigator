# Navigator

<img src="logo.png" width="120" align="right"/>

A vision base navigation system for controlling small boats down narrow rivers

Contains all the items needed to create the system. Including training the model, C++ inference program, Go based display web server (to-do), and Github Actions for automated CI deployment. 

## Quick Start
Navigator is dockerised and releases are provided for both Linux x86-64 and Linux ARMV8 (64bit Raspberry Pi's) \
The easiest way to use Navigator is to clone the latest `navigator-deploy` container from this repo - That can be done in with this command: \
```docker pull ghcr.io/fletcherpwalmsley/navigator-deploy:latest```
Currently you have to manualy run Navigator, this is done with:
```docker run -it --rm --privileged --net host -v <local_video_path>:/river_videos ghcr.io/fletcherpwalmsley/navigator-deploy:latest```

_TODO: Add start instuctions when there is an elegent way to get running_

## Development guide
### CNN Training 
_TODO_
### Inference development
_TODO_

## Current issues
### CI
Both the base and deploy ARM64 builds are created using the multi-platform option in docker buildx. Currently the entire process uses the emulation method.
This is very slow, too slow for the base to be built on the free Github Action runner. Hence the base action is disabled - I generate and push this package from my local machine instead.
Also, modern versions of Tensorflow Lite appear to have a bug that breaks the CMake install-able lib method. So a bunch of Tensorflow Lite items are compiled during a basic navigator-deploy run. As such, a deploy container takes ~80 minutes to create. During this time the Microsoft gods may, or may not kill the running action.



## References and Credit

For re-jigging my memory on the basic usage of CNN's in Tensorflow. Formed the initial basis of the training implementation: \
[U-NET Implementation from Scratch using TensorFlow](https://medium.com/geekculture/u-net-implementation-from-scratch-using-tensorflow-b4342266e406) 
