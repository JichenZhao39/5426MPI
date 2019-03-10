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
//重复非阻塞通信例子实现Jacobi迭代

int chongfufeizuse(void){
    int rank,value;
    MPI_Request request;
    MPI_Status status;
    MPI_Init(NULL,NULL);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (rank == 0){
        MPI_Send_init(&value,1,MPI_INT,1,99,MPI_COMM_WORLD,&request);
        for (int i = 0; i < 10; i++) {
            value = i;
            MPI_Start(&request);
            MPI_Wait(&request,&status);
        }
        MPI_Request_free(&request);
    }
    if (rank == 1){
        MPI_Recv_init(&value,1,MPI_INT,0,99,MPI_COMM_WORLD,&request);
        for (int i = 0; i < 10; i++) {
            MPI_Start(&request);
            MPI_Wait(&request,&status);
            printf("value is %d\n",value);
        }
        MPI_Request_free(&request);
    }
    MPI_Finalize();
    return 0;
}

//non-blocking to achieve Jacobi
void mpi_jacobi_new2(){
    int m = 18;
    int n = 18;
    int a[m][n];
    int b[m][n];
    int i,j,k;
    for (i = 0;i<m;i++){
        for (int j = 0; j < n; j++) {
            a[i][j] = rand() / (RAND_MAX+1.0) * 10 * (i+j);
        }
    }

    int size,rank;
    MPI_Init(NULL,NULL);
    MPI_Status status[4];
    MPI_Request request[4];
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    //每个进程计算的行数，为了简单这里假设正好可以除尽
    int gap = (m - 2) / size;
    int start = gap * rank + 1;
    int end = gap * (rank + 1);
    int bound;
    int left,right;

    //使用虚拟进程
    left = rank - 1;
    if (left < 0){
        left = MPI_PROC_NULL;
    }
    right = rank + 1;
    if (right > size - 1){
        right = MPI_PROC_NULL;
    }

    MPI_Send_init(&b[start][0],n,MPI_INT,left,99,MPI_COMM_WORLD,&request[0]);
    MPI_Send_init(&b[end][0],n,MPI_INT,right,99,MPI_COMM_WORLD,&request[1]);
    MPI_Send_init(&a[start-1][0],n,MPI_INT,left,99,MPI_COMM_WORLD,&request[2]);
    MPI_Send_init(&a[end+1][0],n,MPI_INT,right,99,MPI_COMM_WORLD,&request[3]);

    //迭代10次，计算时忽略了0，n-1行和0，n-1列
    for (int k = 0; k < 10; k++) {
        //计算边界的值
        bound = start;
        for (j = 1;j<m-1;j++){
            b[bound][j] = 0.25 * (a[bound-1][j] + a[bound+1][j] + a[bound][j+1] + a[bound][j-1]);
        }
        bound = end;
        for (int j = 1; j < m-1; j++) {
            b[bound][j] = 0.25 * (a[bound-1][j] + a[bound+1][j] + a[bound][j+1] + a[bound][j-1]);
        }
        MPI_Startall(4,request);
        //计算剩余的部分
        for (int i = start+1; i < end; i++) {
            for (int j = 1; j < m-1; j++) {
                b[i][j] = 0.25 * (a[i-1][j] + a[i+1][j] + a[i][j+1] + a[i][j-1]);
            }
        }
        for (int i = start; i <= end; i++) {
            for (int j = 1; j < n-1; j++) {
                a[i][j] = b[i][j];
            }
        }
        MPI_Waitall(4,request,status);
    }
    //这里按照顺序输出结果
    for (int k = 0; k < size; k++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == k){
            for (int i = start; i <= end; i++) {
                for (i = 1;j < n-1;j++){
                    printf("a[%d][%d] is %-4d ",i,j,a[i][j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        MPI_Request_free(&request[i]);
    }
    MPI_Finalize();
}
//广播程序示例📢
int ex38(void){
    int rank,value;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    do{
        if (rank == 0){
            scanf("%d",&value);
        }
        MPI_Bcast(&value,1,MPI_INT,0,MPI_COMM_WORLD);//将数据广播出去
        printf("Process %d got %d\n",rank,value);
    }while (value > 0);
    MPI_Finalize();
    return 0;
}

//收集：MPI_Gather可以将其他进程中的数据收集到根进程
void gather(){
    int size;
    int rank;
    int n = 10;
    int send_array[n];
    int *recv_array;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    //初始化其他进程的数据
    for (int i = 0; i < n; i++) {
        send_array[i] = i + rank * n;
    }
    if (rank == 0){
        recv_array = (int *)malloc(sizeof(int) * n * size);
    }
    MPI_Gather(send_array,n,MPI_INT,recv_array,n,MPI_INT,0,MPI_COMM_WORLD);

    if (rank == 0){
        for (int i = 0; i < n*size; i++) {
            printf("recv_array[%d] id %d\n",i,recv_array[i]);
        }
        free(recv_array);
    }
    MPI_Finalize();

}

//散发：MPI_Scatter是一对多的组通信调用。
void scatter(){
    int size;
    int rank;
    int n = 10;
    int * send_array;
    int recv_array[n];

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (rank == 0){
        send_array = (int *)malloc(sizeof(int) * n *size);
        for (int i = 0; i < n * size; i++) {
            send_array[i] = i;
        }
    }
    MPI_Scatter(send_array,n,MPI_INT,recv_array,n,MPI_INT,0,MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == i){
            for (int j = 0; j < n; j++) {
                printf("Process %d recv[%d] is %d\n",rank,j,recv_array[j]);
            }
        }
    }
    MPI_Finalize();
}
//全互换：MPI_Alltoall 是组内进程完全交换，每个进程都向其他所有的进程发送消息，同时每一个进程
//都从其他所有的进程接收消息
void all_to_all(){
    int size;
    int rank;
    int n = 2;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int send_array[n * size];
    int recv_array[n * size];

    for (int i = 0; i < n * size; i++) {
        send_array[i] = (rank+1) * (i+1);
    }
    MPI_Alltoall(send_array,n,MPI_INT,recv_array,n,MPI_INT,MPI_COMM_WORLD);

    for (int j = 0; j < size; j++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == j){
            for (int i = 0; i < n * size; i++) {
                printf("Process %d recv[%d] is %d\n",rank,i,recv_array[i]);
            }
        }
    }
    MPI_Finalize();
}

void ex45(void){
    int rank,size;
    int chunk = 2;  //发送到一个进程的数据块的大小
    int *sb;
    int *rb;
    int status,gstatus;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    sb = (int *)malloc(size * chunk * sizeof(int));//申请发送缓冲区
    if (!sb){
        perror("can't allocate send buffer");
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }
    rb = (int *)malloc(size * chunk * sizeof(int));//申请接收缓冲区
    if (!rb){
        perror("Can't allocate recv buffer");
        free(sb);
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < chunk;j++){
            sb[i*chunk+j] = chunk +i*chunk+j;//设置发送缓冲区的数据
            printf("myid = %d, send to id = %d,data[%d] = %d\n",
                    rank,i,j,sb[i*chunk+j]);
            rb[i*chunk+j] = 0;//将接收缓冲区清0
        }
    }
    //执行MPI_Alltoall 调用
    MPI_Alltoall(sb,chunk,MPI_INT,rb,chunk,MPI_INT,MPI_COMM_WORLD);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < chunk; j++) {
            printf("myid = %d, recv from id = %d, data[%d] = %d\n",
                    rank,i,j,rb[i*chunk+j]);
            //打印接收缓冲区从其他进程收到的数据
        }
    }
    free(sb);
    free(rb);
    MPI_Finalize();
}

