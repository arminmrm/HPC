#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define N 1000
#define BS 64
#define min(a, b) (((a) < (b)) ? (a) : (b)) 
#define NUM_THREADS 32
#define CHUNKSIZE N/NUM_THREADS
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

void print_mat(double A[][N])
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
void LU_factorization(double L[][N], double U[][N])
{
   //Given the matrix U, find its LU decompistion and return the result as L and U matrices.
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
void Forward_subst_blocked(double L[][N], double Z[N][N], double I[N][N])
{
   //Given L, I(identity) solve the system, LZ = I, and store the result in Z.
   int i,j,ii,jj,col;
   int th_id;
   int chunk = CHUNKSIZE;
   #pragma omp parallel shared(Z,L,I,chunk) private(i,j,jj,col,th_id) num_threads(NUM_THREADS)
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
void Backward_subst_blocked(double U[N][N],double A_inv[N][N],double Z[N][N]) 
{
    //Given U and Z solve U*A_inv = Z, where A_inv will be the transpose of the inverse (because of memory optimization). 
    int i,j,ii,jj,col;
    int chunk=CHUNKSIZE;
    int th_id;

    #pragma omp parallel shared(A_inv,U,Z,chunk) private(th_id,i,j,jj,col)  num_threads(NUM_THREADS)
    {
      th_id = omp_get_thread_num();
      #pragma omp for schedule(dynamic,chunk) 
      for(col=0; col<N;col++)
      {
        for(ii=0;ii<N;ii+=BS)
            for(i=ii;i<min(N,ii+BS);i++)
            {
                if(U[N-i-1][N-i-1]!=0)
                     A_inv[col][N-i-1] = Z[col][N-i-1]/U[N-i-1][N-i-1];
                else
                   {
                    printf("Matrix is not invertible!\n");
                    break;
                   }

                for(jj=0;jj<i;jj+=BS)
                    for(j=jj;j<min(i,jj+BS);j++)
                        A_inv[col][N-i-1]-=U[N-i-1][N-j-1]*A_inv[col][N-j-1]/U[N-i-1][N-i-1];
            }
     }
   }
}
void Transpose(double A[][N])
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=i+1;j<N;j++)
        {
            double temp = A[i][j];
            A[i][j] = A[j][i];
            A[j][i] = temp;
         }
}
double CLOCK(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return(t.tv_sec*1000)+(t.tv_nsec*1e-6);
    }
void initialization(double L[][N],double Z[][N], double A_inv[][N],double U[][N], double I[][N]){ 
    //Some initializiations... Note that the matrix that the main matrix is stored in U. 
    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            {
             L[i][j] = 0.0;
             Z[i][j] = 0.0;
             A_inv[i][j] = 0.0;
             I[i][j] = 0.0;
            }
    for(i=0;i<N;i++)
        {
        L[i][i] = 1;
        I[i][i] = 1.0;
        for(j=0;j<N;j++)
            U[i][j] = (double)rand()/RAND_MAX;
        }

}
int main()
{
    //Dynamilcally allocating memory to the matrices.
    //L will be the lower-triangular matrix.
    //Z will be the solution of LZ = I.
    //A_inv will be the inverse of the matrix A.
    double (*L) [N];
    L = malloc(sizeof(*L) * N);
    double (*Z) [N];
    Z = malloc(sizeof(*Z) * N);
    double (*A_inv) [N];
    A_inv = malloc(sizeof(*A_inv) * N); 
    double (*I) [N];
    I =  malloc(sizeof(*I) * N);
   
    double U[N][N];
    
    double tend,tstart;
       
    initialization(L,Z,A_inv,U,I);


   // print_mat(U); 
 //   
    tstart = CLOCK();
    LU_factorization(L, U);
   // print_mat(L);    
   // print_mat(U);
    Forward_subst_blocked(L, Z, I);
   // print_mat(U);
    Backward_subst_blocked(U,A_inv,Z);
    Transpose(A_inv);
    tend = CLOCK();
   // printf("The inverse is:\n");
   // print_mat(A_inv);
    printf("Time taken is %f\n",tend-tstart);
    return 0;
}

