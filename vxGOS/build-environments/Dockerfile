# Custom docker image to allow quick availability of the vxGOS toolchain
# It will provide:
#    * `sh-elf-vhex`    - the main compiler
#    * `python3.11`     - CPython 3.11 needed for the vxSDK
#    * `poetry`         - the python package manager for the vxSDK
FROM ubuntu:latest
LABEL maintainer="vxgos"


# Install python 3.11 and poetry
RUN \
       apt-get update \
    && apt-get -y full-upgrade \
    && apt-get install -y software-properties-common wget \
    && add-apt-repository ppa:deadsnakes/ppa -y \
    && apt-get update \
    && apt-get -y full-upgrade \
    && apt-get install -y python3.11 \
    && wget https://bootstrap.pypa.io/get-pip.py \
    && python3.11 get-pip.py \
    && python3.11 -m pip install --upgrade pip \
    && python3.11 -m pip install poetry \
    && rm get-pip.py


# Install the compiler
# @notes
# - we setup DEBIAN_FRONTEND and TZ to avoid goofy behaviour with apt that
#   request user interaction (like the timezone)
# - we manually setup CFFI otherwise poetry may fail to install some
#   dependencies (seem only appear in the latest version on ubuntu docker
#   image)
RUN \
       apt-get install -y git sudo \
    && python3.11 -m pip install cffi \
    && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y tzdata \
    && wget https://raw.githubusercontent.com/YannMagnin/sh-elf-vhex/HEAD/scripts/install.sh \
    && chmod +x ./install.sh \
    && ./install.sh \
        --noconfirm \
        --prefix-sysroot='/usr/local/share/sh-elf-vhex' \
        --prefix-install='/usr/local/bin' \
        --verbose \
    && rm -f ./install.sh


# force clean the image to reduce the final build size
# @notes
# - cleaning cache and logs does not change enough the image size to be noticed
RUN \
       apt-get clean autoclean \
    && apt-get autoremove --yes \
    && rm -rf /var/lib/cache /var/lib/log \
