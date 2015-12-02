#ifndef _TIMER_H
#define _TIMER_H

#include <sys/defs.h>

void timer_init();

void isr_timer(uint64_t cs);

#endif
