#!/bin/bash

BASE_DIR=$(cd `dirname $0`;pwd)/../
LCOV_RELEASE=https://github.com/linux-test-project/lcov/releases/download/v1.15/lcov-1.15-1.noarch.rpm
LCOV_EXCLUDE="*/gtest/*  **/include/**"

function install_lcov() {
    cd ${BASE_DIR}
    if [ $(lcov --version | grep version | wc -l) -eq 0 ]; then
        wget ${LCOV_RELEASE}
        sudo yum localinstall $(find . -name "lcov*rpm")
    else
        echo "lcov package is installed"
    fi
}

function gen_lcov() {
    set -e
    lcov -c -d ./ -o result.info
    lcov -r result.info *.h -o result.info
    if [-d lcov]; then
        rm -rf lcov
        mkdir lcov
    fi
    lcov -r result.info ${LCOV_EXCLUDE}  -o result.info
    genhtml -o lcov result.info
}

function main() {
    ## 安装lcov
    install_lcov
    gen_lcov
}

main

