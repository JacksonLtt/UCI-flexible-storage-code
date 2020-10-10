/**
  test_single_to_single.c
 点对点传输10000次，数据量变化采用动态内存方式, 传4kb, 1mb, 100mb, 400mb
**/
 
#include<stdio.h>//标准输入输出头文件
#include<stdlib.h>//标准库
#include<mpi.h>//mpi的接口

 
int main(int argc,char *argv[])
{

    int rank, size;
    MPI_Init(&argc,&argv);//启动并行环境
    MPI_Comm_size(MPI_COMM_WORLD, &size);//获取总进程数
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);//获取本地进程编号
    int times = 1000;
    FILE *fp;
    fp=fopen("1mb_1000.txt","a+");

    for (int i=1; i<=times; i++) {
        MPI_Status status_r;
        MPI_Request handle_r;
        
        int flag_r = 0;
        int p_size =250000;// 4kb->1000; 1mb->250000
        int buzzer[p_size];
        int num = 1;
        int *p;// 动态分配内存,存放int型数据 1个int 4个字节（Byte）
        
        p = (int*)malloc( (sizeof(int))*p_size); //分配i个内存空间，每个大小为sizeof(int)*1000
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();
        if(rank==0)
        {
            MPI_Irecv (&buzzer[0], p_size, MPI_INT, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &handle_r);
            while(flag_r == 0){
                double end_time = MPI_Wtime();
                double diff = end_time-start_time;
                MPI_Test(&handle_r, &flag_r, &status_r);    //监听消息接收状态，接收完成，返回flag_s为true，否则为false
                if(flag_r==1){
                    printf("%d---diff%lf:\n",i,diff);
                    fprintf(fp,"%lf\n",diff);
                }
            }
        }
        else
        {
            MPI_Send(p, p_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }


        free(p);
    }
    fclose(fp);
    MPI_Finalize();//结束并行环境
    
    return 0;
}


