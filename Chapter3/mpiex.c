//
// Created by JICHEN on 4/3/19.
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>

const int MAX_STRING = 100;
int ex3_1(void){
    char greeting[MAX_STRING];
    int comm_sz;    //Number of processes
    int my_rank;    //my process rank

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

    if (my_rank != 0){
        sprintf(greeting,"Greetings from process %d of %d!",
                my_rank,comm_sz);
        MPI_Send(greeting,strlen(greeting)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
    } else{
        printf("Greetings from process %d of %d!\n",my_rank,comm_sz);

        for (int i = 1; i<comm_sz;i++){
            MPI_Recv(greeting,MAX_STRING,MPI_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            printf("%s\n",greeting);
        }
    }
    MPI_Finalize();
    return 0;
}
double Trap(double left_endpt, double right_endpt,int trap_count,
            double base_len){
    double estimate,x;
    int i;

    estimate = ((float) left_endpt + (float)right_endpt) / 2.0;
    for( i =1;i<trap_count - 1;i++){
        x = left_endpt + i *base_len;
        estimate +=(float)x;
    }
    estimate = estimate * base_len;
    return estimate;
}
//deal with I/O parallel trapezoidal rules
int ex3_3_1(void){
    int my_rank,comm_sz,n = 1024,local_n;
    double a = 0.0,b = 3.0,h,local_a,local_b;
    double local_int,total_int;
    int source;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    h = (b - a) / n;    //h is the same for all processes
    local_n = n / comm_sz;  //so is the number of trapezoids

    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    local_int = Trap(local_a,local_b,local_n,h);

    if (my_rank != 0){
        MPI_Send(&local_int,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    } else{
        total_int = local_int;
        for (source = 1;source < comm_sz;source++){
            MPI_Recv(&local_int,1,MPI_DOUBLE,source,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }
    if (my_rank == 0){
        printf("With n = %d trapezoids,our estimate\n",n);
        printf("Of the integral from %f to %f = %.15e\n",a,b,total_int);
    }
    MPI_Finalize();
    return 0;
}

int main(void){
    //ex3_1();
    ex3_3_1();

    return 0;
}