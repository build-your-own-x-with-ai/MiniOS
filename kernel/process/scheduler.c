#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "vmm.h"

extern struct process* get_process_list(void);

void scheduler_init(void) {
    // 初始化调度器
}

void schedule(void) {
    struct process* current = get_current_process();
    struct process* next = NULL;
    
    // 简单的轮转调度
    if (current && current->state == PROCESS_RUNNING) {
        current->state = PROCESS_READY;
        next = current->next;
    } else if (current) {
        next = current->next;
    } else {
        next = get_process_list();
    }
    
    // 如果没有下一个进程，从头开始
    if (!next) {
        next = get_process_list();
    }
    
    // 跳过已终止的进程
    while (next && next->state == PROCESS_TERMINATED) {
        next = next->next;
        if (!next) {
            next = get_process_list();
        }
        if (next == current) {
            break;  // 避免无限循环
        }
    }
    
    // 如果找到可运行的进程，切换
    if (next && next->state == PROCESS_READY) {
        switch_to_process(next);
    }
}

void switch_to_process(struct process* proc) {
    if (!proc) {
        return;
    }
    
    struct process* current = get_current_process();
    
    // 如果已经是当前进程，不需要切换
    if (current == proc) {
        return;
    }
    
    // 设置为当前进程
    set_current_process(proc);
    proc->state = PROCESS_RUNNING;
    
    // 切换页目录
    switch_page_directory(proc->page_dir);
    
    // 切换上下文（通过汇编实现）
    // 这里简化实现，实际需要保存和恢复所有寄存器
}

void yield(void) {
    schedule();
}
