//
// Created by JICHEN on 6/3/19.
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

void grid_print(int **grid,n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; ++j) {
            printf("%d ",grid[i][j]);
        }
        printf("\n");
    }
}


int main(void){
    /*
   white = 0, red = 1, blue = 2,
   red or blue just moved in = 3 and
   red or blue (in the first row or column) just moved out = 4
    */

    int n,MAX_ITRS,t,c;
    int **grid; 	/* grid[row][col] */
    bool finished = false;
    int n_itrs = 0;
    int redcount, bluecount;
    int i, j;
    int **tile1,**tile2,**tile3,**tile4;
    srand((unsigned)time(0));//we use time as our random number seeds


    //get user input
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

    //dynamic apply 2D array
    grid = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++) {
        grid[i] = (int *)malloc(sizeof(int) * n);
    }


    //initialize the board. board_init();
    printf("\n##########Board Initialize##########\n");
    board_init(grid,n);
    //pint out initialize board
    grid_print(grid,n);
    printf("\n##########Board Initialize##########\n");
    //printf("\n%d %d %d %d",n,t,c,MAX_ITRS);


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

        //count the number in each tile
        int count_red=0,count_blue=0;
        float red_percentage=0.0,blue_percentage=0.0;
        for (int i = 0; i < (int)n/t; i++) {
            for (int j = 0; j < (int)n/t; j++) {
                //grid of tile
                for (int k = t * i; k < (t+i*t); k++) {
                    for (int l = t*j; l < (t+j*t); l++) {
                        if (grid[k][l] == 1)
                            count_red++;
                        if (grid[k][l] == 2)
                            count_blue++;
                    }
                }
                red_percentage = (float)count_red * 100 / (t*t);
                blue_percentage = (float)count_blue * 100 / (t*t);
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
                    printf("Program Terminated at %d row,%d column.\n",t*i+1,t*j+1);
                    //printf("\n%f.......%f.....%f\n",red_percentage,blue_percentage,(float)c);
                    if (red_percentage >= (float)c)
                        printf("Red colour cell percentage more than threshold: %f%% > %f%%(threshold)",red_percentage,(float)c);
                    if (blue_percentage >= (float)c)
                        printf("Blue colour cell percentage more than threshold: %f%% > %f%%(threshold)",blue_percentage,(float)c);
                    printf("\nTerminated at iteration %d\n",n_itrs);
                    //break;
                    grid_print(grid,n);
                    return 0;
                }
                count_red = 0;
                count_blue = 0;
            }
        }

    }

    return 0;
}