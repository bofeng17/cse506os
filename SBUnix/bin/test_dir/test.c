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
	//char temp[100];
	//size_t len=0;
	int i;

	/*struct file* new_file = open("bin/hello", O_RDONLY);
	len = read(new_file,temp,50);

	printf("test tarfs: %d\n", len);
	printf("test tarfs: %s\n", temp);
	close(new_file);*/
	int* t=malloc(sizeof(int));
	t[0]=1;
	printf("t[0]: %d\n", t[0]);

	char* direct = malloc(sizeof(char));

	get_cwd(direct);

	printf("TESTING GET_CWD: %s \n", direct);

	struct dirent* a = malloc(sizeof(struct dirent));
	printf("dirent size: %d\n", sizeof(struct dirent));


	void* b = opendir("bin/");
	
	readdir(b, a);

	for(i=0; i<a->num; i++)
	{
	printf("READDIR TESTING!!!====== %s\n", a[i].name);
}
	
	
}

