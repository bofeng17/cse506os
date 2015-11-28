#include <sys/sbunix.h>
#include <sys/printf.h>
#include <sys/stdlib.h>
#include <sys/string.h>

// variable and function declarations used by terminal are in sbunix.h

char terminal_buffer[MAX_BUFF];
int terminal_buf_count; // number of char in the buffer
extern volatile int press_over;
// for terminal write
int terminal_write(int fd, char *buf, int count) {
    // TODO: stdout vs stderr
    console_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    print_string(buf, count);
    // TODO: return value
    return count;
}

//void local_echo() {
//    dprintf("%c", terminal_buffer[terminal_buf_count]);
//}

int terminal_read(char *buf, int count) {
    // isr_keyboard puts char into terminal buffer

    // copy terminal buffer to libc scanf buffer

    // TODO: how to deal with empty buffer and count?
    press_over = 0;
    __asm__ __volatile__ ("sti");

    //begin local echo
    while (press_over == 0) {
        //local_echo();
    }
    memcpy((void*) buf, (void*) terminal_buffer, count);
    int n = count > terminal_buf_count ? terminal_buf_count : count; // number of chars put to buffer

//    do {
//        if (n > count)
//            break;
//        else {
//            dprintf("[in terminal] char is %c, count is %d\n",
//                    terminal_buffer[n], n);
//            *(buf + n) = *(terminal_buffer + n);
//            n++;
//
//        }
//    } while (terminal_buffer[n] != '\n');
//    *(buf + n) ='\n';
    memset(terminal_buffer, 0, MAX_BUFF);
    terminal_buf_count = 0;

//    __asm__ __volatile__ ("cli;");

    return n;
}

void terminal_get_char(uint8_t ch) {
    if (ch == 0x08) { // backspace \b
        if (terminal_buf_count > 0) {
            terminal_buf_count--;
            terminal_buffer[terminal_buf_count] = ' ';
            console_column--;
            printf("%c", terminal_buffer[terminal_buf_count]);
            console_column--;

        }

    }
//    else if (ch == 0x0A) { // line feed \n
//
//        // TODO: push content to user process waiting for input
//    }
    else { // normal char
        if (terminal_buf_count < MAX_BUFF) {
            terminal_buffer[terminal_buf_count] = ch;
            terminal_buf_count++;
            // for testing
           // local_echo();
            printf("%c", ch);
            //dprintf("char is %c, buffer count is %d\n", ch, terminal_buf_count);
        } else {
            printf("Terminal buffer is full!! And will be cleared !\n");
            memset(terminal_buffer, 0, MAX_BUFF);
            terminal_buf_count = 0;
            // __asm__ __volatile("hlt");
        }
    }
    //terminal_local_echo();
}
//
//void terminal_local_echo () {
//    
//}
