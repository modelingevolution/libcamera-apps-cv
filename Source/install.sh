#!/bin/bash

rm -rf build
mkdir build
pushd build
cmake .. -DENABLE_DRM=1 -DENABLE_X11=0 -DENABLE_QT=0 -DENABLE_OPENCV=0 -DENABLE_TFLITE=0
make -j4
sudo make install
popd
sudo ldconfig
