#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int swap_count=0;
unsigned int comparison_count=0;

//1. Merge sort
void merge(int arr[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));


    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];

    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2)
    {
        comparison_count++;
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];

    free(L);    
    free(R);
}

void mergeSort(int arr[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}




//2. Quick sort
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
    swap_count+=1;
}

int partition(int arr[], int low, int high)
{
    int pivotIndex = low + rand() % (high - low + 1);

    // Move pivot to the end
    swap(&arr[pivotIndex], &arr[high]);

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (arr[j] < pivot)
        {
            comparison_count++;
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);

    return i + 1;

}

void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}



//3. heapsort
void heapify(int arr[], int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // Check left child
    if (left < n && arr[left] > arr[largest]){
        comparison_count++;
        largest = left;}

    // Check right child
    if (right < n && arr[right] > arr[largest]){
        comparison_count++;
        largest = right;}

    // If largest is not the root
    if (largest != i)
    {
        swap(&arr[i], &arr[largest]);

        // Heapify the affected subtree
        heapify(arr, n, largest);
    }
}

void heapSort(int arr[], int n)
{
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements one by one
    for (int i = n - 1; i > 0; i--)
    {
        swap(&arr[0], &arr[i]);

        // Restore heap
        heapify(arr, i, 0);
    }
}



/*
int main()
{
    int arr[] = {38, 27, 43, 3, 9, 82, 10};
    int n = sizeof(arr) / sizeof(arr[0]);

    //mergeSort(arr, 0, n - 1);
    //quickSort(arr, 0, n - 1);
    heapSort(arr, n);
    printf("Sorted array: ");

    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    return 0;
}
*/