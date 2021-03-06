//
// Created by JICHEN on 15/3/19.
//

#include "lib/tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "lib/curr_time.h"
#include <signal.h>
#include <time.h>
#include <pthread.h>

static int idata = 111;
/*
int t_fork(void){
    int istack = 222;               //// Allocated in stack segment
    pid_t childPid;

    switch (childPid = fork()) {
        case -1:
            errExit("fork");

        case 0:
            idata *= 3;
            istack *= 3;
            break;

        default:
            sleep(3);                   // Give child a chance to execute
            break;
    }
    //Both parent and child come here

    printf("PID=%ld %s idata=%d istack=%d\n", (long) getpid(),
           (childPid == 0) ? "(child) " : "(parent)", idata, istack);

    exit(EXIT_SUCCESS);
}
int fork_file_sharing(void){
    int fd,flags;
    char template[] = "/tmp/testXXXXXXX";

    setbuf(stdout,NULL);    //Disable buffering of stdout
    //open a temporary file,set its file offset to some arbitrary value
    //and change the setting of one of the open file status flags

    fd = mkstemp(template);
    if (fd == -1)
        errExit("mkstemp");
    printf("File offset before fork(): %lld\n",
            (long long)lseek(fd,0,SEEK_CUR));
    flags = fcntl(fd,F_GETFL);
    if (flags == -1)
        errExit("fcntl - F_GETFL");
    printf("O_APPEND flag before fork() is: %s\n",
           (flags & O_APPEND) ? "ON" : "OFF");

    switch (fork()){
        case -1:
            errExit("fork");
        case 0:     //Child:change file offset and status flags
            if (lseek(fd,1000,SEEK_SET) == -1)
                errExit("lseek");
            flags = fcntl(fd,F_GETFL);  //Fetch current flags
            if (flags == -1)
                errExit("fcntl - F_GETFL");
            flags |= O_APPEND;          //Turn O_APPEND on
            if (fcntl(fd,F_SETFL,flags) == -1)
                errExit("fcntl - F_SETFL");
            _exit(EXIT_SUCCESS);
        default:    //Parent: can see file changes made by child
            if (wait(NULL) == -1)
                errExit("wait");    //wait for child exit
            printf("Child has exited\n");

            printf("File offset in parent: %lld\n",
                   (long long) lseek(fd,0,SEEK_CUR));
            flags = fcntl(fd,F_GETFL);
            if (flags == -1)
                errExit("fcntl - F_GETFL");
            printf("O_APPEND flag in parent is:%s\n",
                   (flags & O_APPEND) ? "on":"off");
            exit(EXIT_SUCCESS);
    }
}
//调用函数而不改变进程的内存需求量

static int func(int arg){
    int j;
    for (int j = 0; j < 0x100; j++) {
        if (malloc(0x8000) == NULL)
            errExit("malloc");
    }
    printf("Program break in child: %10p\n",sbrk(0));
    return arg;
}

int footprint(int argc,char *argv[]){
    int arg = (argc > 1) ? getInt(argv[1],0,"arg") : 0;
    pid_t childPid;
    int status;
    setbuf(stdout,NULL);
    printf("Program break in parent: %10p\n",sbrk(0));
    childPid = fork();
    if (childPid == -1)
        errExit("fork");
    //Child calls func() and uses return value as exit value
    if (childPid == 0)
        exit(func(arg));
    //Parent waits for child to terminate.It can determine the result of func()
    //by inspecting 'status'
    if (wait(&status) == -1)
        errExit("wait");
    printf("program break in parent: %10p\n",sbrk(0));
    printf("Status = %d %d\n",status,WEXITSTATUS(status));
    exit(EXIT_SUCCESS);
}
//使用vfork
#define _BSD_SOURCE
int t_vfork(void){
    int istack = 222;
    switch (vfork()){
        case -1:
            errExit("vfork");
        //Child executes first,in parent's memory space,
        //even if we sleep for a while parent still is not scheduled
        case 0:
            sleep(3);
            write(STDOUT_FILENO,"Child executing\n",16);
            istack *= 3;        //This change will be seen by parent
            _exit(EXIT_SUCCESS);
        default:
            write(STDOUT_FILENO,"Parent executing\n",17);
            printf("istack=%d\n",istack);
            exit(EXIT_SUCCESS);
    }
}

int fork_whos_on_first(int argc,char *argv[]){
    //fork()之后，父子进程竞争输出信息
    int numChildren,j;
    pid_t childPid;
    if (argc > 1 && strcmp(argv[1],"--help") == 0)
        usageErr("%s [num-children]\n",argv[0]);
    numChildren = (argc > 1) ? atoi(argv[1]) : 1;
    setbuf(stdout,NULL);    //make stdout unbuffered
    for (int i = 0; i < numChildren; i++) {
        switch (childPid = fork()){
            case -1:
                errExit("fork");
            case 0:
                printf("%d child\n",i);
                _exit(EXIT_SUCCESS);
            default:
                printf("%d parent\n",j);
                wait(NULL);     //wait for children to terminate
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

//利用信号来同步进程间动作
//Synchronization signal
#define SYNC_SIG SIGUSR1
//Singal handler-does nothing but return
static void handler(int sig){

}
int fork_sig_sync(void){
    pid_t childPid;
    sigset_t blockMask,origMask,emptyMask;
    struct sigaction sa;
    setbuf(stdout,NULL);    //Disable buffering of stdout

    sigemptyset(&blockMask);
    sigaddset(&blockMask,SYNC_SIG); //block signal
    if (sigprocmask(SIG_BLOCK,&blockMask,&origMask) == -1)
        errExit("sigprocmask");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG,&sa,NULL) == -1)
        errExit("sigaction");

    switch (childPid = fork()){
        case -1:
            errExit("fork");
        //Child
        //child does some required action here..
        case 0:
            printf("[%ld %ld] Child about to signal parent\n",
                    time(NULL),(long)getpid());
            //simulate time spent doing some work
            sleep(3);
            //and then signals praent that it's done
            printf("[%ld %ld] Child about to signal parent\n",
                    time(NULL),(long)getpid());
            if (kill(getppid(),SYNC_SIG) == -1)
                errExit("kill");
            //now child can do other things
            _exit(EXIT_SUCCESS);
        default:
            //parent may do some work here,and then waits for child to complete
            //the required action
            printf("[%ld %ld] Parent about to wait for signal\n",
                    time(NULL),(long)getpid());
            sigemptyset(&emptyMask);
            if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
                errExit("sigsuspend");
            printf("[%ld %ld] Parent got signal\n",
                    time(NULL),(long)getpid());
            //if required,return signal mask to its original state
            if (sigprocmask(SIG_SETMASK,&origMask,NULL) == -1)
                errExit("sigprocmask");

            //parent carries on to do other things
            exit(EXIT_SUCCESS);
    }
}
//fork()与stdio缓冲区的交互
int fork_stdio_buf(void){
    printf("Hello wworld!!\n");
    write(STDOUT_FILENO,"Arthur.\n",6);

    if (fork() == -1)
        errExit("fork");
    //both child and parent continue execution here
    exit(EXIT_SUCCESS);
}
*/
///===============================Thread Chapters==========================================
/*static void *threadFunc(void *arg){
    char *s = (char *)arg;
    printf("%s",s);
    return (void *)strlen(s);
}
int simple_thread(void){
    pthread_t t1;
    void *res;
    int s;
    s = pthread_create(&t1,NULL,threadFunc,"Hello world\n");
    if (s != 0)
        errExitEN(s,"pthread_create");
    printf("Message from main()\n");
    s = pthread_join(t1,&res);
    if (s != 0)
        errExitEN(s,"pthread_join");
    printf("Thread return %ld\n",(long)res);
    exit(EXIT_SUCCESS);
}
static void * threadFunc1(void *x){
    return x;
}
int detached_attrib(void){
    pthread_t thr;
    pthread_attr_t attr;
    int s;

    //assigns default values
    s = pthread_attr_init(&attr);
    if (s != 0)
        errExitEN(s,"pthread_attr_init");
    s = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    if (s != 0)
        errExitEN(s,"pthread_attr_setdetachstate");
    s = pthread_create(&thr,&attr,threadFunc1,(void *) 1);
    if (s != 0)
        errExitEN(s,"pthread_create");
    s = pthread_attr_destroy(&attr);    //no longer needed
    if (s != 0)
        errExitEN(s,"pthread_attr_destory");

    s = pthread_join(thr,NULL);
    if (s != 0)
        errExitEN(s,"pthread_join failed as expected");
    exit(EXIT_SUCCESS);
}*/
//'volatile' prevents compiler optimizations of arithmetic operations on'glob'
static volatile int glob = 0;
static void * threadFunc3(void *arg){
    int loops = *((int *)arg);
    int loc,j;
    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
    }
    return NULL;
}

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
//loop arg times incrementing 'glob'
static void * threadFunc4(void *arg){
    int loops = *((int *) arg);
    int loc,j,s;
    for (j = 0; j < loops; j++) {
        s = pthread_mutex_lock(&mtx);
        if (s != 0)
            errExitEN(s,"pthread_mutex_lock");
        loc = glob;
        loc++;
        glob = loc;
        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            errExitEN(s,"pthread_mutex_unlock");
    }
    return NULL;
}
int thread_incr_mutex(int arg){
    pthread_t t1,t2;
    int loops,s;
    loops = arg;
    s = pthread_create(&t1,NULL,threadFunc4,&loops);
    if (s != 0)
        errExitEN(s,"pthread_create");
    s = pthread_create(&t2,NULL,threadFunc4,&loops);
    if (s != 0)
        errExitEN(s,"pthread_create");
    s = pthread_join(t1,NULL);
    if (s != 0)
        errExitEN(s,"pthread_join");
    s = pthread_join(t2,NULL);
    if (s != 0)
        errExitEN(s,"pthread_join");

    printf("glob = %d\n",glob);
    exit(EXIT_SUCCESS);
}


int main(int argc,char *argv[]){
    //t_fork();
    //fork_file_sharing();
    //footprint(0,NULL);
    //t_vfork();
    //fork_sig_sync();
    //fork_stdio_buf();
    //simple_thread();
    //detached_attrib();
    //thread_incr(2,NULL);
    thread_incr_mutex(100000);


    return 0;
}