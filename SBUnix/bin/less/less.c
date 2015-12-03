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

int main(int argc, char* argv[], char* envp[]) {
    char* input = argv[1];
    if (input == NULL) {
        printf("===[ERROR] please enter file name!===\n");
        return 0;
    }

    char cur[MAX_LENGTH];
    memset((void*) cur, 0, MAX_LENGTH);

    get_cwd(cur);

    size_t size = strlen(cur);

    char tmp[MAX_LENGTH];
    memset((void*) tmp, 0, MAX_LENGTH);
    // char* tmp = malloc(30*sizeof(char));

    //char tmp[MAX_LENGTH];
    //memset((void*) tmp, 0, MAX_LENGTH);

    strcpy(tmp, cur + 7);
    memset((void*) cur, 0, size);

    strcpy(cur, tmp);

    strcat(cur, input);

    //char* input_filename = malloc(sizeof(char));
    //scanf("%s", input_filename);
    char test_wr[MAX_BUFFER];
    memset((void*) test_wr, 0, MAX_BUFFER);

    struct file* file = malloc(sizeof(struct file));
    int i = open(cur, file, O_RDONLY);
    if (i == -1) {
        printf("===[ERROR] no such file!===\n");
        return 0;
    }
    read(file, test_wr, 200);
    printf("%s\n", test_wr);
    return 0;

}
