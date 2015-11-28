#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {

    ps_t ups= malloc(sizeof(ps_state));

    int no=ps(ups);

    int i=0;
    printf("PID        NAME        STATE\n");
    for(i=0;i<no;i++){
        printf("%d        %s        %s\n",ups->id[i],ups->name[i],ups->state[i]);
    }

    return 0;
}
