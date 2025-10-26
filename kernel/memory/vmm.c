#include "types.h"
#include "vmm.h"
#include "pmm.h"

static struct page_directory* kernel_directory = NULL;
static struct page_directory* current_directory = NULL;

// 简单的内核堆分配器
static uint32_t heap_start = 0x00400000;  // 4MB
static uint32_t heap_current = 0x00400000;

void* kmalloc(size_t size) {
    // 对齐到 4 字节
    size = (size + 3) & ~3;
    
    void* addr = (void*)heap_current;
    heap_current += size;
    
    return addr;
}

void kfree(void* ptr) {
    // 简单实现：不实际释放内存
    (void)ptr;
}

void vmm_init(void) {
    // 创建内核页目录
    kernel_directory = (struct page_directory*)kmalloc(sizeof(struct page_directory));
    
    // 初始化页目录
    for (int i = 0; i < 1024; i++) {
        kernel_directory->entries[i] = 0;
        kernel_directory->tables[i] = NULL;
    }
    
    // 恒等映射前 4MB（包含内核）
    for (uint32_t i = 0; i < 0x00400000; i += PAGE_SIZE) {
        map_page(kernel_directory, i, i, PAGE_PRESENT | PAGE_WRITE);
    }
    
    // 切换到内核页目录
    switch_page_directory(kernel_directory);
    
    current_directory = kernel_directory;
}

struct page_directory* create_page_directory(void) {
    struct page_directory* pd = (struct page_directory*)kmalloc(sizeof(struct page_directory));
    
    // 初始化页目录
    for (int i = 0; i < 1024; i++) {
        pd->entries[i] = 0;
        pd->tables[i] = NULL;
    }
    
    // 复制内核空间映射（高半部分）
    for (int i = 768; i < 1024; i++) {
        pd->entries[i] = kernel_directory->entries[i];
        pd->tables[i] = kernel_directory->tables[i];
    }
    
    return pd;
}

void map_page(struct page_directory* pd, uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    
    // 如果页表不存在，创建它
    if (!pd->tables[pd_index]) {
        void* page = alloc_page();
        if (!page) {
            return;  // 内存不足
        }
        
        pd->tables[pd_index] = (pte_t*)page;
        pd->entries[pd_index] = (uint32_t)page | PAGE_PRESENT | PAGE_WRITE | (flags & PAGE_USER);
        
        // 清空页表
        for (int i = 0; i < 1024; i++) {
            pd->tables[pd_index][i] = 0;
        }
    }
    
    // 设置页表项
    pd->tables[pd_index][pt_index] = (phys & 0xFFFFF000) | (flags & 0xFFF);
}

void unmap_page(struct page_directory* pd, uint32_t virt) {
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    
    if (pd->tables[pd_index]) {
        pd->tables[pd_index][pt_index] = 0;
    }
}

extern void load_page_directory(uint32_t* pd);
extern void enable_paging(void);

void switch_page_directory(struct page_directory* pd) {
    current_directory = pd;
    
    // 加载页目录到 CR3 并启用分页
    load_page_directory((uint32_t*)&pd->entries);
    enable_paging();
}

struct page_directory* get_kernel_directory(void) {
    return kernel_directory;
}
