#include <stdint.h>
typedef union {
   uint64_t i;
   double d;
} id;

double a(uint64_t i)
{
   return (double)i;
}

uint64_t b(double d)
{
   return (uint64_t)d;
}

double f(uint64_t i)
{
   id a;
   a.i = i;
   return a.d;
}

uint64_t f(double d)
{
   id a;
   a.d = d;
   return a.i;
}

