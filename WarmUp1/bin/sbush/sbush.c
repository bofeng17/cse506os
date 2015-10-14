#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
//#define DEBUG TRUE
#define MAX_LENGTH 4096
#define MAX_ARGS 20

struct ENV{
	char* USER;
	char* HOME;
	char* PATH;
	char* PS1;
}env;//global variale for shell environment

char* getCurDir(){
	char* dir;
	dir=malloc(MAX_LENGTH);
	if(getcwd(dir,MAX_LENGTH)==NULL){ // use system call getcwd()
		printf("===cannot get current directory [error]!===\n");
		exit(0);
	}
	return dir;
}

char* findEnvStr(char* env[],char* var){
	int i = 0;
	while(*env[i]) {
		if(!strncmp(env[i], var, strlen(var))) {
		#ifdef DEBUG
			printf("var is:  %s\n",var);
		#endif
			return env[i]+strlen(var)+1;//escape '='
		}
		i++;
	}
	return NULL;
}

void getEnv(char* envp[]){
	char* user="USER";
	char* home="HOME";
	char* path="PATH";
	
	env.USER=findEnvStr(envp,user);
	env.HOME=findEnvStr(envp,home);
	env.PATH=findEnvStr(envp,path);
	
	env.PS1=env.USER;
	//strcpy(env.PS1,env.USER);
	strcpy(env.PS1+strlen(env.PS1), "@[sbush-qicao]");
#ifdef DEBUG
	printf("PS1 is :%s\n",env.PS1);
	printf("USER is :%s\n",env.USER);
	printf("HOME is :%s\n",env.HOME);
	printf("PATH is :%s\n",env.PATH);
#endif	
}

void shellPrompt(){

	char* curDir=getCurDir();

	if(!strncmp(curDir, env.HOME, strlen(env.HOME))) {
		printf("%s:~%s$ ",env.PS1, curDir+strlen(env.HOME));
	} else {
		printf("%s:%s$ ", env.PS1, curDir);
	}
	
}

int getdents(uint32_t fd, struct dirent *dirp, uint32_t count) ;
void lsCmd(){
	char* dir=getCurDir();
	struct dirent *sd; 
	void *dp=opendir(dir); 

	int BUF_SIZE=1024,nread=0;
	char buf[BUF_SIZE];
	int pos=0;
	
	while(1){
		nread = getdents(*((int *)dp), (struct dirent *)buf, BUF_SIZE);
		if(nread<=0)
			break;
		for(pos=0;pos<nread;){
			sd=(struct dirent *)(buf+pos);
			if(strncmp(sd->d_name,".",1))
			printf("%s  ", sd->d_name);
			pos+=sd->d_reclen;
			
		}
		printf("\n");
	}
	closedir(dp);
}

void catCmd(char* scriptname){
	if(scriptname==NULL){
		printf("===please input file name!===\n");
		return;
	}
	int fd=0;
	char* buffer=malloc(MAX_LENGTH);
	if((fd = open(scriptname, O_RDONLY)) < 0) {
		printf("===cannot open file:%s [error]!===\n",scriptname);
		printf("enter 'exit' to quit or other to continue!\n");
		return;
	}
	char ch;
	int i=0;
	while(1) {		
		if(read(fd, &ch, 1) == 0) {
			break;
		}
		buffer[i++]=ch;
		if(ch == '\n') {
			buffer[i]='\0';
			printf("%s",buffer);
			i=0;
			memset(buffer,0,MAX_LENGTH);
		}
	}
	close(fd);	
}	

void cdBuiltin(char* param){
	if(param == NULL) {
		chdir(env.HOME);
	}
	else if(chdir(param)!=0){
		printf("===change to directory :%s [error]!===\n",param);
		printf("enter 'exit' to quit or other to continue!\n");
	}
}

int indexOfEq(char* param){
	if(param==NULL){
		return -1;
	}
	int index=0;
	while(param[index]!='='){
		index++;
		if(param[index]=='\0')
			return -1;
	}
	return index;
}

void setEnv(char* env,char* var){
	memset(env,0,sizeof(env));
	strcpy(env,var);
}

void exportBuiltin(char* param){
	int index=indexOfEq(param);
	if(index<0){
		printf("===Invalid env params:%s [error]!===\n",param==NULL?"NULL":param);	
		printf("enter 'exit' to quit or other to continue!\n");	
		return;
	}
	char* varType=malloc(sizeof(char)*index);
	strncpy(varType,param,index);
	
	char* varVal=param+index+1;
	#ifdef DEBUG
	printf("type is: %s\n",varType);
	printf("value is: %s\n",varVal);
	#endif
	
	if(!strcmp(varType,"HOME")){
		setEnv(env.HOME,varVal);
	}else if(!strcmp(varType,"PATH")){
		setEnv(env.PATH,varVal);
	}else if(!strcmp(varType,"PS1")){
		setEnv(env.PS1,varVal);
	}else{
		printf("===Invalid env params:%s [error]!===\n",param);	
		printf("enter 'exit' to quit or other to continue!\n");
	}
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
	#ifdef DEBUG
	int i=0;
	for(i=0;i<count;i++)
		printf("(parseInputToParams) arg %d is :%s\n",i,param[i]);

	printf("count is: %d\n",count);
	#endif	
return count;
}

