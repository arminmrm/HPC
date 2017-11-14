
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <time.h>
#include <math.h>
#define TIMER_CREATE(t)               \
  cudaEvent_t t##_start, t##_end;     \
  cudaEventCreate(&t##_start);        \
  cudaEventCreate(&t##_end);               
 
 
#define TIMER_START(t)                \
  cudaEventRecord(t##_start);         \
  cudaEventSynchronize(t##_start);    \
 
 
#define TIMER_END(t)                             \
  cudaEventRecord(t##_end);                      \
  cudaEventSynchronize(t##_end);                 \
  cudaEventElapsedTime(&t, t##_start, t##_end);  \
  cudaEventDestroy(t##_start);                   \
  cudaEventDestroy(t##_end);     
  
#define TILE_SIZE 16
#define CUDA_TIMING

unsigned char *input_gpu;
unsigned char *output_gpu;

/*******************************************************/
/*                 Cuda Error Function                 */
/*******************************************************/
inline cudaError_t checkCuda(cudaError_t result) {
    #if defined(DEBUG) || defined(_DEBUG)
    if (result != cudaSuccess) {
        fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
        exit(-1);
    }
    #endif
    return result;
}
                
// GPU kernel and functions
__global__ void kernel(unsigned char *input, 
                       unsigned char *output,
                       unsigned int height,
                       unsigned int width){
        
    int x = blockIdx.x*TILE_SIZE+threadIdx.x;
    int y = blockIdx.y*TILE_SIZE+threadIdx.y;
    
    int i,j; 
    int sumx,sumy;
    unsigned long int sum;
    int hx[9] = {1,0,-1,2,0,-2,1,0,-1};
    int hy[9] = {1,2,1,0,0,0,-1,-2,-1};
    
    if (x < width && y < height ){ 
        sumx=0;
        sumy=0;
        for(j=x-1;j<x+2;j++)
            for(i=y-1;i<y+2;i++)
                if(j>=0 && j<width && i>=0 && i<height)
                {
                    sumx += input[i*width+j]*hx[(y-i+1)*3+x-j+1];
                    sumy += input[i*width+j]*hy[(y-i+1)*3+x-j+1];
                }
    }
    
    sum = sumx*sumx+sumy*sumy;

  
    if(sum>100)
        output[y*width+x] = 255;
    else
        output[y*width+x] = 0;    
}

void transpose_img(unsigned char *in_mat, 
                   unsigned char *out_mat, 
                   unsigned int height, 
                   unsigned int width){
                         
    int gridXSize = 1 + (( width - 1) / TILE_SIZE);
    int gridYSize = 1 + ((height - 1) / TILE_SIZE);
    
    int XSize = gridXSize*TILE_SIZE;
    int YSize = gridYSize*TILE_SIZE;
    
    // Both are the same size (CPU/GPU).
    int size = XSize*YSize;
    
    // Allocate arrays in GPU memory
    checkCuda(cudaMalloc((void**)&input_gpu    , size*sizeof(unsigned char)));
    checkCuda(cudaMalloc((void**)&output_gpu  , size*sizeof(unsigned char)));
    
    checkCuda(cudaMemset(output_gpu , 0 , size*sizeof(unsigned char)));
                
    // Copy data to GPU
    checkCuda(cudaMemcpy(input_gpu, 
                        in_mat, 
                        height*width*sizeof(char), 
                        cudaMemcpyHostToDevice));

    checkCuda(cudaDeviceSynchronize());
    
    // Execute algorithm
    dim3 dimGrid(gridXSize, gridYSize);
    dim3 dimBlock(TILE_SIZE, TILE_SIZE);

    #if defined(CUDA_TIMING)
        float Ktime;
        TIMER_CREATE(Ktime);
        TIMER_START(Ktime);
    #endif
    
    // Kernel Call
    kernel<<<dimGrid, dimBlock>>>(input_gpu, output_gpu, height, width);
    
    checkCuda(cudaDeviceSynchronize());
    
    #if defined(CUDA_TIMING)
        TIMER_END(Ktime);
        printf("Kernel Execution Time: %f ms\n", Ktime);
    #endif
        
    // Retrieve results from the GPU
    checkCuda(cudaMemcpy(out_mat, 
                        output_gpu, 
                        height*width*sizeof(unsigned char), 
                        cudaMemcpyDeviceToHost));
                        
    // Free resources and end the program
    checkCuda(cudaFree(output_gpu));
    checkCuda(cudaFree(input_gpu));

}
