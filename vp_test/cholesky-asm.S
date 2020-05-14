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
	li	a0, N
	la	a1, A
	jal	kernel_cholesky_double
	li	a0, N
	la	a1, A
	jal	print_array_cholesky_double
	li	t0, 0x100000
	li	t1, 0x5555
	sd	t1, 0(t0)
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
	li	t2, 0	# row offset
ni:	bge	t0, a0, 1f	# break if i >= n

	li	t1, 0	# j = 0
	li	t3, 0	# column offset
nj1:	bge	t1, t0, 2f	# break if j >= i
	negw	t5, t1
	remw	t5, t5, a0
	fcvt.d.w	ft2,t5
	fdiv.d	ft2,ft2,fa0
	fadd.d	ft2,ft2,ft1
	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft2, 0(t4)	# a[i][j]
	add	t3, t3, a3	# update column offset
	addi	t1, t1, 1	# j++
	j	nj1	# back to loop j1 check

2:	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft1, 0(t4)	# a[i][j] = 1 when i == j
	add	t3, t3, a3	# update column offset
	addi	t1, t1, 1	# j++

nj2:	bge	t1, a0, 3f	# break if j >= n
	add	t4, t2, t3
	add	t4, a1, t4
	fsd	ft0, 0(t4)	# a[i][j] = 0
	add	t3, t3, a3	# update column offset
	addi	t1, t1, 1
	j	nj2	# back to loop j2 check

3:	add	t2, t2, a2	# update row offset
	addi	t0, t0, 1
	j	ni	# back to loop i check

1:	# Second loopnest
	la	a7, B	# dirty global, not really stack allocatable
	li	t0, 0	# r = 0
	li	t2, 0	# row offset
nr1:	bge	t0, a0, 1f	# break if r >= n

	li	t1, 0	# s = 0
	li	t3, 0	# column offset
ns1:	bge	t1, a0, 3f	# break if s >= n
	add	t4, t2, t3
	add	t4, a7, t4
	fsd	ft0, 0(t4)	# b[r][s] = 0
	add	t3, t3, a3	# update column offset
	addi	t1, t1, 1
	j	ns1	# back to loop s check

3:	add	t2, t2, a2	# update row offset
	addi	t0, t0, 1
	j	nr1	# back to loop r check

1:	# Third loopnest
	# r is always a row, t always a column and s, both
	li	t0, 0	# t = 0
	li	t1, 0	# column offset
nt:	bge	t0, a0, 1f	# break if t >= n

	li	t2, 0	# r = 0
	li	t3, 0	# row offset
nr2:	bge	t2, a0, 2f	# break if r >= n
	
	add	a4, t3, t1	#
	add	a4, a1, a4	# a4 is now &a[r][t]
	fld	ft0, 0(a4)	# ft0 is a[r][t]

	li	t4, 0	# s = 0
	li	t5, 0	# column offset
	li	t6, 0	# row offset
ns2:	bge	t4, a0, 3f	# break if s >= n

	add	a4, t3, t5	# a4 is a free temporary
	add	a4, a7, a4	# column address
	fld	ft1, 0(a4)	# ft1 = b[r][s]

	add	a5, t6, t1	#
	add	a5, a1, a5	# a5 is now &a[s][t]
	fld	ft2, 0(a5)	# ft0 is a[s][t]

	fmul.d	ft2, ft2, ft0
	fadd.d	ft2, ft1, ft2

	fsd	ft2, 0(a4)

	add	t5, t5, a3	# update column offset
	add	t6, t6, a2	# update row offset
	addi	t4, t4, 1
	j	ns2	# back to loop s check

3:	add	t3, t3, a2	# update row address
	addi	t2, t2, 1
	j	nr2	# back to loop r check

2:	add	t1, t1, a3	# update column address
	addi	t0, t0, 1
	j	nt	# back to loop t check

1:	# Fourth loopnest
	li	t0, 0	# r = 0
	li	t2, 0	# row offset
nr3:	bge	t0, a0, 1f	# break if r >= n

	li	t1, 0	# s = 0
	li	t3, 0	# column offset
