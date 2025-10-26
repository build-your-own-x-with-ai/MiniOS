#ifndef IO_H
#define IO_H

#include "types.h"

// I/O 端口操作函数
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

#endif
