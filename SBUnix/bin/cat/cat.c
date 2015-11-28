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
   struct file* file = open(input_filename, O_RDONLY);
    if (file == NULL) {
   	 printf("no such file exists!!!");
     return -1;
     }
   read(file, test_wr, 1000);
   printf("%s\n", test_wr);

   //printf("READDIR cat!!!====== %s\n", test_wr);
	
	
}
