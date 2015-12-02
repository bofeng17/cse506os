#include <stdio.h>
#include <sys/defs.h>


int main (int argc, char* argv[], char* envp[])
{

    printf("%d %s %s %s %s %s\n", argc, argv[0],argv[1], envp[0],envp[1],envp[2]);
    return 0;
}
