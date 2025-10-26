# 构建说明

## macOS 构建问题

由于 macOS 的工具链限制：
1. Clang 编译器不完全支持 32 位 x86 汇编指令
2. 系统链接器 (ld) 不支持 32 位 x86 目标

你会看到错误：`ld: unknown -arch name: armv4t`

### 解决方案

有三种方法解决这个问题：

#### 方案 1: 使用交叉编译工具链（推荐）

安装 i386-elf-gcc 交叉编译工具链：

```bash
# 方法 A: 使用 Homebrew tap
brew tap nativeos/i386-elf-toolchain
brew install i386-elf-binutils i386-elf-gcc

# 方法 B: 从源码编译
# 参考 OSDev Wiki: https://wiki.osdev.org/GCC_Cross-Compiler
```

然后修改 Makefile：
```makefile
CC = i386-elf-gcc
LD = i386-elf-ld
```

#### 方案 2: 使用 Docker（最简单）

我已经创建了一个构建脚本：

```bash
# 确保 Docker 正在运行
./build-docker.sh
```

或者手动运行：

```bash
docker run --rm -v "$(pwd):/work" -w /work \
    randomdude/gcc-cross-x86_64-elf \
    bash -c "apt-get update && apt-get install -y nasm && make clean && make"
```

#### 方案 3: 在 Linux 虚拟机中构建

在 VirtualBox 或 VMware 中安装 Ubuntu，然后：

```bash
sudo apt-get install build-essential nasm grub-pc-bin xorriso qemu-system-x86
make
```

## 当前代码状态

代码已经针对 macOS Clang 的限制进行了修改：

1. **特权指令**: `hlt`, `cli`, `sti` 等指令已移到汇编文件 (`kernel/halt.asm`)
2. **I/O 端口操作**: `in`, `out` 指令已移到汇编文件 (`kernel/io.asm`)
3. **控制寄存器**: CR0, CR2, CR3 访问已移到汇编文件
4. **系统调用**: 用户程序的 `int 0x80` 已移到汇编文件 (`userland/syscall.asm`)

## 编译错误参考

如果遇到以下错误：

### 错误 1: `invalid instruction: hlt`
**原因**: Clang 不支持特权指令  
**解决**: 已修复，使用汇编函数 `cpu_halt()`

### 错误 2: `invalid output constraint '=a'`
**原因**: Clang 的内联汇编约束不兼容  
**解决**: 已修复，使用独立的汇编文件

### 错误 3: `ld: unknown -arch name`
**原因**: macOS 的 ld 不支持 32 位 x86  
**解决**: 需要使用 i386-elf-ld 交叉链接器

## 推荐的开发环境

1. **Linux (Ubuntu/Debian)**: 最佳选择，原生支持
2. **Docker**: 跨平台，隔离环境
3. **macOS + 交叉编译工具链**: 可行但需要额外设置
4. **Windows + WSL2**: 通过 Linux 子系统

## 测试

即使无法在 macOS 上编译，你仍然可以：

1. 在 Linux 虚拟机中编译
2. 使用 Docker 编译
3. 在 VirtualBox 中测试生成的 ISO

## 参考资源

- [OSDev Wiki - GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)
- [OSDev Wiki - Building GCC](https://wiki.osdev.org/Building_GCC)
- [Docker OS Dev Image](https://hub.docker.com/r/randomdude/gcc-cross-x86_64-elf)
