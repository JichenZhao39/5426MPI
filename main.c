#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>


//broadcasting with MPI_Send and MPI_Recv
void my_bcast(void* data,int count,MPI_Datatype datatype,int root,
        MPI_Comm communicator){
    int world_rank;
    MPI_Comm_rank(communicator,&world_rank);
    int world_size;
    MPI_Comm_size(communicator,&world_size);

    if (world_rank == root){
        //If we are root process, send our data to everyone
        int i;
        for (i = 0;i < world_size;i++){
            if (i != world_rank){
                MPI_Send(data,count,datatype,i,0,communicator);
            }
        }
    } else{
        MPI_Recv(data,count,datatype,root,0,communicator,MPI_STATUS_IGNORE);
    }
}

//Create an array of random numbers.Each number has a value from 0-1
float *create_rand_nums(int num_elements){
    float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
    assert(rand_nums != NULL);
    int i;
    for (i =0;i < num_elements;i++){
        rand_nums[i] = (rand() / (float)RAND_MAX);
    }
    return rand_nums;
}

//computes the average of an array of numbers
float compute_avg(float *array,int num_elements){
    float sum = 0.f;
    int i;
    for (i = 0; i < num_elements;i++){
        sum += array[i];
    }
    return sum / num_elements;
}

// Holds the communicator rank of a process along with the corresponding number.
// This struct is used for sorting the values and keeping the owning process information
// intact.
typedef struct {
    int comm_rank;
    union {
        float f;
        int i;
    } number;
} CommRankNumber;

// Gathers numbers for TMPI_Rank to process zero. Allocates enough space given the MPI datatype and
// returns a void * buffer to process 0. It returns NULL to all other processes.
void *gather_numbers_to_root(void *number, MPI_Datatype datatype, MPI_Comm comm) {
    int comm_rank, comm_size;
    MPI_Comm_rank(comm, &comm_rank);
    MPI_Comm_size(comm, &comm_size);

    // Allocate an array on the root process of a size depending on the MPI datatype being used.
    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    void *gathered_numbers;
    if (comm_rank == 0) {
        gathered_numbers = malloc(datatype_size * comm_size);
    }

    // Gather all of the numbers on the root process
    MPI_Gather(number, 1, datatype, gathered_numbers, 1, datatype, 0, comm);

    return gathered_numbers;
}

// A comparison function for sorting float CommRankNumber values
int compare_float_comm_rank_number(const void *a, const void *b) {
    CommRankNumber *comm_rank_number_a = (CommRankNumber *)a;
    CommRankNumber *comm_rank_number_b = (CommRankNumber *)b;
    if (comm_rank_number_a->number.f < comm_rank_number_b->number.f) {
        return -1;
    } else if (comm_rank_number_a->number.f > comm_rank_number_b->number.f) {
        return 1;
    } else {
        return 0;
    }
}

// A comparison function for sorting int CommRankNumber values
int compare_int_comm_rank_number(const void *a, const void *b) {
    CommRankNumber *comm_rank_number_a = (CommRankNumber *)a;
    CommRankNumber *comm_rank_number_b = (CommRankNumber *)b;
    if (comm_rank_number_a->number.i < comm_rank_number_b->number.i) {
        return -1;
    } else if (comm_rank_number_a->number.i > comm_rank_number_b->number.i) {
        return 1;
    } else {
        return 0;
    }
}

// This function sorts the gathered numbers on the root process and returns an array of
// ordered by the process's rank in its communicator. Note - this function is only
// executed on the root process.
int *get_ranks(void *gathered_numbers, int gathered_number_count, MPI_Datatype datatype) {
    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);

    // Convert the gathered number array to an array of CommRankNumbers. This allows us to
    // sort the numbers and also keep the information of the processes that own the numbers
    // intact.
    CommRankNumber *comm_rank_numbers = malloc(gathered_number_count * sizeof(CommRankNumber));
    int i;
    for (i = 0; i < gathered_number_count; i++) {
        comm_rank_numbers[i].comm_rank = i;
        memcpy(&(comm_rank_numbers[i].number), gathered_numbers + (i * datatype_size), datatype_size);
    }

    // Sort the comm rank numbers based on the datatype
    if (datatype == MPI_FLOAT) {
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_float_comm_rank_number);
    } else {
        qsort(comm_rank_numbers, gathered_number_count, sizeof(CommRankNumber), &compare_int_comm_rank_number);
    }

    // Now that the comm_rank_numbers are sorted, create an array of rank values for each process. The ith
    // element of this array contains the rank value for the number sent by process i.
    int *ranks = (int *)malloc(sizeof(int) * gathered_number_count);
    for (i = 0; i < gathered_number_count; i++) {
        ranks[comm_rank_numbers[i].comm_rank] = i;
    }

    // Clean up and return the rank array
    free(comm_rank_numbers);
    return ranks;
}

