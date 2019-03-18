//
// Created by JICHEN on 15/3/19.
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


//initialize the board
void board_init(int **grid,n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int rn = rand() % 3; //random number produce our grid
            grid[i][j] = rn;
            //printf(grid[i][j]);
        }
    }
}
//print grid colour
void grid_print(int **grid,n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; ++j) {
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }
}
int main(int argc, char **argv) {

    //any  thing you want to add.
    //... please fill.


    // initialize MPI environment and get the total number of processes and process id.
    int  myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    int n,MAX_ITRS,t,c;     //n is grid size, t represent tile grid size, c represent terminating threshold
    int **grid; 	/* grid[row][col] */
    bool finished = false;
    int n_itrs = 0;
    int redcount, bluecount;
    int i, j;
    int **tile1,**tile2,**tile3,**tile4;
    srand((unsigned)time(0));//we use time as our random number seeds


    //get user input
    //obtain four parameters for cell grid size, tile grid size, terminating threshold, and maximum number of iterations
    printf("Please enter cell grid size:\n");
    scanf("%d",&n);
    //fflush(stdin);
    //rewind(stdin);
    printf("Please enter tile grid size:\n");
    scanf("%d",&t);
    printf("Please enter terminating threshold number:\n");
    scanf("%d",&c);
    printf("Please enter maximum number of iterations:\n");
    scanf("%d",&MAX_ITRS);


    //process 0
    if (myid == 0){
        //create the full cell grid matrix and initialize it


        //if only one process created,do a sequential iterative computation.
        //print out tiles with the colored squares more than %c one color (blue or red)and exit



        //else if more than one process created, patition and distribute the task to all process
    } else{
        //create other processes
        //create a sub-grid from process 0
    }


    //parallel iterative computation
    //print out which tile (or tiles if more than one)has the colored squares
    //more than c% one color (blue or red)




    //all processes send the sub-grid back to process 0


    //process 0:print out the result then
    //do a sequential iterative computation using the same data set,
    //compare the two results, and print the differences if any.

    

    MPI_Finalize();
    return 0;
}
