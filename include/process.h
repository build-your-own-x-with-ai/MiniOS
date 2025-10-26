#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"
#include "vmm.h"

// 进程状态
enum process_state {
    PROCESS_READY = 0,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
};

// CPU 上下文
struct cpu_context {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip;
    uint32_t eflags;
    uint32_t cr3;
} __attribute__((packed));

// 进程控制块
struct process {
    uint32_t pid;
    enum process_state state;
    struct cpu_context context;
    struct page_directory* page_dir;
    void* kernel_stack;
    void* user_stack;
    uint32_t heap_start;
    uint32_t heap_end;
    struct process* next;
};

// 进程管理函数
void process_init(void);
struct process* create_process(void* entry_point);
void destroy_process(struct process* proc);
struct process* get_current_process(void);
void set_current_process(struct process* proc);

#endif
