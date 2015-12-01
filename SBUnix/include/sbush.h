#ifndef _SBUSH_H
#define _SBUSH_H

#include <sys/defs.h>

int parseInputToParams(char* input, char* param[], char sep) ;

void sh_cmd(char* param,char* envp[]);
void executeCmd(char* input, char* envp[]);

#endif
