FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential cmake ninja-build pkg-config \
    libsdl2-dev libsdl2-image-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
COPY . /work

RUN cmake --preset docker-linux && cmake --build --preset docker-linux-release