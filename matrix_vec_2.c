#include <omp.h>
#include <stdio.h>


#define M 1000
#define N 1000
#define CHUNKSIZE 50


void print_array(int *x,int len)
{
    int i;
    for(i=0;i<len;i++)
        printf("The %d element is: %d\n", i, x[i]);
}
void initialize_matrix(int matrix[N][M])
{
    int i,j;
    for( i=0;i<N;i++)
        {
        for(j=0;j<M;j++)
            matrix[i][j] = 1;
        }
}
int main()
{
    int i,j;
    int thread_id;
    int n=N,m=M;
    int sum;
    int vec[M]={[0 ... M-1] = 1};
    int vec_mult[N] = {[0 ... N-1]=0};
    int matrix[N][M];
    int chunk = CHUNKSIZE;
    initialize_matrix(matrix);
    
        thread_id = omp_get_thread_num();
    for(i=0;i<n;i++)
    {
         sum=0;
         #pragma omp parallel shared(matrix,vec,m,n,i,chunk) private(j,thread_id) reduction(+:sum)
         {
              thread_id = omp_get_thread_num();
              #pragma omp for schedule(dynamic,chunk) nowait
              for(j=0;j<m;j++)
                  sum += matrix[i][j]*vec[j];
         }
         printf("Element %d is: %d\n",i,sum);
         vec_mult[i] = sum;
      
    }
  
    print_array(vec_mult,N);
 
    return 0;

}

