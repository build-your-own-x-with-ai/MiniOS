#ifndef VGA_H
#define VGA_H

#include "types.h"

void terminal_init(void);
void terminal_putchar(char c);
void terminal_write(const char* str, size_t len);
void terminal_writestring(const char* str);
void terminal_clear(void);
void terminal_scroll(void);

#endif
