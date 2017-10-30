#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define N 500
#define BS 20
#define CHUNKSIZE 10
#define min(a, b) (((a) < (b)) ? (a) : (b)) 

void swap(float *xp, float *yp)
{
    float temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void init(int A[][N])
{

    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
             A[i][j] = rand();
}

void print_mat(float A[][N])
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
          {
            if(j==N-1)
                printf("%.2f\n",A[i][j]);
            else 
                printf("%.2f\t",A[i][j]);
           }
}
void LU_factorization(float L[][N], float U[][N])
{
    int i,j,k;
    for(i=0;i<N-1;i++)
      {
        for(j=i+1;j<N;j++)
        {
            L[j][i]=U[j][i]/U[i][i];
            for(k=i;k<N;k++)
                U[j][k] -= L[j][i]*U[i][k];
        }
      }
}
void Forward_subst_blocked(float L[][N], float Z[N][N], float I[N][N])
{
   int i,j,ii,jj,col;
   int th_id;
   int chunk = CHUNKSIZE;
   #pragma omp parallel shared(Z,L,I,chunk) private(i,j,jj,col,th_id) 
   {
      th_id = omp_get_thread_num();
      #pragma omp for schedule(dynamic,chunk) 
      for(col=0; col<N;col++)
      {
        // printf("Hello from thread %d, clmn %d\n",th_id,col);
         for(ii=0;ii<N;ii+=BS)
            for(i=ii;i<min(N,ii+BS);i++)
            {
                Z[col][i] = I[i][col]; 
                for(jj=0;jj<i;jj+=BS)
                    for(j=jj;j<min(i,jj+BS);j++)
                        Z[col][i]-=L[i][j]*Z[col][j];
            }
      }
   }
}
void Backward_subst_blocked(float U[N][N],float A_inv[N][N],float Z[N][N]) 
{
    int i,j,ii,jj,col;
    int chunk=CHUNKSIZE;
    int th_id;

    #pragma omp parallel shared(A_inv,U,Z,chunk) private(th_id,i,j,jj,col)  
    {
      th_id = omp_get_thread_num();
      #pragma omp for schedule(dynamic,chunk) 
      for(col=0; col<N;col++)
      {
        //printf("Hello from thread %d, clmn %d\n",th_id,col);
        for(ii=0;ii<N;ii+=BS)
            for(i=ii;i<min(N,ii+BS);i++)
            {
                A_inv[col][N-i-1] = Z[col][N-i-1]/U[N-i-1][N-i-1];
                for(jj=0;jj<i;jj+=BS)
                    for(j=jj;j<min(i,jj+BS);j++)
                        A_inv[col][N-i-1]-=U[N-i-1][N-j-1]*A_inv[col][N-j-1]/U[N-i-1][N-i-1];
            }
     }
   }
}
void Transpose(float A[][N])
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=i+1;j<N;j++)
        {
            float temp = A[i][j];
            A[i][j] = A[j][i];
            A[j][i] = temp;
         }
}
double CLOCK(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return(t.tv_sec*1000)+(t.tv_nsec*1e-6);
    }
int main()
{
    int A[N][N]; //= {1, 0, 2, 2, -1, 3, 4, 1, 8} ;
    init(A);
    float L[N][N]={0.0}; 
    float U[N][N];
    float Z[N][N]={0.0};
    float A_inv[N][N]={0.0};
    float I[N][N]={0.0};
    int i,j;
    float tend,tstart;
    for(i=0;i<N;i++)
        I[i][i] = 1.0;
    //Copying the marix A, to U matrix, which will be the upper-triangular matrix. 
    for(i=0;i<N;i++) 
        {
        //The loop for initializing L matrix to identity has been fused. 
        L[i][i] = 1;
        for(j=0;j<N;j++)
            U[i][j] = (float)A[i][j]/RAND_MAX;
        }
 //   print_mat(U); 
 //   
   tstart = CLOCK();
    LU_factorization(L, U);
    
    Forward_subst_blocked(L, Z, I);
    //print_mat(U);
    Backward_subst_blocked(U,A_inv,Z);
    Transpose(A_inv);
    tend = CLOCK();
 //   printf("The inverse is:\n");
//    print_mat(A_inv);
    printf("Time taken is %f\n",tend-tstart);
    return 0;
}

