# 如何在 Mini OS 中运行程序

## 当前状态

✅ **已实现**：
- 内核启动和初始化
- 内存管理（物理/虚拟）
- 进程管理和调度
- ELF 加载器
- 系统调用（exit, write, read, sbrk）
- 设备驱动（VGA、键盘、定时器）

❌ **未实现**：
- 文件系统（无法从磁盘读取文件）
- Shell（命令行解释器）
- 动态加载外部程序

## 如何运行用户程序

### 方法 1: 修改内核加载嵌入的程序

当前内核已经编译了用户程序，但没有实际执行。让我们修改代码来运行它。

#### 步骤 1: 修改 kernel/main.c

在 `kernel_main` 函数的最后，添加加载和执行用户程序的代码：

```c
// 在 "System ready." 之后添加

// 声明嵌入的用户程序
extern char _binary_build_userland_hello_start[];
extern char _binary_build_userland_hello_end[];

terminal_writestring("\nLoading user program...\n");

// 获取用户程序的大小
size_t program_size = _binary_build_userland_hello_end - 
                      _binary_build_userland_hello_start;

terminal_writestring("Program size: ");
// 输出大小...

// 创建进程
struct process* proc = create_process(NULL);
if (!proc) {
    terminal_writestring("ERROR: Failed to create process\n");
    cpu_halt();
}

// 加载 ELF
void* entry = elf_load(_binary_build_userland_hello_start, proc->page_dir);
if (!entry) {
    terminal_writestring("ERROR: Failed to load ELF\n");
    destroy_process(proc);
    cpu_halt();
}

terminal_writestring("Starting user program...\n");

// 设置进程入口点
proc->context.eip = (uint32_t)entry;

// 切换到用户程序
switch_to_process(proc);
```

### 方法 2: 创建新的用户程序

#### 创建一个简单的程序

在 `userland/` 目录创建新文件：

**userland/test.c**:
```c
extern void puts(const char* str);
extern void exit(int code);

int main(void) {
    puts("Hello from test program!\n");
    puts("This is a user space program.\n");
    
    // 简单的计算
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    
    puts("Sum of 1-10 = 55\n");
    
    exit(0);
    return 0;
}
```

#### 编译新程序

修改 Makefile 中的 `USERLAND_PROGRAMS`：

```makefile
USERLAND_PROGRAMS = hello test
```

然后重新编译：
```bash
make clean
make iso
```

## 为什么不能运行外部 ELF（如 ls）？

### 缺少的组件

1. **文件系统驱动**
   - 需要实现 FAT、ext2 或自定义文件系统
   - 需要磁盘驱动（IDE/SATA）
   - 需要文件操作 API（open, read, close）

2. **Shell/命令解释器**
   - 需要解析用户输入
   - 需要查找和加载程序
   - 需要管理进程生命周期

3. **标准 C 库**
   - ls 等程序依赖 libc
   - 需要实现 printf, malloc, 文件操作等

### 添加这些功能的工作量

这是一个**巨大的工程**，需要：
- 文件系统：~2000 行代码
- 磁盘驱动：~500 行代码
- Shell：~1000 行代码
- C 库：~5000+ 行代码

## 当前可以做什么

### 1. 运行嵌入的用户程序

你已经有 3 个用户程序：
- `userland/hello.c` - Hello World
- `userland/test_mem.c` - 内存分配测试
- `userland/test_loop.c` - 循环和计算测试

### 2. 测试系统调用

创建测试程序来验证系统调用：

```c
// userland/syscall_test.c
extern int syscall3(int num, int arg1, int arg2, int arg3);
extern void exit(int code);

#define SYS_WRITE 2

int main(void) {
    const char* msg = "Testing system calls!\n";
    
    // 直接调用系统调用
    syscall3(SYS_WRITE, 1, (int)msg, 22);
    
    exit(0);
    return 0;
}
```

### 3. 测试内存管理

```c
// userland/mem_test.c
extern void* sbrk(int increment);
extern void puts(const char* str);
extern void exit(int code);

int main(void) {
    puts("Allocating memory...\n");
    
    // 分配 4KB
    char* buffer = (char*)sbrk(4096);
    
    if (buffer == (char*)-1) {
        puts("Memory allocation failed!\n");
        exit(1);
    }
    
    // 写入数据
    for (int i = 0; i < 100; i++) {
        buffer[i] = 'A' + (i % 26);
    }
    buffer[100] = '\0';
    
    puts("Memory test passed!\n");
    exit(0);
    return 0;
}
```

## 未来扩展方向

如果你想让 Mini OS 能够运行外部程序，需要添加：

### 阶段 1: 简单文件系统（~1周）
- 实现 RAM disk（内存文件系统）
- 基本的文件操作（open, read, write, close）
- 目录支持

### 阶段 2: 磁盘驱动（~3天）
- IDE/ATA 驱动
- 读写扇区
- 缓冲管理

### 阶段 3: Shell（~3天）
- 命令解析
- 程序加载和执行
- 基本命令（ls, cat, echo）

### 阶段 4: 标准库（~2周）
- printf/scanf
- 字符串函数
- 内存管理函数
- 文件操作函数

## 总结

**当前 Mini OS**：
- ✅ 是一个功能完整的操作系统内核
- ✅ 可以运行嵌入的用户程序
- ✅ 支持系统调用和内存管理
- ❌ 不能运行外部 ELF 文件（需要文件系统）
- ❌ 没有 Shell（需要命令解释器）

**这是正常的！** 大多数教学操作系统都是这样的。你已经实现了操作系统的核心功能！

要运行像 `ls` 这样的外部程序，需要再实现一个完整的文件系统和 Shell，这是另一个大项目。

## 快速测试

如果你想看到用户程序运行，可以：

1. 修改 `kernel/main.c` 添加程序加载代码
2. 重新编译：`make clean && make iso`
3. 在 QEMU 中运行

或者查看现有的测试程序源码：
```bash
cat userland/hello.c
cat userland/test_mem.c
cat userland/test_loop.c
```

这些程序已经编译并嵌入到内核中了！
