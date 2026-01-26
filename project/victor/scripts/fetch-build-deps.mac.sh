#!/bin/bash
set -e
set -u

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

#vlog "Check brew installation."
is_brew=`which brew`
set -e

if [ -z "$is_brew" ]; then
   echo "Brew not found installing now.  You will be prompted."
   /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
fi

vlog "Check homebrew dependencies"
./tools/build/tools/ankibuild/installBuildDeps.py \
    -d python2 \
    ninja \
    python3 \
    libsndfile \
    node \
    rsync \
    openssl \
    curl

vlog "vicos sdk"
#./tools/build/tools/ankibuild/vicos.py --install 5.3.0-r07

vlog "CMake"
./tools/build/tools/ankibuild/cmake.py

#vlog "Go"
#./tools/build/tools/ankibuild/go.py

# vlog "git-lfs"
# $GIT lfs install

if [ -d "/Applications/Webots.app" ]; then
  vlog "check webots version"
  webotsVer=`cat /Applications/Webots.app/resources/version.txt`
  supportedVerFile=./simulator/supportedWebotsVersions.txt
  if ! grep -Fxq "$webotsVer" $supportedVerFile ; then
    vlog "Webots version $webotsVer is unsupported. Here are the supported versions:"
    cat $supportedVerFile
    exit 1
  fi
fi

vlog "Build output dirs"
mkdir -p generated
mkdir -p _build

if [[ ${DONT_ANIM} != "1" ]]; then
  vlog "Extract and encode animation and sound assets. This will take a few seconds."
	./project/buildScripts/dependencies.py -v --deps-file DEPS --externals-dir EXTERNALS
else
	vlog "Not extracting animation assets"
fi

vlog "Configure audio library"
./lib/audio/configure.py

popd > /dev/null 2>&1
