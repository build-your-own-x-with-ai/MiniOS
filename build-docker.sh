#!/bin/bash
# 使用 Docker 构建 Mini OS

docker run --rm --platform linux/amd64 -v "$(pwd):/work" -w /work \
    randomdude/gcc-cross-x86_64-elf \
    bash -c "apt-get update -qq && apt-get install -y -qq nasm && make clean && make"
