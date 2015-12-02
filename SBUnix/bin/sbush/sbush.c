#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>
#include <sbush.h>

#define MAX_ARGS 20
#define MAX_LENGTH 100
#define MAX_BUFFER 1024

char tmp[MAX_LENGTH];

void cut_rootfs(char* name)
{

    memset((void*) tmp, 0, MAX_LENGTH);
   // char* tmp = malloc(30*sizeof(char));

    //char tmp[MAX_LENGTH];
    //memset((void*) tmp, 0, MAX_LENGTH);
    strcpy(tmp, name+7);
    strcpy(name, tmp);


    //return tmp;
}

void add_rootfs(char* name)
{
    //char* tmp = malloc(30*sizeof(char));

    memset((void*) tmp, 0, MAX_LENGTH);
    //char tmp[MAX_LENGTH];
   // memset((void*) tmp, 0, MAX_LENGTH);

    strcpy(tmp, "rootfs/");
    strcat(tmp, name);
    strcpy(name, tmp);

    //return tmp;
}

void shellPrompt() {

    char cur_dir[MAX_LENGTH];
    memset((void*) cur_dir, 0, MAX_LENGTH);

    get_cwd(cur_dir);
    cut_rootfs(cur_dir);

    char root_dir[MAX_LENGTH];
    memset((void*) root_dir, 0, MAX_LENGTH);

    strcpy(root_dir, "rootfs/");
    strcat(root_dir, cur_dir);

    printf("root@SBUINX:%s#", root_dir);

    memset((void*) cur_dir, 0, MAX_LENGTH);
    memset((void*) root_dir, 0, MAX_LENGTH);

}

int parseInputToParams(char* input, char* param[], char sep) {
    char ch;
    int status = 0;   //0 indicates current char is a space and 1 not
    int count = 0;
    while (*input) {
        ch = *input;
        if (ch == sep && status == 0) {
            status = 0;         //  ls
        }                       // ^
        if (ch == sep && status == 1) {
            *input = '\0';
            status = 0;         //ls -a
        }                       //  ^
        if (ch != sep && status == 0) {
            //args[count]=malloc(sizeof(input));
            param[count++] = input;
            status = 1;         //ls -a
        }                       //  ^
        if (ch != sep && status == 1) {
            status = 1;         //pwd
        }                       // ^
        input++;
        if (*input == '\n')
            *input = '\0';
    }
    param[count] = NULL;
    /*#ifdef DEBUG
     int i=0;
     for(i=0;i<count;i++)
     printf("(parseInputToParams) arg %d is :%s\n",i,param[i]);

     printf("count is: %d\n",count);
     #endif */
    return count;
}

void ps_cmd() {
    ps_t ups = malloc(sizeof(ps_state));

    if (ups == NULL) {
        printf("===[ERROR] malloc failed,out of memory!===\n");
        return;
    }
    int no = ps(ups);

    int i = 0;
    printf("PID        STATE     NAME\n"); //name field is 14 char length, add space to keep aligned
    for (i = 0; i < no; i++) {
        printf("%d          %s%s\n", ups->id[i], ups->state[i], ups->name[i]);
    }
}

void ls_cmd() {
    int i;

    char direct[MAX_LENGTH];
    memset((void*) direct, 0, MAX_LENGTH);

    get_cwd(direct);

     if(!strcmp(direct, "rootfs/"))
    {
       /* printf("im in rootfs\n");
        printf("bin/\n");
        printf("lib/\n");
        printf("mnt/\n");*/

        struct dirent* a = malloc(sizeof(struct dirent));

        read_rootfs(a);

        for(i=0;i<a->num;i++)
        {
            printf("%s\n", a[i].name);
        }


        return;
    }

    cut_rootfs(direct);


    size_t length = strlen(direct);
    //printf("TESTING GET_CWD: %s \n", direct);

    struct dirent* a = malloc(sizeof(struct dirent));
    //printf("dirent size: %d\n", sizeof(struct dirent));

    if (a == NULL) {
        printf("===[ERROR] malloc failed,out of memory!===\n");
        return;
    }

    void* b = opendir(direct);

    readdir(b, a);

    char final_name[MAX_LENGTH];
    memset((void*) final_name, 0, MAX_LENGTH);

    for (i = 0; i < a->num; i++) {
        strcpy(final_name, a[i].name + length);

        printf("%s\n", final_name);
    }

    memset((void*) direct, 0, MAX_LENGTH);
    memset((void*) final_name, 0, MAX_LENGTH);
}

