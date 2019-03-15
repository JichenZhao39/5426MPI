//
// Created by JICHEN on 15/3/19.
//

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {

    //any  thing you want to add.
    //... please fill.


    // initialize MPI environment and get the total number of processes and process id.
    int  myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    // obtain four parameters for cell grid size, tile grid size, terminating threshold, and maximum number of iterations
    //... please fill.


    // process 0:
    if (myid == 0) {

        //create the full cell grid matrix and initialize it.
        //... please
        //fill.


        // if only one process created, do a sequential iterative computation,
        // print out tiles with the colored squares more than c% one color (blue or red) and exit.
        //... please
        //fill.


        //else if more than one process created, patition and distribute the task to all processes.
        //... please
        //fill.
    } else { // all other processes:
            //create a sub-grid from process 0.
            ... please
            fill.

    }

        // parallel iterative computation.
        //print out which tile (or tiles if more than one) has the colored squares more than c% one color (blue or red)
        ... please
        fill.


        // all processes send the sub-grid  back to process 0.
        ... please
        fill.


        // process 0: print out the result, then
        // do a sequential iterative computation using the same data set, compare the two results, and print the differences if any.
        ... please
        fill.


                MPI_Finalize();
        return 0;
}
