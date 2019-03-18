#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    int myid, numprocs;
    int token = 0;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if(myid == 0)
    {
        token = 1;
        printf("Hello World! This is processor %i from %i - I have the token now!\n", myid,numprocs);

        MPI_Send(&token, 1, MPI_INT, (myid + 1)%numprocs, 0, MPI_COMM_WORLD);
        MPI_Recv(&token,1,MPI_INT,(myid-1)%numprocs,0,MPI_COMM_WORLD,&status);
    }else{
        MPI_Recv(&token,1,MPI_INT,(myid-1)%numprocs,0,MPI_COMM_WORLD,&status);
        printf("Hello World! This is processor %i from %i - I have the token now!\n", myid,numprocs);
        MPI_Send(&token,1,MPI_INT,(myid + 1)%numprocs,0,MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

