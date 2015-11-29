#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// for printf

// buffer used by write syscall
char printf_buf[MAX_BUFF];
/*
 * number of char in the buffer
 * printf_buf[printf_buf_count] = c;
 * printf_buf_count ++;
 */
int printf_buf_count;

/*
 * the main difference from kernel printf lies here
 * other functions are almost the same
 */
// TODO: not inline for the ease of debugging
void write_char(char c) {
    printf_buf[printf_buf_count] = c;
    printf_buf_count++;
}

int printf(const char *format, ...) {
    va_list val;
    int printed = 0;
    int desire_length = 0;
    int val_int = 0;
    uint64_t val_long = 0;
    char val_char = 0;
    char* val_string = NULL;

    // global variable, added to support user printf
    printf_buf_count = 0; // number of char in the buffer

    va_start(val, format);

    while (*format) {
        if (*format == '%') {
            desire_length = 0;
            format++;
            if (*format <= 0x39 && *format >= 0x30) {
                desire_length = *format - 0x30;
                format++;
            }
            switch (*format) {
            case 'd':
                val_int = va_arg(val, int);
                print_int(val_int, desire_length);
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

    // TODO: be cautious here, not fully verified
    write(1, printf_buf, printf_buf_count);

    return printed;
}

inline void print_char(char arg) {
    write_char(arg);
}

void print_string(char* arg) {
    while (*arg) { //be careful here
        print_char(*arg);
        arg++;
    }
}

void print_int(int arg, int desire_length) {
    int stack[10], top = -1;
    for (int i = 0; i < 10; i++)
        stack[i] = 0;
    if (arg < 0) {
        print_char('-');
        arg = -arg;
    }
    while (arg / 10) {
        top++;
        stack[top] = arg % 10;
        arg /= 10;
    }
    top++;
    stack[top] = arg;
    if ((top + 1) < desire_length)
        top = desire_length - 1;
    for (int i = top; i >= 0; i--) {
        print_char(stack[i] + '0');
    }
}

//mode = hex or ptr
void print_hex_or_ptr(uint64_t arg, int mode) {
    int stack[16], top = -1;
    for (int i = 0; i < 16; i++)
        stack[i] = 0;
    print_string("0x");
    while (arg / 16) {
        top++;
        stack[top] = arg % 16;
        arg /= 16;
    }
    top++;
    stack[top] = arg;
    if (mode == ptr_p) //if %p, print full 64-bit address
        top = 15;
    for (int i = top; i >= 0; i--) {
        if (stack[i] < 10) {
            print_char(stack[i] + '0'); //print 0-9
        } else {
            print_char(stack[i] - 10 + 'A'); //print A-F
        }
        if (i % 4 == 0 && mode == ptr_p) {
            print_char(' ');
        }
    }
}

// for scanf
//int scanf(const char *format, ...) {
//
//    va_list     val;
//    va_start(val, format);
//    char*       recvBuffer=malloc(MAX_BUFF*sizeof(char));
//    int         scannedArgs = 0;
//
//    read(0, recvBuffer, MAX_BUFF);
//    while(*format) {
//        if(*format == '%') {
//            format++;
//            scannedArgs++;
//            if(*format == 's') {
//                char* valStr = va_arg(val, char*);
//                while((*valStr++=*recvBuffer++)!= '\n');
//            }
//        }
//        format++;
//    }
//    va_end(val);
//
//    return scannedArgs;
//}

// buffer used by read syscall
char scanf_buf[MAX_BUFF];
int scanf_buf_count; // TODO: usage differs from printf, how to use it?

int scanf(const char *format, ...) {
    va_list val;
    int scaned = 0;

    char* val_string = NULL;
    int* val_int = NULL;
    scanf_buf_count = 0;

    va_start(val, format);

    read(0, scanf_buf, MAX_BUFF);
//    printf("read chars is: %d\n", n);

    while (*format) {
        if (*format == '%') {
            format++;
            scaned++;
            switch (*format) {
            case 's':
                val_string = va_arg(val, char*);
                // use line feed as string termination
                while (scanf_buf[scanf_buf_count] != '\n'
                        && scanf_buf[scanf_buf_count] != ' ') {
                    *val_string = scanf_buf[scanf_buf_count];
                    val_string++;
                    scanf_buf_count++;
                }
//                printf("buf_count is: %d\n", scanf_buf_count);
                break;
            case 'd':
                val_int = va_arg(val, int*);
                int tmp = 0;
//                printf("tmp is: %d\n", tmp);
//                printf("buf_count is: %d\n", scanf_buf_count);
                while (scanf_buf[scanf_buf_count] >= '0'
                        && scanf_buf[scanf_buf_count] <= '9') {
//                    printf("buffer is: %c\n", scanf_buf[scanf_buf_count]);
                    tmp *= 10;
                    tmp += (scanf_buf[scanf_buf_count] - '0');
                    scanf_buf_count++;
                }
                *val_int = tmp;
//                printf("tmp is: %d\n", tmp);

                break;
            default:
                break;
            }
            scanf_buf_count++;

        }
        format++;
    }
    va_end(val);

    return scaned;
}

//get a line of stdin_input
int gets(char *str) {

    int len = read(0, str, MAX_BUFF);
    str[len-1]='\0';

    return len;
}
