#!/bin/bash
set -e
set -u

SCRIPT_PATH=$(dirname $([ -L $0 ] && echo "$(dirname $0)/$(readlink -n $0)" || echo $0))

set +e
CURRENT_EXTERNALS_VERSION=$(cat "EXTERNALS/VERSION") >> /dev/null
set -e
EXTERNALS_VERSION_LATEST=15

GIT=`which git`
if [ -z $GIT ]; then
  echo git not found
  exit 1
fi
: ${TOPLEVEL:=`$GIT rev-parse --show-toplevel`}

function vlog()
{
    echo "[fetch-build-deps] $*"
}

pushd "${TOPLEVEL}" > /dev/null 2>&1

OS_NAME=$(uname -s)
case $OS_NAME in
    "Darwin")
        HOST="mac"
        ;;
    "Linux")
        HOST="linux"
        ;;
esac

if [[ -d EXTERNALS/ ]]; then
    if [ $EXTERNALS_UPDATE_SKIP != 1 ]; then
        if [ "$CURRENT_EXTERNALS_VERSION" -le "6" ]; then
            echo "This is a old version of EXTERNALS from before it was a submodule"
            echo "Please reclone the repo in order to get the submodule"
            echo "Current version = $CURRENT_EXTERNALS_VERSION"
            echo "Latest version = $EXTERNALS_VERSION_LATEST "
            exit 1
        else
            if [ "$CURRENT_EXTERNALS_VERSION" != "$EXTERNALS_VERSION_LATEST" ]; then
                echo "Old EXTERNALS version found"
                echo "Updating EXTERNALS"
                cd EXTERNALS/
                git pull
                git checkout externals-$EXTERNALS_VERSION_LATEST
                cd ..
            else
                echo "EXTERNALS up to date (EXTERNALS version = $CURRENT_EXTERNALS_VERSION, Latest version = $EXTERNALS_VERSION_LATEST)"
            fi
        fi
    else
        echo "EXTERNALS update skip detected, not updating EXTERNALS"
    fi
else
    echo "This repo was cloned incorrectly, please reclone with the "--recurse-submodules" flag"
    exit 1
fi

HOST_FETCH_DEPS=${SCRIPT_PATH}/"fetch-build-deps.${HOST}.sh"

if [ -x "${HOST_FETCH_DEPS}" ]; then
    ${HOST_FETCH_DEPS}
else
    echo "ERROR: Could not determine platform for system name: $OS_NAME"
    exit 1
fi
