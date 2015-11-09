
#include <sys/string.h>
#include <sys/defs.h>

char* strcpy(char *d, const char *s) {
    while((*d++=*s++));
    return d;
}

char* strncpy(char *d, char *s, size_t n) {
    
    while(n--> 0 && (*d++=*s++)) {
    }
    return d;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {s1++;s2++;}
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}


int strncmp(const char *s1, const char *s2, size_t n) {
    if (!n--) return 0;

    while(*s1 && *s2 && n-- && (*s1 == *s2)) {s1++;s2++;}
    
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while(*s++!='\0') {len++;}
    return len;
}




