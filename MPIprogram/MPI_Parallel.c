//
// Created by JICHEN on 5/3/19.
//

#include "MPI_Parallel.h"
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#define totalsize 8
#define mysize totalsize/4
#define steps 10

void Hello(void){
    //任意两个进程间交换问候信息，问候信息由发送进程标识和接收进程标识组成
    int nproc,me;
    int type = 1;
    int buffer[2],node;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&me);

    if (me == 0){
        //进程0负责打印提示信息
        printf("\nHello test from all to all\n");
        fflush(stdout);
    }
    for (node=0;node<nproc;node++){
        //循环对每一个进程进行问候
        if (node != me){
            //得到一个和自身不同的进程标识
            buffer[0] = me;     //将自身标识放入消息中
            buffer[1] = node;   //将被问候的进程标识也放入消息中
            //首先将问候消息发出
            MPI_Send(buffer,2,MPI_INT,node,type,MPI_COMM_WORLD);
            //然后接收被问候进程对自己发送的问候消息
            if (buffer[0] != node || buffer[1] != me){
                //若接收到的消息和内容不是问候自己的或不是以被问候方
                //的身份问候自己，则出错
                (void)fprintf(stderr,"Hello:%d!=%d or %d != %d\n",
                        buffer[0],node,buffer[1],me);
                printf("Mismatch on hello process ids; node = %d\n",node);
            }
            printf("Hello from %d to %d\n",me,node);
            fflush(stdout);

        }
    }
}


