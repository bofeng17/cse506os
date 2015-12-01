#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {

    char* g=malloc(100 * sizeof(char));
    printf("please input :(enter to finish):");

    int len=gets(g);

    printf("you entered: %s,length is %d\n",g, len);

    char* s = malloc(100 * sizeof(char));
    int n = 0;
    printf("please input :(enter to finish):");

    scanf("%s %d", s, &n);

    printf("begin printing s\n");
    while (n-- > 0)
        printf("%s\n", s);

    int a=0,b=0;
    char* c = malloc(100 * sizeof(char));
    char* d = malloc(100 * sizeof(char));
    printf("please input a(integer) c(string) b(integer) d(string) :(enter to finish):\n");

    scanf("%d %s %d %s",&a,c,&b,d);

    while (a-- > 0)
        printf("%s\n", c);
    while (b-- > 0)
        printf("%s\n", d);
    printf("printing finished!\n");

    return 0;
}
