#include "types.h"
#include "idt.h"
#include "vga.h"

extern void cpu_halt(void);
extern uint32_t read_cr2(void);

// 页错误处理函数
void page_fault_handler(struct interrupt_frame* frame) {
    // 获取导致页错误的地址
    uint32_t faulting_address = read_cr2();
    
    // 分析错误码
    int present = frame->err_code & 0x1;    // 页不存在
    int rw = frame->err_code & 0x2;         // 写操作
    int us = frame->err_code & 0x4;         // 用户模式
    int reserved = frame->err_code & 0x8;   // 覆盖保留位
    int id = frame->err_code & 0x10;        // 指令获取
    
    terminal_writestring("\n*** PAGE FAULT ***\n");
    terminal_writestring("Faulting address: 0x");
    
    // 输出地址（十六进制）
    char hex[9];
    for (int i = 7; i >= 0; i--) {
        int digit = (faulting_address >> (i * 4)) & 0xF;
        hex[7-i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
    }
    hex[8] = '\0';
    terminal_writestring(hex);
    terminal_writestring("\n");
    
    terminal_writestring("Error: ");
    if (!present) terminal_writestring("Page not present ");
    if (rw) terminal_writestring("Write operation ");
    if (us) terminal_writestring("User mode ");
    if (reserved) terminal_writestring("Reserved bit set ");
    if (id) terminal_writestring("Instruction fetch ");
    terminal_writestring("\n");
    
    terminal_writestring("EIP: 0x");
    for (int i = 7; i >= 0; i--) {
        int digit = (frame->eip >> (i * 4)) & 0xF;
        hex[7-i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
    }
    hex[8] = '\0';
    terminal_writestring(hex);
    terminal_writestring("\n");
    
    terminal_writestring("System halted.\n");
    
    // 停止系统
    cpu_halt();
}

void page_fault_init(void) {
    register_interrupt_handler(14, page_fault_handler);
}
