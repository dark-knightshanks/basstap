#ifndef FFT_H
#define FFT_H

#include <stddef.h>

typedef struct {
double real;
double imag;
}Complex;

void dft(
    const double *input,
    Complex *output,
    int N
);

#endif
