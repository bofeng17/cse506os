#ifndef _SBUNIX_H
#define _SBUNIX_H

#include <sys/defs.h>



extern void console_initialize();

extern void timer_init();

extern int printf(const char *fmt, ...);

// for terminal

#define MAX_BUFF 4096

extern char terminal_buffer[MAX_BUFF];

int terminal_write(int fd, char *buf, int count);
int terminal_read(char *buf, int count);

void terminal_put_char(uint8_t c);


#define DEBUG 1
#if DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

#endif
