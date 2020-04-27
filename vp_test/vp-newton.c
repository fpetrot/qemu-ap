#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>

void pp(mpfr_t x)
{
   mpfr_out_str (stdout, 10, 0, x, MPFR_RNDD);
}
void p(mpfr_t x)
{
   pp(x); putchar ('\n');
}

/*
 * Let us use global variables, ...
 * Eases using mpfr, and our processor registers are
 * globals anyway
 */

static  mpfr_t two, three, a, b, c, d, e;

/*
 * func is the polynomial whose root we search.
 * x is both in and out, unless I'm misinformed
 * f(x) = x^3 - x^2  + 2
 */

void f(mpfr_t x)
{
   mpfr_mul(a, x, x, MPFR_RNDD);
   mpfr_mul(b, a, x, MPFR_RNDD);
   mpfr_sub(c, b, a, MPFR_RNDD);
   mpfr_add(x, c, two, MPFR_RNDD);
}

/* f'(x) = 3*x^x - 2*x */
void fp(mpfr_t x)
{
   mpfr_mul(a, x, x, MPFR_RNDD);
   mpfr_mul(b, a, three, MPFR_RNDD);
   mpfr_mul(c, x, two, MPFR_RNDD);
   mpfr_sub(x, b, c, MPFR_RNDD);
}

// Function to find the root 
void newton_raphson(mpfr_t x, unsigned int prec)
{
   /* Creating and initializing our globals */
   mpfr_init2(two,    prec);
   mpfr_set_d(two,    2.0, MPFR_RNDD);
   mpfr_init2(three,  prec);
   mpfr_set_d(three,  3.0, MPFR_RNDD);
   mpfr_init2(a, prec);
   mpfr_init2(b,   prec);
   mpfr_init2(c,   prec);

   /* Locals for the control of the algorithm */
   mpfr_t h, xn, xp, epsilon;
   mpfr_init2(h,  prec);
   mpfr_set_d(h,  1.0, MPFR_RNDD);
   mpfr_init2(xn, prec);
   mpfr_init2(xp, prec);
   mpfr_init2(epsilon, prec);
   mpfr_set_d(epsilon, 10e-300, MPFR_RNDD);


   /* We have to compute iteratively 
    * x(i+1) = x(i) - f(x) / f'(x) */
   int i = 0;
   while (1) {
      mpfr_abs(h, h, MPFR_RNDD);
      if (mpfr_less_p(h, epsilon))
         break;  /* we're good */
      mpfr_set(xp, x, MPFR_RNDD); /* keep current x value */
      mpfr_set(xn, x, MPFR_RNDD); /* keep current x value */
      f(x);      /* compute func in x */
      fp(xp);    /* compute func' in x */
      mpfr_div(h, x, xp, MPFR_RNDD);
      printf("i:%5d ", ++i);
      p(x);
      printf("i:%5d ", i);
      p(xp);
      printf("i:%5d ", i);
      p(h);
      mpfr_sub(x, xn, h, MPFR_RNDD);
   }

   printf("The value of the root is : ");
   p(x);
}

int main(void)
{
   const int precision = 100;
   mpfr_t x0;
   mpfr_init2(x0, precision);
   mpfr_set_d(x0, -20.0, MPFR_RNDD);

   newton_raphson(x0, precision);

   return 0;
}
