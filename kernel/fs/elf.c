#include "types.h"
#include "elf.h"
#include "vmm.h"
#include "pmm.h"

int elf_validate(void* elf_data) {
    struct elf_header* header = (struct elf_header*)elf_data;
    
    // 检查 ELF 魔数
    if (header->e_ident[0] != 0x7F ||
        header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L' ||
        header->e_ident[3] != 'F') {
        return 0;  // 无效的 ELF 魔数
    }
    
    // 检查 32 位
    if (header->e_ident[4] != 1) {
        return 0;  // 不是 32 位 ELF
    }
    
    // 检查小端序
    if (header->e_ident[5] != 1) {
        return 0;  // 不是小端序
    }
    
    // 检查可执行文件类型
    if (header->e_type != ET_EXEC) {
        return 0;  // 不是可执行文件
    }
    
    // 检查 x86 架构
    if (header->e_machine != EM_386) {
        return 0;  // 不是 x86 架构
    }
    
    return 1;  // 验证通过
}

void* elf_load(void* elf_data, struct page_directory* pd) {
    struct elf_header* header = (struct elf_header*)elf_data;
    
    // 验证 ELF 文件
    if (!elf_validate(elf_data)) {
        return NULL;
    }
    
    // 获取程序头表
    struct elf_program_header* ph = (struct elf_program_header*)((uint32_t)elf_data + header->e_phoff);
    
    // 遍历程序头
    for (int i = 0; i < header->e_phnum; i++) {
        if (ph[i].p_type == PT_LOAD) {
            // 计算需要的页数
            uint32_t pages = (ph[i].p_memsz + PAGE_SIZE - 1) / PAGE_SIZE;
            
            // 为每一页分配物理内存并映射
            for (uint32_t j = 0; j < pages; j++) {
                void* phys_page = alloc_page();
                if (!phys_page) {
                    return NULL;  // 内存不足
                }
                
                uint32_t virt_addr = ph[i].p_vaddr + (j * PAGE_SIZE);
                map_page(pd, virt_addr, (uint32_t)phys_page, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
            }
            
            // 复制段数据
            uint8_t* src = (uint8_t*)((uint32_t)elf_data + ph[i].p_offset);
            uint8_t* dst = (uint8_t*)ph[i].p_vaddr;
            
            for (uint32_t j = 0; j < ph[i].p_filesz; j++) {
                dst[j] = src[j];
            }
            
            // 清零 BSS 段（memsz > filesz 的部分）
            for (uint32_t j = ph[i].p_filesz; j < ph[i].p_memsz; j++) {
                dst[j] = 0;
            }
        }
    }
    
    // 返回入口点地址
    return (void*)header->e_entry;
}
