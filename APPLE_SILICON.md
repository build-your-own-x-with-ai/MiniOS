# 在 Apple Silicon (M4) 上运行 Mini OS

## 重要说明

⚠️ **VirtualBox 限制**: VirtualBox 在 Apple Silicon Mac 上只支持 ARM 64-bit 虚拟机，**不支持 x86 架构**。

✅ **解决方案**: 使用 QEMU 或 UTM，它们支持跨架构模拟。

## 推荐方法

### 方法 1: QEMU（最简单，命令行）

#### 安装
```bash
brew install qemu
```

#### 运行 Mini OS
```bash
# 基本运行
qemu-system-i386 -cdrom mini-os.iso -m 128M

# 带图形界面
qemu-system-i386 -cdrom mini-os.iso -m 128M -vga std

# 全屏模式
qemu-system-i386 -cdrom mini-os.iso -m 128M -vga std -full-screen
```

#### QEMU 快捷键
- `Ctrl + Alt + G`: 释放鼠标
- `Ctrl + Alt + F`: 全屏切换
- `Ctrl + Alt + Q`: 退出

### 方法 2: UTM（推荐，图形界面）

UTM 是 macOS 原生应用，基于 QEMU，但提供了友好的图形界面。

#### 安装
1. 访问 https://mac.getutm.app/
2. 下载并安装 UTM
3. 或使用 Homebrew: `brew install --cask utm`

#### 配置步骤
1. 打开 UTM
2. 点击 "+" 创建新虚拟机
3. 选择 "Emulate" (模拟)
4. 操作系统选择 "Other"
5. 架构选择 "x86_64" 或 "i386"
6. 内存设置为 128 MB
7. 不创建硬盘
8. 在 "Drives" 中添加 CD/DVD
9. 选择 mini-os.iso
10. 保存并启动

#### UTM 优势
- ✅ 原生 macOS 应用
- ✅ 图形界面友好
- ✅ 支持快照和暂停
- ✅ 性能优化

### 方法 3: Docker + QEMU

如果你已经安装了 Docker：

```bash
./test-qemu.sh
```

或手动运行：
```bash
docker run --rm -it \
    --platform linux/amd64 \
    -v $(pwd):/work \
    tianon/qemu \
    qemu-system-i386 -cdrom /work/mini-os.iso -m 128M
```

## 性能说明

### QEMU 模拟性能
- Apple Silicon 需要模拟 x86 架构
- 性能会比原生慢，但对于 Mini OS 这样的小型系统完全够用
- 启动和运行都很流畅

### 优化建议
```bash
# 使用 KVM 加速（如果可用）
qemu-system-i386 -cdrom mini-os.iso -m 128M -enable-kvm

# 增加 CPU 核心
qemu-system-i386 -cdrom mini-os.iso -m 128M -smp 2

# 使用更好的显示
qemu-system-i386 -cdrom mini-os.iso -m 128M -vga virtio
```

## 预期输出

成功启动后，你应该看到：

```
Mini OS Starting...
Multiboot magic verified
Memory detected: 128 MB
Initializing GDT...
Initializing IDT...
Initializing PMM...
Initializing VMM...
Initializing process management...
Initializing timer...
Initializing syscalls...
Initializing keyboard...

Mini OS initialized successfully!
System ready.

Testing system calls...
System call test: write
```

## 故障排除

### 问题 1: QEMU 安装失败
```bash
# 更新 Homebrew
brew update
brew upgrade

# 重新安装
brew reinstall qemu
```

### 问题 2: 黑屏无输出
```bash
# 尝试不同的显示选项
qemu-system-i386 -cdrom mini-os.iso -m 128M -vga cirrus
qemu-system-i386 -cdrom mini-os.iso -m 128M -vga std
qemu-system-i386 -cdrom mini-os.iso -m 128M -display cocoa
```

### 问题 3: 性能太慢
- 这是正常的，因为需要模拟 x86 架构
- Mini OS 很小，应该还是很快的
- 可以尝试增加内存: `-m 256M`

### 问题 4: 无法启动
```bash
# 检查 ISO 文件
file mini-os.iso

# 使用调试模式
qemu-system-i386 -cdrom mini-os.iso -m 128M -d int,cpu_reset
```

## 为什么不能用 VirtualBox？

VirtualBox 在 Apple Silicon 上的限制：
- ❌ 不支持 x86/x86_64 虚拟机
- ✅ 只支持 ARM 64-bit 虚拟机
- 这是 VirtualBox 的设计限制，不是 bug

## 替代方案对比

| 方案 | 优点 | 缺点 | 推荐度 |
|------|------|------|--------|
| QEMU | 免费、轻量、命令行 | 需要记命令 | ⭐⭐⭐⭐ |
| UTM | 图形界面、易用 | 需要下载 | ⭐⭐⭐⭐⭐ |
| Docker+QEMU | 隔离环境 | 需要 Docker | ⭐⭐⭐ |
| VirtualBox | - | 不支持 x86 | ❌ |

## 快速开始

最简单的方法：

```bash
# 1. 安装 QEMU
brew install qemu

# 2. 运行 Mini OS
qemu-system-i386 -cdrom mini-os.iso

# 就这么简单！
```

## 需要帮助？

如果遇到问题：
1. 确保 ISO 文件存在: `ls -lh mini-os.iso`
2. 检查 QEMU 版本: `qemu-system-i386 --version`
3. 尝试不同的显示选项
4. 查看 QEMU 日志输出

## 总结

在 Apple Silicon Mac 上运行 x86 操作系统：
- ✅ 使用 QEMU 或 UTM
- ❌ 不要使用 VirtualBox
- 🚀 性能足够运行 Mini OS
- 📚 这是学习操作系统的好方法！
