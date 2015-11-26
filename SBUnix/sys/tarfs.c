#include <sys/sbunix.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/process.h>
#include <sys/virmm.h>
#include <sys/tarfs.h>
#include <sys/string.h>

typedef struct file tarfs_file;

/*void *memmove(void *dest, const void *src, size_t n)
 {
	const char *s;
	char *d;
 
	s = src;
	d = dest;
	if (s < d && s + n > d) {
 s += n;
 d += n;
 while (n-- > 0)
 *--d = *--s;
	} else {
 while (n-- > 0)
 *d++ = *s++;
	}
 
	return dest;
 }*/



size_t get_size_oct(char* string)
{
    int i;
    int oct=1;
    int result=0;
    for(i=0;i<12;i++)
    {
        result=result+(string[10-i]-'0')*oct;
        oct=oct*8;
    }
    
    return result;
}


static inline struct posix_header_ustar *tarfs_next_header(void *ptr, size_t size)
{
    return ptr + ((size + 512 - 1)/512 + 1) * 512;
}

static inline struct posix_header_ustar *tarfs_header_walk(struct posix_header_ustar *hdr)
{
    uint64_t size = get_size_oct(hdr->size);
    //printf("================================TARFS size: %x\n", size);
    //struct posix_header_ustar *new = (void *)hdr +((size + 512 - 1)/512 + 1) * 512;
    struct posix_header_ustar *new = (void *)hdr +((size + 511)/512 + 1)*512;
    
    return new;
}


// prototype of do_open, which has two parameters
tarfs_file* tarfs_open(char *name, int flags)
{
    if(flags == O_RDONLY)
    {
    struct posix_header_ustar *header_start=(struct posix_header_ustar*)&_binary_tarfs_start;
    uint64_t size;
    //printf("im in tarfs open 1\n");
    if(name==NULL)
    {
        printf("ERROR: provided file name is null\n");
        //printf("im in tarfs open 2\n");
        return NULL;
    }
    
    while(header_start<(struct posix_header_ustar*)&_binary_tarfs_end)
    {
        //printf("im in tarfs open 3\n");
        size=get_size_oct(header_start->size);
        
        //printf("size= %d\n", size);
        
        //if(!tarfs_namencmp(header_start->name, name, sizeof(header_start->name)))
        if(!strcmp(header_start->name, name))//here may be a bug in future,using strcmp
        {
            //printf("im in tarfs open 4\n");
            printf("size= %d\n", size);
            tarfs_file* file=(tarfs_file*)kmalloc(1);
            file->file_header=header_start;
            file->size=size;
            
            file->start=(void*) header_start+512;
            
            return file;
        }
        //printf("im in tarfs open 5\n");
        //header_start=tarfs_next_header(header_start, size);
        header_start=(struct posix_header_ustar*)((void*)header_start+((size+511)/512 + 1)*512);
        
        
    }
    
    //printf("im in tarfs open 6\n");
    
    printf("ERROR: tarfs open file failed\n");
    return NULL;
    }
    printf("we only support O_RDONLY flags\n");
    return NULL;
    
}

size_t tarfs_read(tarfs_file* fd, void* buf, size_t size)
{
    size_t i;
    char* tmp1,*tmp2;
    //printf("im in tarfs read 1\n");
    if(fd->size==0)
    {
        printf("WARNING: the file is empty\n");
        return 0;
    }
    
    if(fd->size<size)
    {
        //printf("im in tarfs read 2\n");
        size=fd->size;
    }
    
    tmp1=(char*)fd->start;
    tmp2=buf;
    for(i=0;i<size;i++)
    {
        //printf("im in tarfs read 3\n");
        //buf=fd->start;
        *tmp2++=*tmp1++;
    }
    //memmove(buf, fd->start, size);
    
    
    //printf("im in tarfs read 4\n");
    
    fd->start+=size;
    
    return size;
    
}

void tarfs_close(tarfs_file* fd)
{
    kfree(fd, 1);
}

void* find_file(char* filename)
{
    struct posix_header_ustar *header_start=(struct posix_header_ustar*)&_binary_tarfs_start;
    uint64_t size;
    
    if(filename==NULL)
    {
        printf("ERROR: provided file name is null\n");
        return NULL;
    }
    
    while(header_start<(struct posix_header_ustar*)&_binary_tarfs_end)
    {
        
        size=get_size_oct(header_start->size);
        if(!strcmp(header_start->name, filename))//here may be a bug in future,using strcmp
        {
            header_start=(struct posix_header_ustar*)header_start+1;
            return (void*)header_start;
        }
        
        header_start=(struct posix_header_ustar*)((void*)header_start+((size+511)/512 + 1)*512);
    }
    
    return NULL;
    
}

void tarfs_test()
{
    
    struct posix_header_ustar *p = (struct posix_header_ustar *)&_binary_tarfs_start;
    tarfs_file *fp;
    char temp[100];
    size_t len;
    printf("im in tarfs test\n");
    
    //while(p < (struct posix_header_ustar *)&_binary_tarfs_end)
    
    
    printf("================================TARFS name: %s\n", p->name);
    //p=tarfs_header_walk(p);
    //uint64_t size1=get_size_oct(p->size);
    p=(void*)p+(511/512 + 1)*512;
    //p=tarfs_header_walk(p);
    printf("================================TARFS name: %s\n", p->name);
    p=tarfs_header_walk(p);
    printf("================================TARFS name: %s\n", p->name);
    printf("================================TARFS size: %s\n", p->size);
    uint64_t size_ = get_size_oct(p->size);
    printf("================================TARFS size: %d\n", size_);
    
    
    fp = tarfs_open("bin/hello", O_RDONLY);
    memset(temp, 0, sizeof(temp));
    len = tarfs_read(fp,temp,50);
    printf("test tarfs: %d\n", len);
    printf("test tarfs: %s\n", temp);
    
    return;
}

// write syscall service routine
ssize_t do_write(int fd, const void *buf, size_t count) {
    if (fd == STD_OUT || fd == STD_ERR) {
        // TODO: return value
        return terminal_write(fd, (char *)buf, count);
    } else {
        // write to file
        printf("Don't Support write to file\n");
        return -1;
    }
}