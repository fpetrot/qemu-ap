#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

int main(int argc, char *argv[])
{
  for (int k = 0; k < 10000; k++)
    {
      unsigned int i;
      mpfr_t s, t, u;
      mpfr_init2 (t, 200);
      mpfr_set_d (t, 1.0, MPFR_RNDD);
      mpfr_init2 (s, 200);
      mpfr_set_d (s, 1.0, MPFR_RNDD);
      mpfr_init2 (u, 200);
      for (i = 1; i <= atoi(argv[1]); i++)
        {
          mpfr_mul_ui (t, t, i, MPFR_RNDU);
          mpfr_set_d (u, 1.0, MPFR_RNDD);
          mpfr_div (u, u, t, MPFR_RNDD);
          mpfr_add (s, s, u, MPFR_RNDD);
        }

      mpfr_clear (s);
      mpfr_clear (t);
      mpfr_clear (u);
      mpfr_free_cache ();
    }
  return 0;
}