ns3:	bge	t1, a0, 3f	# break if s >= n
	add	t4, t2, t3
	add	t5, a7, t4
	fld	ft0, 0(t5)	# ft0 = b[r][s]
	add	t6, a1, t4
	fsd	ft0, 0(t6)	# a[r][s] = ft0
	add	t3, t3, a3	# update column offset
	addi	t1, t1, 1
	j	ns3	# back to loop s check

3:	add	t2, t2, a2	# update row offset
	addi	t0, t0, 1
	j	nr3	# back to loop r check
1:
	ret
/*
static void kernel_cholesky_double(int n, double a[n][n])
{
   int i, j, k;
   double x;

   for (i = 0; i < n; i++) {
      for (j = 0; j < i; j++) {
         x = a[i][j];
         for (k = 0; k < j; k++)
            x -= a[i][k] * a[j][k];
         x /= a[j][j];
         a[i][j] = x;
      }
      x = a[i][i];
      for (k = 0; k < i; k++)
         x -= a[i][k] * a[i][k];
      a[i][i] = sqrt(x);
   }
}
*/
	.align	2
	.type	kernel_cholesky_double, @function
kernel_cholesky_double:
	# Initialization of the offset displacement variables
	li	a3, DTSZ	# compute n * DTSZ before the loops
	mul	a2, a3, a0	# suboptimal if DTSZ = 2^p

	# i and j might be both column and row, k is always a column
	li	t0, 0	# i = 0
	li	t1, 0	# i column offset
	li	t2, 0	# i row offset
kni:	bge	t0, a0, 1f	# break if i >= n

	li	t3, 0	# j = 0
	li	t4, 0	# j column offset
	li	t5, 0	# j row offset
knj:	bge	t3, t0, 2f	# break if j >= i
	
	add	a4, t2, t4	#
	add	a4, a1, a4	# a4 is now &a[i][j]
	fld	ft0, 0(a4)	# ft0 = a[i][j]

	li	t6, 0	# k = 0
	li	a6, 0	# k column offset
knk1:	bge	t6, t3, 3f	# break if k >= j

	add	a5, t2, a6	# [i][k]
	add	a5, a1, a5	# &a[i][k]
	fld	ft1, 0(a5)	# ft1 = a[i][k]

	add	a5, t5, a6	# [j][k]
	add	a5, a1, a5	# &a[j][k]
	fld	ft2, 0(a5)	# ft2 = a[j][k]

	fmul.d	ft2, ft2, ft1
	fsub.d	ft0, ft0, ft2

	add	a6, a6, a3	# update column offset
	addi	t6, t6, 1	# k++
	j	knk1	# back to loop k1 check

3:	add	a5, t5, t4	# [j][j]
	add	a5, a1, a5	# &a[j][j]
	fld	ft1, 0(a5)	# ft1 = a[j][j]
	fdiv.d	ft0, ft0, ft1
	fsd	ft0, 0(a4)	# a[i][j] = ft0

	add	t4, t4, a3	# update j column offset
	add	t5, t5, a2	# update j row offset
	addi	t3, t3, 1	# j++
	j	knj	# back to loop j check

2:	add	a4, t2, t1	#
	add	a4, a1, a4	# a4 is now &a[i][i]
	fld	ft0, 0(a4)	# ft0 = a[i][i]

	li	t6, 0	# k = 0
	li	a6, 0	# k column offset
knk2:	bge	t6, t0, 4f	# break if k >= i

	add	a5, t2, a6	# [i][k]
	add	a5, a1, a5	# &a[i][k]
	fld	ft1, 0(a5)	# ft1 = a[i][k]

	fmul.d	ft1, ft1, ft1
	fsub.d	ft0, ft0, ft1

	add	a6, a6, a3	# update k column offset
	addi	t6, t6, 1	# k++
	j	knk2	# back to loop k2 check

4:	fsqrt.d	ft0, ft0
	fsd	ft0, 0(a4)	# a[i][i] = ft0

	add	t1, t1, a3	# update i column offset
	add	t2, t2, a2	# update i row offset
	addi	t0, t0, 1	# i++
	j	kni	# back to loop i check
1:	ret