void executeBin(char* cmd,char* args[],char* envp[]){
	int result=0;
	if(!strncmp(cmd,".",1)||!strncmp(cmd,"/",1)){// direct execute
		result=execve(cmd, args, envp);	
		#ifdef DEBUG
		printf("(direct) result is %d\n",result);
		#endif
		if(result<0){
			printf("===cannot execute :%s [error]!===\n",cmd);
			printf("enter 'exit' to quit or other to continue!\n");
			return;
		}
	}else{
		char* path[MAX_ARGS];
		int n=parseInputToParams(env.PATH,path,':');//get cmd path in PATH
		int i=0;
		int times=n;
		while(i<n){
			char* fullPath=malloc(MAX_LENGTH*sizeof(char));
			strcpy(fullPath, path[i]);
			strcpy(fullPath+strlen(fullPath), "/");
			strcpy(fullPath+strlen(fullPath), cmd);
			if(execve(fullPath, args, envp)<0){
				times--;
				#ifdef DEBUG
				printf("(PATH) failed this time %d!\n",i);
				#endif
			}else{
				return;
				#ifdef DEBUG
				printf("(PATH) result succeed!\n");
				#endif
			}
			
			//printf("(PATH) result is %d\n",result);
			
			i++;
			// if(result==0){
			// 	printf("(return) result is %d\n",result);
			// 	return result;
			// }
		}	

		if(times==0){
			printf("===cannot execute :%s [error]!===\n",cmd);
			printf("enter 'exit' to quit or other to continue!\n");
		}
	}


	//printf("(return) result is %d\n",result);
	

	//return result;
}

void executeCmd(char* input,char* envp[]){
	int pid=0;
	int status=0;
	char* args[MAX_ARGS];

	int n=parseInputToParams(input,args,' ');

	char* cmd=args[0];
	char* param;
	if(n>1){
		param=args[1];
	}else{
		param=NULL;
	}
	
	
	int isBgJob=!strcmp(args[n-1],"&");
	if(isBgJob){
		args[n-1]=NULL;
	}
	//judge cmd type
	if(!strcmp(cmd, "exit")) {
		exit(0);
	}else if(!strcmp(cmd, "sbuls")) {
		lsCmd();
	}else if(!strcmp(cmd, "sbucat")) {
		catCmd(param);
	}else if(!strcmp(cmd, "pwd")) {
		printf("%s\n",getCurDir());
	}else if(!strcmp(cmd, "cd")) {
		cdBuiltin(param);
	}else if(!strcmp(cmd, "export")) {
		exportBuiltin(param);
	}else {//execute bin or executables
		pid = fork();
		if(pid == 0) {
			executeBin(args[0],args,envp);
			exit(0);
		}else{
			if(isBgJob){
				printf("backgroud job pid is:%d\n",pid);
			}else{
				waitpid(pid, &status, 0);
			}
		}			
	}
}

void executeScript(char* scriptname,char* envp[]){
	int fd=0,pid=0,status=0;
	char* buffer=malloc(MAX_LENGTH);
	char* input=malloc(MAX_LENGTH);
	if((fd = open(scriptname, O_RDONLY)) < 0) {
		printf("===cannot open file:%s [error]!===\n",scriptname);
		printf("enter 'exit' to quit or other to continue!\n");
	}

	char ch;
	int i=0;
	while(1) {
		
		if(read(fd, &ch, 1) == 0) {
			#ifdef DEBUG	
			printf("not read file\n");
			#endif
			break;
		}
		buffer[i++]=ch;
		#ifdef DEBUG			
		printf("read a char:%c\n",buffer[i]);
		#endif
		if(ch == '\n') {
			input=buffer;
			buffer[i]='\0';
			#ifdef DEBUG
			printf("ch is:%c\n",buffer[0]);
			printf("Input is: %s\n",input);
			#endif
			pid = fork();
			if(pid == 0) {
				//printf("Input is:%s\n",buffer);
				executeCmd(input,envp);
				exit(0);
			} else {
				waitpid(pid, &status, 0);
			}
			//buffer=malloc(MAX_LENGTH);
			//memset(buffer,0,sizeof(buffer));
			//printf("i is:%d\n",i);
			i=0;
		}
		//i++;
	}

	close(fd);
}

int main(int argc, char* argv[],char* envp[]) {

	getEnv(envp);
	#ifdef DEBUG
	printf("%s\t%s\t%s\n",envp[0],envp[1],envp[2]);
	#endif
	if(argc > 1) {
		executeScript(argv[1],envp);
		exit(0);
	}

	while(TRUE){

		shellPrompt();
		char* input=malloc(MAX_LENGTH);
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

		executeCmd(input,envp);
		
	}

}
