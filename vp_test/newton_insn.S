/*
 * vim:list:ts=4:sw=4:noet:
 */
#include "apmacros.h"
    .text
    .align 1
    .globl  newton_raphson
	.type	newton_raphson, @function

newton_raphson:
    # Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

    # Precision
    li   x7, 100
	csrrw x0, 6, x7
	# Rounding mode
	csrrwi x0, 2, MPFR_RNDD

    # Init x with x0 = -20.0
    li t0, -20
    fcvt.d.l fa0, t0 # -20 => -20.0
    fcvt_p_d(fa0, MPFR_RNDD, p0)

    # ################
    # newton_raphson #
    # ################

    # Value mpfr 2
    li t0, 2
    fcvt.d.lu fa0, t0 # 2 => 2.0
    fcvt_p_d(fa0, MPFR_RNDD, p1)

    # Value mpfr 3
    li t0, 3
    fcvt.d.lu fa0, t0 # 3 => 3.0
    fcvt_p_d(fa0, MPFR_RNDD, p2)

    # Init h
    li t0, 1
    fcvt.d.lu fa0, t0 # 1 => 1.0
    fcvt_p_d(fa0, MPFR_RNDD, p3)

    # Init epsilon
    # li t0, 0x1dac9a7b3b7302f # 10e-300
    li t0, 0x37d5c72fb1552d83 # 10e-40
    fmv.d.x   fa0, t0
    fcvt_p_d(fa0, MPFR_RNDD, p4)

    # Init loop
    fsgnjx_p(p3, p3, p3)
    fle_p(p4, p3, t1)
    bnez t1, loopend

loop:
    # xp
    fsgnj_p(p0, p0, p5)

    # xn
    fsgnj_p(p0, p0, p6)

    # Compute func in x : res in p0
    fmul_p(p0, p0, MPFR_RNDD, p7)
    fmul_p(p0, p7, MPFR_RNDD, p8)
    fsub_p(p7, p8, MPFR_RNDD, p9)
    fadd_p(p1, p9, MPFR_RNDD, p0)

    # Compute func' in x : res in p5
    fmul_p(p5, p5, MPFR_RNDD, p7)
    fmul_p(p2, p7, MPFR_RNDD, p8)
    fmul_p(p1, p5, MPFR_RNDD, p9)
    fsub_p(p9, p8, MPFR_RNDD, p5)

    # Compute h
    fdiv_p(p5, p0, MPFR_RNDD, p3)

    # Compute x
    fsub_p(p3, p6, MPFR_RNDD, p0)

    # Condition to jump again to loop
    fsgnjx_p(p3, p3, p3)
    fle_p(p4, p3, t1)
    beqz t1, loop

loopend:
    la	t0, room_for_apr
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
	ret

	.data
room_for_apr: .space 1024