// grand.h

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _STDINT_H
#include <stdint.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _FLOAT_H
#include <float.h>
#endif

int grand(void); // like rand();
int grand_0_m(int m); // result <= m, m+1 outcomes
int grand_n_m(int n, int m); // n <= result <= m, n <= m not checked
double grand_f_0_1(); // different method
double grand_f_0_a(double a, int inclusive); // double < a e.g. with a = 2.0 * M_PI, angles evenly distributed arround circle [with inclusive = 0]
                                             // double <= a [with inclusive = 1]
double grand_f_a_b(double a, double b); // a <= n <= b
double normal(void); // zero mean, unit variance
double normal_d(int d); // recursive combination of 2^d normal() calls
double laplace(void); // scale = 1

typedef struct {
    unsigned *buckets;
    unsigned less;
    unsigned more;
    double min;
    double width;
    int bins;
    double dmin;
    double dmax;
    long double dsum;
    long double d2sum;
    long unsigned dcount;
} histogram;

histogram * histogram_init(double min, double max, double width);
void histogram_count(histogram * h, double datum);
void histogram_dump(histogram * h, int num_decimals);
