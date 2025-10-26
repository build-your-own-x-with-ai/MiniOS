#include "types.h"
#include "multiboot.h"
#include "vga.h"

// 前向声明
void gdt_init(void);
void idt_init(void);
void pmm_init(uint32_t mem_size);
void vmm_init(void);
void page_fault_init(void);
void process_init(void);
void scheduler_init(void);
void timer_init(uint32_t frequency);
void syscall_init(void);
void keyboard_init(void);
void exceptions_init(void);
extern void cpu_halt(void);

// 用户程序演示
extern void run_user_demo(void);

// 全局变量：可用内存大小
uint32_t total_memory = 0;

void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
    // 初始化 VGA 终端
    terminal_init();
    
    terminal_writestring("Mini OS Starting...\n");
    
    // 验证 Multiboot 魔数
    if (magic != MULTIBOOT_MAGIC) {
        terminal_writestring("ERROR: Invalid Multiboot magic number!\n");
        cpu_halt();
    }
    
    terminal_writestring("Multiboot magic verified\n");
    
    // 获取内存信息
    if (mbi->flags & 0x1) {
        total_memory = (mbi->mem_lower + mbi->mem_upper) * 1024;
        terminal_writestring("Memory detected: ");
        // 简单的数字输出（MB）
        uint32_t mem_mb = total_memory / (1024 * 1024);
        char buf[16];
        int i = 0;
        if (mem_mb == 0) {
            buf[i++] = '0';
        } else {
            int temp = mem_mb;
            int digits = 0;
            while (temp > 0) {
                temp /= 10;
                digits++;
            }
            for (int j = digits - 1; j >= 0; j--) {
                buf[j] = '0' + (mem_mb % 10);
                mem_mb /= 10;
            }
            i = digits;
        }
        buf[i] = '\0';
        terminal_writestring(buf);
        terminal_writestring(" MB\n");
    }
    
    // 初始化 GDT
    terminal_writestring("Initializing GDT...\n");
    gdt_init();
    
    // 初始化 IDT
    terminal_writestring("Initializing IDT...\n");
    idt_init();
    
    // 初始化物理内存管理
    terminal_writestring("Initializing PMM...\n");
    pmm_init(total_memory);
    
    // 初始化虚拟内存管理
    terminal_writestring("Initializing VMM...\n");
    vmm_init();
    
    // 初始化异常处理
    exceptions_init();
    
    // 初始化页错误处理
    page_fault_init();
    
    // 初始化进程管理
    terminal_writestring("Initializing process management...\n");
    process_init();
    scheduler_init();
    
    // 初始化定时器（100Hz）
    terminal_writestring("Initializing timer...\n");
    timer_init(100);
    
    // 初始化系统调用
    terminal_writestring("Initializing syscalls...\n");
    syscall_init();
    
    // 初始化键盘
    terminal_writestring("Initializing keyboard...\n");
    keyboard_init();
    
    terminal_writestring("\nMini OS initialized successfully!\n");
    terminal_writestring("System ready.\n");
    
    // 运行用户程序演示
    run_user_demo();
    
    terminal_writestring("\n=== System Halted ===\n");
    terminal_writestring("Press Ctrl+C to exit QEMU\n");
    
    // 主循环
    cpu_halt();
}
