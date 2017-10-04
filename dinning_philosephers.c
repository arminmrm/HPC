#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int position;
    int count;
    sem_t *forks;
    sem_t *lock;
    sem_t *queue;
    int  *priority;
} params_t;
void initialize_semaphores(sem_t *lock, sem_t *forks, sem_t *queue,int num_forks);
void run_all_threads(pthread_t *threads, sem_t *forks, sem_t *lock,sem_t *qeueu,int* priority,int num_philosophers);

void *philosopher(void *params);
void think(int position);
void eat(int position);
void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


int main(int argc, char *args[])
{
    int num_philosophers = 5;
    sem_t lock;
    sem_t queue;
    sem_t forks[num_philosophers];
    int priority[2]={0,0};
    pthread_t philosophers[num_philosophers];
    initialize_semaphores(&lock, forks,&queue, num_philosophers);
    run_all_threads(philosophers, forks, &lock,&queue,priority, num_philosophers);
    pthread_exit(NULL);
}

void initialize_semaphores(sem_t *lock, sem_t *forks,sem_t *queue, int num_forks)
{
    int i;
    for(i = 0; i < num_forks; i++) {
        sem_init(&forks[i], 0, 1);
    }
    sem_init(lock, 0, num_forks - 1);
    sem_init(queue,0,1);
}

void run_all_threads(pthread_t *threads, sem_t *forks, sem_t *lock,sem_t *queue, int *priority,int num_philosophers)
{
    int i;
    for(i = 0; i < num_philosophers; i++) {
        params_t *arg = malloc(sizeof(params_t));
        arg->position = i;
        arg->count = num_philosophers;
        arg->lock = lock;
        arg->queue =queue;
        arg->forks = forks;
        arg->priority= priority;

        pthread_create(&threads[i], NULL, philosopher, (void *)arg);
    }
}

void *philosopher(void *params)
{
    int i;
    params_t self = *(params_t *)params;
    for(i = 0; i < 3; i++) {
       
        sem_wait(self.lock);
               
        sem_wait(&self.forks[self.position]);
        sem_wait(&self.forks[(self.position + 1) % self.count]);
        sleep(10);
        sem_wait(self.queue);
        printf("Philosepher %d is holding the lock, and queue semaphore is:%d\n",self.position+1,*self.queue);
         
        printf("Currently the priority list is %d,%d\n",self.priority[0],self.priority[1]);
        if(self.priority[1]==self.position+1||self.priority[0]==self.position+1){
             think(self.position+1);
             sem_post(self.queue);
             sleep(10);
             sem_post(&self.forks[self.position]);
             sem_post(&self.forks[(self.position + 1) % self.count]);
             sem_post(self.lock);

             }

        else{ 
            swap(&self.priority[0],&self.priority[1]);
            self.priority[0] = self.position+1;
            eat(self.position+1);
            sem_post(self.queue);
            sleep(10);
            sem_post(&self.forks[self.position]);
            sem_post(&self.forks[(self.position + 1) % self.count]);
            sem_post(self.lock);
            }
         /*
        printf("Philly %d is puting down forks:\n",self.position+1);
         */
       
    }
    
    think(self.position+1);
    pthread_exit(NULL);
}

void think(int position)
{
    printf("Philosopher %d thinking...\n", position);
}

void eat(int position)
{
    printf("Philosopher %d eating...\n", position);
}
