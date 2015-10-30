#ifndef _PRINTF_H
#define _PRINTF_H

#include <sys/defs.h>

//for console
enum vga_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;

uint16_t* console_buffer = (uint16_t*) 0xFFFFFFFF800B8000;

static inline uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t make_vgaentry(char c, uint8_t color){
    return (uint16_t)c | ((uint16_t)color) << 8;
}
void console_initialize();
void console_putchar(char c);

//for kernel_printf
void print_char(char); 	 //%c
void print_string(char*);//%s
void print_int(int, int); 	 //%d
void print_hex_or_ptr(uint64_t,int);// %x, %p
#define hex_x 0
#define ptr_p 1

int printf(const char *format, ...);

#endif
