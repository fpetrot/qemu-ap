/*
 * vim:list:ts=4:sw=4:noet:
 */
#include "apmacros.h"
	.align	1
	.text

func:
	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Using already existing rounding mode instruction
	csrrwi x0, 2, MPFR_RNDD

	# Write the precision as csr
	li   x7, 200
	csrrw x0, 6, x7

    li   x29, -6
	li   x30, -678
	li   x28, 1023
	slli x28, x28, 52 # 0x3ff0000000000000 = 1,0
	li   x6, 0x3FC4000000000000 # 0,15625
	
	# Test conversions int -> mpfr
	fcvt_p_l(x29, MPFR_RNDD, p1)
	fcvt_p_l(x30, MPFR_RNDD, p2)

	li   x29, 8
	li   x30, 4
	fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4

	# Test conversions uint -> mpfr
	fcvt_p_lu(x29, MPFR_RNDD, p1)
	fcvt_p_lu(x30, MPFR_RNDD, p2)

	# Test conversions double -> mpfr
	fcvt_p_d(fa0, MPFR_RNDD, p1) # fcvt_p_d p1, fa0
	fcvt_p_d(fa1, MPFR_RNDD, p2) # fcvt_p_d p2, fa1

	# Test arithmetic operations
	fadd_p(p2, p1, MPFR_RNDD, p3) # fadd_p   p3, p1, p2
	fsub_p(p2, p1, MPFR_RNDD, p3) # fsub_p   p3, p1, p2
	fmul_p(p2, p1, MPFR_RNDD, p3) # fmul_p   p3, p1, p2
	fsqrt_p(p3, MPFR_RNDD, p3) # fsqrt_p  p3, p3
	fdiv_p(p2, p1, MPFR_RNDD, p3) # fdiv_p   p3, p1, p2
	fmadd_p(p3, p2, p1, MPFR_RNDD, p4) # fmadd_p p4, p1, p2, p3
	fmsub_p(p3, p2, p1, MPFR_RNDD, p4) # fmsub_p p4, p1, p2, p3
	fnmadd_p(p3, p2, p1, MPFR_RNDD, p4) # fnmadd_p p4, p1, p2, p3
	fnmsub_p(p3, p2, p1, MPFR_RNDD, p4) # fnmadd_p p4, p1, p2, p3

	# Test comparaisons
	feq_p(p2, p1, x7)  # feq_p   x7, p1, p2
	flt_p(p2, p1, x7)  # flt_p   x7, p1, p2
	fle_p(p2, p1, x7)  # fle_p   x7, vp1, vp2
	fmin_p(p2, p1, p3)  # fmin_p   p3, p1, p2
	fmax_p(p2, p1, p3)  # fmax_p   p3, p1, p2

	# Test signe
	fsgnj_p(p2, p1, p3)  # fsgnj_p p3, p1, p2
	fsgnjn_p(p2, p1, p3)  # fsgnjn_p  p3, p1, p2
	fsgnjx_p(p2, p1, p3)  # fsgnjx_p  p3, p1, p2

	# Test conversion mpfr -> double
	fcvt_d_p(p4, MPFR_RNDD, fa2) # fcvt_d_p fa2, p4

	# Test conversion mpfr -> uint
	fcvt_lu_p(p3, MPFR_RNDD, x30)

	la	t0, room_for_vpr
	.word 0x0032d027 # fsp p3, 0(t0)
	flp(0, t0, p0) # flp p0, 0(t0)

	# Test load after precision modification
	li   x7, 300
	csrrw x0, 6, x7
	flp(0, t0, p1) # flp p1, 0(t0)

	ret

	.data
room_for_vpr: .space 1024
