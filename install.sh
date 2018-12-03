#!/bin/bash

USR_LIB_DIR=/usr/local/lib
USR_INC_DIR=/usr/local/include
KEYWORD_AML_CPP=datamodel-aml-cpp

# COPY shared library to /usr/local/lib
sudo cp out/linux/armhf/release/libaml.so $USR_LIB_DIR

# COPY header file to /usr/local/include/datamodel-aml-cpp
sudo rm -rf $USR_INC_DIR/$KEYWORD_AML_CPP
sudo mkdir $USR_INC_DIR/$KEYWORD_AML_CPP
sudo cp -r ./include/* $USR_INC_DIR/$KEYWORD_AML_CPP/
