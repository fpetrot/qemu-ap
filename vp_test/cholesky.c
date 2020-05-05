/* Kernel extracted from the polybench and adapted to bare metal */
#ifdef x86
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#endif
/* Something to start with */
#define SMALL_DATASET

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

# if !defined(N)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define N 40
#  endif 

#  ifdef SMALL_DATASET
#   define N 120
#  endif 

#  ifdef MEDIUM_DATASET
#   define N 400
#  endif 

#  ifdef LARGE_DATASET
#   define N 2000
#  endif 

#  ifdef EXTRALARGE_DATASET
#   define N 4000
#  endif
# endif

#define N_CHOLESKY N

typedef unsigned int size_t;


#ifdef RISCV
/*
 * Q&D hacks to compile for riscv
 */

static inline void *memset(void *dest, int c, size_t n)
{
    char *p = dest;
    while (n-- > 0) {
        *(char*)dest++ = c;
    }
    return p;
}

#define UART_BASE (volatile int *)0x10000000
#define UART_TXFIFO 0

static inline int putchar(int ch)
{
    *(UART_BASE + UART_TXFIFO) = ch & 0xff;
    return ch & 0xff;
}

static inline int putstr(const char *s)
{
    while (*s) putchar(*s++);
    return 0;
}

static inline void *memcpy(void *dest, const void *src, size_t n)
{
    char *p = dest;
    while (n-- > 0) {
        *(char*)dest++ = *(char*)src++;
    }
    return p;
}

static inline double sqrt(double x)
{
   asm ("fsqrt.d %0, %1" : "=f" (x) : "f" (x));
   return x;
}

/* 
 * Thanks to https://stackoverflow.com/questions/16647278/minimal-implementation-of-sprintf-or-printf
 */
int normalize(double *val)
{
   int exponent = 0;
   double value = *val;

   while (value >= 1.0) {
      value /= 10.0;
      ++exponent;
   }

   while (value < 0.1) {
      value *= 10.0;
      --exponent;
   }
   *val = value;
   return exponent;
}

static void ftoa_fixed(char *buffer, double value)
{
   /* carry out a fixed conversion of a double value to a string, with a precision of 5 decimal digits. 
    * Values with absolute values less than 0.000001 are rounded to 0.0
    * Note: this blindly assumes that the buffer will be large enough to hold the largest possible result.
    * The largest value we expect is an IEEE 754 double precision real, with maximum magnitude of approximately
    * e+308. The C standard requires an implementation to allow a single conversion to produce up to 512 
    * characters, so that's what we really expect as the buffer size.     
    */

   int exponent = 0;
   int places = 0;
   static const int width = 4;

   if (value == 0.0) {
      buffer[0] = '0';
      buffer[1] = '\0';
      return;
   }

   if (value < 0.0) {
      *buffer++ = '-';
      value = -value;
   }

   exponent = normalize(&value);

   while (exponent > 0) {
      int digit = value * 10;
      *buffer++ = digit + '0';
      value = value * 10 - digit;
      ++places;
      --exponent;
   }

   if (places == 0)
      *buffer++ = '0';

   *buffer++ = '.';

   while (exponent < 0 && places < width) {
      *buffer++ = '0';
      --exponent;
      ++places;
   }

   while (places < width) {
      int digit = value * 10.0;
      *buffer++ = digit + '0';
      value = value * 10.0 - digit;
      ++places;
   }
   *buffer = '\0';
}

void print_double(double f)
{
   char buffer[64];
   ftoa_fixed(buffer, f);
   putstr(buffer);
}
#endif

static double B[N_CHOLESKY][N_CHOLESKY];

static
void init_array_cholesky_double(int n, double A[n][n])
{
   int i, j;

   for (i = 0; i < n; i++) {
      for (j = 0; j <= i; j++)
         A[i][j] = (double) (-j % n) / n + 1;
      for (j = i + 1; j < n; j++) {
         A[i][j] = 0;
      }
      A[i][i] = 1;
   }

   int r, s, t;
   for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
         B[r][s] = 0;

   for (t = 0; t < n; ++t)
      for (r = 0; r < n; ++r)
         for (s = 0; s < n; ++s)
            B[r][s] += A[r][t] * A[s][t];

   for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
         A[r][s] = B[r][s];
}

#if DUMP_ARRAY
static
void print_array_cholesky_double(int n, double A[n][n])
{
   int i, j;

   putstr("==BEGIN DUMP_ARRAYS==\n");
   putstr("begin dump: A\n");
   for (i = 0; i < n; i++)
      for (j = 0; j <= i; j++) {
         if ((i * n + j) % 20 == 0)
            putstr("\n");
         print_double(A[i][j]); putstr(" ");
      }
   putstr("\nend   dump: A\n");
   putstr("==END   DUMP_ARRAYS==\n");
}
#endif

static
void kernel_cholesky_double(int n, double A[n][n])
{
   int i, j, k;

   for (i = 0; i < n; i++) {
      for (j = 0; j < i; j++) {
         for (k = 0; k < j; k++) {
            A[i][j] -= A[i][k] * A[j][k];
         }
         A[i][j] /= A[j][j];
      }

      for (k = 0; k < i; k++) {
         A[i][i] -= A[i][k] * A[i][k];
      }
      A[i][i] = sqrt(A[i][i]);
   }
}

int cholesky_double(int n, double A[n][n])
{

   init_array_cholesky_double(n, A);
   kernel_cholesky_double(n, A);
#if DUMP_ARRAY
   print_array_cholesky_double(n, A);
#endif

   return 0;
}

static double A_double[N_CHOLESKY][N_CHOLESKY];
void cholesky(void)
{
   putstr("Start Cholesky\n");
   cholesky_double(N_CHOLESKY, A_double);

#if 0
   diff_double_unum_2d((double *) A_double, (vpfloat < mpfr | 16 | 101 > *)A_unum, N_CHOLESKY, N_CHOLESKY);
#endif

   print_array_cholesky_double(N_CHOLESKY, A_double);
   putstr("End Cholesky\n");
}

int main(void)
{
   cholesky();
   return 0;
}
