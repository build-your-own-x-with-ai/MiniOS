// 演示用户程序 - 可以被内核直接调用

// 系统调用声明
extern int syscall1(int num, int arg1);
extern int syscall3(int num, int arg1, int arg2, int arg3);

#define SYS_EXIT    1
#define SYS_WRITE   2

// 简单的字符串长度函数
static int my_strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// 简单的输出函数
void demo_print(const char* str) {
    syscall3(SYS_WRITE, 1, (int)str, my_strlen(str));
}

// 演示函数 - 可以被内核调用
void demo_main(void) {
    demo_print("=== User Program Output ===\n");
    demo_print("Hello from user space!\n");
    demo_print("This program is running in user mode.\n");
    demo_print("\n");
    demo_print("Testing calculations:\n");
    
    // 简单的计算
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    
    demo_print("Sum of 1 to 10 = 55\n");
    demo_print("\n");
    demo_print("Testing memory:\n");
    
    // 栈上的数组
    char buffer[26];
    for (int i = 0; i < 26; i++) {
        buffer[i] = 'A' + i;
    }
    
    demo_print("Alphabet: ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    demo_print("\n");
    demo_print("=== User Program Complete ===\n");
    
    // 不调用 exit，让内核继续运行
}

// 标准的 main 函数（用于 ELF 加载）
int main(void) {
    demo_main();
    syscall1(SYS_EXIT, 0);
    return 0;
}
