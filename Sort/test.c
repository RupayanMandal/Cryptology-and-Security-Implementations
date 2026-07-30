#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort.h"

#define my_RAND_MAX 10000;

int* create_rand_arr(int size)
{
    srand(time(NULL));

    int* arr=(int*)malloc(size* sizeof(int));
    int i;
    for(i=0;i<size;i++) 
        arr[i]=rand()%my_RAND_MAX;
    
    return arr;
} 

void copy_arr(int* org_arr, int* new_arr, int size)
{
    int i;
    for(i=0;i<size;i++)
        new_arr[i]=org_arr[i];
}

void print_arr(const double arr[],int size)
{
    int i;
    for(i=0;i<size;i++)
        printf(" %.2f", arr[i]);
}


int main()
{
    int size=100;
    int iteration=100;
    int iter;

    double time_arr_quicksort[10];
    double time_arr_mergesort[10];
    double time_arr_heapsort[10];

    clock_t start, end;

    for(size=100;size<=1000;size+=100)
    {
        
        int* new_arr=(int*)malloc(size* sizeof(int));

        double total_merge_time = 0.0;
        double total_heap_time  = 0.0;
        double total_quick_time = 0.0;

        for(iter=0;iter<iteration;iter+=1)
        {   
            int* org_arr=create_rand_arr(size);
            //1. MergeSort Time
            copy_arr(org_arr, new_arr, size);
            start = clock();
            mergeSort(new_arr, 0, size - 1);
            end = clock();
            total_merge_time += ((double)(end - start)) / CLOCKS_PER_SEC;
            //printf("%f ",((double)(end - start)) / CLOCKS_PER_SEC);

            //1. QuickSort Time
            copy_arr(org_arr, new_arr, size);
            start = clock();
            quickSort(new_arr, 0, size - 1);
            end = clock();
            total_quick_time += ((double)(end - start)) / CLOCKS_PER_SEC;

            //1. HeapSort Time
            copy_arr(org_arr, new_arr, size);
            start = clock();
            heapSort(new_arr, size);
            end = clock();
            total_heap_time += ((double)(end - start)) / CLOCKS_PER_SEC;

            free(org_arr);
        }

        time_arr_mergesort[(size/100)-1]=(total_merge_time/iteration)*1000;
        time_arr_quicksort[(size/100)-1]=(total_quick_time/iteration)*1000;
        time_arr_heapsort[(size/100)-1]=(total_heap_time/iteration)*1000;
        free(new_arr);
    }

    printf("\nSize     ->  100   200  300  400  500 600 700 800 900 1000");
    printf("\nTime in ms");
    printf("\nMergeSort->");
    print_arr(time_arr_mergesort,10);
    printf("\nQuickSort->");
    print_arr(time_arr_quicksort,10);
    printf("\nHeapSort->");
    print_arr(time_arr_heapsort,10);


    return 1;
}