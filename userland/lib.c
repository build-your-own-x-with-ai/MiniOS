// 用户程序库 - 系统调用包装函数

typedef unsigned int size_t;
typedef int ssize_t;

// 系统调用号
#define SYS_EXIT    1
#define SYS_WRITE   2
#define SYS_READ    3
#define SYS_SBRK    4

// 系统调用接口（在 syscall.asm 中实现）
extern int syscall1(int num, int arg1);
extern int syscall3(int num, int arg1, int arg2, int arg3);

// 系统调用包装函数
void exit(int code) {
    syscall1(SYS_EXIT, code);
    while(1);  // 不应该到达这里
}

ssize_t write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, fd, (int)buf, (int)count);
}

ssize_t read(int fd, void* buf, size_t count) {
    return syscall3(SYS_READ, fd, (int)buf, (int)count);
}

void* sbrk(int increment) {
    return (void*)syscall1(SYS_SBRK, increment);
}

// 辅助函数
static size_t my_strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void puts(const char* str) {
    write(1, str, my_strlen(str));
}

// 用户程序入口点
extern int main(void);

void _start(void) {
    int ret = main();
    exit(ret);
}
