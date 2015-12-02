#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>


int
main (int argc, char* argv[], char* envp[])
{
	
   char* input_filename = malloc(sizeof(char));
   scanf("%s", input_filename);
   char* test_wr=malloc(sizeof(char));
   struct file* file = malloc(sizeof(struct file));
   int i= open(input_filename, file,  O_RDONLY);
    if (i == -1) {
   	 printf("no such file exists!!!");
     return -1;
     }
   read(file, test_wr, 1000);
   printf("%s\n", test_wr);

   //printf("READDIR cat!!!====== %s\n", test_wr);
	
	
}
