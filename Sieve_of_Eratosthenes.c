#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#define NUM 20
#define DELTA 4
#define segment_number NUM/DELTA
int a[NUM+1]={[2 ... NUM]=1,[1]=0,[0]=0};
int done=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
struct segment{
    int *arr;
    int L;
};



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
void segment_sieve(int *arr,int L,int R)
{
    int i,j,m;
    for(i=2;i<DELTA+1;i++)
    {
        if(arr[i]==1)
          {
              if(L>=(int)pow(i,2))
              {  
                   m=0;
              }
              else
              {
                   m= (L-(int)pow(i,2))/i +1;
              }
              for(j=(int)pow(i,2)+m*i;j<L+DELTA+1;j=j+i){
                   a[j]=0;
               }
          }
    }
}
void print_primes(int * a,int N)
{
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
    int L = my_segment->L;
    regular_sieve(arr,DELTA);
    pthread_mutex_lock( &mutex );
    done++;
    printf("The firsr segment is done so done is: %d\n",done);
    pthread_cond_signal( &cond ); 
    pthread_mutex_unlock( & mutex );

    return NULL;
}
void *next_sieve(void *arg)
{
    struct segment *my_segment=malloc(sizeof(struct segment*));
    my_segment = (struct segment*)arg;
    int *arr = my_segment->arr;
    int L = my_segment->L;
    pthread_mutex_lock( &mutex );
    while(done <1)
    {
         pthread_cond_wait( & cond, & mutex ); 
    }
    pthread_mutex_unlock( &mutex );    
    segment_sieve(arr,L,L+DELTA-1);
    return NULL;
}
int main()
{   
   /*
    int a[NUM+1]={[2 ... NUM]=1};
    a[0] = 0;
    a[1]=0;
    */
    printf("DELTA , segment num: %d %d\n",DELTA,segment_number);
    pthread_t init_thread;
    pthread_t next_threads[segment_number-1];
    int i;
    struct segment first_segment;
    struct segment next_segments[segment_number-1];
    first_segment.arr = a;
    first_segment.L=2;
    pthread_create(&init_thread,NULL,first_sieve,(void *)&first_segment);
    
    for(i=0;i<segment_number-1;i++)
    {   
        next_segments[i].arr = a;
        next_segments[i].L = (i+1)*DELTA+1;
        pthread_create(&next_threads[i],NULL,next_sieve,(void *)&next_segments[i]);
    }
    pthread_join(init_thread,NULL);
    print_primes(a,NUM+1);
}
