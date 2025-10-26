#ifndef PMM_H
#define PMM_H

#include "types.h"

#define PAGE_SIZE 4096

void pmm_init(uint32_t mem_size);
void* alloc_page(void);
void free_page(void* page);
uint32_t get_free_page_count(void);

#endif
