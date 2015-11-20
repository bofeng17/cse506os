//#include <stdio.h>
//
//int main(int argc, char* argv[], char* envp[]) {
//	printf("Hello World!\n");
//	return 0;
//}

#include <sys/defs.h>

void
test (int x, int y, char *s, char a, int d)
{
    
}

int c;
int d;

int
main (int argc, char* argv[], char* envp[])
{
    //printf("Hello World!\n");
    int n = 1, fd = 1, count = 5;
    char* s = "hello\n";
    
    __asm__ __volatile__("syscall"
                         :
                         :"a"(n), "D"(fd), "S"((uint64_t)s), "d"(count));
    return 0;
}