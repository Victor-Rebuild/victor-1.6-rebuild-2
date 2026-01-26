#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/.."
echo $DIR
cd $DIR

function IS_ROBOT_IP_THERE() {
    if [[ ! -f robot_ip.txt ]]; then
        echo "Robot IP file doesn't exist. What is your robot's IP?"
        echo -n "(XXX.XXX.XXX.XXX): "
        read ipaddr
        echo "$ipaddr" > robot_ip.txt
    fi
}

IS_ROBOT_IP_THERE

if [[ ! -f robot_sshkey ]]; then
    echo "You must copy your robot's SSH key the root of this repo with the name robot_sshkey before running this script."
    exit 1
fi

if [[ "$(uname -a)" == *"Darwin"* ]]; then
    ssh-add robot_sshkey && \
    ./project/victor/scripts/victor_deploy.sh -c Release -b && \
    ./project/victor/scripts/victor_start.sh
else
    if [[ "${NO_DOCKER}" = "1" ]]; then
        echo "NO DOCKER"
        eval $(ssh-agent) && \
        ssh-add robot_sshkey && \
        ./project/victor/scripts/victor_deploy.sh -c Release -b && \
        ./project/victor/scripts/victor_start.sh
        exit 0

    else
        echo "DOCKER"
        docker run --rm -it \
            -v $(pwd)/anki-deps:/home/$USER/.anki \
            -v $(pwd):$(pwd) \
            -v $(pwd)/build/cache:/home/$USER/.ccache \
            -v /home/$USER/.ssh:/home/$USER/.ssh \
            vic-standalone-builder-8 bash -c \
            "cd $(pwd) && \
            eval \$(ssh-agent) && \
            ssh-add robot_sshkey && \
            ./project/victor/scripts/victor_deploy.sh $@ -c Release -b && \
            ./project/victor/scripts/victor_start.sh"
    fi
fi