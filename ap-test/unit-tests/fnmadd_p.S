        #include "apmacros.h"
	.text
	.type	fnmadd_p, @function
fnmadd_p:
	# Make FPU usable
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Define precision
	li	x7, 200
	csrrw	x0, 6, x7
	# and rounding mode
	csrrwi	x0, 2, MPFR_RNDD

	la	s0, x
	fld	fa0,0(s0)
	fld	fa1,8(s0)
	fld	fa2,16(s0)
	fcvt_p_d(p0, fa0, MPFR_RNDD);
	fcvt_p_d(p1, fa1, MPFR_RNDD);
	fcvt_p_d(p2, fa2, MPFR_RNDD);
	fnmadd_p (p3, p0, p1, p2, MPFR_RNDD);
	fcvt_d_p(fa2, p3, MPFR_RNDD);
end:
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
1:	j 1b

	.globl	x
	.data
	.align	3
x:      .double	3.14159265358979323844
y:      .double	2.71828182845904523536
z:      .double 1.41421356237309504880
