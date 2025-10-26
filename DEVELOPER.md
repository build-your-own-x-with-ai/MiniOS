# Mini OS 开发者文档

本文档面向希望理解、修改或扩展 Mini OS 的开发者。

## 代码结构

### 启动流程

1. **BIOS/UEFI** 加载 GRUB
2. **GRUB** 读取 `grub.cfg`，加载内核到内存（1MB 地址）
3. **boot.asm** 设置栈，跳转到 C 入口
4. **kernel_main()** 初始化各个子系统：
   - GDT（全局描述符表）
   - IDT（中断描述符表）
   - 物理内存管理器
   - 虚拟内存管理器
   - 异常处理
   - 进程管理
   - 定时器
   - 系统调用
   - 键盘驱动

### 关键组件

#### 1. 内存管理

**物理内存管理器 (PMM)**
- 文件: `kernel/memory/pmm.c`
- 使用位图管理 4KB 页
- 函数:
  - `pmm_init()`: 初始化，标记已用内存
  - `alloc_page()`: 分配一个物理页
  - `free_page()`: 释放一个物理页

**虚拟内存管理器 (VMM)**
- 文件: `kernel/memory/vmm.c`
- 管理页目录和页表
- 函数:
  - `vmm_init()`: 创建内核页目录，启用分页
  - `create_page_directory()`: 为进程创建页目录
  - `map_page()`: 映射虚拟地址到物理地址
  - `switch_page_directory()`: 切换页目录（CR3）

**内核堆分配器**
- 简单的线性分配器
- `kmalloc()`: 分配内核内存
- `kfree()`: 释放内存（当前实现为空操作）

#### 2. 进程管理

**进程控制块 (PCB)**
```c
struct process {
    uint32_t pid;                    // 进程 ID
    enum process_state state;        // 状态
    struct cpu_context context;      // CPU 上下文
    struct page_directory* page_dir; // 页目录
    void* kernel_stack;              // 内核栈
    void* user_stack;                // 用户栈
    uint32_t heap_start;             // 堆起始地址
    uint32_t heap_end;               // 堆结束地址
    struct process* next;            // 链表指针
};
```

**进程状态**
- `PROCESS_READY`: 就绪
- `PROCESS_RUNNING`: 运行中
- `PROCESS_BLOCKED`: 阻塞
- `PROCESS_TERMINATED`: 已终止

**调度器**
- 文件: `kernel/process/scheduler.c`
- 算法: 轮转调度（Round-Robin）
- 时间片: 100ms（由定时器触发）

#### 3. 中断处理

**中断描述符表 (IDT)**
- 文件: `kernel/interrupt/idt.c`
- 256 个表项
- 类型:
  - 0-31: CPU 异常
  - 32-47: 硬件中断（IRQ）
  - 128: 系统调用

**中断服务例程 (ISR)**
- 文件: `kernel/interrupt/isr.asm`
- 保存/恢复寄存器
- 调用 C 处理函数

**PIC 重映射**
- 主 PIC: IRQ 0-7 → INT 32-39
- 从 PIC: IRQ 8-15 → INT 40-47

#### 4. 系统调用

**调用约定**
- 中断: `INT 0x80`
- 参数传递:
  - EAX: 系统调用号
  - EBX: 参数 1
  - ECX: 参数 2
  - EDX: 参数 3
- 返回值: EAX

**实现的系统调用**
- `SYS_EXIT (1)`: 退出进程
- `SYS_WRITE (2)`: 写入输出
- `SYS_READ (3)`: 读取输入
- `SYS_SBRK (4)`: 调整堆大小

#### 5. ELF 加载器

**加载流程**
1. 验证 ELF 魔数 (`0x7F 'E' 'L' 'F'`)
2. 检查架构（x86）和类型（可执行）
3. 遍历程序头表
4. 为每个 `PT_LOAD` 段：
   - 分配物理页
   - 映射到虚拟地址
   - 复制段数据
   - 清零 BSS 段
5. 返回入口点地址

#### 6. 设备驱动

**VGA 文本模式**
- 缓冲区: `0xB8000`
- 大小: 80x25 字符
- 格式: [字符][属性]
- 功能: 输出、滚动、清屏

**键盘**
- 端口: 0x60（数据）、0x64（状态）
- IRQ: 1
- 扫描码转 ASCII
- 环形缓冲区

**定时器 (PIT)**
- 频率: 100Hz
- IRQ: 0
- 用途: 进程调度

