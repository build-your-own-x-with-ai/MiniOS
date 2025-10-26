#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

// 系统调用号
#define SYS_EXIT    1
#define SYS_WRITE   2
#define SYS_READ    3
#define SYS_SBRK    4

void syscall_init(void);

#endif
