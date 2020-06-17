/*
 * vim:list:ts=8:sw=8:noet:
 */

#define ARBITRARY
#include "apmacros.h"
	.text
	.align	1
	.globl	e_approx
	.type	e_approx, @function

e_approx:

	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Precision
	li	x7, 200
	csrrw	x0, 6, x7
	# Rounding mode
	csrrwi	x0, 2, MPFR_RNDD

	# Met 1.0 dans fa0
	li	t0, 1
	fcvt.d.lu fa0,t0 # 1 => 1.0

#if defined (ARBITRARY)
	fcvt_p_d(p0, fa0, MPFR_RNDD)
	fcvt_p_d(p1, fa0, MPFR_RNDD)
#else
	fcvt.d.lu fa5,t0 # t = 1.0
	fcvt.d.lu fa6,t0 # s = 1.0
#endif

	li	t1, 1		# indice de boucle, i
	li	t2, 10		# borne de la boucle
loop:	
	fcvt.d.lu fa1,t1 # 1 => 1.0

#if defined (ARBITRARY)
	fcvt_p_d(p3, fa1, MPFR_RNDD)

	fmul_p(p0, p0, p3, MPFR_RNDD)

	fcvt_p_d(p2, fa0, MPFR_RNDD)

	fdiv_p(p2, p2, p0, MPFR_RNDD)

	fadd_p(p1, p1, p2, MPFR_RNDD)
 #else
	fmul.d fa5,fa5,fa1 # t *= i
	fcvt.d.lu fa7,t0 # u = 1.0
	fdiv.d fa7,fa7,fa5 # u /= t
	fadd.d fa6, fa6, fa7 # s += u
 #endif

	addi	t1, t1, 1
	ble	t1, t2, loop
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
	ret
