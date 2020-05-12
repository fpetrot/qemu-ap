/*
 * vim:list:ts=4:sw=4:noet:
 */
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
    li   x7, 200
	csrrw x0, 6, x7
	# Rounding mode
	csrrwi x0, 2, MPFR_RNDD

	# Met 1.0 dans fa0
	li	t0, 1
    fcvt.d.lu fa0,t0 # 1 => 1.0

	fcvt_p_d(fa0, MPFR_RNDD, p0)
	fcvt_p_d(fa0, MPFR_RNDD, p1)

	li	t1, 1		# indice de boucle, i
	li	t2, 100		# borne de la boucle
loop:	
	fcvt.d.lu fa1,t1 # 1 => 1.0

	fcvt_p_d(fa1, MPFR_RNDD, p3)

	fmul_p(p3, p0, MPFR_RNDD, p0)

	fcvt_p_d(fa0, MPFR_RNDD, p2)

	fdiv_p(p0, p2, MPFR_RNDD, p2)

	fadd_p(p2, p1, MPFR_RNDD, p1)

	addi	t1, t1, 1
	ble	t1, t2, loop
	la	t0, room_for_vpr
	ret

	.data
room_for_vpr: .space 1024
