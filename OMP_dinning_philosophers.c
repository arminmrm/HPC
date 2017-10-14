#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#define NUM_PHILOSOPHERS 5
main(int argc, char *argv[]){
    
    int state,i, thread_id;
   
    int forks[NUM_PHILOSOPHERS]={[0 ... NUM_PHILOSOPHERS-1]=1};
          
    
    int lock = 2;
    #pragma omp parallel private(state,i,thread_id) shared(forks,lock) num_threads(NUM_PHILOSOPHERS)
    {
          thread_id = omp_get_thread_num();
          printf("Hello from thread %d\n",thread_id);
          i = 0;
          state =0;
          printf("Philosopher %d state is %d\n",thread_id+1,state);
          for(i=0;i<10;i++)
          {
              #pragma omp critical (grabbing)
              {
                   if(forks[thread_id]==1 && forks[(thread_id+1)% NUM_PHILOSOPHERS]==1 && lock>=0)
                   {
                        state =  1;
                        printf("Philosopher %d is grabbing the forks...\n",thread_id+1);
                        forks[thread_id]=0;
                        forks[(thread_id+1)% NUM_PHILOSOPHERS]= 0;
                        lock -=1;
                    }
               }
               printf("Philosopher %d state is %d\n",thread_id+1,state);
               sleep(10);
               if(state==1)
               {
                   #pragma omp critical (putting_down)
                   {
                        printf("Philosopher %d is putting down the forks...\n",thread_id+1);
                        forks[thread_id] = 1;
                        forks[(thread_id+1)% NUM_PHILOSOPHERS] = 1;
                        lock +=1;
                   }
                   state = 0;

               } 
          }
    }
                                                                                                                    
}


