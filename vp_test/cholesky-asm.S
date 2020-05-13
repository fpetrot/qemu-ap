/*
 * vim: nospell: tw=0: ai: sw=12: ts=12: sts=12: noet: nolist
 *
 * Naïve implementation of Cholesky algorithm in riscv asm.
 * No fancy asm optimization, the idea is to be able to support
 * several types without pain, and be able to compare amongst these
 * types, for demonstration purposes only.
 */

#define MINI_DATASET

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

/* For double at first */
#define DTSZ 8

	/*
	 * Global arrays, makes things simpler
	 * static type A[N][N];
	 * static type B[N][N];
	*/
	.bss
	.align	3
	.type	A, @object
	.size	A, N * N * DTSZ
A:	.zero	N * N * DTSZ

	.type	B, @object
	.size	B, N * N * DTSZ
B:	.zero	N * N * DTSZ

	.text
	.globl	main
	.align	2
	.type	main, @function
main:	li	a0, N
	la	a1, A
	jal	init_array_cholesky_double
	jal	print_array_cholesky_double
	ret

init_array_cholesky_double:
/*
static void init_array_cholesky_type(int n, type a[n][n])
{
   int i, j;

   for (i = 0; i < n; i++) {
      for (j = 0; j < i; j++)
         a[i][j] = (type) (-j % n) / n + 1;
      a[i][i] = 1;
      for (j = i + 1; j < n; j++)
         a[i][j] = 0;
   }

   int r, s, t;
   for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
         B[r][s] = 0;

   for (t = 0; t < n; ++t)
      for (r = 0; r < n; ++r)
         for (s = 0; s < n; ++s)
            B[r][s] += a[r][t] * a[s][t];

   for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
         a[r][s] = B[r][s];
}
*/

	.align	2
	.type	init_array_cholesky_double, @function
init_array_cholesky_double:

	# Initialization of a few reused values
	li	a3, DTSZ	# compute n * DTSZ before the loops
	mul	a2, a3, a0	# suboptimal if DTSZ = 2^p
	fcvt.d.w	fa0, a0	# n in whatever type is required
	fcvt.d.w    ft0, zero	#
	li	t1, 1	# 1 in whatever type is required
	fcvt.d.w	ft1, t1	# 1 => 1.0

	# First loopnest
	li	t0, 0	# i = 0
	li	t2, 0	# column offset
ni:	bge	t0, a0, 1f	# break if i >= n

	li	t1, 0	# j = 0
	li	t3, 0	# row offset
nj1:	bge	t1, t0, 2f	# break if j >= i
	negw	t5, t1
	remw	t5, t5, a0
	fcvt.d.w	ft2,t5
	fdiv.d	ft2,ft2,fa0
	fadd.d	ft2,ft2,ft1
	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft2, 0(t4)	# a[i][j]
	add	t3, t3, a3	# update row offset
	addi	t1, t1, 1	# j++
	j	nj1	# back to loop j1 check

2:	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft1, 0(t4)	# a[i][j] = 1 when i == j
	add	t3, t3, a3	# update row offset
	addi	t1, t1, 1	# j++

nj2:	bge	t1, a0, 3f	# break if j >= n
	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft0, 0(t4)	# a[i][j] = 0
	add	t3, t3, a3	# update row offset
	addi	t1, t1, 1
	j	nj2	# back to loop j2 check

3:	add	t2, t2, a2	# update column offset
	addi	t0, t0, 1
	j	ni	# back to loop i check

1:	# Second loopnest
	la	a7, B	# dirty global, not really stack allocatable
	li	t0, 0	# r = 0
	li	t2, 0	# column offset
nr1:	bge	t0, a0, 1f	# break if r >= n

	li	t1, 0	# s = 0
	li	t3, 0	# row offset
ns1:	bge	t1, a0, 3f	# break if s >= n
	add	t4, t2, t3
	add	t4, a7, t4
	fsd	ft0, 0(t4)	# b[r][s] = 0
	add	t3, t3, a3	# update row offset
	addi	t1, t1, 1
	j	ns1	# back to loop s check

3:	add	t2, t2, a2	# update column offset
	addi	t0, t0, 1
	j	nr1	# back to loop r check

1:	# Third loopnest
	# r is always a column, t always a row and s, both
	li	t0, 0	# t = 0
	li	t1, 0	# row offset
nt:	bge	t0, a0, 1f	# break if t >= n

	li	t2, 0	# r = 0
	li	t3, 0	# column offset
nr2:	bge	t2, a0, 2f	# break if r >= n
	
	add	a4, t3, t1	#
	add	a4, a1, a4	# a4 is now &a[r][t]
	fld	ft0, 0(a4)	# ft0 is a[r][t]

	li	t4, 0	# s = 0
	li	t5, 0	# row offset
	li	t6, 0	# column offset
ns2:	bge	t4, a0, 3f	# break if s >= n

	add	a4, t3, t5	# a4 is a free temporary
	add	a4, a7, a4	# row address
	fld	ft1, 0(a4)	# ft1 = b[r][s]

	add	a5, t6, t1	#
	add	a5, a1, a5	# a5 is now &a[s][t]
	fld	ft2, 0(a5)	# ft0 is a[s][t]

	fmul.d	ft2, ft2, ft0
	fadd.d	ft2, ft1, ft2

	fsd	ft2, 0(a4)

	add	t5, t5, a3	# update row offset
	add	t6, t6, a2	# update column offset
	addi	t4, t4, 1
	j	ns2	# back to loop s check

3:	add	t3, t3, a2	# update column address
	addi	t2, t2, 1
	j	nr2	# back to loop r check

2:	add	t1, t1, a3	# update row address
	addi	t0, t0, 1
	j	nt	# back to loop t check

1:	# Fourth loopnest
	li	t0, 0	# r = 0
	li	t2, 0	# column offset
nr3:	bge	t0, a0, 1f	# break if r >= n

	li	t1, 0	# s = 0
	li	t3, 0	# row offset
ns3:	bge	t1, a0, 3f	# break if s >= n
	add	t4, t2, t3
	add	t5, a7, t4
	fld	ft0, 0(t5)	# ft0 = b[r][s]
	add	t6, a1, t4
	fsd	ft0, 0(t6)	# a[r][s] = ft0
	add	t3, t3, a3	# update row offset
	addi	t1, t1, 1
	j	ns3	# back to loop s check

3:	add	t2, t2, a2	# update column offset
	addi	t0, t0, 1
	j	nr3	# back to loop r check
1:
	ret
/*
static void kernel_cholesky_type(int n, type a[n][n])
{
   int i, j, k;

   for (i = 0; i < n; i++) {
      for (j = 0; j < i; j++) {
         for (k = 0; k < j; k++) {
            a[i][j] -= a[i][k] * a[j][k];
         }
         a[i][j] /= a[j][j];
      }

      for (k = 0; k < i; k++) {
         a[i][i] -= a[i][k] * a[i][k];
      }
      a[i][i] = sqrt(a[i][i]);
   }
}
*/
