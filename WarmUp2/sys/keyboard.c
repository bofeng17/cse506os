#include <sys/sbunix.h>
#include <sys/pic.h>
#include <sys/keyboard.h>

volatile int SHIFT;
volatile int CONTROL;
volatile uint8_t char_1 = ' ';
volatile uint8_t char_2 = ' ';
volatile uint8_t key_code;


void isr_keyboard() {
    key_code = inb(0x60);
    
    switch (key_code) {
        case 0x2A:
        case 0x36:
            if (CONTROL != 1) {
                SHIFT = 1;
                break;
            }
        case 0xAA:
        case 0xB6:
            SHIFT = 0;
            break;
        case 0x1D:
            CONTROL = 1;
            break;
        case 0x9D:
            CONTROL = 0;
            break;
    }
    if (key_code < 0x80) {
        if (SHIFT != 1) {// SHIFT not pressed
            if (key_code >= KB_F1 && key_code <=KB_F10) {
                char_1 = 'F';
                char_2 = scan_code[key_code];
            } else {
                char_1 = scan_code[key_code];
                char_2 = 0;
                if (char_1 >= 'a' && char_1 <= 'z') {
                    if (CONTROL == 1) {
                        char_2 = char_1 - 0x20;
                        char_1 = '^';
                }
            }
            }
        } else {//SHITF pressed
            char_1 = scan_code_shift[key_code];
            char_2 = 0;
            if (char_1 >= 'A' && char_1 <= 'Z') {
                if (CONTROL == 1) {
                    char_2 = char_1;
                    char_1 = '^';
                }
            }
        }

    }
    pic_sendEOI(33);
    print_key(char_1,1);
    print_key(char_2,2);
}

void print_key(uint8_t key,int num) {
    size_t _console_row = console_row;
    size_t _console_column = console_column;
    console_row = 24;
    console_column = num + 64;
    printf("%c",key);
    console_row = _console_row;
    console_column = _console_column;
}