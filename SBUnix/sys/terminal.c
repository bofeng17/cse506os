#include <sys/sbunix.h>
#include <sys/printf.h>
#include <sys/string.h>

// for terminal write
int terminal_write(int fd, char *buf, int count) {
    // TODO: stdout vs stderr
    console_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    print_string(buf, count);
    // TODO: return value
    return count;
}

int terminal_read() {
    return 0;
}
