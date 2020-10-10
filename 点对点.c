/**
  test_single_to_single.c
 点对点传输10000次，数据量变化采用动态内存方式, 传4kb, 1mb, 100mb, 400mb
**/
 
#include<stdio.h>//标准输入输出头文件
#include<stdlib.h>//标准库
#include<mpi.h>//mpi的接口

 
int main(int argc,char *argv[])
{

    int i, k, my_rank, nprocs,times;
    double start_time,end_time;
    int *p;// 动态分配内存,存放int型数据 1个int 4个字节（Byte）



    MPI_Init(&argc,&argv);//启动并行环境
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);//获取总进程数
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);//获取本地进程编号
    MPI_Status status_r;
    MPI_Request handle_r;
    int flag_r =0;
    int size =1;
    int num = 1;
    times = 2;
    FILE *fp;
    fp=fopen("4kb_10.txt","a+");
//
//
//    if(my_rank == 0){
//        fp=fopen("4kb_0.txt","a+");
//    }
//    else if(my_rank == 1){
//        fp=fopen("4kb_1.txt","a+");
//    }
    
       

    for (i=1; i<=times; i++) {
        p = (int*)malloc( (sizeof(int))*size); //分配i个内存空间，每个大小为sizeof(int)*1000 即4000B=4kb, 分配的内存内随机赋值

        if(!p)
           {
             printf("动态分配内存失败！\n");
             exit(1);
            }


        MPI_Barrier(MPI_COMM_WORLD);
          if(my_rank==0)
            {

//                start_time=MPI_Wtime();//获取墙上时间
                printf("Before sending time %d: %lf\n", i,MPI_Wtime());
                MPI_Send(p, size, MPI_INT, 1, i, MPI_COMM_WORLD);
                printf("After 1 sending time %d: %lf\n",i, MPI_Wtime());
                MPI_Send(p, size, MPI_INT, 1, i, MPI_COMM_WORLD);
                printf("After 2 sending time %d: %lf\n",i, MPI_Wtime());
//                fprintf(fp,"0,%d,%lf\n", i,start_time);
            }
          else if(my_rank==1)
            {
                printf("Before recving time %d: %lf\n", i,MPI_Wtime());
                MPI_Irecv (p, size*times, MPI_INT, 0, i, MPI_COMM_WORLD, &handle_r);
                while(flag_r == 0){
                    MPI_Test(&handle_r, &flag_r, &status_r);    //监听消息接收状态，接收完成，返回flag_s为true，否则为false
                }
                printf("After recving time %d: %lf\n", i,MPI_Wtime());
                MPI_Irecv (p, size*times, MPI_INT, 0, i, MPI_COMM_WORLD, &handle_r);
                flag_r = 0;
                while(flag_r == 0){
                    MPI_Test(&handle_r, &flag_r, &status_r);    //监听消息接收状态，接收完成，返回flag_s为true，否则为false
                }
                //                MPI_Wait(&handle_r, &status);
                printf("After recving time %d: %lf\n", i,MPI_Wtime());
//                MPI_Send(p, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
//                fprintf(fp,"1,%d,%lf\n", i,end_time);
             }


        free(p);
//        sleep(5);

    }
    fclose(fp);
    MPI_Finalize();//结束并行环境
    
    return 0;
}


