/*
 * vim:list:ts=8:sw=8:noet:
 */
#include "apmacros.h"
	.text
	.align 1
	.globl  newton_raphson
	.type   newton_raphson, @function

newton_raphson:
	# We use the FPU
	li  t0, MSTATUS_FS
	csrs    mstatus, t0

	# Precision
	li   x7, 100
	csrrw x0, 6, x7
	# Rounding mode
	csrrwi x0, 2, MPFR_RNDD

	# Init x with x0 = -20.0
	li t0, -20
	fcvt.d.l fa0, t0 # -20 => -20.0
	fcvt_p_d(p0, fa0, MPFR_RNDD)

	# ################
	# newton_raphson #
	# ################

	# Value mpfr 2
	li t0, 2
	fcvt.d.lu fa0, t0 # 2 => 2.0
	fcvt_p_d(p1, fa0, MPFR_RNDD)

	# Value mpfr 3
	li t0, 3
	fcvt.d.lu fa0, t0 # 3 => 3.0
	fcvt_p_d(p2, fa0, MPFR_RNDD)

	# Init h
	li t0, 1
	fcvt.d.lu fa0, t0 # 1 => 1.0
	fcvt_p_d(p3, fa0, MPFR_RNDD)

	# Init epsilon
	# li t0, 0x1dac9a7b3b7302f # 10e-300
	li t0, 0x37d5c72fb1552d83 # 10e-40
	fmv.d.x   fa0, t0
	fcvt_p_d(p4, fa0, MPFR_RNDD)

	# Init loop
	fsgnjx_p(p3, p3, p3)
	fle_p(t1, p3, p4)
	bnez t1, loopend

loop:
	# xp
	fsgnj_p(p5, p0, p0)

	# xn
	fsgnj_p(p6, p0, p0)

	# Compute func in x : res in p0
	fmul_p(p7, p0, p0, MPFR_RNDD)
	fmul_p(p8, p7, p0, MPFR_RNDD)
	fsub_p(p9, p8, p7, MPFR_RNDD)
	fadd_p(p0, p9, p1, MPFR_RNDD)

	# Compute func' in x : res in p5
	fmul_p(p7, p5, p5, MPFR_RNDD)
	fmul_p(p8, p7, p2, MPFR_RNDD)
	fmul_p(p9, p5, p1, MPFR_RNDD)
	fsub_p(p5, p8, p9, MPFR_RNDD)

	# Compute h
	fdiv_p(p3, p0, p5, MPFR_RNDD)

	# Compute x
	fsub_p(p0, p6, p3, MPFR_RNDD)

	# Condition to jump again to loop
	fsgnjx_p(p3, p3, p3)
	fle_p(t1, p3, p4)
	beqz t1, loop

loopend:
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
	ret
