#include <sys/sbunix.h>
#include <sys/stdlib.h>
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

int contain_slash(char* name) {
    int i;
    int length = strlen(name);

    for (i = 0; i < length; i++) {
        if (name[i] == '/') {
            return 1;
        }
    }

    return 0;

}

size_t get_size_oct(char* string) {
    int i;
    int oct = 1;
    int result = 0;
    for (i = 0; i < 12; i++) {
        result = result + (string[10 - i] - '0') * oct;
        oct = oct * 8;
    }

    return result;
}

static inline struct posix_header_ustar *tarfs_header_walk(
        struct posix_header_ustar *hdr) {
    uint64_t size = get_size_oct(hdr->size);
    //printf("================================TARFS size: %x\n", size);
    //struct posix_header_ustar *new = (void *)hdr +((size + 512 - 1)/512 + 1) * 512;
    struct posix_header_ustar *new = (void *) hdr
            + ((size + 511) / 512 + 1) * 512;

    return new;
}

// prototype of do_open, which has two parameters
tarfs_file* tarfs_open(char *name, int flags) {
    if (flags == O_RDONLY) {
        struct posix_header_ustar *header_start =
                (struct posix_header_ustar*) &_binary_tarfs_start;
        uint64_t size;
        //printf("im in tarfs open 1\n");
        if (name == NULL) {
            dprintf("ERROR: provided file name is null\n");
            //printf("im in tarfs open 2\n");
            return NULL;
        }

        while (header_start < (struct posix_header_ustar*) &_binary_tarfs_end) {
            //printf("im in tarfs open 3\n");
            size = get_size_oct(header_start->size);

            //printf("size= %d\n", size);

            //if(!tarfs_namencmp(header_start->name, name, sizeof(header_start->name)))
            if (!strcmp(header_start->name, name)) //here may be a bug in future,using strcmp
                    {
                //printf("im in tarfs open 4\n");
                //dprintf("size= %d\n", size);
                tarfs_file* file = (tarfs_file*) kmalloc(1);
                file->file_header = header_start;
                file->size = size;

                file->start = (void*) header_start + 512;

                return file;
            }
            //printf("im in tarfs open 5\n");
            //header_start=tarfs_next_header(header_start, size);
            header_start = (struct posix_header_ustar*) ((void*) header_start
                    + ((size + 511) / 512 + 1) * 512);

        }

        //printf("im in tarfs open 6\n");

        dprintf("ERROR: tarfs open file failed\n");
    }
    dprintf("we only support O_RDONLY flags\n");
    return NULL;

}

size_t tarfs_read(struct file *fd, void* buf, size_t size) {
    size_t i;
    char* tmp1, *tmp2;
    //printf("im in tarfs read 1\n");
    if (fd->size == 0) {
        printf("WARNING: the file is empty\n");
        return 0;
    }

    if (fd->size < size) {
        //printf("im in tarfs read 2\n");
        size = fd->size;
    }

    tmp1 = (char*) fd->start;
    tmp2 = buf;
    for (i = 0; i < size; i++) {
        //printf("im in tarfs read 3\n");
        //buf=fd->start;
        *tmp2++ = *tmp1++;
    }
    //memmove(buf, fd->start, size);

    //printf("im in tarfs read 4\n");

    fd->start += size;

    return size;

}

void tarfs_close(tarfs_file* fd) {
    kfree(fd, 1);
}

void* find_file(char* filename) {
    struct posix_header_ustar *header_start =
            (struct posix_header_ustar*) &_binary_tarfs_start;
    uint64_t size;

    if (filename == NULL) {
        dprintf("ERROR: provided file name is null\n");
        return NULL;
    }

    while (header_start < (struct posix_header_ustar*) &_binary_tarfs_end) {

        size = get_size_oct(header_start->size);
        if (!strcmp(header_start->name, filename)) //here may be a bug in future,using strcmp
                {
            header_start = (struct posix_header_ustar*) header_start + 1;
            return (void*) header_start;
        }

        header_start = (struct posix_header_ustar*) ((void*) header_start
                + ((size + 511) / 512 + 1) * 512);
    }

    return NULL;

}

void* do_opendir(const char* name) {
    struct posix_header_ustar *header_start =
            (struct posix_header_ustar*) &_binary_tarfs_start;
    uint64_t size;

    if (name == NULL) {
        dprintf("ERROR: provided file name is null\n");
        return NULL;
    }

    while (header_start < (struct posix_header_ustar*) &_binary_tarfs_end) {

        size = get_size_oct(header_start->size);
        if ((!strcmp(header_start->name, name))
                & (!strcmp(header_start->typeflag, "5"))) //here may be a bug in future,using strcmp
                {
            //header_start=(struct posix_header_ustar*)header_start+1;
            return (void*) header_start;
        }

        header_start = (struct posix_header_ustar*) ((void*) header_start
                + ((size + 511) / 512 + 1) * 512);
    }

    return NULL;

}

/*struct dirent* do_readdir(void* fd)
 {
 uint64_t i=0;
 uint64_t size;
 struct dirent* output = kmalloc(1);
 struct posix_header_ustar *header_start = (struct posix_header_ustar*)&_binary_tarfs_start;
 struct posix_header_ustar *file = (struct posix_header_ustar*)(fd);

 char* name = file->name;

 while(header_start<(struct posix_header_ustar*)&_binary_tarfs_end)
 {

 size=get_size_oct(header_start->size);
 if((!strncmp(header_start->name, name, 3)) & (strcmp(header_start->typeflag, "5")))//here may be a bug in future,using strcmp
 {
 strcpy(output[i].name ,header_start->name);
 //header_start=(struct posix_header_ustar*)header_start+1;
 //return (void*)header_start;
 i++;
 }

 header_start=(struct posix_header_ustar*)((void*)header_start+((size+511)/512 + 1)*512);
 }

 output->num = i;

 return output;


 }*/

