#ifndef _TIMER_H
#define _TIMER_H

#include <sys/defs.h>

void timer_init();
//uint16_t timer_read();

void isr_timer();

#endif
