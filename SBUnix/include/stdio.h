#ifndef _STDIO_H
#define _STDIO_H

#define MAX_LINE 2048
#define MAX_BUFF 4096

#include <sys/defs.h>

// for printf
// buffer used by write syscall
extern char printf_buf[MAX_BUFF];
extern int printf_buf_count;

/*
 * the main difference from kernel printf lies here
 * other functions are almost the same
 */
void write_char(char c);

void print_char(char); 	 //%c
void print_string(char*);//%s
void print_int(int, int); 	 //%d
#define hex_x 0
#define ptr_p 1
void print_hex_or_ptr(uint64_t,int);// %x, %p

int printf(const char *format, ...);


// for scanf
// buffer used by read syscall
extern char scanf_buf[MAX_BUFF];
extern int scanf_buf_count;


int scanf(const char *format, ...);
int gets(char *str) ;
#endif
