#include "types.h"
#include "gdt.h"

// GDT 表项结构
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// GDT 指针结构
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// GDT 表（5 个表项：空、内核代码、内核数据、用户代码、用户数据）
struct gdt_entry gdt[5];
struct gdt_ptr gp;

// 外部汇编函数，用于加载 GDT
extern void gdt_flush(uint32_t);

// 设置 GDT 表项
static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base = (uint32_t)&gdt;
    
    // 空描述符
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // 内核代码段
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 0, Code, Executable, Readable
    // Granularity: 4KB blocks, 32-bit
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // 内核数据段
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 0, Data, Writable
    // Granularity: 4KB blocks, 32-bit
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // 用户代码段
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 3, Code, Executable, Readable
    // Granularity: 4KB blocks, 32-bit
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    
    // 用户数据段
    // Base = 0, Limit = 0xFFFFFFFF
    // Access: Present, Ring 3, Data, Writable
    // Granularity: 4KB blocks, 32-bit
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    
    // 加载 GDT
    gdt_flush((uint32_t)&gp);
}