double f(double x){
    return (4.0 / (1.0 + x * x));
}
void pivalue(void){
    int done = 0,n,myid,numprocs;
    double PI25DT = 3.141592653589793238462643;//给出已知的较为准确的π值
    double mypi,pi,h,sum,x;
    double startwtime= 0.0,endwtime;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);

    fprintf(stdout,"Process %d of %d on %s\n",myid,numprocs,processor_name);

    n = 0;
    if (myid == 0){
        printf("Please give N =\n");
        scanf("%d",&n);
        startwtime = MPI_Wtime();
    }
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);//将n值广播出去
    h = 1.0 / (double)n;    //得到矩形的宽度，所有矩形的宽度都相同
    sum = 0.0;  //给矩形面积赋初值
    for (int i = myid+1; i <= n; i+= numprocs) {
        /*
         * 每一个进程计算一部分矩形的面积，若进程总数numprocs为4，将0-1区间划分为
         * 100个矩形，则各个进程分别计算矩形块
         * 0进程      1，5，9，13，....，97
         * 1进程      2，6，10，14，...，98
         * 2进程      3，7，11，15，...，99
         * 3进程      4，8，12，16，...，100
         */
        x = h * ((double)i - 0.5);
        sum += f(x);
    }
    mypi = h * sum; //各个进程并行计算得到的部分和
    MPI_Reduce(&mypi,&pi,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //将部分和累加得到所有矩形的面积，该面积即为近似π值
    if (myid == 0){
        //执行累加的0号进程将近似值打印出来
        printf("pi is approximately %.16f, Error is %.16f\n",
                pi,fabs(pi-PI25DT));
        endwtime = MPI_Wtime();
        printf("Wall clock time = %f\n",endwtime - startwtime);
        fflush(stdout);
    }
    MPI_Finalize();
}
int main(void){
    //ex3_1();
    //ex3_3_1();
    //chongfufeizuse();
    //mpi_jacobi_new2();
    //gather();
    //scatter();
    //all_to_all();
    //ex45();
    pivalue();

    return 0;
}