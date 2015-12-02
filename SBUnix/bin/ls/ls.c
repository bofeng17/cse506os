#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>

#define MAX_ARGS 20
#define MAX_NAME 20

#define MAX_LENGTH 100
#define MAX_BUFFER 1024

int
main (int argc, char* argv[], char* envp[])
{
    int i;

     char direct[MAX_LENGTH];
     memset((void*) direct, 0, MAX_LENGTH);

     get_cwd(direct);

     if (!strcmp(direct, "rootfs/")) {
         /* printf("im in rootfs\n");
          printf("bin/\n");
          printf("lib/\n");
          printf("mnt/\n");*/

         struct dirent* a = malloc(sizeof(struct dirent));

         read_rootfs(a);

         for (i = 0; i < a->num; i++) {
             printf("%s    ", a[i].name);
         }

         printf("\n");
         return 0;
     }

     //cut_rootfs(direct);

     char tmp[MAX_LENGTH];

     size_t size = strlen(direct);

     memset((void*) tmp, 0, MAX_LENGTH);
     // char* tmp = malloc(30*sizeof(char));

     //char tmp[MAX_LENGTH];
     //memset((void*) tmp, 0, MAX_LENGTH);

     strcpy(tmp, direct + 7);
     memset((void*) direct, 0, size);

     strcpy(direct, tmp);

     size_t length = strlen(direct);
     //printf("TESTING GET_CWD: %s \n", direct);

     struct dirent* a = malloc(sizeof(struct dirent));
     //printf("dirent size: %d\n", sizeof(struct dirent));

     if (a == NULL) {
         printf("===[ERROR] malloc failed,out of memory!===\n");
         return 0;
     }

     void* b = opendir(direct);

     readdir(b, a);

     char final_name[MAX_LENGTH];
     memset((void*) final_name, 0, MAX_LENGTH);

     for (i = 0; i < a->num; i++) {
         strcpy(final_name, a[i].name + length);

         printf("%s    ", final_name);
     }
     printf("\n");

     return 0;
}
