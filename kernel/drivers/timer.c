#include "types.h"
#include "timer.h"
#include "idt.h"
#include "scheduler.h"
#include "io.h"

static uint32_t tick_count = 0;

// 定时器中断处理函数
void timer_handler(struct interrupt_frame* frame) {
    (void)frame;
    tick_count++;
    
    // 每 10 个时钟周期触发一次调度（约 100ms）
    if (tick_count % 10 == 0) {
        schedule();
    }
}

void timer_init(uint32_t frequency) {
    // 注册定时器中断处理函数
    register_interrupt_handler(32, timer_handler);
    
    // 计算 PIT 分频值
    uint32_t divisor = 1193180 / frequency;
    
    // 发送命令字节
    outb(0x43, 0x36);
    
    // 发送分频值低字节
    outb(0x40, (uint8_t)(divisor & 0xFF));
    
    // 发送分频值高字节
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t timer_ticks(void) {
    return tick_count;
}

extern void cpu_halt(void);

void timer_wait(uint32_t ticks) {
    uint32_t end_tick = tick_count + ticks;
    while (tick_count < end_tick) {
        // 简单的忙等待
    }
}
