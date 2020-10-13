/* 
   Expected output values
*/

        #include "apmacros.h"
	.text
	.type	fcvt_p_l, @function
fcvt_p_l:
	# Make FPU usable
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Define precision
	li	t1, 200
	csrrw	zero, 6, t1
	# and rounding mode
	csrrwi	zero, 2, MPFR_RNDD

	li	t0, 0
	li	t1, +10000
.L2:
	fcvt_p_lu(p0, t0, MPFR_RNDD);
	addi	t0,t0,1
	bge 	t1,t0,.L2
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
1:	j 1b
