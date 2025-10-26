#!/bin/bash
# 使用 Docker 生成 ISO 镜像

echo "使用 Docker 生成 Mini OS ISO 镜像..."

docker run --rm --platform linux/amd64 -v "$(pwd):/work" -w /work \
    randomdude/gcc-cross-x86_64-elf \
    bash -c "apt-get update -qq && \
             apt-get install -y -qq grub-pc-bin xorriso mtools && \
             make iso"

if [ -f mini-os.iso ]; then
    echo ""
    echo "✅ ISO 镜像生成成功！"
    echo "文件: mini-os.iso"
    ls -lh mini-os.iso
    echo ""
    echo "现在可以在 VirtualBox 或 QEMU 中运行："
    echo "  qemu-system-i386 -cdrom mini-os.iso"
else
    echo ""
    echo "❌ ISO 生成失败"
fi
