#ifndef _STRING_H
#define _STRING_H
#include <sys/defs.h>

char *
strcpy (char *d, const char *s);

char *
strncpy (char *dest, char *src, size_t n);

int
strcmp (const char *s1, const char * s2);

int
strncmp (const char *str1, const char *str2, size_t maxlen);

size_t
strlen (const char *s);

char* strcat(char* destin,const char* source);

char* strstr(char* str1, char* str2);

#endif
