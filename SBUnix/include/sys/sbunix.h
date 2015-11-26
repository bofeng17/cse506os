#ifndef _SBUNIX_H
#define _SBUNIX_H

#include <sys/defs.h>

size_t console_row;
size_t console_column;
uint8_t console_color;
uint16_t* console_buffer;

extern void console_initialize();

extern void timer_init();
//do we need keyboard initializer??

extern int printf(const char *fmt, ...);

int terminal_write(int fd, char *buf, int count);

#define DEBUG 1
#if DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

#endif
