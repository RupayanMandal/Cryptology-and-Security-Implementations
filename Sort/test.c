#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort.h"

#define my_RAND_MAX 10000
#define iteration 1000

extern unsigned int swap_count;
extern unsigned int comparison_count;

int* create_rand_arr(int size)
{
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

void print_doublearr(const double arr[],int size)
{
    int i;
    for(i=0;i<size;i++)
        printf(" %.2f", arr[i]);
}

void print_uintarr(const unsigned int arr[],int size)
{
    int i;
    for(i=0;i<size;i++)
        printf(" %u", arr[i]);
}



int main()
{
    int size=100;
    int iter;

    double time_arr_quicksort[10];
    double time_arr_mergesort[10];
    double time_arr_heapsort[10];

    unsigned int swap_arr_quicksort[10];
    unsigned int swap_arr_mergesort[10];
    unsigned int swap_arr_heapsort[10];

    unsigned int comp_arr_quicksort[10];
    unsigned int comp_arr_mergesort[10];
    unsigned int comp_arr_heapsort[10];


    clock_t start, end;

    FILE *fp_time = fopen("time.csv", "w");
    if (!fp_time) {
        fprintf(stderr, "could not open time.csv\n");
        return 1;
    }
    fprintf(fp_time, "size,mergesort,heapsort,quicksort\n");

    FILE *fp_swap = fopen("swap.csv", "w");
    if (!fp_swap) {
        fprintf(stderr, "could not open swap.csv\n");
        return 1;
    }
    fprintf(fp_swap, "size,mergesort,heapsort,quicksort\n");

    FILE *fp_comp = fopen("comp.csv", "w");
    if (!fp_comp) {
        fprintf(stderr, "could not open comp.csv\n");
        return 1;
    }
    fprintf(fp_comp, "size,mergesort,heapsort,quicksort\n");


    srand(time(NULL));

    for(size=100;size<=1000;size+=100)
    {
        
        int* new_arr=(int*)malloc(size* sizeof(int));

        double total_merge_time = 0.0;
        double total_heap_time  = 0.0;
        double total_quick_time = 0.0;

        unsigned int total_merge_swap=0;
        unsigned int total_heap_swap=0;
        unsigned int total_quick_swap=0;

        unsigned int total_merge_comp=0;
        unsigned int total_heap_comp=0;
        unsigned int total_quick_comp=0;

        for(iter=0;iter<iteration;iter+=1)
        {   
            int* org_arr=create_rand_arr(size);
            //1. MergeSort Time
            swap_count=0;
            comparison_count=0;
            copy_arr(org_arr, new_arr, size);
            start = clock();
            mergeSort(new_arr, 0, size - 1);
            end = clock();
            total_merge_time += ((double)(end - start)) / CLOCKS_PER_SEC;
            total_merge_swap+=swap_count;
            total_merge_comp+=comparison_count;
            //printf("%f ",((double)(end - start)) / CLOCKS_PER_SEC);

            //1. QuickSort Time
            swap_count=0;
            comparison_count=0;
            copy_arr(org_arr, new_arr, size);
            start = clock();
            quickSort(new_arr, 0, size - 1);
            end = clock();
            total_quick_time += ((double)(end - start)) / CLOCKS_PER_SEC;
            total_quick_swap+=swap_count;
            total_quick_comp+=comparison_count;
            //printf("Quick: %u",swap_count);

            //1. HeapSort Time
            swap_count=0;
            comparison_count=0;
            copy_arr(org_arr, new_arr, size);
            start = clock();
            heapSort(new_arr, size);
            end = clock();
            total_heap_time += ((double)(end - start)) / CLOCKS_PER_SEC;
            total_heap_swap+=swap_count;
            total_heap_comp+=comparison_count;
            free(org_arr);
        }
        free(new_arr);

        int k=(size/100)-1;
        time_arr_mergesort[k]=(total_merge_time/iteration)*1000;
        time_arr_quicksort[k]=(total_quick_time/iteration)*1000;
        time_arr_heapsort[k]=(total_heap_time/iteration)*1000;
        
        swap_arr_mergesort[k]=total_merge_swap/iteration;
        swap_arr_quicksort[k]=total_quick_swap/iteration;
        swap_arr_heapsort[k]=total_heap_swap/iteration;

        comp_arr_mergesort[k]=total_merge_comp/iteration;
        comp_arr_quicksort[k]=total_quick_comp/iteration;
        comp_arr_heapsort[k]=total_heap_comp/iteration;
        
        
        
        printf("\nPrinting %d",k+1);
        fprintf( fp_time, "%d,%14.4f,%14.4f,%14.4f\n", size, time_arr_mergesort[k], time_arr_heapsort[k], time_arr_quicksort[k]);
        fprintf( fp_swap, "%d,%u,%u,%u\n",size,swap_arr_mergesort[k], swap_arr_heapsort[k], swap_arr_quicksort[k]);
        fprintf( fp_comp, "%d,%u,%u,%u\n",size,comp_arr_mergesort[k], comp_arr_heapsort[k], comp_arr_quicksort[k]);
        //printf("\n\n\n");
    }

    fclose(fp_time);
    fclose(fp_swap);
    fclose(fp_comp);
    printf("\nSize     ->  100   200  300  400  500 600 700 800 900 1000");
    printf("\nTime in ms");
    printf("\nMergeSort->");
    print_doublearr(time_arr_mergesort,10);
    printf("\nQuickSort->");
    print_doublearr(time_arr_quicksort,10);
    printf("\nHeapSort->");
    print_doublearr(time_arr_heapsort,10);

    printf("\nSwap");
    printf("\nMergeSort->");
    print_uintarr(swap_arr_mergesort,10);
    printf("\nQuickSort->");
    print_uintarr(swap_arr_quicksort,10);
    printf("\nHeapSort->");
    print_uintarr(swap_arr_heapsort,10);

    printf("\nComparisons");
    printf("\nMergeSort->");
    print_uintarr(comp_arr_mergesort,10);
    printf("\nQuickSort->");
    print_uintarr(comp_arr_quicksort,10);
    printf("\nHeapSort->");
    print_uintarr(comp_arr_heapsort,10);
    
    
    FILE *gp = popen("gnuplot -persist", "w");

    if (!gp) {
        fprintf(stderr, "could not open gnuplot (is it installed?)\n");
        return 1;
    }


    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key autotitle columnhead\n");
    fprintf(gp, "set multiplot layout 3,1\n");
    fprintf(gp, "set mouse\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key outside\n");

    /* First graph: execution time */
    fprintf(gp, "set xlabel 'Array size'\n");
    fprintf(gp, "set ylabel 'Time (ms)'\n");
    fprintf(gp, "set title 'Sorting algorithm - Execution Time'\n");

    fprintf(gp,
            "plot 'time.csv' using 1:2 with linespoints title 'Mergesort', "
            "'' using 1:3 with linespoints title 'Heapsort', "
            "'' using 1:4 with linespoints title 'Randomised Quicksort'\n");

    /* Second graph: number of swaps */
    fprintf(gp, "set xlabel 'Array size'\n");
    fprintf(gp, "set ylabel 'Swaps'\n");
    fprintf(gp, "set title 'Sorting algorithm - Number of Swaps'\n");

    fprintf(gp,
            "plot 'swap.csv' using 1:2 with linespoints title 'Mergesort', "
            "'' using 1:3 with linespoints title 'Heapsort', "
            "'' using 1:4 with linespoints title 'Randomised Quicksort'\n");


    /*Third graph: number of comparisons*/
    fprintf(gp, "set xlabel 'Array size'\n");
    fprintf(gp, "set ylabel 'Comparisons'\n");
    fprintf(gp, "set title 'Sorting algorithm - Number of comparisons'\n");

    fprintf(gp,
            "plot 'comp.csv' using 1:2 with linespoints title 'Mergesort', "
            "'' using 1:3 with linespoints title 'Heapsort', "
            "'' using 1:4 with linespoints title 'Randomised Quicksort'\n");
        
    fprintf(gp, "unset multiplot\n");

    fprintf(gp, "pause mouse close\n");
    fflush(gp);
    pclose(gp);;
    
    return 1;

}

/*
gcc test.c sort.c -o test.exe
./test.exe
*/