#include "types.h"
#include "panic.h"
#include "vga.h"

extern void cpu_halt(void);

void kernel_panic(const char* message) {
    // 显示 panic 消息
    terminal_writestring("\n\n");
    terminal_writestring("*** KERNEL PANIC ***\n");
    terminal_writestring(message);
    terminal_writestring("\n");
    terminal_writestring("System halted.\n");
    
    // 停止系统
    cpu_halt();
}
