#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/tarfs.h>

//struct dirent *a;
int
main (int argc, char* argv[], char* envp[])
{
	int i;

	//char* tmp = get_cwd();
	char* direct = malloc(sizeof(char));

	get_cwd(direct);

	printf("TESTING GET_CWD: %s \n", direct);

	struct dirent* a = malloc(sizeof(struct dirent));
	//printf("dirent size: %d\n", sizeof(struct dirent));


	void* b = opendir(direct);
	
	readdir(b, a);

	for(i=0; i<a->num; i++)
	{
	printf("%s\n", a[i].name);
}
	
	
}
