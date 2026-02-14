# victor-1.6-rebuild-private

Welcome to `victor-1.6-rebuild-2`. This is where my modifed 1.6 source for Vector lives

## Changes from regular 1.6

You can see all the changes made compared to normal 1.6 in [CHANGES.md](/CHANGES.md)

## Installation
Check here for info [ABOUT.md](/ABOUT.md)

## Building (Linux, amd64 only, bare metal)

- Prerequisites:
  - glibc 2.35 or above - this means anything Debian Bookworm-era and newer will work.
  - The following packages need to be installed: `git wget curl openssl ninja g++ gcc pkg-config ccache`
```
# Arch Linux:
sudo pacman -S git wget curl openssl ninja gcc pkgconf ccache
# Ubuntu/Debian:
sudo apt-get update && sudo apt-get install -y git wget curl openssl ninja-build gcc g++ pkg-config ccache
# Fedora
sudo dnf install -y git wget curl openssl ninja-build gcc gcc-c++ pkgconf-pkg-config ccache
```

1. Clone the repo and cd into it:

```
cd ~
git clone --recursive https://github.com/Victor-Rebuild/victor-1.6-rebuild-2 -b master
cd victor-1.6-rebuild-2
```

2. Source `setenv.sh`:
```
source setenv.sh
```

3. (OPTIONAL) Run this so you don't have to perform step 2 every time:
```
echo "source \"$(pwd)/setenv.sh\"" >> $HOME/.bashrc
```

4. Build:
```
vbuild
```

## Building (Linux, amd64 only, docker)

 - Prereqs: Make sure you have `docker` and `git` installed.

1. Clone the repo and cd into it:

```
cd ~
git clone --recursive https://github.com/Victor-Rebuild/victor-1.6-rebuild-2 -b master
cd victor-1.6-rebuild-2
```

2. Make sure you can run Docker as a normal user. This will probably involve:

```
sudo groupadd docker
sudo gpasswd -a $USER docker
newgrp docker
sudo chown root:docker /var/run/docker.sock
sudo chmod 660 /var/run/docker.sock
```

3. Run the build script:
```
cd ~/victor-1.6-rebuild-2
./build/build-v.sh
```

3. It should just work! The output will be in `./_build/vicos/Release/`

## Deploying

1. Echo your robot's IP address to robot_ip.txt (in the root of the victor repo):

```
echo 192.168.1.150 > robot_ip.txt
```

2. Copy your bot's SSH key to a file called `robot_sshkey` in the root of this repo.

3. Run:

```
# Linux, docker
./build/deploy-v.sh

# Linux, bare metal
vdeploy
```

<small><sub><sup>DDL, if you're reading this, sosumi.</sup></sub></small>
