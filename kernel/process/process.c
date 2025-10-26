#include "types.h"
#include "process.h"
#include "vmm.h"
#include "pmm.h"

#define USER_STACK_SIZE 0x4000    // 16KB
#define KERNEL_STACK_SIZE 0x4000  // 16KB
#define USER_HEAP_START 0x20000000

static struct process* current_process = NULL;
static struct process* process_list = NULL;
static uint32_t next_pid = 1;

void process_init(void) {
    current_process = NULL;
    process_list = NULL;
    next_pid = 1;
}

struct process* create_process(void* entry_point) {
    // 分配进程控制块
    struct process* proc = (struct process*)kmalloc(sizeof(struct process));
    if (!proc) {
        return NULL;
    }
    
    // 设置 PID
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    
    // 创建页目录
    proc->page_dir = create_page_directory();
    if (!proc->page_dir) {
        kfree(proc);
        return NULL;
    }
    
    // 分配内核栈
    proc->kernel_stack = alloc_page();
    if (!proc->kernel_stack) {
        kfree(proc);
        return NULL;
    }
    
    // 分配用户栈
    void* user_stack_phys = alloc_page();
    if (!user_stack_phys) {
        free_page(proc->kernel_stack);
        kfree(proc);
        return NULL;
    }
    
    // 映射用户栈到虚拟地址空间
    uint32_t user_stack_virt = 0x40000000 - USER_STACK_SIZE;
    map_page(proc->page_dir, user_stack_virt, (uint32_t)user_stack_phys, 
             PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    proc->user_stack = (void*)user_stack_virt;
    
    // 设置堆
    proc->heap_start = USER_HEAP_START;
    proc->heap_end = USER_HEAP_START;
    
    // 初始化 CPU 上下文
    proc->context.eax = 0;
    proc->context.ebx = 0;
    proc->context.ecx = 0;
    proc->context.edx = 0;
    proc->context.esi = 0;
    proc->context.edi = 0;
    proc->context.ebp = 0;
    proc->context.esp = user_stack_virt + USER_STACK_SIZE - 4;
    proc->context.eip = (uint32_t)entry_point;
    proc->context.eflags = 0x202;  // IF=1 (中断使能)
    proc->context.cr3 = (uint32_t)&proc->page_dir->entries;
    
    // 添加到进程列表
    proc->next = process_list;
    process_list = proc;
    
    return proc;
}

void destroy_process(struct process* proc) {
    if (!proc) {
        return;
    }
    
    // 从进程列表中移除
    if (process_list == proc) {
        process_list = proc->next;
    } else {
        struct process* p = process_list;
        while (p && p->next != proc) {
            p = p->next;
        }
        if (p) {
            p->next = proc->next;
        }
    }
    
    // 释放用户栈
    if (proc->user_stack) {
        uint32_t user_stack_virt = (uint32_t)proc->user_stack;
        // 获取物理地址并释放
        // 简化实现：直接释放
        free_page(proc->user_stack);
    }
    
    // 释放内核栈
    if (proc->kernel_stack) {
        free_page(proc->kernel_stack);
    }
    
    // 释放页目录（简化实现）
    // 实际应该遍历页表并释放所有页
    
    // 释放进程控制块
    kfree(proc);
}

struct process* get_current_process(void) {
    return current_process;
}

void set_current_process(struct process* proc) {
    current_process = proc;
}

struct process* get_process_list(void) {
    return process_list;
}
