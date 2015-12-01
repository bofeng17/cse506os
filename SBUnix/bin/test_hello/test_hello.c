#include <stdio.h>
#include <sys/defs.h>

void
test (int x, int y, char *s, char a, int d)
{
    printf("%d\n",x);
    printf("%d\n",y);
    printf("%s\n",s);
    printf("%c\n",a);
    printf("%d\n",d);
    
}

// bss
int c[4096];
//int d;

// data
int data_arr[8192] = {1,2,3,4};
char* s = "hello\n";

int
main (int argc, char* argv[], char* envp[])
{
    //printf("Hello World!\n");
    
    //    __asm__ __volatile__("INT $0x80;");
    //        char *ptr_test = (char *)0x200000;
    //        *ptr_test = 0;
    //    while (1) {
    //        ;
    //    }
    printf("Hello TEST World!\n");

//    printf("%c, %s, %d, %x, %p\n",'f', "this is a simple test", 987, 0x12345678, 0x12345678);

    int x = 1;
    char y='h';
    test(c[3], data_arr[0], s, y, x);
    
//    int n = 1, fd = 1, count = 5;
//    char* s = "hello\n";
    //    __asm__ __volatile__("syscall"
    //                         :
    //                         :"a"(n), "D"(fd), "S"((uint64_t)s), "d"(count));
    return 0;
}
