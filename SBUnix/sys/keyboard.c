#include <sys/sbunix.h>
#include <sys/pic.h>
#include <sys/printf.h>
#include <sys/keyboard.h>

volatile int SHIFT;
volatile int CONTROL;
volatile uint8_t char_1 = ' ';
volatile uint8_t char_2 = ' ';
volatile uint8_t terminal_ch = ' ';
volatile uint8_t key_code;

volatile int press_over;

void isr_keyboard() {
    key_code = inb(0x60);
    
    switch (key_code) {
        case 0x2A:
        case 0x36:// left/right shift pressed
            if (CONTROL != 1) {
                SHIFT = 1;
            }
            break;
        case 0xAA:
        case 0xB6:// left/right shift released
            SHIFT = 0;
            break;
        case 0x1D:// left control pressed
            CONTROL = 1;
            break;
        case 0x9D:// left control released
            CONTROL = 0;
            break;
        case 0x0E:// backspace pressed
            terminal_get_char (0x08); // ASCII of backspace is 0x08
            // TODO: should directly push to shell, instead of buffering it
            break;
        case 0x1C:// enter pressed
            // ASCII of line feed, the same to Linux
            if(press_over==0){
                press_over=1;
            }
            terminal_get_char (0x0A);
            // TODO: should directly push to shell, instead of buffering it
            break;
        default:
            if (key_code < 0x80) {
                if (SHIFT != 1) {// SHIFT not pressed
                    if (key_code >= KB_F1 && key_code <=KB_F10) {
                        char_1 = 'F';
                        char_2 = scan_code[key_code];
                        // needn't put into terminal_buffer
                    } else {
                        char_1 = scan_code[key_code];
                        char_2 = 0;
                        terminal_get_char (char_1);

                        // TODO: should put format check here!
                        if (char_1 >= 'a' && char_1 <= 'z') {
                          //  terminal_get_char (char_1);
                            if (CONTROL == 1) {
                                /*
                                 * ctl + a -> 1, ctl + z -> 26
                                 * currently not supported
                                 */
                                //terminal_ch = char_1 - 0x60;
                                char_2 = char_1 - 0x20;
                                char_1 = '^';
                            }
                        }
                        if (key_code >= 2 && key_code <= 13) {// 1 -- =
                            //terminal_get_char (char_1);
                        }
                    }
                } else {//SHITF pressed
                    char_1 = scan_code_shift[key_code];
                    char_2 = 0;
                    if (char_1 >= 'A' && char_1 <= 'Z') {
                        terminal_get_char (char_1);
                        if (CONTROL == 1) {
                            // needn't put into terminal_buffer
                            char_2 = char_1;
                            char_1 = '^';
                        }
                    }
                    if (key_code >= 2 && key_code <= 13) { // shift+1 -- shift+=
                        terminal_get_char (char_1);
                    }
                }
            }
            break;
    }
    pic_sendEOI(33);
//    print_key(char_1,1);
//    print_key(char_2,2);
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
