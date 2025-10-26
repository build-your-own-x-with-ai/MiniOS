#ifndef IDT_H
#define IDT_H

#include "types.h"

// 中断帧结构
struct interrupt_frame {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*interrupt_handler_t)(struct interrupt_frame*);

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void register_interrupt_handler(uint8_t num, interrupt_handler_t handler);

#endif
