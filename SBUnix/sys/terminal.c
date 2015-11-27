#include <sys/sbunix.h>
#include <sys/printf.h>
#include <sys/string.h>

// variable and function declarations used by terminal are in sbunix.h

char terminal_buffer[MAX_BUFF];
int terminal_buf_count; // number of char in the buffer

// for terminal write
int terminal_write(int fd, char *buf, int count) {
    // TODO: stdout vs stderr
    console_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    print_string(buf, count);
    // TODO: return value
    return count;
}

int terminal_read(char *buf, int count){
    // isr_keyboard puts char into terminal buffer
    
    
    // copy terminal buffer to libc scanf buffer
    strcpy (buf, terminal_buffer);
    terminal_buf_count = 0;
    // TODO: how to deal with empty buffer and count?
    return 0;
}

void terminal_put_char(char c) {
    if (terminal_buf_count < MAX_BUFF) {
        terminal_buffer[terminal_buf_count] = c;
        terminal_buf_count ++;
    } else {
        printf("terminal buffer is full!\n");
        __asm__ __volatile("hlt");
    }
}