#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>
#include <sbush.h>

#define MAX_ARGS 20


void shellPrompt() {

    char* cur_dir = malloc(10 * sizeof(char));

    get_cwd(cur_dir);
    char* root_dir = malloc(20 * sizeof(char));
    strcpy(root_dir, "rootfs/");
    strcat(root_dir, cur_dir);

    printf("root@SBUINX:%s#", root_dir);

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

    int no = ps(ups);

    int i = 0;
    printf("PID        STATE     NAME\n"); //name field is 14 char length, add space to keep aligned
    for (i = 0; i < no; i++) {
        printf("%d          %s%s\n", ups->id[i], ups->state[i], ups->name[i]);
    }
}

void ls_cmd() {
    int i;

    char* direct = malloc(sizeof(char));

    get_cwd(direct);

    size_t length = strlen(direct);
    //printf("TESTING GET_CWD: %s \n", direct);

    struct dirent* a = malloc(sizeof(struct dirent));
    //printf("dirent size: %d\n", sizeof(struct dirent));

    void* b = opendir(direct);


    readdir(b, a);
    char* final_name = malloc(30 * sizeof(char));

    for (i = 0; i < a->num; i++) {
        strcpy(final_name, a[i].name + length);

        printf("%s\n", final_name);
    }
}

void cat_cmd(char* input) {
    if (input == NULL) {
        printf("===[ERROR] please enter file name!===\n");
        return;
    }

    char* cur = malloc(30 * sizeof(char));

    get_cwd(cur);

    strcat(cur, input);

    //char* input_filename = malloc(sizeof(char));
    //scanf("%s", input_filename);
    char* test_wr = malloc(sizeof(char));
    struct file* file = open(cur, O_RDONLY);
    if (file == NULL) {
        printf("===[ERROR] no such file!===\n");
        return;
    }
    read(file, test_wr, 1000);
    printf("%s\n", test_wr);
}

void cd_cmd(char* input) {
    if (input == NULL) {
        //set_pwd("rootfs");
        printf("===[ERROR] cd parameter is NULL!===\n");
        return;
    }
    size_t i;
    char* path = malloc(30 * sizeof(char));

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

    void* tmp = opendir(path);
    if (tmp == NULL) {
        printf("===[ERROR] cd input is not a directory!===\n");
        return;
    }

    set_cwd(path);

}


void pwd_cmd() {

    char* pwd = malloc(30 * sizeof(char));

    get_cwd(pwd);

    printf("%s\n", pwd);

}

void sh_cmd(char* param,char* envp[])
{

   if (param == NULL) {
       printf("===[ERROR] please enter file name!===\n");
       return;
   }

   char* cur = malloc(30 * sizeof(char));

   get_cwd(cur);

   strcat(cur, param);

   struct file* file = open(cur, O_RDONLY);

   if (file == NULL) {
       printf("===[ERROR] no such script file!===\n");
       return;
   }

   char* input = malloc(30*sizeof(char));

   read(file, input, 1000);

   if(input[0]=='#' && input[1]=='!')
   {

       char* tmp = malloc(30*sizeof(char));

       while(strlen(input)>2){

       input = strstr(input, "\n");
       if(strlen(input)==1)
       {
           break;
       }
       strcpy(input, input+1);
       strcpy(tmp, input);
       executeCmd(tmp,envp);
       }

   }
   else
   {
       printf("===[ERROR] Not a script file!===\n");
   }

}

void executeBin(char* cmd,char* args[],char* envp[]){
    int result=0;
    if(!strncmp(cmd,".",1)||!strncmp(cmd,"/",1)){// direct execute
        result=execve(cmd, args, envp);

        if(result<0){
            printf("===[ERROR] cannot direct execute :%s !===\n",cmd);
            return;
        }
    }else{
        printf("===[ERROR] cannot execute :%s !===\n",cmd);

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
}

void executeCmd(char* input,char* envp[]) {

    int status=0;
    char* args[MAX_ARGS];

    int n = parseInputToParams(input, args, ' ');

    char* cmd = args[0];
    char* param;
    if (n > 1) {
        param = args[1];
    } else {
        param = NULL;
    }

    int isBgJob=!strcmp(args[n-1],"&");
    if(isBgJob){
        args[n-1]=NULL;
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
    }else if (!strcmp(cmd, "sh")) {
        sh_cmd(param,envp);
    } else if (!strcmp(cmd, "help")) {

    }else {//execute bin or executables
       pid_t pid = fork();
        if(pid == 0) {
            executeBin(args[0],args,envp);
            exit(0);
        }else if(pid>0){
            if(isBgJob){
                printf("backgroud job pid is:%d\n",pid);
            }else{
                waitpid(pid, &status, 0);
            }
        }else{
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

    printf("---------------------------------------------------------------\n");
    printf("--------------Welcome! Thanks for using SBUINX!----------------\n");
    printf("------Copyright © 2015 by Junao Wang, Bo Feng, Qingqing Cao----\n");
    printf("---------------------All rights reserved-----------------------\n");
    printf("---------------------------------------------------------------\n");
    printf("\n");

    while (1) {

        shellPrompt();
        char* input = malloc(1024);
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
            executeCmd(input,envp);

    }

}