## 添加新功能

### 添加新的系统调用

1. 在 `include/syscall.h` 中定义系统调用号：
```c
#define SYS_MYNEWCALL 5
```

2. 在 `kernel/interrupt/syscall.c` 中实现：
```c
int32_t sys_mynewcall(int32_t arg1) {
    // 实现逻辑
    return 0;
}
```

3. 在 `syscall_handler()` 中添加 case：
```c
case SYS_MYNEWCALL:
    result = sys_mynewcall((int32_t)arg1);
    break;
```

4. 在 `userland/lib.c` 中添加包装函数：
```c
int mynewcall(int arg) {
    return syscall1(SYS_MYNEWCALL, arg);
}
```

### 添加新的设备驱动

1. 创建头文件 `include/mydevice.h`
2. 创建实现文件 `kernel/drivers/mydevice.c`
3. 实现初始化函数 `mydevice_init()`
4. 如果需要中断，注册处理函数：
```c
register_interrupt_handler(IRQ_NUM, mydevice_handler);
```
5. 在 `kernel_main()` 中调用初始化函数

### 添加新的异常处理

在 `kernel/interrupt/exceptions.c` 中注册处理函数：
```c
register_interrupt_handler(EXCEPTION_NUM, my_exception_handler);
```

## 调试技巧

### 使用 QEMU 调试

```bash
# 启动 QEMU 并等待 GDB 连接
qemu-system-i386 -cdrom mini-os.iso -s -S

# 在另一个终端启动 GDB
gdb build/kernel.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

### 串口输出

添加串口输出用于调试（端口 0x3F8）：
```c
void serial_putchar(char c) {
    while ((inb(0x3FD) & 0x20) == 0);
    outb(0x3F8, c);
}
```

### 内核日志

使用不同的颜色区分日志级别：
```c
terminal_setcolor(VGA_COLOR_RED);  // 错误
terminal_writestring("ERROR: ...\n");
terminal_setcolor(VGA_COLOR_LIGHT_GREY);  // 恢复
```

## 常见问题

### Q: 如何增加内核堆大小？

修改 `kernel/memory/vmm.c` 中的 `heap_start` 和内存映射。

### Q: 如何支持更多进程？

当前实现使用简单的链表。可以改用更高效的数据结构（如红黑树）。

### Q: 如何实现文件系统？

1. 选择文件系统类型（FAT、ext2 等）
2. 实现块设备驱动（IDE/SATA）
3. 实现文件系统解析和操作
4. 添加文件相关系统调用（open、read、write、close）

### Q: 如何支持多核？

需要实现：
- APIC（高级可编程中断控制器）
- SMP（对称多处理）初始化
- 每个 CPU 的数据结构
- 自旋锁和其他同步原语

## 性能优化

### 内存分配

当前的 `kmalloc` 是线性分配器，效率低。可以实现：
- Slab 分配器
- Buddy 系统
- 内存池

### 调度器

可以实现更复杂的调度算法：
- 优先级调度
- 多级反馈队列
- 完全公平调度器（CFS）

### 页表

使用 TLB（转换后备缓冲器）优化：
- 减少页表切换
- 使用大页（2MB/4MB）

## 测试

### 单元测试

为关键函数编写测试：
```c
void test_pmm() {
    void* page1 = alloc_page();
    void* page2 = alloc_page();
    assert(page1 != page2);
    free_page(page1);
    void* page3 = alloc_page();
    assert(page3 == page1);  // 应该重用
}
```

### 集成测试

编写用户程序测试系统功能：
- 内存分配和释放
- 进程创建和销毁
- 系统调用正确性
- 异常处理

## 代码风格

- 使用 4 空格缩进
- 函数名使用小写加下划线
- 结构体名使用小写加下划线
- 宏和常量使用大写加下划线
- 注释使用 C 风格 `/* */` 或 C++ 风格 `//`

## 贡献指南

1. Fork 项目
2. 创建特性分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 参考资源

### 书籍
- "Operating Systems: Three Easy Pieces" by Remzi H. Arpaci-Dusseau
- "Operating System Concepts" by Silberschatz, Galvin, Gagne
- "Modern Operating Systems" by Andrew S. Tanenbaum

### 在线资源
- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Linux Kernel Source](https://github.com/torvalds/linux)

### 社区
- [OSDev Forum](https://forum.osdev.org/)
- [Reddit r/osdev](https://www.reddit.com/r/osdev/)
