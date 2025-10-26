#include "types.h"
#include "pmm.h"

// 物理内存管理器使用位图

#define KERNEL_START 0x00100000  // 1MB
#define KERNEL_SIZE  0x00300000   // 3MB (预留给内核)

extern uint32_t _kernel_end;  // 链接脚本中定义的内核结束地址

static uint32_t* page_bitmap;
static uint32_t total_pages;
static uint32_t used_pages;
static uint32_t bitmap_size;

// 设置位图中的位
static void bitmap_set(uint32_t bit) {
    page_bitmap[bit / 32] |= (1 << (bit % 32));
}

// 清除位图中的位
static void bitmap_clear(uint32_t bit) {
    page_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

// 测试位图中的位
static int bitmap_test(uint32_t bit) {
    return page_bitmap[bit / 32] & (1 << (bit % 32));
}

// 查找第一个空闲页
static int find_free_page(void) {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            return i;
        }
    }
    return -1;
}

void pmm_init(uint32_t mem_size) {
    // 计算总页数
    total_pages = mem_size / PAGE_SIZE;
    
    // 位图大小（字节）
    bitmap_size = (total_pages + 7) / 8;
    
    // 位图放在内核结束后
    page_bitmap = (uint32_t*)((uint32_t)&_kernel_end);
    
    // 初始化位图为全 0（所有页空闲）
    for (uint32_t i = 0; i < bitmap_size / 4; i++) {
        page_bitmap[i] = 0;
    }
    
    used_pages = 0;
    
    // 标记前 1MB 为已使用（BIOS 和引导区域）
    for (uint32_t i = 0; i < (0x00100000 / PAGE_SIZE); i++) {
        bitmap_set(i);
        used_pages++;
    }
    
    // 标记内核区域为已使用
    uint32_t kernel_pages = ((uint32_t)&_kernel_end - KERNEL_START + bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = 0; i < kernel_pages; i++) {
        bitmap_set((KERNEL_START / PAGE_SIZE) + i);
        used_pages++;
    }
}

void* alloc_page(void) {
    int page_index = find_free_page();
    if (page_index == -1) {
        return NULL;  // 内存不足
    }
    
    bitmap_set(page_index);
    used_pages++;
    
    // 返回物理地址
    void* addr = (void*)(page_index * PAGE_SIZE);
    
    // 清零页面
    uint32_t* page = (uint32_t*)addr;
    for (int i = 0; i < PAGE_SIZE / 4; i++) {
        page[i] = 0;
    }
    
    return addr;
}

void free_page(void* page) {
    uint32_t page_index = (uint32_t)page / PAGE_SIZE;
    
    if (page_index >= total_pages) {
        return;  // 无效地址
    }
    
    if (!bitmap_test(page_index)) {
        return;  // 页面已经空闲
    }
    
    bitmap_clear(page_index);
    used_pages--;
}

uint32_t get_free_page_count(void) {
    return total_pages - used_pages;
}
