/*
 * vim:list:ts=4:sw=4:noet:
 */
#include "apmacros.h"
	.align	1
	.text

func:
	# Indicates that we want to use the FPU
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
	
	# Conversions int -> mpfr
	fcvt_p_l(p1, x29, MPFR_RNDD)
	fcvt_p_l(p2, x30, MPFR_RNDD)

	li   x29, 8
	li   x30, 4
	fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4

	# Conversions uint -> mpfr
	fcvt_p_lu(p1, x29, MPFR_RNDD)
	fcvt_p_lu(p2, x30, MPFR_RNDD)

	# Conversions double -> mpfr
	fcvt_p_d(p1, fa0, MPFR_RNDD) # fcvt_p_d p1, fa0
	fcvt_p_d(p2, fa1, MPFR_RNDD) # fcvt_p_d p2, fa1

	# Arithmetic operations
	fadd_p(p3, p1, p2, MPFR_RNDD) # fadd_p   p3, p1, p2
	fsub_p(p3, p1, p2, MPFR_RNDD) # fsub_p   p3, p1, p2
	fmul_p(p3, p1, p2, MPFR_RNDD) # fmul_p   p3, p1, p2
	fsqrt_p(p3, p3, MPFR_RNDD) # fsqrt_p  p3, p3
	fdiv_p(p3, p1, p2, MPFR_RNDD) # fdiv_p   p3, p1, p2
	fmadd_p(p4, p1, p2, p3, MPFR_RNDD) # fmadd_p p4, p1, p2, p3
	fmsub_p(p4, p1, p2, p3, MPFR_RNDD) # fmsub_p p4, p1, p2, p3
	fnmadd_p(p4, p1, p2, p3, MPFR_RNDD) # fnmadd_p p4, p1, p2, p3
	fnmsub_p(p4, p1, p2, p3, MPFR_RNDD) # fnmadd_p p4, p1, p2, p3

	# Comparisons
	feq_p(x7, p1, p2)  # feq_p   x7, p1, p2
	flt_p(x7, p1, p2)  # flt_p   x7, p1, p2
	fle_p(x7, p1, p2)  # fle_p   x7, p1, p2
	fmin_p(p3, p1, p2)  # fmin_p   p3, p1, p2
	fmax_p(p3, p1, p2)  # fmax_p   p3, p1, p2

	# Sign test
	fsgnj_p(p3, p1, p2)  # fsgnj_p p3, p1, p2
	fsgnjn_p(p3, p1, p2)  # fsgnjn_p  p3, p1, p2
	fsgnjx_p(p3, p1, p2)  # fsgnjx_p  p3, p1, p2

	# Conversion mpfr -> double
	fcvt_d_p(fa2, p4, MPFR_RNDD) # fcvt_d_p fa2, p4

	# Conversion mpfr -> uint
	fcvt_lu_p(x30, p3, MPFR_RNDD)

	la	t0, room_for_apr
	fsp(p3, 0, t0)
	flp(p0, 0, t0) # flp p0, 0(t0)

	# Load after precision modification
	li   x7, 300
	csrrw x0, 6, x7
	flp(p1, 0, t0) # flp p1, 0(t0)

	ret

	.data
room_for_apr: .space 1024
