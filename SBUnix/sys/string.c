
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

char* strcat(char* destin, char* source) {
    char* tmp=destin;
    while(*destin) {
        destin++;
    }
    while(*source) {
        *destin = *source;
        destin++;
        source++;
    }
    //*tmp = '\0';
    return tmp;
}

char* strstr(char* str1, char* str2) {
    char* string;
    char* search;
    char* tmp;
    int offset = 0;
    if(!str1 || !str2) {
        return NULL;
    }
    string = str1;
    while(*string) {
        search = str2;
        while(*string != *search && *string) {
            string++;
            offset++;
        }
        tmp = string;
        while(*tmp == *search) {
            tmp++;
            search++;
            if(*search == '\0') {
                return str1+offset;
            }
        }
        string++;
        offset++;
    }
    return NULL;
}





