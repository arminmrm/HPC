#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;

extern void transpose_img(unsigned char *in_mat, 
                          unsigned char *out_mat, 
                          unsigned int height, 
                          unsigned int width);

double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,  &t);
    return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

int main( int argc, const char** argv ) { 
        // arg 1: Input image 
        // arg 2: output file name
        
        double start_gpu, finish_gpu;
        
        // Read input image from argument in black and white
        Mat input_image = imread(argv[1], IMREAD_GRAYSCALE);

        if (input_image.empty()){
            cout << "Image cannot be loaded..!!" << endl;
            return -1;
        }
        
        unsigned int height = input_image.rows;
        unsigned int  width = input_image.cols;
        
        ///////////////////////
        // START GPU Processing
        ///////////////////////
        
        start_gpu = CLOCK();
   
        // New mat has inverted height/width
        Mat transpose = Mat::zeros(height, width, CV_8U);

        transpose_img((unsigned char *) input_image.data, 
                               (unsigned char *) transpose.data, 
                               height, 
                               width);

        finish_gpu = CLOCK();
        
        cout << "GPU execution time: " << finish_gpu - start_gpu << " ms" << endl;
               
        cout << "writing output image " << argv[2] << endl;
        imwrite (argv[2], transpose);

        return 0;
}