/*int do_readdir(void* fd, struct dirent *dirp)
 {
 uint64_t i=0;
 uint64_t size;
 struct posix_header_ustar *header_start = (struct posix_header_ustar*)&_binary_tarfs_start;
 struct posix_header_ustar *file = (struct posix_header_ustar*)(fd);

 char* name = file->name;

 size_t name_len = strlen(name);

 while(header_start<(struct posix_header_ustar*)&_binary_tarfs_end)
 {

 size=get_size_oct(header_start->size);
 if((!strncmp(header_start->name, name, name_len)) & (strcmp(header_start->typeflag, "5")))//here may be a bug in future,using strcmp
 {
 strcpy(dirp[i].name ,header_start->name);
 //header_start=(struct posix_header_ustar*)header_start+1;
 //return (void*)header_start;
 i++;
 }

 header_start=(struct posix_header_ustar*)((void*)header_start+((size+511)/512 + 1)*512);
 }

 dirp->num = i;

 if(i==0)
 {
 printf("the directory is empty!!!\n");
 return -1;
 }
 else
 {
 return 0;
 }


 //return output;

 }*/

int do_readdir(void* fd, struct dirent *dirp) {
    uint64_t i = 0;
    uint64_t size;
    struct posix_header_ustar *header_start =
            (struct posix_header_ustar*) &_binary_tarfs_start;
    struct posix_header_ustar *file = (struct posix_header_ustar*) (fd);

    char* name = file->name;

    size_t name_len = strlen(name);

    while (header_start < (struct posix_header_ustar*) &_binary_tarfs_end) {

        size = get_size_oct(header_start->size);
        if ((!strncmp(header_start->name, name, name_len))
                && (strcmp(header_start->name, name))) //here may be a bug in future,using strcmp
                //(strcmp(header_start->typeflag, "5"))
                {
            char tmp[100];

            strcpy(tmp, (header_start->name) + strlen(name));

            if (contain_slash(tmp)) {
                if (!strcmp(header_start->typeflag, "5")) {
                    strcpy(dirp[i].name, header_start->name);
                    i++;
                }
            } else {
                strcpy(dirp[i].name, header_start->name);
                i++;
            }

            //strcpy(dirp[i].name ,header_start->name);
            //header_start=(struct posix_header_ustar*)header_start+1;
            //return (void*)header_start;
            //i++;
        }

        header_start = (struct posix_header_ustar*) ((void*) header_start
                + ((size + 511) / 512 + 1) * 512);
    }

    dirp->num = i;

    if (i == 0) {
        printf("the directory is empty!!!\n");
        return -1;
    } else {
        return 0;
    }

//return output;

}

int do_closedir(struct dirent* close) {
    //kfree(close, 1);
    return 0;
}

char* get_cwd(char* buf) {
    // char* tmp = buf;
    strcpy(buf, current->cur_dir);
    //strcpy(buf, tmp);
    return buf;
}

char* set_cwd(char* buf) {
    // char* tmp = buf;
    //memset(current->cur_dir, )
    strcpy(current->cur_dir, buf);
    //WARNING: THE CURDIR MAY CONTAIN ITS ORIGINAL CONTENTS AFTER /0
    //strcpy(buf, tmp);
    return buf;
}

void tarfs_test() {

    struct posix_header_ustar *p =
            (struct posix_header_ustar *) &_binary_tarfs_start;
    tarfs_file *fp;
    char temp[100];
    size_t len;
    printf("im in tarfs test\n");

    //while(p < (struct posix_header_ustar *)&_binary_tarfs_end)

    printf("================================TARFS name: %s\n", p->name);
    //p=tarfs_header_walk(p);
    //uint64_t size1=get_size_oct(p->size);
    p = (void*) p + (511 / 512 + 1) * 512;
    //p=tarfs_header_walk(p);
    printf("================================TARFS name: %s\n", p->name);
    p = tarfs_header_walk(p);
    printf("================================TARFS name: %s\n", p->name);
    printf("================================TARFS size: %s\n", p->size);
    uint64_t size_ = get_size_oct(p->size);
    printf("================================TARFS size: %d\n", size_);

    fp = tarfs_open("bin/hello", O_RDONLY);
    memset(temp, 0, sizeof(temp));
    len = tarfs_read(fp, temp, 50);
    printf("test tarfs: %d\n", len);
    printf("test tarfs: %s\n", temp);

    return;
}

// write syscall service routine
ssize_t do_write(int fd, const void *buf, size_t count) {
    if (fd == STD_OUT || fd == STD_ERR) {
        // TODO: return value
        return terminal_write(fd, (char *) buf, count);
    } else {
        // write to file
        printf("Don't Support write to file\n");
        return -1;
    }
}

// read syscall service routine
ssize_t do_read(struct file *fd, void *buf, size_t count) {
    if (fd == STD_IN) {
        return terminal_read((char *) buf, count);
    } else {
        return tarfs_read(fd, buf, count);
    }
}
