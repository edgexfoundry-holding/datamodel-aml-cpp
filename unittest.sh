###############################################################################
# Copyright 2018 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###############################################################################

#!/bin/bash

PROJECT_ROOT=$(pwd)
AML_TARGET_ARCH=x86_64

function build(){
    cd $PROJECT_ROOT
    scons TARGET_OS=linux TARGET_ARCH=${AML_TARGET_ARCH} RELEASE=0
    if [ $? -ne 0 ]; then 
        echo -e "\033[31m"Build failed"\033[0m" 
        exit 1 
    fi
}

function run_test(){
    cd $PROJECT_ROOT/out/linux/${AML_TARGET_ARCH}/debug/unittests
    ./aml_rep_test
    if [ $? -ne 0 ]; then 
        echo -e "\033[31m"Unittests failed"\033[0m" 
        exit 1 
    fi
}

function coverage() {
    cd $PROJECT_ROOT
    if [ ! -d "./coverage" ]; then
        mkdir coverage
    fi

    gcovr -r . \
            -e "build_common.*" \
            -e "coverage.*" \
            -e "dependencies.*" \
            -e "docs.*" \
            -e "extlibs.*" \
            -e "out.*" \
            -e "protobuf.*" \
            -e "samples.*" \
            -e "unittests.*" \
            -e "src/logger.*" \
            --html --html-details -o ./coverage/report.html
}

echo -e "Building AML DataModel library("${AML_TARGET_ARCH}").."
build
echo -e "Done building AML DataModel library("${AML_TARGET_ARCH}")"

echo -e "Running AML DataModel Unittests"
run_test
echo -e "Done Unittests"

if [ "$1" == --coverage ]; then
    coverage
fi

exit 0
