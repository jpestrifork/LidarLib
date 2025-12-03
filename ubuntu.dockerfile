FROM ubuntu:24.04

RUN apt update && \
    apt-get install -y git \
        build-essential \
        curl \
        python3 \
        python3-pip \
        python3.12-venv