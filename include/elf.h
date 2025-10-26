#ifndef ELF_H
#define ELF_H

#include "types.h"

// 前向声明
struct page_directory;

// ELF 魔数
#define ELF_MAGIC 0x464C457F  // "\x7FELF"

// ELF 类型
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4

// ELF 机器类型
#define EM_NONE  0
#define EM_386   3

// 程序头类型
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

// 程序头标志
#define PF_X 0x1  // 可执行
#define PF_W 0x2  // 可写
#define PF_R 0x4  // 可读

// ELF 头部
struct elf_header {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed));

// 程序头
struct elf_program_header {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} __attribute__((packed));

int elf_validate(void* elf_data);
void* elf_load(void* elf_data, struct page_directory* pd);

#endif
