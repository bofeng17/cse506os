#include <stdio.h>
#include <sys/defs.h>

void
test (int x, int y, char *s, char a, int d)
{
    
}

// bss
int c;
int d;

// data
//int arr[10] = {1,2,3,4};
//char* s = "hello\n";
char *s = (char *) 0x800000;

int
main (int argc, char* argv[], char* envp[])
{
    //printf("Hello World!\n");
    
    //    __asm__ __volatile__("INT $0x21;");
    //        char *ptr_test = (char *)0x200000;
    //        *ptr_test = 0;
    //    while (1) {
    //        ;
    //    }
    
    printf("Hello World!\n");
    
    //int n = 1, fd = 1, count = 5;
    //char* s = "hello\n";
    //    __asm__ __volatile__("syscall"
    //                         :
    //                         :"a"(n), "D"(fd), "S"((uint64_t)s), "d"(count));
    return 0;
}