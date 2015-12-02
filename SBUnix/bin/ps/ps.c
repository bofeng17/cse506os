#include <stdio.h>
#include <stdlib.h>
#include <sys/defs.h>

int main(int argc, char* argv[], char* envp[]) {

    ps_t ups = malloc(sizeof(ps_state));

    if (ups == NULL) {
        printf("===[ERROR] malloc failed,out of memory!===\n");
        return 0;
    }
    int no = ps(ups);

    int i = 0;
    printf("PID        STATE     NAME\n"); //name field is 14 char length, add space to keep aligned
    for (i = 0; i < no; i++) {
        printf("%d          %s%s\n", ups->id[i], ups->state[i], ups->name[i]);
    }

    return 0;
}
