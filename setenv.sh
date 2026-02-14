#!/usr/bin/env bash

export GIT_PROJ_ROOT="$(
  cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 \
  && git rev-parse --show-toplevel
)"

alias victor_restart="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_restart.sh"
alias victor_start="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_start.sh"
alias victor_stop="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_stop.sh"
alias victor_stage="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/stage.sh -c Release-Makefiles -b"
alias victor_stage_debug="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/stage.sh -c Debug-Makefiles -b"
alias watch_backtraces="cd ${GIT_PROJ_ROOT} && echo Waiting for backtraces... && python3 project/victor/scripts/backtracewatcher.py"
alias watch_backtraces_debug="cd ${GIT_PROJ_ROOT} && echo Waiting for backtraces... && python3 project/victor/scripts/backtracewatcherdebug.py"

alias victor_build_release="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_build_release.sh"
alias victor_build_debug="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_build_debug.sh"
alias victor_deploy_run="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_deploy.sh -c Release-Makefiles -b && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_start.sh"
alias victor_deploy_run_debug="cd ${GIT_PROJ_ROOT} && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_deploy.sh -c Debug-Makefiles -b && ${GIT_PROJ_ROOT}/project/victor/scripts/victor_start.sh"

function IS_ROBOT_IP_THERE() {
    if [[ ! -f "$GIT_PROJ_ROOT/robot_ip.txt" ]]; then
        echo "Robot IP file doesn't exist. What is your robot's IP?"
        echo -n "(XXX.XXX.XXX.XXX): "
        read ipaddr
        echo "$ipaddr" > "$GIT_PROJ_ROOT/robot_ip.txt"
    fi
}

alias vclean='rm -rf ${GIT_PROJ_ROOT}/_build ${GIT_PROJ_ROOT}/generated'
alias vbuild=victor_build_release
alias vbuilddebug=victor_build_debug
alias vdeploy='IS_ROBOT_IP_THERE; victor_stage && victor_deploy_run'
alias vdeploydebug='IS_ROBOT_IP_THERE; victor_stage_debug && victor_deploy_run_debug'
alias vbd='victor_build_release && victor_stage && victor_deploy_run'
alias vbdd='victor_build_debug && victor_stage_debug && victor_deploy_run_debug'
alias stage='victor_stage'
alias see_size_of_silly_victor_build='victor_stage && du -sh _build/staging'
alias wbtrace='watch_backtraces'
