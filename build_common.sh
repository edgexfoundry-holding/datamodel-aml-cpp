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
set +e
#Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
BLUE="\033[0;34m"
NO_COLOUR="\033[0m"

PROJECT_ROOT=$(pwd)
AML_TARGET_ARCH="x86_64"
AML_INSTALL_PREREQUISITES=false
AML_BUILD_MODE="release"
AML_LOGGING="off"
AML_DISABLE_PROTOBUF=false

RELEASE="1"
LOGGING="0"

install_prerequisites() {
    # download required tool chain for cross compilation [arm/arm64/armhf]
    if [ "arm" = ${AML_TARGET_ARCH} ]; then
        if [ -x "/usr/bin/arm-linux-gnueabi-g++" ] && [ -x "/usr/bin/arm-linux-gnueabi-gcc" ]; then
            echo -e "${BLUE}Cross compile tool-chain found for arm${NO_COLOUR}"
        else
            echo -e "${RED}No cross compile tool-chain found for arm, install using below commands:${NO_COLOUR}"
            echo -e "${BLUE} $ sudo apt-get update"
            echo -e " $ sudo apt-get install gcc-arm-linux-gnueabi"
            echo -e " $ sudo apt-get install g++-arm-linux-gnueabi${NO_COLOUR}"
            exit 0
        fi
    elif [ "arm64" = ${AML_TARGET_ARCH} ]; then
        if [ -x "/usr/bin/aarch64-linux-gnu-g++-4.8" ] && [ -x "/usr/bin/aarch64-linux-gnu-gcc-4.8" ] && [ -x "/usr/bin/aarch64-linux-gnu-g++" ] && [ -x "/usr/bin/aarch64-linux-gnu-gcc" ]; then
            echo -e "${BLUE}Cross compile tool-chain found for arm64${NO_COLOUR}"
        else
            echo -e "${RED}No cross compile tool-chain found for arm64, install using below commands:${NO_COLOUR}"
            echo -e "${BLUE} $ sudo apt-get update"
            echo -e " $ sudo apt-get install gcc-4.8-aarch64-linux-gnu"
            echo -e " $ sudo apt-get install g++-4.8-aarch64-linux-gnu"
            echo -e " $ cd /usr/bin"
            echo -e " $ sudo ln -s aarch64-linux-gnu-g++-4.8 aarch64-linux-gnu-g++"
            echo -e " $ sudo ln -s aarch64-linux-gnu-gcc-4.8 aarch64-linux-gnu-gcc${NO_COLOUR}"
            exit 0
        fi
    elif [ "armhf" = ${AML_TARGET_ARCH} ]; then
        if [ -x "/usr/bin/arm-linux-gnueabihf-g++-4.8" ] && [ -x "/usr/bin/arm-linux-gnueabihf-gcc-4.8" ] && [ -x "/usr/bin/arm-linux-gnueabihf-g++" ] && [ -x "/usr/bin/arm-linux-gnueabihf-gcc" ]; then
            echo -e "${BLUE}Cross compile tool-chain found for armhf${NO_COLOUR}"
        else
            echo -e "${RED}No cross compile tool-chain found for armhf, install using below commands:${NO_COLOUR}"
            echo -e "${BLUE} $ sudo apt-get update"
            echo -e " $ sudo apt-get install gcc-4.8-arm-linux-gnueabihf"
            echo -e " $ sudo apt-get install g++-4.8-arm-linux-gnueabihf"
            echo -e " $ cd /usr/bin"
            echo -e " $ sudo ln -s arm-linux-gnueabihf-g++-4.8 arm-linux-gnueabihf-g++"
            echo -e " $ sudo ln -s arm-linux-gnueabihf-gcc-4.8 arm-linux-gnueabihf-gcc${NO_COLOUR}"
            exit 0
        fi
    fi

    # Check, clone, build and install the depencency libraries
    if [ ! -d "./dependencies" ]; then
        mkdir dependencies
    fi
    cd ./dependencies
    DEP_ROOT=$(pwd)

    # Protobuf
    if [ false = ${AML_DISABLE_PROTOBUF} ]; then
        # build, install protobuf library
        FILENAME="protobuf-cpp-3.4.0.tar.gz"
        cd $DEP_ROOT
        if [ -e "$FILENAME" ]; then
            echo "Protobuf tar exist"
        else
            wget https://github.com/google/protobuf/releases/download/v3.4.0/protobuf-cpp-3.4.0.tar.gz
        fi

        if [ -d "./protobuf-3.4.0" ]; then
            echo "Protobuf library folder exist"
        else
            tar -xvf protobuf-cpp-3.4.0.tar.gz
        fi

        cd protobuf-3.4.0/
        chmod +x autogen.sh
        ./autogen.sh
        if [ "arm" = ${AML_TARGET_ARCH} ]; then
            echo -e "${BLUE}Protobuf configuring for arm${NO_COLOUR}"
            ./configure --host=arm-linux CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
            make -j 4
        elif [ "arm64" = ${AML_TARGET_ARCH} ]; then
            echo -e "${BLUE}Protobuf configuring for arm64${NO_COLOUR}"
            ./configure --host=aarch64-unknown-linux-gnu CC=/usr/bin/aarch64-linux-gnu-gcc-4.8 CXX=/usr/bin/aarch64-linux-gnu-g++-4.8
            make -j 4
            sudo make install
        elif [ "armhf" = ${AML_TARGET_ARCH} ]; then
            echo -e "${BLUE}Protobuf configuring for armhf${NO_COLOUR}"
            ./configure --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc-4.8 CXX=arm-linux-gnueabihf-g++-4.8
            make -j 4
        else
            ./configure
            make -j 4
            sudo make install
        fi

        #handle protobuf error for cross compilation
        if [ "arm" = ${AML_TARGET_ARCH} ] || [ "arm64" = ${AML_TARGET_ARCH} ] || [ "armhf" = ${AML_TARGET_ARCH} ] || [ "armhf-qemu" = ${AML_TARGET_ARCH} ]; then
            echo -e "${BLUE}Copying libs from protobuf built directory to /usr/local/lib${NO_COLOUR}"
            sudo cp src/.libs/protoc /usr/bin
            sudo cp src/.libs/* /usr/local/lib
        fi
        sudo ldconfig
    fi

    echo -e "${GREEN}Install dependencies done${NO_COLOUR}"
}

usage() {
    echo -e "${BLUE}Usage:${NO_COLOUR} ./build.sh <option>"
    echo -e "${GREEN}Options:${NO_COLOUR}"
    echo "  --build_mode=[release|debug](default: release)               :  Build aml library and samples in release or debug mode"
    echo "  --logging=[on|off](default: off)                             :  Build aml library including logs"
    echo "  --disable_protobuf=[true|false](default: false)              :  Disable protobuf feature"
    echo "  --install_prerequisites=[true|false](default: false)         :  Install the prerequisite S/W to build aml"
    echo "  -c                                                           :  Clean aml repository"
    echo "  -h / --help                                                  :  Display help and exit"
}

build_x86() {
    echo -e "Building for x86"
    scons TARGET_OS=linux TARGET_ARCH=x86 RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_x86_64() {
    echo -e "Building for x86_64"
    scons TARGET_OS=linux TARGET_ARCH=x86_64 RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_arm() {
    echo -e "Building for arm"
    scons TARGET_ARCH=arm TC_PREFIX=/usr/bin/arm-linux-gnueabi- TC_PATH=/usr/bin/ RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_arm64() {
    echo -e "Building for arm64"
    scons TARGET_ARCH=arm64 TC_PREFIX=/usr/bin/aarch64-linux-gnu- TC_PATH=/usr/bin/ RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_armhf() {
    echo -e "Building for armhf"
    scons TARGET_ARCH=armhf TC_PREFIX=/usr/bin/arm-linux-gnueabihf- TC_PATH=/usr/bin/ RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_armhf_native() {
    echo -e "Building for armhf_native"
    scons TARGET_ARCH=armhf RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}
}

build_armhf_qemu() {
    echo -e "Building for armhf-qemu"
    scons TARGET_ARCH=armhf RELEASE=${RELEASE} LOGGING=${LOGGING} DISABLE_PROTOBUF=${AML_DISABLE_PROTOBUF}

    if [ -x "/usr/bin/qemu-arm-static" ]; then
        echo -e "${BLUE}qemu-arm-static found, copying it to current directory${NO_COLOUR}"
        cp /usr/bin/qemu-arm-static .
    else
        echo -e "${RED}No qemu-arm-static found${NO_COLOUR}"
        echo -e "${BLUE} - Install qemu-arm-static and build again${NO_COLOUR}"
    fi
}

build() {
    if [ true = ${AML_INSTALL_PREREQUISITES} ]; then
        install_prerequisites
    fi

    if [ "debug" = ${AML_BUILD_MODE} ]; then
        RELEASE="0"
    fi
    if [ "on" = ${AML_LOGGING} ]; then
        LOGGING="1"
    fi

    cd $PROJECT_ROOT
    if [ "x86" = ${AML_TARGET_ARCH} ]; then
         build_x86;
    elif [ "x86_64" = ${AML_TARGET_ARCH} ]; then
         build_x86_64;
    elif [ "arm" = ${AML_TARGET_ARCH} ]; then
         build_arm;
    elif [ "arm64" = ${AML_TARGET_ARCH} ]; then
         build_arm64;
    elif [ "armhf" = ${AML_TARGET_ARCH} ]; then
         build_armhf;
    elif [ "armhf-qemu" = ${AML_TARGET_ARCH} ]; then
         build_armhf_qemu;
    elif [ "armhf-native" = ${AML_TARGET_ARCH} ]; then
         build_armhf_native;
    else
         echo -e "${RED}Not a supported architecture${NO_COLOUR}"
         usage; exit 1;
    fi
    if [ $? -ne 0 ]; then 
        echo -e "${RED}Build failed${NO_COLOUR}" 
        exit 1 
    fi
}

clean() {
    scons -c
    rm -r "${PROJECT_ROOT}/out/" "${PROJECT_ROOT}/.sconsign.dblite"
    find "${PROJECT_ROOT}" -name "*.memcheck" -delete -o -name "*.gcno" -delete -o -name "*.gcda" -delete -o -name "*.os" -delete -o -name "*.o" -delete
    echo -e "Finished Cleaning"
}

process_cmd_args() {
    while [ "$#" -gt 0  ]; do
        case "$1" in
            --install_prerequisites=*)
                AML_INSTALL_PREREQUISITES="${1#*=}";
                if [ ${AML_INSTALL_PREREQUISITES} != true ] && [ ${AML_INSTALL_PREREQUISITES} != false ]; then
                    echo -e "${RED}Unknown option for --install_prerequisites${NO_COLOUR}"
                    shift 1; exit 0
                fi
                echo -e "${GREEN}Install the prerequisites before build: ${AML_INSTALL_PREREQUISITES}${NO_COLOUR}"
                shift 1;
                ;;
            --target_arch=*)
                AML_TARGET_ARCH="${1#*=}";
                echo -e "${GREEN}Target Arch is: $AML_TARGET_ARCH${NO_COLOUR}"
                shift 1
                ;;
            --build_mode=*)
                AML_BUILD_MODE="${1#*=}";
                echo -e "${GREEN}Build mode is: $AML_BUILD_MODE${NO_COLOUR}"
                shift 1;
                ;;
            --logging=*)
                AML_LOGGING="${1#*=}";
                echo -e "${GREEN}Logging option is: $AML_LOGGING${NO_COLOUR}"
                shift 1;
                ;;
            --disable_protobuf=*)
                AML_DISABLE_PROTOBUF="${1#*=}";
                if [ ${AML_DISABLE_PROTOBUF} != true ] && [ ${AML_DISABLE_PROTOBUF} != false ]; then
                    echo -e "${RED}Unknown option for --disable_protobuf${NO_COLOUR}"
                    shift 1; exit 0
                fi
                echo -e "${GREEN}is Protobuf disabled : $AML_DISABLE_PROTOBUF${NO_COLOUR}"
                shift 1;
                ;;
            -c)
                clean
                shift 1; exit 0
                ;;
            -h)
                usage; exit 0
                ;;
            --help)
                usage; exit 0
                ;;
            -*)
                echo -e "${RED}"
                echo "unknown option: $1" >&2;
                echo -e "${NO_COLOUR}"
                usage; exit 1
                ;;
            *)
                echo -e "${RED}"
                echo "unknown option: $1" >&2;
                echo -e "${NO_COLOUR}"
                usage; exit 1
                ;;
        esac
    done
}

process_cmd_args "$@"
echo -e "Building AML DataModel library("${AML_TARGET_ARCH}").."
build
echo -e "Done building AML DataModel library("${AML_TARGET_ARCH}")"

exit 0
