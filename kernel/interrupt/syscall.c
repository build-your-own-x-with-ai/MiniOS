#include "types.h"
#include "syscall.h"
#include "idt.h"
#include "vga.h"
#include "process.h"
#include "pmm.h"
#include "vmm.h"

extern void schedule(void);

// 前向声明系统调用实现
int32_t sys_exit(int32_t code);
int32_t sys_write(int32_t fd, const char* buf, uint32_t len);
int32_t sys_read(int32_t fd, char* buf, uint32_t len);
void* sys_sbrk(int32_t increment);

// 系统调用处理函数
void syscall_handler(struct interrupt_frame* frame) {
    uint32_t syscall_num = frame->eax;
    uint32_t arg1 = frame->ebx;
    uint32_t arg2 = frame->ecx;
    uint32_t arg3 = frame->edx;
    
    int32_t result = -1;
    
    switch (syscall_num) {
        case SYS_EXIT:
            result = sys_exit((int32_t)arg1);
            break;
            
        case SYS_WRITE:
            result = sys_write((int32_t)arg1, (const char*)arg2, arg3);
            break;
            
        case SYS_READ:
            result = sys_read((int32_t)arg1, (char*)arg2, arg3);
            break;
            
        case SYS_SBRK:
            result = (int32_t)sys_sbrk((int32_t)arg1);
            break;
            
        default:
            result = -1;  // 未知系统调用
            break;
    }
    
    // 将结果存放在 EAX 中
    frame->eax = result;
}

void syscall_init(void) {
    // 注册系统调用中断处理函数（INT 0x80 = 128）
    register_interrupt_handler(128, syscall_handler);
}

// 系统调用实现

int32_t sys_exit(int32_t code) {
    terminal_writestring("\nProcess exited with code: ");
    
    // 简单的数字输出
    if (code < 0) {
        terminal_putchar('-');
        code = -code;
    }
    
    char buf[16];
    int i = 0;
    if (code == 0) {
        buf[i++] = '0';
    } else {
        int temp = code;
        int digits = 0;
        while (temp > 0) {
            temp /= 10;
            digits++;
        }
        for (int j = digits - 1; j >= 0; j--) {
            buf[j] = '0' + (code % 10);
            code /= 10;
        }
        i = digits;
    }
    buf[i] = '\0';
    terminal_writestring(buf);
    terminal_putchar('\n');
    
    // 获取当前进程并标记为终止
    struct process* current = get_current_process();
    if (current) {
        current->state = PROCESS_TERMINATED;
        destroy_process(current);
    }
    
    // 触发调度切换到下一个进程
    schedule();
    
    return 0;
}

int32_t sys_write(int32_t fd, const char* buf, uint32_t len) {
    // 简单实现：只支持 stdout (fd=1)
    if (fd != 1) {
        return -1;
    }
    
    // 验证指针（简化实现）
    if (!buf) {
        return -1;
    }
    
    // 输出到终端
    terminal_write(buf, len);
    
    return (int32_t)len;
}

// 外部函数声明
extern char keyboard_getchar(void);
extern int keyboard_available(void);

int32_t sys_read(int32_t fd, char* buf, uint32_t len) {
    // 简单实现：只支持 stdin (fd=0)
    if (fd != 0) {
        return -1;
    }
    
    // 验证指针
    if (!buf || len == 0) {
        return -1;
    }
    
    uint32_t count = 0;
    
    // 从键盘缓冲区读取字符
    while (count < len && keyboard_available()) {
        char c = keyboard_getchar();
        if (c != 0) {
            buf[count++] = c;
            
            // 如果读到换行符，停止读取
            if (c == '\n') {
                break;
            }
        }
    }
    
    return (int32_t)count;
}

void* sys_sbrk(int32_t increment) {
    struct process* current = get_current_process();
    if (!current) {
        return (void*)-1;
    }
    
    uint32_t old_heap_end = current->heap_end;
    
    if (increment > 0) {
        // 扩展堆
        uint32_t new_heap_end = old_heap_end + increment;
        
        // 分配新页面
        uint32_t start_page = (old_heap_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        uint32_t end_page = (new_heap_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        
        for (uint32_t addr = start_page; addr < end_page; addr += PAGE_SIZE) {
            void* phys = alloc_page();
            if (!phys) {
                return (void*)-1;  // 内存不足
            }
            map_page(current->page_dir, addr, (uint32_t)phys, 
                    PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
        }
        
        current->heap_end = new_heap_end;
    } else if (increment < 0) {
        // 缩小堆（简化实现：不实际释放）
        current->heap_end = old_heap_end + increment;
        if (current->heap_end < current->heap_start) {
            current->heap_end = current->heap_start;
        }
    }
    
    return (void*)old_heap_end;
}
