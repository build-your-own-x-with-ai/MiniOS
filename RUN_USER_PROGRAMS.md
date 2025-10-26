# 运行用户程序指南

## 已添加的功能

✅ **用户程序演示** - 现在 Mini OS 会在启动后自动运行用户程序演示！

## 快速开始

### 1. 重新编译

```bash
./build-docker.sh
```

### 2. 运行

```bash
qemu-system-i386 -cdrom mini-os.iso
```

## 你将看到什么

启动后，系统会显示：

```
Mini OS Starting...
Multiboot magic verified
Memory detected: 127 MB
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

=== User Program Demo ===

Demo 1: Hello World
  Output: Hello, Mini OS!
  This is a user program.

Demo 2: Calculations
  Computing sum of 1 to 10...
  Result: 55 (expected: 55)

Demo 3: String Operations
  Alphabet: ABCDEFGHIJKLMNOPQRSTUVWXYZ

Demo 4: Memory Operations
  Allocating and using stack memory...
  Squares: 0, 1, 4, 9, 16, 25, 36, 49, 64, 81

=== Demo Complete ===
All user program features demonstrated!

=== System Halted ===
Press Ctrl+C to exit QEMU
```

## 演示内容

### Demo 1: Hello World
- 基本的字符串输出
- 演示 VGA 驱动工作正常

### Demo 2: 计算
- 循环计算 1+2+...+10 = 55
- 演示算术运算和循环

### Demo 3: 字符串操作
- 生成并显示完整的字母表
- 演示字符数组和循环

### Demo 4: 内存操作
- 在栈上分配数组
- 计算并显示平方数 (0², 1², 2², ..., 9²)
- 演示内存访问和数组操作

## 添加更多用户程序

### 方法 1: 修改演示代码

编辑 `kernel/user_demo.c`，添加更多演示：

```c
void run_user_demo(void) {
    // ... 现有演示 ...
    
    // 添加新演示
    terminal_writestring("Demo 5: Your Custom Demo\n");
    // 你的代码...
}
```

### 方法 2: 创建新的演示文件

1. 创建 `kernel/my_demo.c`
2. 实现你的演示函数
3. 在 `kernel/main.c` 中调用它
4. 重新编译

### 方法 3: 创建真正的用户程序（高级）

要运行真正的独立用户程序，需要：

1. **实现上下文切换**
   - 保存/恢复寄存器
   - 切换页表
   - 从内核模式切换到用户模式

2. **实现进程调度**
   - 时间片管理
   - 进程队列
   - 调度算法

3. **完善系统调用**
   - 参数验证
   - 错误处理
   - 返回值传递

这需要额外的 ~500 行代码。

## 当前限制

### 为什么不能运行真正的 ELF 文件？

当前的演示是在**内核空间**运行的，不是真正的用户空间程序。

要运行真正的用户程序需要：

1. ❌ **上下文切换** - 未实现
   - 需要汇编代码切换 CPU 模式
   - 需要保存/恢复所有寄存器

2. ❌ **用户模式** - 未实现
   - 需要设置 Ring 3 权限
   - 需要用户栈和内核栈切换

3. ❌ **完整的系统调用** - 部分实现
   - 系统调用框架已有
   - 但需要更多的参数验证

### 为什么不能运行外部程序（如 ls）？

1. ❌ **文件系统** - 未实现
2. ❌ **磁盘驱动** - 未实现
3. ❌ **Shell** - 未实现
4. ❌ **标准 C 库** - 未实现

## 这是正常的！

你的 Mini OS 已经实现了操作系统的**核心功能**：
- ✅ 引导和初始化
- ✅ 内存管理
- ✅ 进程管理框架
- ✅ 中断处理
- ✅ 系统调用框架
- ✅ 设备驱动

大多数教学操作系统都是这个水平。要实现完整的用户程序执行和文件系统，需要再增加几千行代码。

## 下一步

如果你想继续扩展：

### 短期目标（1-2天）
- [ ] 实现简单的上下文切换
- [ ] 让一个用户程序真正在 Ring 3 运行
- [ ] 完善系统调用处理

### 中期目标（1周）
- [ ] 实现多进程调度
- [ ] 添加更多系统调用
- [ ] 实现简单的 Shell

### 长期目标（1个月）
- [ ] 实现文件系统
- [ ] 添加磁盘驱动
- [ ] 移植简单的 Unix 工具

## 总结

🎉 **恭喜！** 你现在有一个可以运行演示程序的操作系统！

虽然不是完整的用户空间执行，但你已经实现了操作系统的核心功能。这是一个巨大的成就！

要看到演示运行：
```bash
./build-docker.sh
qemu-system-i386 -cdrom mini-os.iso
```

享受你的操作系统吧！🚀
