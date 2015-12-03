#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>
#include <sbush.h>

#define MAX_ARGS 20
#define MAX_NAME 20

#define MAX_LENGTH 100
#define MAX_BUFFER 1024

struct ENV {
    char USER[MAX_NAME];
    char HOME[MAX_NAME];
    char PATH[MAX_NAME];
    char PS1[MAX_NAME];
} env; //global variale for shell environment

void findEnvStr(char* env[], char* var, char* ret) {
    int i = 0;
    if (env && var && ret) {
        while (*env[i]) {
            if (!strncmp(env[i], var, strlen(var))) {
                strcpy(ret, env[i] + strlen(var) + 1); //escape '='
                return;
            }
            i++;
        }
    } else {
        printf("===[ERROR] parameters null!===\n");
        return;
    }

}

void getEnv(char* envp[]) {
    char* user = "USER";
    char* home = "HOME";
    char* path = "PATH";

    findEnvStr(envp, user, env.USER);
    findEnvStr(envp, home, env.HOME);
    findEnvStr(envp, path, env.PATH);

    // env.PS1 = env.USER;
    strcpy(env.PS1, env.USER);
    strcpy(env.PS1 + strlen(env.PS1), "@[SBUnix]");
//#ifdef DEBUG
//    printf("PS1 is :%s\n",env.PS1);
//    printf("USER is :%s\n",env.USER);
//    printf("HOME is :%s\n",env.HOME);
//    printf("PATH is :%s\n",env.PATH);
//#endif
}

char tmp[MAX_LENGTH];

void cut_rootfs(char* name) {
    size_t size = strlen(name);

    memset((void*) tmp, 0, MAX_LENGTH);
    // char* tmp = malloc(30*sizeof(char));

    //char tmp[MAX_LENGTH];
    //memset((void*) tmp, 0, MAX_LENGTH);

    strcpy(tmp, name + 7);
    memset((void*) name, 0, size);

    strcpy(name, tmp);

    //return tmp;
}

void add_rootfs(char* name) {
    //char* tmp = malloc(30*sizeof(char));
    size_t size = strlen(name);
    memset((void*) tmp, 0, MAX_LENGTH);
    //char tmp[MAX_LENGTH];
    // memset((void*) tmp, 0, MAX_LENGTH);

    strcpy(tmp, "rootfs/");
    strcat(tmp, name);
    memset((void*) name, 0, size);
    strcpy(name, tmp);

    //return tmp;
}

void shellPrompt() {

    char cur_dir[MAX_LENGTH];
    memset((void*) cur_dir, 0, MAX_LENGTH);

    get_cwd(cur_dir);
//    cut_rootfs(cur_dir);

//    char root_dir[MAX_LENGTH];
//    memset((void*) root_dir, 0, MAX_LENGTH);
//
//    strcpy(root_dir, "rootfs/");
//    strcat(root_dir, cur_dir);

    if (!strncmp(cur_dir, env.HOME, strlen(env.HOME))) {
        printf("%s:~%s$ ", env.PS1, cur_dir + strlen(env.HOME));
    } else {
        printf("%s:%s$ ", env.PS1, cur_dir);
    }

    //printf("=>root@SBUINX:%s# ", root_dir);

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
        if (!strcmp(path, "rootfs/")) {
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
    if (!strncmp(input, "#!", 2)) {
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

void executeBin(char* cmd, char* args[], char* envp[]) {
    int result = 0;
    result = execve(cmd, args, envp);

    if (result < 0) {
        printf("===[ERROR] cannot direct execute :%s !===\n", cmd);
        return;
    }

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
//
//    if (!strcmp(cmd, "ls")) {
//        ls_cmd();
//    } else
//    if (!strcmp(cmd, "cat")) {
//        cat_cmd(param);
//    } else
    if (!strcmp(cmd, "cd")) {
        cd_cmd(param);
    }
//    else if (!strcmp(cmd, "ps")) {
//        ps_cmd();
//    }
    else if (!strcmp(cmd, "pwd")) {
        pwd_cmd();
    } else if (!strcmp(cmd, "sh")) {
        sh_cmd(param, envp);
    } else if (!strcmp(cmd, "clear")) {
        clear_screen();
    } else if (!strcmp(cmd, "exit")) {
        printf("===[WARNING] current session will exit in 3s ! ===\n");
        sleep(3);
        exit(0);
    } else if (!strcmp(cmd, "help")) {

    } else {    //execute bin or executables
        char full_path[MAX_LENGTH];
        memset((void*) full_path, 0, MAX_LENGTH);
        if (strncmp(args[0], "bin/", 4)) {

            get_cwd(full_path);
            cut_rootfs(full_path);

            strcat(full_path, args[0]);

        } else {
            strcpy(full_path, args[0]);
        }
//
        if (check_file(full_path) == -1) {

            char* path[MAX_ARGS];
            int n = parseInputToParams(env.PATH, path, ':'); //get cmd path in PATH
            int i = 0;
            int times = n;
//        char full_path[MAX_LENGTH];
            while (i < n) {
                strcpy(full_path, path[i]);
                strcpy(full_path + strlen(full_path), "/");
                strcpy(full_path + strlen(full_path), cmd);

                if (check_file(full_path) < 0) {
                    times--;
                } else {
                    break;
                }
                i++;
            }

            if (times == 0) {
                printf("===[ERROR] not find executable :%s !===\n", cmd);
                return;
            }
        }
        pid_t pid = fork();
        if (pid == 0) {
            executeBin(full_path, args, envp);
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

    getEnv(envp);
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
