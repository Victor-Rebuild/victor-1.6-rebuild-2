#!/usr/bin/env bash

GIT_PROJ_ROOT=`git rev-parse --show-toplevel`
${GIT_PROJ_ROOT}/project/victor/scripts/systemctl.sh "$@" start mm-qcamera-daemon
sleep 2
${GIT_PROJ_ROOT}/project/victor/scripts/systemctl.sh "$@" start mm-anki-camera
sleep 2
${GIT_PROJ_ROOT}/project/victor/scripts/systemctl.sh "$@" start anki-robot.target
