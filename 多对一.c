// 多个非阻塞通信（非缓冲）阻塞发送+非阻塞接收 16 传 1
// 测试 MPI_Testsome(int incount,MPI_Request array_of_requests[],int *outcount,int,array_of_indices[],MPI_Status array_of_statuses[])
//     MPI_TESTSOME和MPI_WAITSOME类似, 只不过它可以立即返回. 有几个非阻塞通信
//     已经完成, 则outcount就等于几, 而且完成对象在array_of_requests 中的下标依次记录在完成
//     对象下标数组array_of_indices中, 完成状态记录在相应的状态数组array_of_statuses中,
//     若没有非阻塞通信完成,则返回值outcount=0

// Behaves like MPI_WAITSOME, except that it returns immediately. If no operation has completed
// it returns outcount = 0. If there is no active handle in the list it returns outcount =
// MPI_UNDEFINED.

#include <mpi.h>
#include<stdio.h>//标准输入输出头文件
#include<stdlib.h>//标准库
#include <unistd.h>

#define SIZE_RECV 97 // num_of_node*num_of_sending_times+1

void print_arr_recording(int arr[])
{
    printf("array_of_indices[");
    for (int i=0;i<16;i++){
        printf("%d,",arr[i]);
    }
    printf("]\n");
}

void print_status_source(MPI_Status arr[],int count,int *temp_status_source)
{
//    printf("array_of_source[");
    for (int i=0;i<count;i++){
        if(arr[i].MPI_SOURCE >= 1 && arr[i].MPI_SOURCE<=16){
//            printf("%d,",arr[i].MPI_SOURCE);
            temp_status_source[i] = arr[i].MPI_SOURCE;
        }
    }
//    printf("]\n");
}

void print_status_tag(MPI_Status arr[],int count)
{
    printf("array_of_tag[");
    for (int i=0;i<count;i++){
        if(arr[i].MPI_SOURCE >= 1 && arr[i].MPI_SOURCE<=16){
             printf("%d,",arr[i].MPI_TAG);
        }
    }
    printf("]\n");
}

void set_zero(int *arr)
{
    for (int i=0;i<SIZE_RECV;i++){
        arr[i]=0;
    }
}

void set_status(MPI_Status *arr)
{
    MPI_Status temp_status;
    for (int i=0;i<SIZE_RECV;i++){
        arr[i]=temp_status;
    }
    
}

void set_request(MPI_Request *arr)
{
    MPI_Request temp_request;
    for (int i=0;i<SIZE_RECV;i++){
        arr[i]=temp_request;
    }
    
}

int check_status(int *arr){
    int count_4 = 0;
    int count_5 = 0;
    int count_6 = 0;
    for(int i = 0; i<16;i++){
        if(arr[i]>=4){
            count_4 +=1;
        }
        if(arr[i]>=5){
            count_5 +=1;
        }
        if(arr[i]>=6){
            count_6 +=1;
        }
    }
    if(count_4 >= 15){
        return 4;
    }
    if(count_5 >= 12){
        return 5;
    }
    if(count_6 >= 10){
        return 6;
    }
    return 0;
}


//void update_record_result

int main(int argc, char *argv[])
{
    int rank, size;
    int times = 1000;
    
    int rt = MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int i, count;
    MPI_Request r[SIZE_RECV];
    MPI_Status status[SIZE_RECV];
    FILE *fp;
    fp=fopen("4kb_4_1000.txt","a+");
    
    for (int times_i=0;times_i<times;times_i++){
        set_status(status);
        set_request(r);
        int count = 0;
        int index[SIZE_RECV];
        int record_result[size];
        int temp_status_source[SIZE_RECV];
        int *p;// 动态分配内存,存放int型数据 1个int 4个字节（Byte）
        int p_size = 1000;//4kb->1000;1mb->250000
        int buffer[p_size];
        p = (int*)malloc( (sizeof(int))*p_size); //分配i个内存空间，每个大小为sizeof(int)*1000
        int total = 0;
        int flag_check = 0;
        set_zero(temp_status_source);
        set_zero(index);
        
        for (int temp_i=0;temp_i<16;temp_i++){
            record_result[temp_i] = 0;
        }
        
        
        
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();
        if (rank == 0)
        {
            int remaining = SIZE_RECV-1;
            for (i=1; i<SIZE_RECV; i++)
            {
                MPI_Irecv(&buffer[0], p_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &r[i-1]);
            }
            while (remaining > 0)
            {
                double end_time = MPI_Wtime();
                double diff = end_time-start_time;
                MPI_Testsome(SIZE_RECV-1, r, &count, index, status);
                if (count > 0)
                {
    //                printf("%d finished\n", count);
                    print_status_source(status,count,temp_status_source);
    //                print_status_tag(status,count);
                    set_status(status);
                    
                    for(int k = 0; k<count;k++){
                        int ind = temp_status_source[k];
                        record_result[ind-1] +=1;
                    }
                    int check = check_status(record_result);
                    if(check != 0 && flag_check ==0){
                        printf("%dCOUNT_%d---diff%lf:\n",times_i,check,diff);
                        fprintf(fp,"%d,%lf\n", check,diff);
                        flag_check=1;
                    }
//                    print_arr_recording(record_result);
                    remaining -= count;
//                    total += count;
//                    printf("total %d: \n",total);
                }
            }
        }
        else
        {
            MPI_Send(p, p_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(p, p_size, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(p, p_size, MPI_INT, 0, 3, MPI_COMM_WORLD);
            MPI_Send(p, p_size, MPI_INT, 0, 4, MPI_COMM_WORLD);
            MPI_Send(p, p_size, MPI_INT, 0, 5, MPI_COMM_WORLD);
            MPI_Send(p, p_size, MPI_INT, 0, 6, MPI_COMM_WORLD);
        }
        free(p);
       
    }
    fclose(fp);
    MPI_Finalize();
    return 0;
}
