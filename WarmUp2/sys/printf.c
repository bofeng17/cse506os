#include <sys/sbunix.h>
#include <sys/stdarg.h>
#include <sys/printf.h>

//for console

void console_initialize() {
    console_row = 0;
    console_column = 0;
    console_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
}

// void console_initialize() {
//	console_row = 0;
//	console_column = 0;
//	console_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
//	for (size_t y = console_row; y < VGA_HEIGHT; y++) {
// if (y == console_row) {
// for (size_t x = console_column; x < VGA_WIDTH; x++) {
// const size_t index = y * VGA_WIDTH + x;
// console_buffer[index] = make_vgaentry(' ', console_color);
// }} else {
// for (size_t x = 0; x < VGA_WIDTH; x++) {
// const size_t index = y * VGA_WIDTH + x;
// console_buffer[index] = make_vgaentry(' ', console_color);
// }
// }
//	}
// }

void console_putchar(char c) {
    const size_t index = console_row * VGA_WIDTH + console_column;
    console_buffer[index] = make_vgaentry(c, console_color);
    if (++console_column == VGA_WIDTH) {
        console_column = 0;
        if (++console_row == VGA_HEIGHT) {
            console_row = 0;
        }
    }
}

//for kernel_printf

int printf(const char *format, ...) {
    va_list 	val;
    int 		printed = 0;
    int 		desire_length = 0;
    int 		val_int = 0;
    char 		val_char = 0;
    char* 		val_string = NULL;
    
    va_start(val, format);
    
    while(*format) {
        if (*format == '%') {
            desire_length = 0;
            format++;
            switch (*format){
                case '2':
                    desire_length = -1;
                case '3':
                    desire_length+=3;
                    format++;
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
                    print_string(val_string);
                    break;
                case 'x':
                    val_int = va_arg(val, int);
                    print_hex((unsigned int)val_int);
                    break;
                case 'p':
                    val_int = va_arg(val, int);
                    print_ptr(val_int);
                    break;
            }
        } else if (*format == '\n'){
                    console_column=0;
                    console_row = (console_row+1)%VGA_HEIGHT;
        } else {
            console_putchar(*format);
        }
    printed++;
    format++;
    }
    va_end(val);
    return printed;
}

inline void print_char(char arg) {
    if (arg == '\n'){
        console_column=0;
        console_row = (console_row+1)%VGA_HEIGHT;
    } else {
        console_putchar(arg);
    }
}

void print_string(char* arg) {
    while(*arg) {//be careful here
        print_char(*arg);
        arg++;
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

void print_hex(int arg) {
    print_hex_base((uint64_t)arg,0);
}

void print_ptr(uint64_t arg){
    print_hex_base(arg,1);
}

void print_hex_base(uint64_t arg,int mode) {
    int stack[16],top=-1;
    for (int i = 0; i < 16; i ++) stack[i] = 0; 
    print_string("0x");
    if (mode == 0)//if print_hex revoke this function, print 32-bit value
        arg = (uint32_t)arg;
    while (arg/16){
        top++;
        stack[top] = arg%16;
        arg/=16;
    }
    top++;
    stack[top]=arg;
    if (mode == 1)//if print_ptr revoke this function, print full 64-bit address
        top = 15;
    for (int i = top; i >= 0; i --){
        if (stack[i] < 10) {
	    print_char(stack[i]+'0');
	} else {
	    switch (stack[i]) {
		case 10: print_char('A'); break;
		case 11: print_char('B'); break;
		case 12: print_char('C'); break;
		case 13: print_char('D'); break;
		case 14: print_char('E'); break;
		case 15: print_char('F'); break;
	    }
        }
    }
}

