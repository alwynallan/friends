// grand.c replacement for rand() and some other functions

// $ gcc -Wall -O3 -c grand.c

#include "grand.h"
#include <assert.h>

FILE * ur = NULL;

int grand(void)
{
  uint32_t bits;

  if(ur==NULL) ur=fopen("/dev/urandom","rb");
  assert(ur != NULL);
  assert(1 == fread(&bits, sizeof(bits), 1, ur));
  bits &= 0x7FFFFFFF;
  return bits;
}

int grand_0_m(int m) // result <= m
{
  unsigned mask = 0x7FFFFFFF;
  int res;

  while((mask/2) >= m) mask /= 2;
  do {
    res = (unsigned)grand() & mask;
  } while(res > m);
  return res;
}

int grand_n_m(int n, int m) // n <= result <= m, n <= m not checked
{
  return n + grand_0_m(m - n);
}

double grand_f_0_1() { // https://stackoverflow.com/a/26867455
  uint64_t  bits;
  
  if(ur==NULL) ur=fopen("/dev/urandom","rb");
  assert(ur != NULL);
  assert(1 == fread(&bits, sizeof(bits), 1, ur));
  return bits * 5.421010862427522170037264004349e-020;
}

double grand_f_0_a(double a, int inclusive) {
  uint64_t  bits;
  
  if(ur==NULL) ur=fopen("/dev/urandom","rb");
  assert(ur != NULL);
  assert(1 == fread(&bits, sizeof(bits), 1, ur));
  if(inclusive) return ((double)bits/((double)(UINT64_MAX)    )) * a;
  else          return ((double)bits/((double)(UINT64_MAX)+1.0)) * a;
}

double grand_f_a_b(double a, double b) {
  return a + grand_f_0_a(b-a, 1);
}

double normal(void) { // https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
// if performance is critical, this might be quicker https://en.wikipedia.org/wiki/Ziggurat_algorithm
  static int have_z1 = 0;
  static double z1;
  double u, v, s, t;
  
  if(have_z1) {
    have_z1 = 0;
    return z1;
  }
  if(ur==NULL) ur=fopen("/dev/urandom","rb");
  do {
    u = grand_f_a_b(-1.0, 1.0);
    v = grand_f_a_b(-1.0, 1.0);
    s = u*u + v*v;
  } while(s == 0.0 || s >= 1.0);
  t = sqrt(-2.0 * log(s) / s);
  z1 = v * t;
  have_z1 = 1;
  return u * t; // z0
}

double normal_d(int d) { // combines 2^d normal() results
  if(d<=0) return normal();
  return M_SQRT1_2 * (normal_d(d-1) + normal_d(d-1));
}

double laplace(void) {
  return log(1. - grand_f_0_a(1.,0)) - log(1. - grand_f_0_a(1.,0));
}

// histogram is a poor fit here, but it's home for now

histogram * histogram_init(double min, double max, double width) {
  histogram *h;
  
  h = malloc(sizeof(histogram));
  h->min = min;
  h->width = width;
  h->bins = round((max-min)/width);
  h->less = 0u;
  h->more = 0u;
  h->buckets = calloc(h->bins, sizeof(unsigned));
  h->dmin = DBL_MAX;
  h->dmax = -DBL_MAX;
  h->dsum = 0.;
  h->d2sum = 0.;
  h->dcount = 0u;
  return h;
}

void histogram_count(histogram * h, double datum) {
  int bin = floor((datum - h->min)/h->width);
  if(bin < 0) h->less++;
  else if(bin >= h->bins) h->more++;
  else h->buckets[bin]++;
  if(datum > h->dmax) h->dmax = datum;
  if(datum < h->dmin) h->dmin = datum;
  h->dsum += datum;
  h->d2sum += datum*datum;
  h->dcount++;
}

void histogram_dump(histogram * h, int num_decimals) { // .CSV format
  int i, d;
  
  if(num_decimals<0) {
    if(h->width >= 10.) d=0;
    else d = ceil(-log10(h->width));
  }
  else d=num_decimals;
  printf("\"n\", %lu\n", h->dcount);
  printf("\"min\", %f\n", h->dmin);
  printf("\"max\", %f\n", h->dmax);
  printf("\"mean\", %Lf\n", h->dsum / (long double)h->dcount);
  printf("\"standard deviation (population)\", %Lf\n", sqrtl(h->d2sum / (long double)h->dcount - powl(h->dsum / (long double)h->dcount,2.)));
  printf("\"less\", %u\n", h->less);
  for(i=0; i<h->bins; i++) printf("\"=\"\"%.*f<x<%.*f\"\"\", %u\n", d, (double)i * h->width + h->min, d, (double)(i+1) * h->width + h->min, h->buckets[i]);
  printf("\"more\", %u\n", h->more);
  free(h->buckets);
  free(h);
}
