# VirtualBox 设置指南

## 问题诊断

如果你看到 UEFI Interactive Shell 而不是 Mini OS，说明虚拟机没有正确从 ISO 启动。

## 正确的 VirtualBox 设置

### 1. 创建虚拟机

1. 打开 VirtualBox
2. 点击 "新建" (New)
3. 设置：
   - **名称**: Mini OS
   - **类型**: Linux
   - **版本**: Other Linux (32-bit) ⚠️ 必须是 32-bit！
   - **内存**: 128 MB（最小）
   - **硬盘**: 不创建虚拟硬盘

### 2. 关键设置（重要！）

#### 系统设置
1. 选择虚拟机 -> 设置 (Settings)
2. 系统 (System) -> 主板 (Motherboard)：
   - ✅ **启用 EFI**: 取消勾选！（重要）
   - ✅ **启动顺序**: 光驱在最前面
   - ✅ **芯片组**: PIIX3
   - ✅ **指点设备**: PS/2 鼠标

3. 系统 (System) -> 处理器 (Processor)：
   - CPU: 1 个即可
   - ✅ **启用 PAE/NX**: 可以勾选

#### 存储设置
1. 存储 (Storage)
2. 控制器: IDE
3. 点击 "添加光驱" 或选择现有的空光驱
4. 点击光盘图标 -> 选择虚拟光盘文件
5. 浏览并选择 `mini-os.iso`
6. ✅ 确保 "Live CD/DVD" 已勾选

#### 显示设置
1. 显示 (Display)
2. 屏幕 (Screen)：
   - 显存: 16 MB
   - ✅ **启用 3D 加速**: 取消勾选

### 3. 启动虚拟机

1. 点击 "启动" (Start)
2. 你应该看到：
   - GRUB 菜单（可能很快闪过）
   - 然后是黑屏，白色文字
   - 显示 "Mini OS Starting..."
   - 显示初始化信息

### 4. 预期输出

成功启动后，你应该看到类似这样的输出：

```
Mini OS Starting...
Multiboot magic verified
Memory detected: XXX MB
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

## 常见问题

### 问题 1: 看到 UEFI Shell
**原因**: 启用了 EFI
**解决**: 在系统设置中取消勾选 "启用 EFI"

### 问题 2: 黑屏无输出
**原因**: 可能是显示设置问题
**解决**: 
- 取消 3D 加速
- 增加显存到 16MB
- 确保选择了 32-bit 版本

### 问题 3: "Operating System not found"
**原因**: ISO 没有正确加载
**解决**:
- 确认 ISO 文件路径正确
- 确认启动顺序中光驱在第一位
- 重新添加 ISO 文件

### 问题 4: 虚拟机立即关闭
**原因**: 可能是内核 panic
**解决**:
- 检查是否选择了 32-bit 版本
- 查看虚拟机日志

## 使用 QEMU 测试（替代方案）

如果 VirtualBox 有问题，可以使用 QEMU：

### 方法 1: 安装 QEMU
```bash
brew install qemu
qemu-system-i386 -cdrom mini-os.iso -m 128M
```

### 方法 2: 使用 Docker
```bash
./test-qemu.sh
```

## 调试技巧

### 查看虚拟机日志
1. 虚拟机 -> 设置 -> 系统
2. 启用串口
3. 查看日志输出

### 使用 QEMU 调试模式
```bash
qemu-system-i386 \
    -cdrom mini-os.iso \
    -m 128M \
    -serial stdio \
    -d int,cpu_reset
```

## 成功标志

如果看到以下内容，说明启动成功：
- ✅ "Mini OS Starting..."
- ✅ "Multiboot magic verified"
- ✅ "Initializing..." 系列消息
- ✅ "Mini OS initialized successfully!"
- ✅ "System ready."

## 需要帮助？

如果仍然有问题：
1. 截图当前显示的内容
2. 检查虚拟机设置（特别是 EFI 选项）
3. 尝试使用 QEMU 测试
4. 查看 VirtualBox 日志文件
