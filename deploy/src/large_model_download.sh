#!/bin/bash
echo "Downloading 265*256 model"
curl -L -o build/model.tar.gz\
  https://www.kaggle.com/api/v1/models/fletcherpw/river-segment/tfLite/256_256_input/1/download

tar -xvf build/model.tar.gz -C build/
rm build/model.tar.gz