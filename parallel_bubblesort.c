//
//  main.c
//  parallel_bubble_sort
//
//  Created by Armin Moharrer on 9/18/17.
//  Copyright © 2017 Armin Moharrer. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#define DIM 100000
#define NTHREADS 2

int a[DIM], b[DIM],swapped = 0;
pthread_t thread[DIM];
struct thread_data{
    int  i;
    int  j;
};
struct thread_data thread_data_array[NTHREADS];
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    
    /* create temp arrays */
    int L[n1], R[n2];
    
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
    
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    /* Copy the remaining elements of L[], if there
     are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    /* Copy the remaining elements of R[], if there
     are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void v_initiate() {
    int i;
    srand(1993);
    for(i = 0; i < DIM; i++)
        a[i] = rand() % DIM;
    
}
void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
void bubbleSort(int arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++)
        
        // Last i elements are already in place
        for (j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1])
                swap(&arr[j], &arr[j+1]);
}
void * subsort(void *ind){
    int i = (int)ind;
    int j;
    int mpoint=DIM/NTHREADS;
    int c[mpoint];
    for(j=0;j<mpoint;j++){
        c[j] = a[j+i*DIM/NTHREADS];
    }
    bubbleSort(c, mpoint);
    for(j=0;j<mpoint;j++){
        a[j+i*DIM/NTHREADS]=c[j];
    }
    return NULL;
}
double CLOCK(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return(t.tv_sec*1000)+(t.tv_nsec*1e-6);
    }
void * submerge(void *threadarg){
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    int i = my_data->i;
    int j =my_data->j;
    int mpoint = DIM/j;
    int l = i*mpoint;
    int r = l+mpoint-1;
    int m = l+(r-l)/2;
    merge(a,l,m,r);
    return NULL;
}
int main() {
    int i,j, blocks,m,par_impar = 0;
    pthread_t THREADS[NTHREADS];
    v_initiate();
    double start, end;
    start = CLOCK();
    m = DIM/NTHREADS;
    printf("Original array: \n");
    printArray(a, DIM);
    printf("%d\n",a[0]);
    for(i=0;i<NTHREADS;i++){
        pthread_create(&THREADS[i],NULL, subsort, (void *)i);
    }
    for(i=0;i<NTHREADS;i++){
        pthread_join(THREADS[i],NULL);
    }
    int levels = log2((float)NTHREADS);
    for(j=levels;j>0;j--){
        blocks = (int)pow(2., (float)(j-1));
        printf("blocks %d \n",blocks);
        for(i=0;i<blocks;i++){
            printf("i is %d\n",i);
            thread_data_array[i].i=i;
            thread_data_array[i].j=blocks;
            pthread_create(&THREADS[i],NULL, submerge, (void *)&thread_data_array[i]);
            }
        for(i=0;i<blocks;i++){
            pthread_join(THREADS[i],NULL);
        }
        printf("half sorted i\n");
        printArray(a, DIM);
        
    }
    end = CLOCK();
    printf("Time taken %f\n",end-start);
    printf("Sortd Array\n");
    printArray(a, DIM);
    printf("Time taken %f\n",end-start);
    return 0;
}