void cat_cmd(char* input) {
    if (input == NULL) {
        printf("===[ERROR] please enter file name!===\n");
        return;
    }

    char cur[MAX_LENGTH];
    memset((void*) cur, 0, MAX_LENGTH);

    get_cwd(cur);

    cut_rootfs(cur);

    strcat(cur, input);

    //char* input_filename = malloc(sizeof(char));
    //scanf("%s", input_filename);
    char test_wr[MAX_BUFFER];
    memset((void*) test_wr, 0, MAX_BUFFER);

    struct file* file = malloc(sizeof(struct file));
    int i = open(cur, file, O_RDONLY);
    if (i == -1) {
        printf("===[ERROR] no such file!===\n");
        return;
    }
    read(file, test_wr, 1000);
    printf("%s\n", test_wr);

//    memset((void*) cur, 0, MAX_LENGTH);
//    memset((void*) test_wr, 0, MAX_BUFFER);

}

void cd_cmd(char* input) {
    if (input == NULL) {
        //set_pwd("rootfs");
        //printf("===[ERROR] cd parameter is NULL!===\n");
        set_cwd("rootfs/");
        return;
    }

    size_t i;

    char path[MAX_LENGTH];
    memset((void*) path, 0, MAX_LENGTH);

    get_cwd(path);

    if (!strcmp(input, "..")) {
        if (strlen(path) == 4) {
            printf("===[WARNING]: bin is already our root directory!===\n");
            return;
        }
        for (i = strlen(path) - 2; i >= 0; i--) {
            if (i == 0) {
                strcpy(path, "/");
                break;
            }
            if (path[i] == '/') {
                path[i + 1] = '\0';
                break;
            }
        }
        if(!strcmp(path, "rootfs/"))
        {
            set_cwd(path);
            return;
        }
        //strcpy(path, "/");

        //add check if exists here

    } else {
        size_t length = strlen(input);
        //trim off the last /
        if (input[length - 1] == '/') {
            input[length - 1] = '\0';
        }

        strcat(path, input);
        strcat(path, "/");

        //add check if exists here

    }

    cut_rootfs(path);
    void* tmp = opendir(path);
    if (tmp == NULL) {
        printf("===[ERROR] cd input is not a directory!===\n");
        return;
    }

    add_rootfs(path);
    set_cwd(path);

//    memset((void*) path, 0, MAX_LENGTH);
}

void pwd_cmd() {

    char pwd[MAX_LENGTH];
    memset((void*) pwd, 0, MAX_LENGTH);

    get_cwd(pwd);

    printf("%s\n", pwd);

//    memset((void*) pwd, 0, MAX_LENGTH);

}

void sh_cmd(char* param, char* envp[]) {

    if (param == NULL) {
        printf("===[ERROR] please enter file name!===\n");
        return;
    }

    char cur[MAX_LENGTH];
    memset((void*) cur, 0, MAX_LENGTH);

    get_cwd(cur);

    cut_rootfs(cur);

    strcat(cur, param);

    struct file* file = malloc(sizeof(struct file));
    int i = open(cur, file, O_RDONLY);

    if (i == -1) {
        printf("===[ERROR] no such script file!===\n");
        return;
    }

    char input[MAX_BUFFER];
    memset((void*) input, 0, MAX_BUFFER);

    int count = read(file, input, MAX_BUFFER);

//    if (input[0] == '#' && input[1] == '!') {
    if (!strncmp(input, "#!",2)) {
        char line[MAX_LENGTH];
        memset((void*) line, 0, MAX_LENGTH);

        int index = 0;

        // move i to the first letter of second line
        while (input[index++] != '\n')
            ;

        // parse command and execute
        while (index < count) {
            int i = read_line(input + index, line);
            executeCmd(line, envp);
            index += i + 1;
        }

//        while (strlen(input) > 2) {
//
//            input = strstr(input, "\n");
//            if (strlen(input) == 1) {
//                break;
//            }
//            strcpy(input, input + 1);
//            strcpy(line, input);
//            executeCmd(line, envp);
//        }

        memset((void*) line, 0, MAX_LENGTH);

    } else {
        printf("===[ERROR] Not a script file!===\n");
    }

//    memset((void*) cur, 0, MAX_LENGTH);
//    memset((void*) input, 0, MAX_BUFFER);

}

void sleep_cmd(char* param){
    int sleep_time = stoi(param);
    printf("sleep time is:%d\n", sleep_time);
    if (sleep_time > 0) {
        sleep(sleep_time);
    } else {
        printf("===[ERROR] invalid sleep time:%d !===\n", sleep_time);
    }
}

