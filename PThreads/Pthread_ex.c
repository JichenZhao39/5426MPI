//
// Created by JICHEN on 12/3/19.
//
#include <sys/types.h>
#include <sys/wait.h>
#include "errors.h"
#include <pthread.h>

void alarm_fork(void){
    int status;
    char line[128];
    int seconds;
    pid_t pid;
    char message[64];

    while (1){
        printf("Alarm > ");
        if (fgets(line, sizeof(line),stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;

        /*
         * Parse input line into seconds (%d) and a message(%64[^\n],)
         * consisting of up to 64 characters separated from the seconds by whitespace
         */
        if (sscanf(line,"%d %64[^\n]",&seconds,message) < 2){
            fprintf(stderr,"Bad command\n");
        } else{
            pid = fork();
            if (pid == (pid_t) - 1)
                errno_abort("Fork");
            if (pid == (pid_t)0){
                //In the child, wait and then print a message
                sleep(seconds);
                printf("(%d) %s\n",seconds,message);
                exit(0);
            } else{
                //in the parent, call waitpid() to collecct children
                //that have already terminated
                do{
                    pid = waitpid((pid_t)-1,NULL,WNOHANG);
                    if (pid == (pid_t)-1)
                        errno_abort("Wait for child");
                }while (pid != (pid_t)0);
            }
        }
    }
}

typedef struct alarm_tag{
    int seconds;
    char message[64];
} alarm_t;

void *alarm_thread(void *arg){
    alarm_t *alarm = (alarm_t*)arg;
    int status;
    //调用pthread_detach函数来分离自己
    status = pthread_detach(pthread_self());
    if (status != 0)
        err_abort(status,"Deach thread");
    sleep(alarm ->seconds);
    printf("(%d) %s\n",alarm->seconds,alarm->message);
    free(alarm);
    return NULL;
}

int alarm_thread_main(void){
    int status;
    char line[128];
    alarm_t *alarm;
    //线程标识符
    pthread_t thread;

    while (1){
        printf("Alarm > ");
        if (fgets(line, sizeof(line),stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;
        alarm = (alarm_t *)malloc(sizeof(alarm_t));
        if (alarm == NULL)
            errno_abort("Allocate alarm");
        /*
         * Parse input line into seconds (%d) and a message(%64[^\n],)
         * consisting of up to 64 characters separated from the seconds by whitespace
         */
        if (sscanf(line,"%d %64[^\n]",&alarm->seconds,alarm->message) < 2){
            fprintf(stderr,"Bad command\n");
            free(alarm);
        } else{
            status = pthread_create(&thread,NULL,alarm_thread,alarm);
            if (status != 0)
                err_abort(status,"Create alarm thread");
        }
    }
    return 0;
}

//thread start routine
//* 表示函数返回一个地址值。
void *thread_routine(void *arg){
    return arg;
}
int lifecycle(void){
    pthread_t thread_id;
    void *thread_result;
    int status;

    status = pthread_create(&thread_id,NULL,thread_routine,NULL);
    if (status != 0)
        err_abort(status,"Create thread");

    status = pthread_join(thread_id,&thread_result);
    if (status != 0){
        err_abort(status,"Join thread");
    }
    if (thread_result == NULL)
        return 0;
    else
        return 1;
}

int main(void){

    //alarm_fork();
    //alarm_thread_main();
    lifecycle();
    return 0;
}
