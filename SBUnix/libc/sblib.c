#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>

// files

// mode(unused): enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };
struct file* open(char *name, int flags) {
    return (struct file* )syscall_2(SYS_open, (uint64_t)name, flags);
}

ssize_t read(struct file* fd, void *buf, size_t count) {
    return syscall_3(SYS_read, (uint64_t)fd, (uint64_t)buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return syscall_3(SYS_write, fd, (uint64_t)buf, count);
}

void* opendir(const char* name){
    return (void* )syscall_1(SYS_open, (uint64_t)name);
}

struct dirent* readdir(void* fd){
    return (struct dirent* )syscall_1(SYS_open, (uint64_t)fd);
}

int closedir(struct dirent* close){
    return syscall_1(SYS_open, (uint64_t)close);
}

////enum { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };
////typedef uint64_t off_t;
//
//off_t lseek(int fildes, off_t offset, int whence) {
//    return syscall_3(SYS_lseek, fildes, offset, whence);
//}
//
void close(struct file* fd) {
     syscall_1(SYS_close, (uint64_t)fd);
}

//duplicate a file descriptor
//int dup(int oldfd) {
//    return syscall_1(SYS_dup, oldfd);
//}
//
//int dup2(int oldfd, int newfd) {
//    return syscall_2(SYS_dup2, oldfd, newfd);
//}
//

// processes
/* terminates the calling process "immediately".
 * Any open file descriptors belonging to the process are closed;
 * any children of the process are inherited by process 1, init,
 * and the process's parent is sent a SIGCHLD signal.
 *
 * The value status is returned to the parent process as the process's exit status,
 * and can be collected using one of the wait(2) family of calls
 */
void exit(int status){
    syscall_1(SYS_exit, status);
}

pid_t fork(void) {
    return syscall_0(SYS_fork);
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
    return syscall_3(SYS_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
}


pid_t getpid(void) {
    return syscall_0(SYS_getpid);
}

// get parent pid
pid_t getppid(void) {
    return syscall_0(SYS_getppid);
}

// in POSIX, return value type is int
void yield(void){
    syscall_0(SYS_yield);
}


//pid_t waitpid(pid_t pid, int *status, int options) {
//    return syscall_3(SYS_wait4, pid, (uint64_t)status, options);
//}
//
//struct timespec {
//    uint64_t  tv_sec;        /* seconds */
//    long   tv_nsec;       /* nanoseconds */
//};
//unsigned int sleep(unsigned int seconds) {
//    struct timespec *a = malloc(sizeof(struct timespec));
//    struct timespec *b = malloc(sizeof(struct timespec));
//
//    a->tv_sec = seconds;
//
//    return syscall_2(SYS_nanosleep,(uint64_t)a,(uint64_t)b);
//}
//


//// memory
////typedef uint64_t size_t;
//void* malloc(size_t size){
//    void *a=(void*)syscall_6(SYS_mmap,(size_t)NULL,size,0x1|0x2|0x4,0x02|0x20,-1,0);
//    if(a == ((void*)(-1))) {
//        return NULL;
//    } else {
//        return a;
//    }
//}
//
//void free(void *ptr){
//    size_t length;
//    length=sizeof(*ptr);
//    syscall_2(SYS_munmap,(uint64_t)ptr,length);
//}
//
//int brk(void *end_data_segment) {
//    return syscall_1(SYS_brk, (uint64_t)end_data_segment);
//}
//
//
//// signals
//
//unsigned int alarm(unsigned int seconds){
//    return (unsigned int)syscall_1(SYS_alarm,seconds);
//}
//#define SIG_DFL ((__sighandler_t)0)
//#define SIG_IGN ((__sighandler_t)1)
//#define SIGINT     2
//#define SIGKILL    9
//#define SIGUSR1   10
//#define SIGSEGV   11
//#define SIGALRM   14
//#define SIGTERM   15
//#define SIGCHLD   17
//typedef void (*sighandler_t)(int);
//sighandler_t signal(int signum, sighandler_t handler);
//int kill(pid_t pid, int sig){
//    return syscall_2(SIGKILL,pid,sig);
//}
//
//
//// paths
//char *getcwd(char *buf, size_t size) {
//    return (char *)syscall_2(SYS_getcwd, (uint64_t)buf, size);
//}
//
//int chdir(const char *path) {
//    return syscall_1(SYS_chdir, (uint64_t)path);
//}
//
//// directories
//#define NAME_MAX 255
///*struct dirent
// {
// long d_ino;
// off_t d_off;
// unsigned short d_reclen;
// char d_name [NAME_MAX+1];
// };
// */
//
//void *opendir(const char *name) {
//    int* fd = malloc(sizeof(int));
//    if((*fd = open(name, O_RDONLY|O_DIRECTORY)) < 0) {
//        return NULL;
//    }
//    return fd;
//}
//
//void *memset(void *s, int ch, size_t n) {
//    char* tmp = s;
//    while(n > 0) {
//        *tmp = ch;
//        tmp++;
//        n--;
//    }
//    return s;
//}
//
//int getdents(uint32_t fd, struct dirent *dirp, uint32_t count) {
//    return syscall_3(SYS_getdents, fd, (uint64_t)dirp, count);
//}
//
//struct dirent *readdir(void *dir) {
//
//    int BUF_SIZE=1024,nread=0;
//    //char buf[BUF_SIZE];
//    //memset(buf, 0, BUF_SIZE);
//    struct dirent* dirp=malloc(BUF_SIZE*sizeof(struct dirent*));
//    if((nread = getdents(*((int *)dir), (struct dirent *)dirp, BUF_SIZE)) <= 0) {
//        return NULL;
//    }
//    //dirp = (struct dirent *) buf;
//    return dirp;
//}
//
//int closedir(void *dir) {
//    int n;
//    n = close(*(int *)dir);
//    free(dir);
//    return n;
//}