void executeBin(char* cmd, char* args[], char* envp[]) {
    int result = 0;
    result = execve(cmd, args, envp);

    if (result < 0) {
        printf("===[ERROR] cannot direct execute :%s !===\n", cmd);
        return;
    }
//    if(!strncmp(cmd,".",1)||!strncmp(cmd,"/",1)){// direct execute
//        result=execve(cmd, args, envp);
//
//        if(result<0){
//            printf("===[ERROR] cannot direct execute :%s !===\n",cmd);
//            return;
//        }
//    }else{
//        printf("===[ERROR] cannot execute :%s !===\n",cmd);

//        char* path[MAX_ARGS];
//        int n=parseInputToParams(env.PATH,path,':');//get cmd path in PATH
//        int i=0;
//        int times=n;
//        while(i<n){
//            char* fullPath=malloc(MAX_LENGTH*sizeof(char));
//            strcpy(fullPath, path[i]);
//            strcpy(fullPath+strlen(fullPath), "/");
//            strcpy(fullPath+strlen(fullPath), cmd);
//            if(execve(fullPath, args, envp)<0){
//                times--;
//            }else{
//                return;
//
//            }
//
//            //printf("(PATH) result is %d\n",result);
//
//            i++;
//            // if(result==0){
//            //  printf("(return) result is %d\n",result);
//            //  return result;
//            // }
//        }
//
//        if(times==0){
//            printf("===cannot execute :%s [error]!===\n",cmd);
//            printf("enter 'exit' to quit or other to continue!\n");
//        }
}

//printf("(return) result is %d\n",result);

//return result;

void executeCmd(char* input, char* envp[]) {

    int status = 0;
    char* args[MAX_ARGS];

    int n = parseInputToParams(input, args, ' ');

    char* cmd = args[0];
    char* param;
    if (n > 1) {
        param = args[1];
    } else {
        param = NULL;
    }

    int isBgJob = !strcmp(args[n - 1], "&");
    if (isBgJob) {
        args[n - 1] = NULL;
    }

    if (!strcmp(cmd, "ls")) {
        ls_cmd();
    } else if (!strcmp(cmd, "cat")) {
        cat_cmd(param);
    } else if (!strcmp(cmd, "cd")) {
        cd_cmd(param);
    } else if (!strcmp(cmd, "ps")) {
        ps_cmd();
    } else if (!strcmp(cmd, "pwd")) {
        pwd_cmd();
    } else if (!strcmp(cmd, "sh")) {
        sh_cmd(param, envp);
    } else if (!strcmp(cmd, "clear")) {
        clear_screen();
    } else if (!strcmp(cmd, "sleep")) {
        sleep_cmd(param);
    } else if (!strcmp(cmd, "exit")) {

    } else if (!strcmp(cmd, "help")) {

    } else {    //execute bin or executables

        char cur_dir[MAX_LENGTH];
        memset((void*) cur_dir, 0, MAX_LENGTH);
        get_cwd(cur_dir);
        cut_rootfs(cur_dir);
        strcat(cur_dir, args[0]);

        if (check_file(cur_dir) == -1) {
            printf("===[ERROR] not find executable %s !===\n", args[0]);
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            executeBin(cur_dir, args, envp);
            exit(0);
        } else if (pid > 0) {
            if (isBgJob) {
                printf("background job pid is:%d\n", pid);
            } else {
                waitpid(pid, &status, 0);
            }
        } else {
            printf("===[ERROR] fork failed!===\n");
        }

    }
//    else {
//
//        printf(
//                "===[ERROR] %s command not found! (Try `help' for more info.)===\n",
//                cmd);
//    }
}

int main(int argc, char* argv[], char* envp[]) {

    //getEnv(envp);
    //#ifdef DEBUG
    //printf("%s\t%s\t%s\n",envp[0],envp[1],envp[2]);
    //#endif
    //if(argc > 1) {
    //executeScript(argv[1],envp);
    //exit(0);
    //}
    char* input = malloc(MAX_LENGTH * sizeof(char));
    clear_screen();

    printf("---------------------------------------------------------------\n");
    printf("--------------Welcome! Thanks for using SBUINX!----------------\n");
    printf("------Copyright © 2015 by Junao Wang, Bo Feng, Qingqing Cao----\n");
    printf("---------------------All rights reserved-----------------------\n");
    printf("---------------------------------------------------------------\n");
    printf("\n");

    while (1) {

        shellPrompt();
        // char* input = malloc(1024);
        memset((void*) input, 0, MAX_LENGTH);

        int n = gets(input);	//
        /*int i=scanf("%[^\n]%*c",input);

         if(i==0){
         char tmp;
         scanf("%c",&tmp);
         continue;
         }*/
        //gets(input);
        //fgets(input,MAX_LENGTH,stdin);
//        printf("input length is:%d",n);
        if (n > 0)
            executeCmd(input, envp);

    }

}
