#ifndef _STDIO_H
#define _STDIO_H

#define MAX_LINE 2048
#define MAX_BUFF 4096

#include <sys/defs.h>

// for printf
void write_putchar(char c);
ssize_t write(int fd, const void *buf, size_t count);

//for kernel_printf
void print_char(char); 	 //%c
void print_string(char*);//%s
void print_int(int, int); 	 //%d
#define hex_x 0
#define ptr_p 1
void print_hex_or_ptr(uint64_t,int);// %x, %p

int printf(const char *format, ...);

// for scanf
int scanf(const char *format, ...);

#endif
