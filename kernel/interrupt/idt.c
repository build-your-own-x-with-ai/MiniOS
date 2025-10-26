#include "types.h"
#include "idt.h"
#include "vga.h"
#include "io.h"

// IDT 表项结构
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT 指针结构
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// IDT 表（256 个表项）
struct idt_entry idt[256];
struct idt_ptr idtp;

// 中断处理函数表
interrupt_handler_t interrupt_handlers[256];

// 外部汇编 ISR 函数声明
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// IRQ 函数声明
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

// 系统调用
extern void isr128(void);

// 外部汇编函数
extern void idt_flush(uint32_t);

// 设置 IDT 表项
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// 重映射 PIC
static void pic_remap(void) {
    // 保存掩码
    uint8_t a1, a2;
    
    // 读取掩码
    a1 = inb(0x21);
    a2 = inb(0xA1);
    
    // 初始化主 PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    
    // 设置中断向量偏移
    outb(0x21, 0x20);  // 主 PIC: IRQ 0-7 -> INT 32-39
    outb(0xA1, 0x28);  // 从 PIC: IRQ 8-15 -> INT 40-47
    
    // 设置级联
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    
    // 设置模式
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    // 恢复掩码
    outb(0x21, a1);
    outb(0xA1, a2);
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    
    // 清空 IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
        interrupt_handlers[i] = NULL;
    }
    
    // 重映射 PIC
    pic_remap();
    
    // 设置 CPU 异常处理函数 (0-31)
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
    
    // 设置硬件中断处理函数 (32-47)
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
    
    // 设置系统调用 (0x80 = 128)
    idt_set_gate(128, (uint32_t)isr128, 0x08, 0xEE);  // DPL=3 允许用户态调用
    
    // 加载 IDT
    idt_flush((uint32_t)&idtp);
    
    // 启用中断（在汇编中完成）
}

void register_interrupt_handler(uint8_t num, interrupt_handler_t handler) {
    interrupt_handlers[num] = handler;
}

// 通用中断处理函数
void isr_handler(struct interrupt_frame* frame) {
    if (interrupt_handlers[frame->int_no] != NULL) {
        interrupt_handlers[frame->int_no](frame);
    }
}

// IRQ 处理函数
void irq_handler(struct interrupt_frame* frame) {
    // 发送 EOI 到 PIC
    if (frame->int_no >= 40) {
        // 从 PIC
        outb(0xA0, 0x20);
    }
    // 主 PIC
    outb(0x20, 0x20);
    
    if (interrupt_handlers[frame->int_no] != NULL) {
        interrupt_handlers[frame->int_no](frame);
    }
}
