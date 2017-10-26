#include <stdio.h>
#define N 3
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
void lower_triangular_solver(float L[][N], float Z[N][N], float I[N][N])
{
   //Solve a linear system where the matrix is lower-triangular and the right-hand side is identity matrix.
   int i,j,col;
   for(col=0; col<N;col++)
   {
       for(i=0;i<N;i++)
       {
           Z[i][col] = I[i][col];
           for(j=0;j<i;j++)
               Z[i][col]-=L[i][j]*Z[j][col];
       }
   }
}
void upper_triangular_solver(float U[N][N],float A_inv[N][N],float Z[N][N])
{
    int i,j,col;
    for(col=0; col<N;col++)
    {
        for(i=0;i<N;i++)
        {
            A_inv[N-i-1][col] = Z[N-i-1][col]/U[N-i-1][N-i-1];
            for(j=0;j<i;j++)
                A_inv[N-i-1][col]-=U[N-i-1][N-j-1]*A_inv[N-j-1][col]/U[N-i-1][N-i-1];
        }
    }

} 
int main()
{
    int A[N][N] = {1, 0, 2, 2, -1, 3, 4, 1, 8} ;
    //init(A);
    float L[N][N]={0.0}; 
    float U[N][N];
    float Z[N][N]={0.0};
    float A_inv[N][N]={0.0};
    float I[N][N]={0.0};
    int i,j;
    for(i=0;i<N;i++)
        I[i][i] = 1.0;
    //Copying the marix A, to U matrix, which will be the upper-triangular matrix. 
    for(i=0;i<N;i++) 
        {
        //The loop for initializing L matrix to identity has been fused. 
        L[i][i] = 1;
        for(j=0;j<N;j++)
            U[i][j] = A[i][j];
        }
    LU_factorization(L, U);
    
    lower_triangular_solver(L, Z, I);
    print_mat(U);
    printf("That was u\n Z is:\n");
    print_mat(Z);
    upper_triangular_solver(U,A_inv,Z);
    print_mat(A_inv);
    return 0;
}

