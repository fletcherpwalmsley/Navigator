# Training set-up

## Notes
Currently, the Hailo model compiler (V3.3.0) has very particular set-up requirements. It requires Ubuntu 20, or 22; Python 3.8-10; and Tensorflow 2.12; This last one is the main issue, as it is rather old (current version is 2.18), and does not work with the GPU stack of the lateer versions. As I cannot be bothered messing with my GPU driver set-up, and because the performance of the un-optimised model is already good enough, I'm just going to use CPU only support for creating the Hailo models.

However, GPU support is absolutely needed for model training. So create two venv's, ie `venv-train`, and `venv-hailo` Then install the requirements.txt in both. In `venv-hailo`, then install the dataflow compiler wheel. Installing the wheel before the requirements will result in the conversion not working

 