int main(int argc,char *argv[]){

    /*MPI时间函数测试
    int err = 0;
    double t1,t2;
    double tick;
    int i;

    MPI_Init(NULL,NULL);
    t1 = MPI_Wtime();   //得到当前时间t1
    t2 = MPI_Wtime();   //得到当前时间t2
    if(t2 - t1 > 0.1 || t2 - t1 < 0.0){
        //若连续的两次时间调用得到的时间间隔过大，这里是超过0.1s
        err++;
        fprintf(stderr,"Two successive calls to MPI_Wtime gave strange results:"
                       "(%f)(%f)\n",t1,t2);
    }
    for (i = 0;i < 10;i++){
        t1 = MPI_Wtime();   //计时开始
        sleep(1);
        t2 = MPI_Wtime();   //计时结束
        if (t2-t1>=(1.0-0.01) && t2-t1 <=5.0)
            break;
        //两次计时得到的时间间隔合理，则退出
        if (t2-t1>5.0)
            i=9;
        //若两次计时得到的时间间隔过大，则改变循环计数变量的值，迫使程序从循环退出
    }
    //若计时函数正确，则不需循环10次程序即从循环退出，否则会重复执行到10次
    if ( i == 10){
        //函数不正确
        fprintf(stderr,"Timer around sleep(1) did not give 1"
                       "second: gave %f\n",t2-t1);
        err++;
    }
    tick = MPI_Wtick();
    //得到一个时钟滴答的时间
    if (tick>1.0 || tick <0.0){
        //该时间太长或者为负数，则该时间不正确
        err++;
        fprintf(stderr,"MPI_Wtick gave a strange result: (%f)\n",
                tick);
    }
    MPI_Finalize();*/

    /*MPI数据接力传送

    int rank,value,size;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    //得到当前进程标识和总的进程个数
    do{
        //循环执行，直到输入的数据为负才退出
        if (rank == 0){
            fprintf(stderr,"\nPlease give new value = ");
            //进程0读入要传递的数据
            scanf("%d",&value);
            fprintf(stderr,"%d read <-<- (%d)\n",rank,value);
            if (size > 1){
                MPI_Send(&value,1,MPI_INT,rank + 1,0,MPI_COMM_WORLD);
                fprintf(stderr,"%d send (%d) ->-> %d\n",rank,value,rank+1);
                //若不少于一个进程，则向下一个进程传递改数据
            }
        } else{
            MPI_Recv(&value,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,&status);
            //其他进程从前一个进程接收传递过来的数据
            fprintf(stderr,"%d receive(%d) <-<- %d\n",rank,value,rank-1);
            if (rank <size - 1){
                MPI_Send(&value,1,MPI_INT,rank+1,0,MPI_COMM_WORLD);
                fprintf(stderr,"%d send (%d) ->-> %d\n",rank,value,rank+1);
                //若当前进程不是最后一个进程，则该数据继续向后传递
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }while (value >=0);
    MPI_Finalize();*/

    /*任意进程间相互问候
    int me,option,namelen,size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&me);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    if (size<2){
        //若总进程数小于2，则出错退出
        fprintf(stderr,"Systest requires at least 2 processes");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    //得到当前机器的名字
    MPI_Get_processor_name(processor_name,&namelen);
    fprintf(stderr,"Process %d is alive on %s\n",me,processor_name);
    //同步
    MPI_Barrier(MPI_COMM_WORLD);
    //调用问候过程
    Hello();
    MPI_Finalize();*/

    /*任意源和任意标识的使用

    int rank,size,i,buf[1];
    MPI_Status status;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    if (rank ==0){
        for (i =0;i<100*(size-1);i++){
            MPI_Recv(buf,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,
                    MPI_COMM_WORLD,&status);
            printf("Msg = %d from %d with tag %d\n",buf[0],status.MPI_SOURCE,
                    status.MPI_TAG);
        }
    } else{
        for (i = 0;i<100;i++){
            buf[0] = rank + i;
            MPI_Send(buf,1,MPI_INT,0,i,MPI_COMM_WORLD);
        }

    }
    MPI_Finalize();*/

    int myid,numprocs,n,i,j,rc;
    float a[totalsize][mysize+2],b[totalsize][mysize+2],tmp[mysize][totalsize],
    c[totalsize][totalsize];    //除分块大小外，还包括左右两边各一列
    float temp[totalsize];  //临时数组
    int begin_col,end_col,ierr;
    MPI_Status status;

    //初始化c数组
    for (i =0;i<totalsize;i++){
        for (j = 0;j<=totalsize;j++)
            c[i][j] = 0;
    }
    for (int j =0;j<mysize;j++){
        for(int i = 0;i < totalsize;i++)
            tmp[j][i] = 8;
    }
    MPI_Init(&argc,&argv);

    //得到当前进程标识和总的进程个数
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    fprintf(stderr,"Process %d of %d is alive.\n",myid,numprocs);

    //数组初始化
    //整个数组置值为0，行数为totalsize，列数为mysize+2
    for (j=0;j<mysize+2;j++){
        for(i=0;i<totalsize;i++)
            a[i][j] = 0.0;
    }
    if (myid == 0){
        for (int i = 0; i < totalsize; i++) {
            a[i][j] = 8.0;
        }
    }
    if(myid == 3){
        for (int i = 0; i < totalsize; i++) {
            a[i][mysize] = 8.0;
        }
    }
    for (int i = 0; i < mysize+1; i++) {
        a[0][i] = 8.0;
        a[totalsize-1][i] = 8.0;
    }

    //Jacobi迭代部分
    for (int n = 0; n < steps; n++) {
        //这里从两边分别获取数据和发送数据，每次获取或发送数据时，先判断是否会越界
        //从右侧的邻居得到数据
        if (myid < 3){
            MPI_Recv(&temp[0],totalsize,MPI_FLOAT,myid+1,10,MPI_COMM_WORLD,&status);
            for (int i = 0; i < totalsize; i++) {
                a[i][mysize+1] = temp[i];
                //a[i][mysize+1]为块最右一列，表示相邻右侧一块最左边一列
            }
        }
        //向左侧的邻居发送数据
        if (myid > 0){
            for (i = 0;i<totalsize;i++){
                temp[i]=a[i][1];
            }
            MPI_Send(&temp[0],totalsize,MPI_FLOAT,myid-1,10,MPI_COMM_WORLD);
        }
        //向右侧的邻居发送数据
        if (myid < 3){
            for (int i = 0; i < totalsize; i++) {
                temp[i] = a[i][mysize];
            }
            MPI_Send(&temp[0],totalsize,MPI_FLOAT,myid+1,10,MPI_COMM_WORLD);
        }
        //从左侧的邻居得到的数据
        if (myid > 3){
            MPI_Recv(&temp[0],totalsize,MPI_FLOAT,myid-1,10,MPI_COMM_WORLD,&status);
            for (int i = 0; i < totalsize; i++) {
                a[i][0]=temp[i];
                //a[i][0]为块最左一列，表示相邻左侧一块最右一列
            }
        }
        begin_col = 1;
        end_col = mysize;
        if (myid == 0)
            begin_col = 2;
        if (myid == 3)
            end_col = mysize - 1;

        for (j = begin_col;j<end_col;j++){
            for (i = 1;i<totalsize-1;i++)
                b[i][j] = 0.25*(a[i][j+1]+a[i][j-1]+a[i+1][j]+a[i-1][j]);
        }
        for (i = 1;i < totalsize-1;i++){
            for (j = begin_col;j<=end_col;j++){
                a[i][j] = b[i][j];
            }
        }
    }//迭代结束
    int loc = begin_col;

    if (0 == myid)
        loc--;
    for(i = 1; i < totalsize-1;i++){
        for (j=begin_col;j<=end_col;j++){
            tmp[j-loc][i] = a[i][j];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(tmp,16,MPI_FLOAT,c,16,MPI_FLOAT,0,MPI_COMM_WORLD);//需要单独在外面

    //输出结果
    if (0==myid){
        fprintf(stderr,"\nProcess %d:\n",myid);
        for (int j = 0; j < mysize; j++) {
            for (int i = 0; i < totalsize; i++) {
                fprintf(stderr,"%.2f\t",tmp[j][i]);
            }
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
        fprintf(stderr,"\n收集后的结果\n");
        for (int i = 0; i < totalsize; i++) {
            for (int j = 0; j < totalsize; j++) {
                fprintf(stderr,"%.2f\t",c[i][j]);
            }
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
    }
    MPI_Finalize();


    //return 0;
}
