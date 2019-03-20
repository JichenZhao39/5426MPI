#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int myid;
    int numprocs;
    MPI_Status status;
    int **A; //the two-dimential matrix
    int M, N; //dimensions of matrix A
    //any other variables you want to add.
    //... please fill.

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);

    if (argc != 3){
        printf("Wrong number of arguments.\n");
        printf("Please enter the command in the following format:\n");
        printf("mpirun –np [proc num] main [matrix rows M] [matrix columns N]\n");
        printf("SAMPLE: mpirun –np 3 main 20 20\n");
        exit(0);
    }

    M = atoi(argv[1]);
    N = atoi(argv[2]);
    //printf("%d\n........",atoi(argv[0]));

    if (myid == 0) {

        //create matrix A of size M X N.
        //... please fill.
        /*A = (int **)malloc(sizeof(int *) * M);
        for (int i = 0; i < N; i++) {
            A[i] = (int *)malloc(sizeof(int) * N);
        }*/
        int *data = (int *)malloc(M*N*sizeof(int));
        **A= (int **)malloc(M*sizeof(int*));

        for(int i=0; i<M; i++)
            A[i] = &(data[N*i]);

        //initialize matrix to 0.
        for (int i=0; i<M; i++)
            for (int j=0; j<N; j++)
                A[i][j] = 0;

        printf("\n Output Initial Matrix A:\n");
        for(int i=0; i<M; i++){
            for(int j=0; j<N; j++){
                printf("%d  ", A[i][j]);
                if(j == N - 1)
                    printf("\n\n");
            }
        }

        if (numprocs > 1){
            //send a submatrix to every other processes
           // ...please fill.
           int K = numprocs - 1;
            for (int i = 0; i < K; i++) {
                MPI_Send(&(A[i*(M/K)][0]),(M/K)*N,MPI_INT,i+1,0,MPI_COMM_WORLD);
            }

            //receive a submatrix from every other processes
            //... please fill.
            for (int j = 0; j < K; j++) {
                MPI_Recv(&(A[j*(M/K)][0]),(M/K)*N,MPI_INT,j+1,0,MPI_COMM_WORLD,&status);
            }

        }

        printf("\n Output Updated Matrix A:\n");
        for(int i=0; i<M; i++){
            for(int j=0; j<N; j++){
                printf("%d  ", A[i][j]);
                if(j == N - 1)
                    printf("\n\n");
            }
        }

    }
    else { /*all other processes. */
        //create a submatrix A of size K X N.
        //... please fill.
        /*A = (int **)malloc(sizeof(int *) * (M/(numprocs-1)));
        for (int k = 0; k < M/(numprocs-1); k++) {
            A[k] = (int *)malloc(sizeof(int)*N);
        }*/
        int *A_data = (int *)malloc((M/(numprocs-1))*N*sizeof(int));
        A= (int **)malloc((M/(numprocs-1))*sizeof(int*));
        for(int i=0; i<M/(numprocs-1); i++)
            A[i] = &(A_data[N*i]);


        /* recv a submatrix from process 0.*/
        //... please fill.
        MPI_Recv((&A[0][0]),(M/(numprocs-1))*N,MPI_INT,0,0,MPI_COMM_WORLD,&status);


        /*update matrix A.*/
        for (int i=0; i<M/(numprocs-1); i++)
            for (int j=0; j<N; j++)
                A[i][j] += myid;

        /* send the submatrix back to process 0. */
        //... please fill.
        MPI_Send((&A[0][0]),(M/(numprocs-1))*N,MPI_INT,0,0,MPI_COMM_WORLD);


    }

    MPI_Finalize();
    return 0;
}
