#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

void scheduler_init(void);
void schedule(void);
void switch_to_process(struct process* proc);
void yield(void);

#endif
