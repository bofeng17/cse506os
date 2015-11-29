#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>

#define MAX_ARGS 20

void shellPrompt(){

	char* cur_dir=malloc(10*sizeof(char));

	get_cwd(cur_dir);
	char* root_dir =malloc(20*sizeof(char));
 	strcpy(root_dir,"rootfs/");
	strcat(root_dir, cur_dir);

	printf("Root @ SBUINX: %s", root_dir);
	
}

void ls_cmd()
{
	int i;

	
	char* direct = malloc(sizeof(char));

	get_cwd(direct);

	//printf("TESTING GET_CWD: %s \n", direct);

	struct dirent* a = malloc(sizeof(struct dirent));
	//printf("dirent size: %d\n", sizeof(struct dirent));


	void* b = opendir(direct);
	
	readdir(b, a);

	for(i=0; i<a->num; i++)
	{
	printf("%s\n", a[i].name);
}
}

void cat_cmd(char* input)
{
	if(input==NULL){
		printf("===please input file name!===\n");
		return;
	}


	char* cur = malloc(30*sizeof(char));

	get_cwd(cur);

	strcat(cur, input);

   //char* input_filename = malloc(sizeof(char));
   //scanf("%s", input_filename);
   char* test_wr=malloc(sizeof(char));
   struct file* file = open(cur, O_RDONLY);
    if (file == NULL) {
   	 printf("no such file exists!!!");
     return ;
     }
   read(file, test_wr, 1000);
   printf("%s\n", test_wr);
}


void cd_cmd(char* input)
{
	/*if(input == NULL) {
		set_pwd("rootfs");
	}*/
	size_t i;
	char* path = malloc(30*sizeof(char));

	get_cwd(path);

	if(!strcmp(input, ".."))
	{
		for(i=strlen(path)-1; i>=0; i--) {
			if(i == 0) {
				strcpy(path, "/");
				break;
			}
			if(path[i] == '/') {
				path[i] = '\0';
				break;
			}
		}

		//add check if exists here

	}
	else {
	size_t length =strlen(input);
	  //trim off the last /
	if(input[length-1]== '/')
	{
		input[length-1] = '\0';
	}

	
	strcat(path, input);
	strcat(path, "/");


	//add check if exists here

	}

	set_cwd(path);


}



int parseInputToParams(char* input,char* param[],char sep){
	char ch;
	int status=0;//0 indicates current char is a space and 1 not
	int count=0;
	while(*input){
		ch=*input;
		if(ch==sep&&status==0){
			status=0;			//  ls
		}						// ^
		if(ch==sep&&status==1){
			*input='\0';
			status=0;			//ls -a
		}						//  ^
		if(ch!=sep&&status==0){
			//args[count]=malloc(sizeof(input));
			param[count++]=input;
			status=1;			//ls -a
		}						//  ^
		if(ch!=sep&&status==1){
			status=1;			//pwd
		}						// ^
		input++;
		if(*input=='\n')
			*input='\0';
	}
	param[count]=NULL;
	/*#ifdef DEBUG
	int i=0;
	for(i=0;i<count;i++)
		printf("(parseInputToParams) arg %d is :%s\n",i,param[i]);

	printf("count is: %d\n",count);
	#endif	*/
return count;
}

void executeCmd(char* input)
{

	//int pid=0;
	//int status=0;
	char* args[MAX_ARGS];

	int n=parseInputToParams(input,args,' ');

	//parseInputToParams(input,args,' ');
	char* cmd=args[0];
	char* param;
	if(n>1){
		param=args[1];
	}else{
		param=NULL;
	}
	
	if(!strcmp(cmd, "ls")) 
	{
		ls_cmd();
	}
	else if(!strcmp(cmd, "cat"))
	{
		cat_cmd(param);
	}
	else if(!strcmp(cmd, "cd"))
	{
		cd_cmd(param);
	}
	else 
	{
		printf("go die! \n");
    }
}

	


int main(int argc, char* argv[],char* envp[]) {

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

	while(1){

		shellPrompt();
		char* input=malloc(1024);
		scanf("%s",input);//different from standard scanf which ignores space!
		/*int i=scanf("%[^\n]%*c",input);
		
		if(i==0){
			char tmp;
			scanf("%c",&tmp);
			continue;
		}*/
		//gets(input);
		//fgets(input,MAX_LENGTH,stdin);
		
		#ifdef DEBUG
		
		printf("(main) input is: %s\n",input);
		#endif

		executeCmd(input);
		
	}

}