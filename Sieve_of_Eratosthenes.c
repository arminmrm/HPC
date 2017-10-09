#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#define NUM 1000000
#define n_threads 16
/*
int a[NUM+1]={[2 ... NUM]=1,[1]=0,[0]=0};
*/
int done=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
struct segment{
    int *arr;
    int DELTA;
    int index;
};

double CLOCK(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return(t.tv_sec*1000)+(t.tv_nsec*1e-6);
    }

void regular_sieve(int *a,int N)
{
     int sqrt_NUM = floor(sqrt(N));
     int i,j;
     for(i=2;i<sqrt_NUM+1;i++){
         if(a[i]==1){
              for(j=(int)pow(i,2);j<N+1;j=j+i)
               {
                   a[j]=0;
               }
          }
      }
}
void segment_sieve(int *arr,int L,int R,int DELTA)
{
    int i,j,m;
    for(i=2;i<DELTA+1;i++)
    {
        if(arr[i]==1)
          {
              if((int)pow(i,2)>=L)
              {  
                   m=0;
              }
              else
              {
                   m = (int)(ceil(((L-pow(i,2))/i)));
              }
              for(j=(int)pow(i,2)+m*i;j<R+1;j=j+i){
                   arr[j]=0;
               }
          }
    }
}
void print_primes(int * a,int N)
{
    printf("The prime numbers are:\n");
    int i;
    for(i= 2;i<N;i++){
        if(a[i] == 1){
           printf("%d\n",i);
        }
     }
}
void *first_sieve(void *arg)
{
    struct segment *my_segment=malloc(sizeof(struct segment*));
    my_segment = (struct segment*)arg;
    int *arr = my_segment->arr;
    int DELTA = my_segment->DELTA;
    regular_sieve(arr,DELTA);
    pthread_mutex_lock( &mutex );
    done++;
    printf("The first segment is done so done is: %d\n",done);
    pthread_cond_signal( &cond ); 
    pthread_mutex_unlock( & mutex );

    return NULL;
}
void *next_sieve(void *arg)
{
    struct segment *my_segment=malloc(sizeof(struct segment*));
    my_segment = (struct segment*)arg;
    int *arr = my_segment->arr;
    int DELTA = my_segment->DELTA;
    int i = my_segment->index;
    int p_len = (NUM-DELTA)/n_threads;
    pthread_mutex_lock( &mutex );
    while(done <1)
    {
         pthread_cond_wait( & cond, & mutex ); 
    }
    pthread_mutex_unlock( &mutex );    
    if(i < n_threads-1)
        {
        printf("Thread %d is working on numbers from %d to %d\n",i,DELTA+p_len*i+1,DELTA+p_len*(i+1));
        segment_sieve(arr,DELTA+p_len*i+1,DELTA+p_len*(i+1),DELTA);
        }
    else
        {
        printf("Thread %d is working on numbers from %d to %d\n",i,DELTA+p_len*i+1,NUM);
        segment_sieve(arr,DELTA+p_len*i+1,NUM,DELTA);
        }
    return NULL;
}
int main()
{   
   /*
    int a[NUM+1]={[2 ... NUM]=1};
    a[0] = 0;
    a[1]=0;
    */
    double start,end;
    int a[NUM+1]={[2 ... NUM]=1,[1]=0,[0]=0};
    int DELTA = sqrt(NUM);
    printf("Finding prime numbers up to %d, running with %d threads.\n",NUM,n_threads);
    pthread_t init_thread;
    pthread_t next_threads[n_threads];
    int i;
    struct segment first_segment;
    struct segment next_segments[n_threads];
    first_segment.arr = a;
    first_segment.index = 0;
    first_segment.DELTA = DELTA;
    start = CLOCK();
    pthread_create(&init_thread,NULL,first_sieve,(void *)&first_segment);
    pthread_join(init_thread,NULL); 
    for(i=0;i<n_threads;i++)
    {   
        next_segments[i].arr = a;
        /*
        next_segments[i].L = (i+1)*DELTA+1;
        */
        next_segments[i].index = i;
        next_segments[i].DELTA = DELTA;
        pthread_create(&next_threads[i],NULL,next_sieve,(void *)&next_segments[i]);
    }
    
    for(i=0;i<n_threads;i++)
    { 
        pthread_join(next_threads[i],NULL);
    }
    end = CLOCK();
   /* 
    print_primes(a,NUM+1);
    */
    printf("Time taken is %f\n",end-start);
}
