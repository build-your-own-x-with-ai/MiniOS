#include "types.h"
#include "idt.h"
#include "vga.h"
#include "process.h"

extern void schedule(void);
extern void cpu_halt(void);

// 异常名称
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

// 输出十六进制数
static void print_hex(uint32_t num) {
    terminal_writestring("0x");
    char hex[9];
    for (int i = 7; i >= 0; i--) {
        int digit = (num >> (i * 4)) & 0xF;
        hex[7-i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
    }
    hex[8] = '\0';
    terminal_writestring(hex);
}

// 通用异常处理函数
void exception_handler(struct interrupt_frame* frame) {
    terminal_writestring("\n*** EXCEPTION ***\n");
    
    if (frame->int_no < 32) {
        terminal_writestring("Exception: ");
        terminal_writestring(exception_messages[frame->int_no]);
        terminal_writestring("\n");
    } else {
        terminal_writestring("Unknown Exception\n");
    }
    
    terminal_writestring("Error Code: ");
    print_hex(frame->err_code);
    terminal_writestring("\n");
    
    terminal_writestring("EIP: ");
    print_hex(frame->eip);
    terminal_writestring("\n");
    
    terminal_writestring("CS: ");
    print_hex(frame->cs);
    terminal_writestring("\n");
    
    terminal_writestring("EFLAGS: ");
    print_hex(frame->eflags);
    terminal_writestring("\n");
    
    // 获取当前进程
    struct process* current = get_current_process();
    if (current) {
        terminal_writestring("Process PID: ");
        char buf[16];
        uint32_t pid = current->pid;
        int i = 0;
        if (pid == 0) {
            buf[i++] = '0';
        } else {
            int temp = pid;
            int digits = 0;
            while (temp > 0) {
                temp /= 10;
                digits++;
            }
            for (int j = digits - 1; j >= 0; j--) {
                buf[j] = '0' + (pid % 10);
                pid /= 10;
            }
            i = digits;
        }
        buf[i] = '\0';
        terminal_writestring(buf);
        terminal_writestring("\n");
        
        // 终止进程
        terminal_writestring("Terminating process...\n");
        current->state = PROCESS_TERMINATED;
        destroy_process(current);
        
        // 触发调度
        schedule();
    } else {
        // 内核异常，停止系统
        terminal_writestring("Kernel exception! System halted.\n");
        cpu_halt();
    }
}

void exceptions_init(void) {
    // 注册异常处理函数
    for (int i = 0; i < 20; i++) {
        if (i != 14) {  // 页错误已经单独处理
            register_interrupt_handler(i, exception_handler);
        }
    }
}
