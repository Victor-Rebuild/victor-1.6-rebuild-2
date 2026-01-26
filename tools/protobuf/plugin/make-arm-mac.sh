#!/usr/bin/env bash

set -e
set -u

SCRIPT_PATH=$(dirname $([ -L $0 ] && echo "$(dirname $0)/$(readlink -n $0)" || echo $0))
SCRIPT_PATH_ABSOLUTE=`pushd ${SCRIPT_PATH} >> /dev/null; pwd; popd >> /dev/null`

if [[ ! -x /usr/bin/g++ ]]; then
    echo /usr/bin/g++ not found
    exit 1
fi

HOST=mac

pushd ${SCRIPT_PATH} >> /dev/null

GIT_ROOT=`git rev-parse --show-toplevel`
PROTOBUF_LOC="${GIT_ROOT}/EXTERNALS/deps/protobuf/${HOST}"
INCLUDES="${PROTOBUF_LOC}/include"
LIBS="${PROTOBUF_LOC}/lib"
SRCS=`find . -type f -iname "*.cpp"`
OUTPUT="${SCRIPT_PATH_ABSOLUTE}/protocCppPlugin"

CXXFLAGS="-arch x86_64 -mmacosx-version-min=10.15"
LDFLAGS="-arch x86_64"

clang++                          \
    --std=c++14                  \
    -I.  -I${INCLUDES}            \
    ${SRCS}                       \
    -o ${OUTPUT}                  \
    -L${LIBS}                     \
    ${LIBS}/libprotoc.a           \
    ${LIBS}/libprotobuf.a         \
    -lpthread                     \
    ${CXXFLAGS} ${LDFLAGS}

popd >> /dev/null

