#include <sys/sbunix.h>
#include <sys/stdarg.h>
#include <sys/printf.h>
#include <sys/string.h>

//for console

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;
uint16_t* console_buffer = (uint16_t*) 0xFFFFFFFF800B8000;
size_t console_row;
size_t console_column;
uint8_t console_color;

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color){
    return (uint16_t)c | ((uint16_t)color) << 8;
}

void console_initialize() {
    console_row = 0;
    console_column = 0;
    console_color = make_color(COLOR_LIGHT_RED, COLOR_BLACK);
}

void console_putchar(char c) {
    if (console_row == 0 && console_column == 0) {
        for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {// don't flush the last row (used by timer)
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                const size_t index = y * VGA_WIDTH + x;
                console_buffer[index] = make_vgaentry(' ', console_color);
            }
        }
    }
    if (c == '\n'){
        console_column=0;
        console_row = (console_row+1) % (VGA_HEIGHT - 1);
    } else {
        const size_t index = console_row * VGA_WIDTH + console_column;
        console_buffer[index] = make_vgaentry(c, console_color);
        if (++console_column == VGA_WIDTH) {
            console_column = 0;
            if (++console_row == (VGA_HEIGHT - 1)) { // don't write to the last row (used by timer)
                console_row = 0;
            }
        }
    }
}

//for kernel_printf

int printf(const char *format, ...) {
    va_list 	val;
    int 		printed = 0;
    int 		desire_length = 0;
    int 		val_int = 0;
    uint64_t    val_long = 0;
    char 		val_char = 0;
    char* 		val_string = NULL;
    
    // Set foreground color as RED
    console_color = make_color(COLOR_LIGHT_RED, COLOR_BLACK);
    
    va_start(val, format);
    
    while(*format) {
        if (*format == '%') {
            desire_length = 0;
            format++;
            if (*format <= 0x39 && *format >= 0x30) {
                desire_length = *format - 0x30;
                format ++;
            }
            switch (*format){
                case 'd':
                    val_int = va_arg(val, int);
                    print_int(val_int,desire_length);
                    break;
                case 'c':
                    val_char = va_arg(val, int);
                    print_char(val_char);
                    break;
                case 's':
                    val_string = va_arg(val, char*);
                    print_string(val_string, strlen(val_string));
                    break;
                case 'x':
                    val_long = va_arg(val, uint64_t);
                    print_hex_or_ptr(val_long, hex_x);
                    break;
                case 'p':
                    val_long = va_arg(val, uint64_t);
                    print_hex_or_ptr(val_long, ptr_p);
                    break;
            }
        } else {
            print_char(*format);
        }
        printed++;
        format++;
    }
    va_end(val);
    return printed;
}

void print_char(char arg) {
    console_putchar(arg);
}

/* 
 * be called by:
 * printf %s, and terminal write
 */
void print_string(char* arg, int count) {
    while(count) {
        print_char(*arg);
        arg++;
        count--;
    }
}

void print_int(int arg, int desire_length) {
    int stack[10],top=-1;
    for (int i = 0; i < 10; i ++) stack[i] = 0;
    if (arg < 0) {
        print_char('-');
        arg = -arg;
    }
    while (arg/10){
        top++;
        stack[top] = arg%10;
        arg/=10;
    }
    top++;
    stack[top]=arg;
    if ((top + 1) < desire_length) top = desire_length - 1;
    for (int i = top; i >= 0; i --){
        print_char(stack[i]+'0');
    }
}

//mode = hex or ptr
void print_hex_or_ptr(uint64_t arg,int mode) {
    int stack[16],top=-1;
    for (int i = 0; i < 16; i ++) stack[i] = 0;
    print_string("0x", 2);
    while (arg/16){
        top++;
        stack[top] = arg%16;
        arg/=16;
    }
    top++;
    stack[top]=arg;
    if (mode == ptr_p)//if %p, print full 64-bit address
        top = 15;
    for (int i = top; i >= 0; i --){
        if (stack[i] < 10) {
            print_char(stack[i]+'0');//print 0-9
        } else {
            print_char(stack[i] - 10 + 'A');//print A-F
        }
        if (i % 4 == 0 && mode == ptr_p) {
            print_char(' ');
        }
    }
}
