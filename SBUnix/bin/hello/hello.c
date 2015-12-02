#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
    printf("Hello World!\n");
    // int neg=-1;
    printf("argc is:%d\n",argc);
    int i;
    for(i=0;i<argc;i++){
        printf("argv[%d] is :%s\n",i,argv[i] );

    }

//    for(i=0;i<argc;i++){
//        if(envp[i]!=NULL){
//        printf("envp[%d] is :%s\n",i,envp[i] );
//        }
//
//    }

    return 0;
}
