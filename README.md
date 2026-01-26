# victor-1.6-rebuild

Welcome to `victor-1.6-rebuild`. This is where my modifed 1.6 source for Vector lives

## Changes from regular 1.6

You can see all the changes made compared to normal 1.6 in [CHANGES.md](/CHANGES.md)

## Installation
Check here for info [ABOUT.md](/ABOUT.md)

## Building (Linux)

 - Prereqs: Make sure you have `docker` installed.

1. Clone the repo and cd into it:

```
cd ~
git clone --recursive https://github.com/Victor-Rebuild/victor-1.6-rebuild -b Main
cd victor-1.6-rebuild
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
cd ~/victor-1.6-rebuild
./wire/build-d.sh
```

3. It should just work! The output will be in `./_build/vicos/Release/`

## Building (Intel macOS)

 - Prereqs: Make sure you have [brew](https://brew.sh/) installed.
   -  Then: `brew install pyenv ccache wget`

1. Clone the repo and cd into it:

```
cd ~
git clone --recursive https://github.com/Victor-Rebuild/victor-1.6-rebuild -b Main
cd victor-1.6-rebuild
```

2. Set up Python 2:

```
pyenv install 2.7.18
pyenv init
```

- Add the following to both ~/.zshrc and ~/.zprofile. After doing so, run the commands in your terminal session:
```
export PYENV_ROOT="$HOME/.pyenv"
[[ -d $PYENV_ROOT/bin ]] && export PATH="$PYENV_ROOT/bin:$PATH"
eval "$(pyenv init -)"
pyenv shell 2.7.18
```

3. Disable security:

```
sudo spctl --master-disable
sudo spctl --global-disable
```
- You will have to head to `System Settings -> Security & Privacy -> Allow applications from` and select "Anywhere".


4. Run the build script:
```
cd ~/victor-1.6-rebuild
./wire/build.sh
```

5. It should just work! The output will be in `./_build/vicos/Release/`

## Deploying

1. Echo your robot's IP address to robot_ip.txt (in the root of the victor repo):

```
echo 192.168.1.150 > robot_ip.txt
```

2. Copy your bot's SSH key to a file called `robot_sshkey` in the root of this repo.

3. Run:

```
# Linux
./wire/deploy-d.sh

# macOS
./wire/deploy.sh
```

<small><sub><sup>DDL, if you're reading this, sosumi.</sup></sub></small>
