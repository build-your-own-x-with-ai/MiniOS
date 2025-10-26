#!/bin/bash
# 使用 Docker 中的 QEMU 测试 Mini OS

echo "使用 Docker 运行 Mini OS..."
echo "按 Ctrl+C 退出"
echo ""

docker run --rm -it \
    --platform linux/amd64 \
    -v "$(pwd):/work" \
    tianon/qemu \
    qemu-system-i386 \
        -cdrom /work/mini-os.iso \
        -m 128M \
        -boot d \
        -serial stdio
