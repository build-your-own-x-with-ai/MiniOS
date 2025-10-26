#ifndef VMM_H
#define VMM_H

#include "types.h"

// 页目录项和页表项标志
#define PAGE_PRESENT   0x1
#define PAGE_WRITE     0x2
#define PAGE_USER      0x4

typedef uint32_t pde_t;
typedef uint32_t pte_t;

struct page_directory {
    pde_t entries[1024];
    pte_t* tables[1024];
    uint32_t physical_addr;
};

void vmm_init(void);
struct page_directory* create_page_directory(void);
void map_page(struct page_directory* pd, uint32_t virt, uint32_t phys, uint32_t flags);
void unmap_page(struct page_directory* pd, uint32_t virt);
void switch_page_directory(struct page_directory* pd);
struct page_directory* get_kernel_directory(void);

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
