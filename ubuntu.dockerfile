FROM ubuntu:24.04

RUN apt update && \
    apt-get install -y git \
        build-essential \
        curl \
        ninja-build \
        cmake \
        libudev-dev \
        pkg-config \
        pipx && \
    rm -rf /var/lib/apt/lists/*

# Install uv
RUN curl -LsSf https://astral.sh/uv/0.9.15/install.sh | sh

WORKDIR /root

RUN pipx install conan

# Build SDK
RUN git clone https://github.com/pbeTrifork/inno-lidar-sdk.git && \
    PATH=$PATH:/root/.local/bin && \
    cd inno-lidar-sdk && \
    conan profile detect && \
    conan build . && \
    conan create .
