#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 1024

int main(int argc, char* argv[], char* envp[]) {

	printf("Hello World!\n");
	// int neg=-1;
	printf("argc is:%d\n",argc);
	int i;
	for(i=0;i<argc;i++){
		printf("argv[%d] is :%s\n",i,argv[i] );
		
	}

	for(i=0;i<argc;i++){
		printf("envp[%d] is :%s\n",i,envp[i] );
		
	}
	//lsCmd();
	// char* input=malloc(MAX_LENGTH*sizeof(char));
	// scanf("%s",input);	
	// int a=123,b=456,c=0,d=-21,e=-9341;
	// printf("a=%d,b=%d,c=%d,d=%d,e=%d\n",a,b,c,d,e );
	// printInteger(12345);
	// printf("\n");
	// printInteger(1);
	// printf("\n");
	// printInteger(10);
	// printf("\n");
	// printInteger(-12345);
	// printf("\n");
	// printInteger(0);
	// printf("\n");
	// printInteger(-10);
	// printf("\n");

	// if(!strcmp(input, "sbuls")){
		
	// }

	return 0;
}
