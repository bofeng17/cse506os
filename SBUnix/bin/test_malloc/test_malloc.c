#include <stdio.h>
#include <stdlib.h>

#include <sys/defs.h>

void test(int x, int y, char *s, char a, int d) {
//    printf("%d\n",x);
//    printf("%d\n",y);
//    printf("%s\n",s);
//    printf("%c\n",a);
//    printf("%d\n",d);

}

// bss
int c[4096];
//int d;

// data
int data_arr[8192] = { 1, 2, 3, 4 };
char* s = "hello\n";

int main(int argc, char* argv[], char* envp[]) {
    //printf("Hello World!\n");

    //    __asm__ __volatile__("INT $0x21;");
    //        char *ptr_test = (char *)0x200000;
    //        *ptr_test = 0;
    //    while (1) {
    //        ;
    //    }
    //   printf("Hello World!\n");

    struct dirent* a = malloc(sizeof(struct dirent));

    printf("dirent size: %d\n", sizeof(struct dirent));

    void* b = opendir("bin/");

    if(b==NULL)
     {
        printf("===[ERROR] opendir failed!===\n");
       return 0;
    }

    readdir(b, a);

    //printf("TESTING READDIR: %s", a[0].name);
    int h = 0;

    for (h = 0; h < a->num; h++) {
       // printf("TESTING READDIR: %d", j);

        printf("TESTING READDIR: %s", a[h].name);
    }

    int size = 4000;
    int * t = malloc(size * sizeof(int));

    if (t) {
        int i = 0;
        for (i = 0; i < size; i++) {
            t[i] = i;
            printf("t[%d] is:%d \n", i, t[i]);
        }

    }

    int x = 1;
    char y = 'h';
    test(c[3], data_arr[0], s, y, x);

//    int n = 1, fd = 1, count = 5;
//    char* s = "hello\n";
    //    __asm__ __volatile__("syscall"
    //                         :
    //                         :"a"(n), "D"(fd), "S"((uint64_t)s), "d"(count));
    return 0;
}
