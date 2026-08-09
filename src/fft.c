#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <complex.h>
#include <fft.h>
#define M_PI 3.14159265358979323846

void dft(const double *input, Complex *output, int N){
    
    for(int i =0; i<N; i++){
        double real = 0;
        double imag = 0;
        for(int j=0; j<N; j++){

            double angle = 2*M_PI*i*j/N;
            real += input[j]*cos(angle);
            imag -= input[j]*sin(angle);    
        }
        output[i].real = real;
        output[i].imag = imag;
        //double magnitude = sqrt(output[i].real * output[i].real + output[i].imag * output[i].imag);
        //printf("bin %d | magnitude: %.2f\n", i, magnitude);
    }

}


