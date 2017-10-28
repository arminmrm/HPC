#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N 620000
int main(int argc, char *argv[])  {
   int numtasks, rank, dest, source, rc, count=0, tag=1;  
   char inmsg;
   char hostname[MPI_MAX_PROCESSOR_NAME];
   int len;
   int i;
   int num_samples;
   double PI;
   MPI_Status Stat; 


   MPI_Init(&argc,&argv);

   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   num_samples = N/(numtasks-1);
   if(rank!=0)
   {
   float x,y;
   srand((unsigned)rank);
   for(i=0;i<num_samples;i++)
   {
       x = (float)rand()/RAND_MAX;
       y = (float)rand()/RAND_MAX;
       //printf("%f,%f\n",x,y);
       if(pow(x,2)+pow(y,2)<=1)
           count++;
   }    
   printf("Hello from process %d and cnt is %d\n",rank,count);
   }
   else
   {
        printf("The number of processes %d.\n",numtasks);
   }
   MPI_Reduce (&count,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
   MPI_Finalize();
   
   if(rank==0)
    {
       printf("On the root process count is: %d\n",count);
       PI = 4*(double)count/N;
       printf("The estimated value of Pi number is %lf\n",PI);
    }
}
