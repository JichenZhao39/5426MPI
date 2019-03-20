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
void board_init(int **grid,int n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int rn = rand() % 3; //random number produce our grid
            grid[i][j] = rn;
            //printf(grid[i][j]);
        }
    }
}
//print grid colour
void grid_print(int **grid,int n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; ++j) {
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }
}
//count the red blue number in each tile
bool count_red_blue(int **grid,int n,int t,int c) {
    int redcount=0, bluecount=0;
    float red_percentage,blue_percentage;
    bool finished = false;
    for (int i = 0; i < (int)n/t; i++) {
        for (int j = 0; j < (int)n/t; j++) {
            //grid of tile
            for (int k = t * i; k < (t+i*t); k++) {
                for (int l = t*j; l < (t+j*t); l++) {
                    if (grid[k][l] == 1)
                        redcount++;    //count the number of red colour
                    if (grid[k][l] == 2)
                        bluecount++;   //count the number of blue colour
                }
            }
            //get the red/blue percentage
            red_percentage = (float)redcount * 100 / (t*t);
            blue_percentage = (float)bluecount * 100 / (t*t);
            //printf("\n%f.......%f\n",red_percentage,blue_percentage);
            //check if the computation can be terminated
            if ((red_percentage >= (float)c) || (blue_percentage >= (float)c)){
                printf("\nTerminated tile colour\n");
                for (int k = t * i; k < (t+i*t); k++) {
                    for (int l = t * j; l < (t + j * t); l++) {
                        printf("%d", grid[k][l]);//print out the title position, and the colour and its percentage
                    }
                    printf("\n");
                }
                printf("Program Terminated at tile(%d,%d)\n",i,j);

                //printf("\n%f.......%f.....%f\n",red_percentage,blue_percentage,(float)c);
                if (red_percentage >= (float)c)
                    printf("Red colour cell percentage has more than threshold: %f%% > %f%%(threshold)\n",red_percentage,(float)c);
                if (blue_percentage >= (float)c)
                    printf("Blue colour cell percentage has more than threshold: %f%% > %f%%(threshold)\n",blue_percentage,(float)c);
                //break;
                grid_print(grid,n);

                //return finished;
                finished = true;
            }
            redcount = 0;
            bluecount = 0;
        }
    }
    return finished;
}

int main(int argc, char *argv[]) {

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
    /*
    printf("Please enter cell grid size:\n");
    scanf("%d",&n);
    //fflush(stdin);
    //rewind(stdin);
    printf("Please enter tile grid size:\n");
    scanf("%d",&t);
    printf("Please enter terminating threshold number:\n");
    scanf("%d",&c);
    printf("Please enter maximum number of iterations:\n");
    scanf("%d",&MAX_ITRS);*/
    n = atoi(argv[1]);
    t = atoi(argv[2]);
    c = atoi(argv[3]);
    MAX_ITRS = atoi(argv[4]);

    int *grid_data = (int *)malloc(sizeof(int)*n*n);
    grid = (int **)malloc(n* sizeof(int*));
    for (int k = 0; k < n; k++) {
        grid[k] = (&grid_data[n*k]);
    }


    //process 0
    if (myid == 0){
        //create the full cell grid matrix and initialize it
        //initialize the board. board_init();
        printf("############Board Initialize############\n");
        board_init(grid,n);
        //pint out initialize board
        grid_print(grid,n);
        printf("############Board Initialize############\n");


        //if only one process created,do a sequential iterative computation.
        //print out tiles with the colored squares more than %c one color (blue or red)and exit
        if (numprocs == 1){

        }



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
