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
void board_init(int **grid,int row){
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < row; j++) {
            int rn = rand() % 3; //random number produce our grid
            grid[i][j] = rn;
            //printf(grid[i][j]);
        }
    }
}
//print grid colour
void grid_print(int **grid,int row){
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < row; ++j) {
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }
}
//count the red blue number in each tile
bool count_red_blue(int **grid,int n,int row,int tile,int c,int n_iters) {
    //first the redcount,bluecount needs to be initialize otherwise the result will be incorrect.
    int redcount=0, bluecount=0;
    float red_percentage,blue_percentage;
    bool finished = false;
    for (int i = 0; i < (int)row/tile; i++) {
        for (int j = 0; j < (int)n/tile; j++) {
            //grid of tile
            for (int k = tile * i; k < (tile+i*tile); k++) {
                for (int l = tile*j; l < (tile+j*tile); l++) {
                    if (grid[k][l] == 1)
                        redcount++;    //count the number of red colour
                    if (grid[k][l] == 2)
                        bluecount++;   //count the number of blue colour
                }
            }
            //get the red/blue percentage
            red_percentage = (float)redcount * 100 / (tile*tile);
            blue_percentage = (float)bluecount * 100 / (tile*tile);
            //printf("\n%f.......%f\n",red_percentage,blue_percentage);
            //check if the computation can be terminated
            if ((red_percentage >= (float)c) || (blue_percentage >= (float)c)){
                printf("\nTerminated tile colour\n");
                for (int k = tile * i; k < (tile+i*tile); k++) {
                    for (int l = tile * j; l < (tile + j * tile); l++) {
                        printf("%d", grid[k][l]);//print out the title position, and the colour and its percentage
                    }
                    printf("\n");
                }
                printf("Program Terminated at tile(%d,%d)\n",i,j);
                printf("Terminated iteration at %d\n",n_iters);

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
void self_checking(int **grid,int **grid_copy,int row){
    int number_count = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < row; j++) {
            if (grid[i][j] == grid_copy[i][j]){
                number_count++;
            }
        }
    }
    printf("%d values is equal between Parallel and Sequential results\n",number_count);
    if (number_count == row*row){
        printf("Parallel and Sequential has same results\n");
    } else{
        printf("The results of Parallel and Sequential is different\n");
    }

}

int main(int argc, char *argv[]) {

    //any  thing you want to add.
    //... please fill.
    if (argc != 5){
        printf("Wrong number of arguments.\n");
        printf("Please enter the command in the following format:\n");
        printf("mpirun –np [proc num] main [grid size] [tile size] [threshold] [maximum iteration]\n");
        printf("SAMPLE: mpirun –np 5 main 20 4 70 40\n");
        exit(0);
    }

    // initialize MPI environment and get the total number of processes and process id.
    int  myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    int n,MAX_ITRS,t,c;     //n is grid size, t represent tile grid size, c represent terminating threshold
    int **grid,**grid_copy,*grid_data1,*grid_data01,*ghost_data,**ghost,**ghost1; 	/* grid[row][col] */
    bool finished = false;
    bool finished_result = false;
    int n_itrs = 0;
    int redcount, bluecount;
    int i, j,K,ib,Kn,q,r;
    int **tile1,**tile2,**tile3,**tile4;
    srand((unsigned)time(0));//we use time as our random number seeds
    MPI_Status status;

    //get user input
    //obtain four parameters for cell grid size, tile grid size, terminating threshold, and maximum number of iterations
    //get the grid size form parameter
    n = atoi(argv[1]);
    //get the tile grid size from parameter
    t = atoi(argv[2]);
    //get the threshold from parameter
    c = atoi(argv[3]);
    //get the maximum number of iterations
    MAX_ITRS = atoi(argv[4]);

    //dynamic apply 2D array
    int *grid_data = (int *)malloc(sizeof(int)*n*n);
    grid = (int **)malloc(n* sizeof(int*));
    for (int k = 0; k < n; k++) {
        grid[k] = (&grid_data[n*k]);
    }
    if (grid == NULL){
        fprintf(stderr, "**grid out of memory\n");
        exit(1);
    }

    //process 0
    if (myid == 0){
        //create the full cell grid matrix and initialize it
        //initialize the board. board_init();
        printf("===============Board Initialize===============\n");
        board_init(grid,n);
        //pint out initialize board
        grid_print(grid,n);
        printf("===============Board Initialize===============\n");

        //copy grid value to grid_copy 2D array. for sequential computational
        int *grid_data = (int *)malloc(sizeof(int)*n*n);
        grid_copy = (int **)malloc(sizeof(int *) * n);
        for (int i = 0; i < n; i++) {
            grid_copy[i] = &grid_data[n*i];
        }
        //memcpy(grid_copy,grid,n*n* sizeof(int));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                grid_copy[i][j] = grid[i][j];
            }
        }
        //if only one process created,do a sequential iterative computation.
        //print out tiles with the colored squares more than %c one color (blue or red)and exit
        if (numprocs == 1){
            while (!finished && n_itrs < MAX_ITRS){

                // count the number of red and blue in each tile and check if the computation can be terminated
                n_itrs++;
                // red color movement
                for (i = 0; i < n; i++){
                    //when column 0 & 1
                    if (grid[i][0] == 1 && grid[i][1] == 0){
                        grid[i][0] = 4;//move out
                        grid[i][1] = 3;//move in
                    }
                    // column 1 to n-1
                    for (j = 1; j < n; j++){
                        if (grid[i][j] == 1 && (grid[i][(j+1)%n] == 0)){
                            grid[i][j] = 0; //this mean it is available
                            grid[i][(j+1)%n] = 3;//red move in
                        } else if (grid[i][j] == 3)//move in
                            grid[i][j] = 1;//change to occupied cell
                    }
                    //column 0
                    if (grid[i][0] == 3)
                        grid[i][0] = 1;
                    else if (grid[i][0] == 4)
                        grid[i][0] = 0;
                }

                /*// blue color movement*/
                for (i = 0;i<n;i++){
                    //when column 0 &1
                    if (grid[0][i] == 2 && grid[1][i] == 0){

                        grid[0][i] = 4; //move out

                        grid[1][i] = 3; //move in
                    }
                    //
                    for (j = 1;j < n; j++){
                        if (grid[j][i] == 2 && grid[(j+1)%n][i] == 0){
                            grid[j][i] = 0;
                            grid[(j+1)%n][i] = 3;
                        } else if (grid[j][i] == 3)
                            grid[j][i] = 2;
                    }
                    if (grid[0][i] == 3)
                        grid[0][i] = 2;
                    else if (grid[0][i] == 4)
                        grid[0][i] = 0;
                }

                //count the number of red and blue in each tile
                finished = count_red_blue(grid,n,n,t,c,n_itrs);
            }
        }
        //else if more than one process created, patition and distribute the task to all process
        else if (numprocs > 1){
            //send submatrix to every other process
            //load balancing
            q = n / numprocs;
            r = n % numprocs;
            for (int i = 1; i < numprocs; i++) {
                //calculate the first row
                if (i < r){
                    ib = i * (q + 1);
                    K = q+1;
                } else{
                    ib = i * q + r;
                    K = q;
                }
                Kn = K * n;
                MPI_Send(&grid[ib][0],Kn,MPI_INT,i,0,MPI_COMM_WORLD);
            }

        }
    } else{
        //create other processes
        //create a sub-grid from process 0
        //please fill
        q = n / numprocs;
        r = n % numprocs;
        if (myid < 1)
            K = q + 1;
        else
            K = q;
        Kn = K * n;

        //create a sub-grid from process 0
        int *grid_data0 = (int *)malloc(sizeof(int)*K*n);
        grid = (int **)malloc(K* sizeof(int*));
        if (grid == NULL){
            fprintf(stderr,"**grid out of memory\n");
            exit(1);
        }
        for (int i = 0; i < K; i++) {
            grid[i] = &grid_data0[i*n];
        }

        //receive a submatrix from process 0
        MPI_Recv(&grid[0][0],Kn,MPI_INT,0,0,MPI_COMM_WORLD,&status);

    }

    //parallel iterative computation
    //print out which tile (or tiles if more than one)has the colored squares
    //more than c% one color (blue or red)
    //red colour movement
    while (!finished && n_itrs < MAX_ITRS){
        // count the number of red and blue in each tile and check if the computation can be terminated
        n_itrs++;

        // red color movement
        for (i = 0; i < t; i++){
            //when column 0 & 1
            if (grid[i][0] == 1 && grid[i][1] == 0){
                grid[i][0] = 4;//move out
                grid[i][1] = 3;//move in
            }
            // column 1 to n-1
            for (j = 1; j < n; j++){
                if (grid[i][j] == 1 && (grid[i][(j+1)%n] == 0)){
                    grid[i][j] = 0; //this mean it is available
                    grid[i][(j+1)%n] = 3;//red move in
                } else if (grid[i][j] == 3)//move in
                    grid[i][j] = 1;//change to occupied cell
            }
            //column 0
            if (grid[i][0] == 3)
                grid[i][0] = 1;
            else if (grid[i][0] == 4)
                grid[i][0] = 0;
        }
        //printf("========There1===========");

        //create ghost cell for blue color movement
        ghost_data = (int*)malloc(sizeof(int)*n);
        ghost = (int**)malloc(sizeof(int*)*1);
        for (int i = 0; i < 1; i++) {
            ghost[i] = &(ghost_data[i*n]);
        }
        ghost_data = (int*)malloc(sizeof(int)*n);
        ghost1 = (int **)malloc(sizeof(int *)*1);
        for (int j = 0; j < 1; j++) {
            ghost1[j] = &(ghost_data[j*n]);
        }

        ///send last row of sub-grid to its next process
        MPI_Sendrecv(&(grid[t-1][0]),n,MPI_INT,(myid+1)%numprocs,1,&(ghost[0][0]),n,MPI_INT,(myid-1+numprocs)%numprocs,1,MPI_COMM_WORLD,&status);

        ///send first row of sub-grid to its previous process
        MPI_Sendrecv(&(grid[0][0]),n,MPI_INT,(myid-1+numprocs)%numprocs,2,&(ghost1[0][0]),n,MPI_INT,(myid+1)%numprocs,2,MPI_COMM_WORLD,&status);

        // blue color movement
        for (i = 0;i<n;i++){
            //when column 0 &1
            if (grid[0][i] == 2 && grid[1][i] == 0){

                grid[0][i] = 4; //move out

                grid[1][i] = 3; //move in
            }
            //
            for (j = 1;j < t-1; j++){
                if (grid[j][i] == 2 && grid[(j+1)%n][i] == 0){
                    grid[j][i] = 0;
                    grid[(j+1)%n][i] = 3;
                } else if (grid[j][i] == 3)
                    grid[j][i] = 2;
            }
            if (grid[j][i] == 2 && ghost1[0][i] == 0){
                grid[j][i] = 0;
            } else if (grid[j][i] == 3){
                grid[j][i] = 2;
            }

            if (ghost[0][i] == 2 && grid[0][i] == 0){
                grid[0][i] = 3;
            }
            //if (grid[i][j])
            if (grid[0][i] == 3)
                grid[0][i] = 2;
            else if (grid[0][i] == 4)
                grid[0][i] = 0;
        }
        //finished_result = count_red_blue(grid,n,t,t,c,n_itrs);
        //printf("===========Parallel Finished========\n");


    }

    //all processes send the sub-grid back to process 0
    if (myid != 0){
        MPI_Send(&(grid[0][0]),t * n, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }

    //process 0:print out the result then
    //do a sequential iterative computation using the same data set,
    //compare the two results, and print the differences if any.
    if (myid == 0){
        //Receive
        for (int i = 1; i < numprocs; i++) {
            if (i < r){
                ib = i * (q + 1);
                K = q+1;
            } else{
                ib = i * q + r;
                K = q;
            }
            MPI_Recv(&grid[ib][0],Kn,MPI_INT,i,3,MPI_COMM_WORLD,&status);
        }
        grid_print(grid,n);

        printf("\n====================Sequential computation=================\n");
        printf("\nCopy 2 dimentional array from grid[][] to grid_copy[][]\n");
        grid_print(grid_copy,n);
        finished = false;
        n_itrs = 0;

        //do sequential iterative computation using the same data set
        while (!finished && n_itrs < MAX_ITRS){

            // count the number of red and blue in each tile and check if the computation can be terminated
            n_itrs++;
            // red color movement
            for (i = 0; i < n; i++){
                //when column 0 & 1
                if (grid_copy[i][0] == 1 && grid_copy[i][1] == 0){
                    grid_copy[i][0] = 4;//move out
                    grid_copy[i][1] = 3;//move in
                }
                // column 1 to n-1
                for (j = 1; j < n; j++){
                    if (grid_copy[i][j] == 1 && (grid_copy[i][(j+1)%n] == 0)){
                        grid_copy[i][j] = 0; //this mean it is available
                        grid_copy[i][(j+1)%n] = 3;//red move in
                    } else if (grid_copy[i][j] == 3)//move in
                        grid_copy[i][j] = 1;//change to occupied cell
                }
                //column 0
                if (grid_copy[i][0] == 3)
                    grid_copy[i][0] = 1;
                else if (grid_copy[i][0] == 4)
                    grid_copy[i][0] = 0;
            }

            /*// blue color movement*/
            for (i = 0;i<n;i++){
                //when column 0 &1
                if (grid_copy[0][i] == 2 && grid_copy[1][i] == 0){

                    grid_copy[0][i] = 4; //move out

                    grid_copy[1][i] = 3; //move in
                }
                //
                for (j = 1;j < n; j++){
                    if (grid_copy[j][i] == 2 && grid_copy[(j+1)%n][i] == 0){
                        grid_copy[j][i] = 0;
                        grid_copy[(j+1)%n][i] = 3;
                    } else if (grid_copy[j][i] == 3)
                        grid_copy[j][i] = 2;
                }
                if (grid_copy[0][i] == 3)
                    grid_copy[0][i] = 2;
                else if (grid_copy[0][i] == 4)
                    grid_copy[0][i] = 0;
            }

            //count the number of red and blue in each tile
            finished = count_red_blue(grid_copy,n,n,t,c,n_itrs);
        }
        printf("===========Sequential result=============\n");
        grid_print(grid_copy,n);
        self_checking(grid,grid_copy,n);


    }
    MPI_Finalize();
    return 0;
}
