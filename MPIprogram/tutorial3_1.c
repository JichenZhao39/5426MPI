//
// Created by JICHEN on 17/3/19.
//
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
/*
Exercise 1
You need to write a program. Each process has a 1D array of 4 integers buf[4]. They are initialized as

buf[0] = buf[3] = -1;

buf[1] = myid;

buf[2] = myid + numprocs;

that is, the second element is assigned a value myid and the third myid+numprocs (numprocs is the total number of processes in MPI_COMM_WORLD, while the first and the fourth are considered as ghost cells and initialized to -1.

1.     Revise the MPI program in Tutorial 2 and still use MPI_Send/MPI_Recv for the following communication among processes:

a.      The process communication topology is a 1D torus

        b.     First each process sends buf[2] to its right neighbour

c.      When received the message from the left neighbour, store it in buf[0]

d.     Then each process sends buf[1] to its left neighbour

e.      When received the message from the right neighbour, store it in buf[3]

f.      Print out the buf values: printf("process %i:  %i, %i, %i, %i.\n", myid, buf[0], buf[1], buf[2], buf[3]);

Run your program and check the results.

2.     Revise your program using MPI_Sendrecv instead of MPI_Send/MPI_Recv.

Run the program and check the results again.
*/
int main(void){
    MPI_Init(NULL,NULL);
    int myid,numprocs;
    int buf[4];
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    //printf("进程数为 %d",numprocs);
    buf[0] = buf[3] = -1;
    buf[1] = myid;
    buf[2] = myid + numprocs;

    /*
    if (myid == 0){
        ///sends buf[2] to its right neighbour
        MPI_Send(&buf[2],1,MPI_INT,(myid + 1)%numprocs,0,MPI_COMM_WORLD);
        //printf("当前进程 ID 为 %d\n",(myid + 1)%numprocs);
        //received the message from the left neighbour, store it in buf[0]
        MPI_Recv(&buf[0],1,MPI_INT,numprocs - 1,0,MPI_COMM_WORLD,&status);
        //printf("接收进程ID为%d\n",numprocs-1);


        ///sends buf[1] to its left neighbour
        MPI_Send(&buf[1],1,MPI_INT,numprocs - 1,0,MPI_COMM_WORLD);
        //received the message from the right neighbour, store it in buf[3]
        MPI_Recv(&buf[3],1,MPI_INT,(myid + 1)%numprocs,0,MPI_COMM_WORLD,&status);


        printf("process %i:  %i, %i, %i, %i.\n", myid, buf[0], buf[1], buf[2], buf[3]);
    } else{
        ///sends buf[2] to its right neighbour
        MPI_Send(&buf[2],1,MPI_INT,(myid + 1)%numprocs,0,MPI_COMM_WORLD);
        //printf("当前进程 ID 为 %d\n",(myid + 1)%numprocs);
        //received the message from the left neighbour, store it in buf[0]
        MPI_Recv(&buf[0],1,MPI_INT,myid - 1,0,MPI_COMM_WORLD,&status);
        //printf("接收进程ID为%d\n",numprocs-1);

        ///sends buf[1] to its left neighbour
        MPI_Send(&buf[1],1,MPI_INT,myid - 1,0,MPI_COMM_WORLD);
        //received the message from the right neighbour, store it in buf[3]
        MPI_Recv(&buf[3],1,MPI_INT,(myid + 1)%numprocs,0,MPI_COMM_WORLD,&status);


        printf("process %i:  %i, %i, %i, %i.\n", myid, buf[0], buf[1], buf[2], buf[3]);
    }*/

    //Revise program using MPI_Sendrecv instead of MPI_Send/MPI_Recv.
    if (myid == 0){
        ///send buf[2] to its right neighbour
        MPI_Sendrecv(&buf[2],1,MPI_INT,(myid+1)%numprocs,0,&buf[0],1,MPI_INT,numprocs-1,0,MPI_COMM_WORLD,&status);

        ///send buf[1] to its left neighbour
        MPI_Sendrecv(&buf[1],1,MPI_INT,numprocs-1,0,&buf[3],1,MPI_INT,(myid+1)%numprocs,0,MPI_COMM_WORLD,&status);

        printf("process %i:  %i, %i, %i, %i.\n", myid, buf[0], buf[1], buf[2], buf[3]);
    } else{
        ///send buf[2] to its right neighbour
        MPI_Sendrecv(&buf[2],1,MPI_INT,(myid+1)%numprocs,0,&buf[0],1,MPI_INT,myid-1,0,MPI_COMM_WORLD,&status);


        ///send buf[1] to its right neighbour
        MPI_Sendrecv(&buf[1],1,MPI_INT,myid-1,0,&buf[3],1,MPI_INT,(myid+1)%numprocs,0,MPI_COMM_WORLD,&status);

        printf("process %i:  %i, %i, %i, %i.\n", myid, buf[0], buf[1], buf[2], buf[3]);
    }

    MPI_Finalize();
}