// Gets the rank of the recv_data, which is of type datatype. The rank is returned
// in send_data and is of type datatype.
int TMPI_Rank(void *send_data, void *recv_data, MPI_Datatype datatype, MPI_Comm comm) {
    // Check base cases first - Only support MPI_INT and MPI_FLOAT for this function.
    if (datatype != MPI_INT && datatype != MPI_FLOAT) {
        return MPI_ERR_TYPE;
    }

    int comm_size, comm_rank;
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &comm_rank);

    // To calculate the rank, we must gather the numbers to one process, sort the numbers, and then
    // scatter the resulting rank values. Start by gathering the numbers on process 0 of comm.
    void *gathered_numbers = gather_numbers_to_root(send_data, datatype, comm);

    // Get the ranks of each process
    int *ranks = NULL;
    if (comm_rank == 0) {
        ranks = get_ranks(gathered_numbers, comm_size, datatype);
    }

    // Scatter the rank results
    MPI_Scatter(ranks, 1, MPI_INT, recv_data, 1, MPI_INT, 0, comm);

    // Do clean up
    if (comm_rank == 0) {
        free(gathered_numbers);
        free(ranks);
    }
    return 0;
}


int main(int argc, char** argv) {
    /* hello world program
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    //通过调用以下得到所有可以工作的进程数量
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    //得到当前进程的秩（每一个进程会被分配一个序号，称作秩）
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();*/

    /*MPI Send and Receive
    //Initialize the MPI environment
    MPI_Init(NULL,NULL);
    //Find our rank,size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //we are assuming at least 2 processes for this task
    if (world_size < 2){
        fprintf(stderr,"World size must be greater than 1 for %s\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    int number;
    if(world_rank == 0){
        //If we are rank 0,set the number to -1 and send it to process 1
        number = -1;
        MPI_Send(
                &number,    //data
                1,          //count
                MPI_INT,    //datatype
                1,          //destination
                0,          //tag
                MPI_COMM_WORLD  //communicator
                );
    } else if (world_rank == 1){
        MPI_Recv(
                &number,        //data
                1,              //count
                MPI_INT,        //datatype
                0,              //source
                0,              //tag
                MPI_COMM_WORLD, //communicator
                MPI_STATUS_IGNORE   //status
                );
        printf("Process 1 received number %d from process 0\n",number);
    }
    MPI_Finalize();*/


    /*MPI ping pong program
    //processes use MPI_send and MPI_Recv to continually bounce
    // messages off od each other until they decide to stop
    const int PING_PONG_LIMIT = 10;

    //Initialize the MPI environment
    MPI_Init(NULL,NULL);
    //Find out rank size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //We are assuming at least 2 processes for this task
    if (world_size != 2){
        fprintf(stderr,"World size must be two for %s\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    int ping_pong_count = 0;
    int partner_rank = (world_rank + 1) % 2;
    while (ping_pong_count < PING_PONG_LIMIT){
        if (world_rank == ping_pong_count % 2){
            //Increment the ping pong count before you send it
            ping_pong_count++;
            MPI_Send(&ping_pong_count,1,MPI_INT,partner_rank,0,MPI_COMM_WORLD);
            printf("%d sent and incremented ping_pong_count %d to %d\n",
                    world_rank,ping_pong_count,partner_rank);
        } else{
            MPI_Recv(&ping_pong_count,1,MPI_INT,partner_rank,0,MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE);
            printf("%d received ping_pong_count %d from %d\n",
                    world_rank,ping_pong_count,partner_rank);
        }
    }
    MPI_Finalize();*/


    /*Ring Program
     * using more than two progresses, in this example, a value is passed
     * around by all processes in a ring-like fashion
     *

    //Initialize the MPI environment
    MPI_Init(NULL,NULL);
    //Find out rank size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    int token;
    //Receive from the lower process and send to the higher process.
    //Take care of the special case when you are the first process to
    //prevent deadlock
    if (world_rank != 0){
        MPI_Recv(&token,1,MPI_INT,world_rank-1,0,MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",world_rank,
                token,world_rank-1);
    } else{
        //Set the token's value if you are process 0
        token = -1;
    }
    MPI_Send(&token,1,MPI_INT,(world_rank+1) % world_size,0,MPI_COMM_WORLD);

    //Now process 0 can receive from the last process. This makes sure that
    //at least one MPI+Send is initialized before all MPI_Recvs(again, to
    // prevent deadlock)
    if (world_rank == 0){
        MPI_Recv(&token,1,MPI_INT,world_size-1,0,MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",world_rank,token,
                world_size-1);
    }
    MPI_Finalize();*/

    /*querying the MPI_Status structure
     * The program send a random amount of numbers to a receiver,
     * and the receiver then finds out how many numbers were sent.
     *
    MPI_Init(NULL,NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    if (world_size != 2){
        fprintf(stderr,"Must use two processes for this example\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    const int MAX_NUMBERS = 100;
    int numbers[MAX_NUMBERS];
    int number_amount;
    if(world_rank == 0){
        //Pick a random amount of integers to send to process one
        srand(time(NULL));
        number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
        //Send the amount of integers to process one
        MPI_Send(numbers,number_amount,MPI_INT,1,0,MPI_COMM_WORLD);
        printf("0 snet %d numbers to 1\n",number_amount);
    } else if (world_rank == 1){
        MPI_Status status;
        //Receive at most MAX_NUMBERS from process zero
        MPI_Recv(numbers,MAX_NUMBERS,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        //After receiving the message,check the status to determine how
        //many numbers were actually received
        MPI_Get_count(&status,MPI_INT,&number_amount);
        //Print off the amount of numbers,and also print additional information
        //in the status object
        printf("1 received %d numbers from 0. Message source = %d, tag = %d\n",
                number_amount,status.MPI_SOURCE,status.MPI_TAG);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();*/

    /*Using MPI_Probe to find out the message size
     * MPI_Probe(int source,int tag,MPI_Comm comm,MPI_Status *status)
     *
    MPI_Init(NULL,NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    if (world_size != 2){
        fprintf(stderr,"Must use two processes for this example\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    int number_amount;
    if (world_rank == 0){
        const int MAX_NUMBERS = 100;
        int numbers[MAX_NUMBERS];
        //Pick a random amount of integers to send to process one
        srand(time(NULL));
        number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
        //Send the amount of integers to process one
        MPI_Send(numbers,number_amount,MPI_INT,1,0,MPI_COMM_WORLD);
        printf("0 sent %d numbers to 1\n",number_amount);
    } else if (world_rank == 1){
        MPI_Status status;
        //Probe for an incoming message from process zero
        MPI_Probe(0,0,MPI_COMM_WORLD,&status);
        //When probe returns, the status object has ths size and other
        //attributes of the incoming message.Get the size of the message
        MPI_Get_count(&status,MPI_INT,&number_amount);
        //Allocate a buffer just big enough to hold the incoming numbers
        int* number_buf = (int*)malloc(sizeof(int) * number_amount);

        //Now receive the message with the allocated buffer
        MPI_Recv(number_buf,number_amount,MPI_INT,0,0,MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        printf("1 dynamically received %d numbers from 0.\n",number_amount);
        free(number_buf);
    }
    MPI_Finalize();*/

    /*Broadcasting with MPI_Send and MPI_Recv
    MPI_Init(NULL,NULL);
    int world_rank;MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    int data;
    if (world_rank == 0){
        data = 100;
        printf("Process 0 broadcasting data %d\n",data);
        my_bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
    } else{
        my_bcast(&data,1,MPI_INT,0,MPI_COMM_WORLD);
        printf("Process %d received data %d from root process\n",world_rank,data);
    }
    MPI_Finalize();*/

    /*Comparison of MPI_Bcast with MPI_Send and MPI_Recv

    if (argc != 3){
        fprintf(stderr,"Usage: compare_bcast num_elements num_trials\n");
        exit(1);
    }

    int num_elements = atoi(argv[1]);
    //this is a variable starting how many timing experiments
    // should be executed
    int num_trials = atoi(argv[2]);

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    double total_my_bcast_time = 0.0;
    double total_mpi_bcast_time = 0.0;
    int i;
    int* data = (int*)malloc(sizeof(int) * num_elements);
    assert(data != NULL);

    for (i =0;i < num_trials;i++){
        //Time my_bcast
        //Synchronize before starting timing
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time -= MPI_Wtime();
        my_bcast(data,num_elements,MPI_INT,0,MPI_COMM_WORLD);
        //Synchronize again before obtaining final time
        MPI_Barrier(MPI_COMM_WORLD);
        total_my_bcast_time += MPI_Wtime();

        //Time MPI_Bcast
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time -= MPI_Wtime();
        MPI_Bcast(data,num_elements,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time += MPI_Wtime();
    }
    //Print off timing information
    if (world_rank == 0){
        printf("Data size = %d, Trials = %d\n",num_elements * (
        int) sizeof(int),num_trials);
        printf("Avg my_bcast time = %lf\n",total_my_bcast_time / num_trials);
        printf("Avg MPI_Bcast time = %lf\n",total_mpi_bcast_time / num_trials);
    }
    free(data);
    MPI_Finalize();
    //mpicc main.c -o hello
    //mpirun -n 16 hello 100000 10*/

    /*Compute the average numbers with MPI_Scatter and MPI_Gaher
    if (argc != 2){
        fprintf(stderr,"Usage: avg num_elements_per_proc\n");
        exit(1);
    }
    int num_elements_per_proc = atoi(argv[1]);

    //seed the random number generator to get different results each time
    srand(time(NULL));

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //create a random array of elements on the root process. Its total
    //size will be the number of elements per process times the number
    //of processes
    float *rand_nums = NULL;
    if (world_rank == 0){
        rand_nums = create_rand_nums(num_elements_per_proc * world_size);
    }

    //for each process create a buffer that will hold a subset of
    //the entire array
    float *sub_rand_nums = (float *)malloc(sizeof(float) * num_elements_per_proc);
    assert(sub_rand_nums != NULL);

    //Scatter the random numbers from the root process to all processes in the
    // MPI world
    MPI_Scatter(rand_nums,num_elements_per_proc,MPI_FLOAT,sub_rand_nums,
            num_elements_per_proc,MPI_FLOAT,0,MPI_COMM_WORLD);

    //Compute the average of your subset
    float sub_avg = compute_avg(sub_rand_nums,num_elements_per_proc);

    //Gather all partial averages down to the root process
    float *sub_avgs = NULL;
    if (world_rank == 0){
        sub_avgs = (float *)malloc(sizeof(float) * world_size);
        assert(sub_avgs != NULL);
    }
    MPI_Gather(&sub_avg,1,MPI_FLOAT,sub_avgs,1,MPI_FLOAT,0,MPI_COMM_WORLD);

    //Now that we have all of the partial averages on the root,compute the
    //total average of all numbers.Since we are assuming each process computed
    //an average across an equal amount of elements, this computation will
    //produce the correct answer
    if (world_rank == 0){
        float avg = compute_avg(sub_avgs,world_size);
        printf("Avg of all elements is %f\n",avg);
        //compute the average across the original data for comparison
        float original_data_avg = compute_avg(rand_nums,num_elements_per_proc * world_size);
        printf("Avg computed across original data is %f\n",original_data_avg);
    }
    //Clean up
    if (world_rank == 0){
        free(rand_nums);
        free(sub_avgs);
    }
    free(sub_rand_nums);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();*/

    /*MPI_Allgather and modification of average program
    //it is useful to be able to send many elements to many processes
    //i.e a many to many communication pattern

    if (argc != 2){
        fprintf(stderr,"Usage: avg num_elements_per_proc\n");
        exit(1);
    }

    int num_elements_per_proc = atoi(argv[1]);
    //Seed the random number generator to get different results each time
    srand(time(NULL));

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //Create a random array or elements on the root process.ITs total size wil
    //be the number of elements per process times the number of processes
    float *rand_nums = NULL;
    if (world_rank ==0){
        rand_nums = create_rand_nums(num_elements_per_proc * world_size);
    }

    //For each process,create a buffer that will hold a subset of the entire array
    float *sub_rand_nums = (float *)malloc(sizeof(float) *num_elements_per_proc);
    assert(sub_rand_nums != NULL);

    //scatter the random numbers from the root process to all processes in
    //the MPI world
    MPI_Scatter(rand_nums,num_elements_per_proc,MPI_FLOAT,sub_rand_nums,
            num_elements_per_proc,MPI_FLOAT,0,MPI_COMM_WORLD);
    //compute the average of your subset
    float sub_avg = compute_avg(sub_rand_nums,num_elements_per_proc);

    //Gather all partial averages down to all the processes
    float *sub_avgs = (float *)malloc(sizeof(float) * world_size);
    assert(sub_avgs !=NULL);
    MPI_Allgather(&sub_avg,1,MPI_FLOAT,sub_avgs,1,MPI_FLOAT,MPI_COMM_WORLD);

    //Now that we have all of the partial averages compute the
    //total average of all numbers.Since we are assuming each process computed
    //an average across an wqual amount of elements, this computation will
    //produce the correct answer
    float avg = compute_avg(sub_avgs,world_size);
    printf("Avg of all elements from proc %d is %f\n",world_rank,avg);

    //clean up
    if (world_rank == 0){
        free(rand_nums);
    }
    free(sub_avgs);
    free(sub_rand_nums);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();*/

    /*Runs the TMPI_Rank function with random input
    MPI_Init(NULL,NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //seed the random number generator to get different results each time
    srand(time(NULL) * world_rank);

    float rand_num = rand() / (float) RAND_MAX;
    int rank;
    TMPI_Rank(&rand_num,&rank,MPI_FLOAT,MPI_COMM_WORLD);
    printf("Rank for %f on process %d - %d\n",rand_num,world_rank,rank);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();*/

    /*Computing average of numbers with MPI_Reduce
    if (argc != 2){
        fprintf(stderr,"Usage: avg num_elements_per_proc\n");
        exit(1);
    }

    int num_elements_per_proc = atoi(argv[1]);
    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    //Create a random array of elements on all processes
    //Seed the random number generator to get different result each time
    //for each processor
    srand(time(NULL) *world_rank);
    float *rand_nums = NULL;
    rand_nums = create_rand_nums(num_elements_per_proc);

    //Sum the numbers locally
    float local_sum = 0;
    int i;
    for (i = 0;i<num_elements_per_proc;i++){
        local_sum += rand_nums[i];
    }

    //Print the random numbers on each process
    printf("Local sum for process %d - %f, avg = %f\n",
            world_rank,local_sum,local_sum / num_elements_per_proc);

    //reduce all of the local sums into the global sum*/
    float global_sum;
    //MPI_Reduce
    //MPI_Reduce(&local_sum,&global_sum,1,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);
    /*MPI_Allreduce************************************************
    MPI_Allreduce(&local_sum,&global_sum,1,MPI_FLOAT,MPI_SUM,
            MPI_COMM_WORLD);
    float mean = global_sum / (num_elements_per_proc * world_size);

    //Compute the local sum of the squared differences from the means
    float local_sq_diff = 0;
    for (i =0;i<num_elements_per_proc;i++){
        local_sq_diff += (rand_nums[i] - mean) * (rand_nums[i] - mean);
    }

    //reduce the global sum of the squared differences to the root process
    //and print off the answer
    float global_sq_diff;
    MPI_Reduce(&local_sq_diff,&global_sq_diff,1,MPI_FLOAT,MPI_SUM,0,
            MPI_COMM_WORLD);

    //print the result
    if (world_rank == 0){
        //printf("Total sum = %f, avg = %f\n",global_sum,
        //        global_sum / (world_size * num_elements_per_proc));
        float stddev = sqrt(global_sq_diff / (num_elements_per_proc * world_size));
        printf("Mean - %f, Standard deviation = %f\n",mean,stddev);
    }
    //clean up
    free(rand_nums);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();*/

    /*Example of using multiple communicators
    MPI_Init(NULL,NULL);

    //Get the rank and size in the original communicator
    int world_rank,world_size;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    int color = world_rank / 4; //Determine color based on row

    //split the communicator based on the color and use the
    // original rank for ordering
    MPI_Comm row_comm;//
    MPI_Comm_split(MPI_COMM_WORLD,color,world_rank,&row_comm);

    int row_rank,row_size;
    MPI_Comm_rank(row_comm,&row_rank);
    MPI_Comm_size(row_comm,&row_size);

    printf("World rank/size: %d / %d --- Row rank/size: %d / %d\n",
            world_rank,world_size,row_rank,row_size);

    MPI_Comm_free(&row_comm);
    MPI_Finalize();*/

    /*Example of using groups*/
    MPI_Init(NULL,NULL);

    int world_rank,world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    //Get the group of processes inMPI_COMM_WORLD
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD,&world_group);

    int n = 7;
    const int ranks[7] = {1,2,3,5,7,11,13};

    //Construct a group containing all of the prime ranks in world_group
    MPI_Group prime_group;
    MPI_Group_incl(world_group,7,ranks,&prime_group);

    //Create a new communicator based on the group
    MPI_Comm prime_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD,prime_group,0,&prime_comm);

    int prime_rank = -1,prime_size = -1;

    //If this rank is not in the new communicator,it will be MPI_COMM_NULL
    //Using MPI_COMM_CULL for MPI_Comm_rank or MPI_Comm_size is erroneous
    if (MPI_COMM_NULL != prime_comm){
        MPI_Comm_rank(prime_comm,&prime_rank);
        MPI_Comm_size(prime_comm,&prime_size);
    }

    printf("World Rank/Size: %d / %d ---- PRIME RANK/SIZE: %d / %d\n",
            world_rank,world_size,prime_rank,prime_size);

    MPI_Group_free(&world_group);
    MPI_Group_free(&prime_group);

    if (MPI_COMM_NULL != prime_comm){
        MPI_Comm_free(&prime_comm);
    }
    MPI_Finalize();

    return 0;
}