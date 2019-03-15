//
// Created by JICHEN on 15/3/19.
//

#include "lib/tlpi_hdr.h"

static int idata = 111; //Allocated in data segment
int t_fork(void){
    int istack = 222;
    pid_t childPid;

    switch (childPid = fork()){
        case -1:
            errExit("fork");
        case 0:
            idata *= 3;
            istack *= 3;
            break;
        default:
            sleep(3);   //give child a chance to execute
            break;
    }
    //both parent and child come here
    printf("PID = %ld %s idata=%d istack=%d\n",(long)getpid(),
           (childPid == 0) ? "(child) " : "(parent)",idata,istack);
    exit(EXIT_SUCCESS);
}


int main(void){
    t_fork();
}