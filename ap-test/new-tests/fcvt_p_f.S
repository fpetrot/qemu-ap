/* 
   Expected output values
*/

        #include "apmacros.h"
	.text
	.globl	x
	.type	fcvt_p_f, @function
fcvt_p_f:
	# Make FPU usable
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Define precision
	li	x7, 200
	csrrw	x0, 6, x7
	# and rounding mode
	csrrwi	x0, 2, MPFR_RNDD
	la	s0, x
	addi	s1,s0,800
.L2:
	fld	fa0,0(s0)
	fcvt_p_f(p0, fa0, MPFR_RNDD);
        fcvt_f_p(fa1, p0, MPFR_RNDD);
	addi	s0,s0,8
	bne	s0,s1,.L2
	li t3, 0x100000
	li t4, 0x5555
	sw t4, 0(t3)
1:	j 1b

	.data
	.align	3
	.size	x, 800
x:
        .float 1.397484e-01
        .float -3.193287e-01
        .float -2.994688e+00
        .float 5.748829e+00
        .float -5.981595e+00
        .float -1.346077e+01
        .float -1.497752e+01
        .float -1.692840e+01
        .float -9.627977e+01
        .float 4.266939e+02
        .float -5.101814e+02
        .float 1.306535e+03
        .float -4.032405e+03
        .float 3.423820e+03
        .float 8.670626e+03
        .float -1.570840e+03
        .float -2.124804e+04
        .float 5.000117e+04
        .float 2.873949e+04
        .float 2.659413e+05
        .float 2.784330e+05
        .float -1.012867e+06
        .float 2.629060e+06
        .float -4.177296e+06
        .float -1.049184e+07
        .float 9.303649e+06
        .float 3.634408e+07
        .float -2.491225e+06
        .float -2.611936e+07
        .float -2.951221e+08
        .float 1.024165e+09
        .float -8.113856e+08
        .float -1.023351e+08
        .float -1.515404e+09
        .float 1.575981e+10
        .float 6.314957e+09
        .float 1.210991e+10
        .float 1.098257e+11
        .float -4.400754e+10
        .float -5.293954e+10
        .float -2.057515e+11
        .float 1.325835e+12
        .float 3.644355e+12
        .float 7.270951e+12
        .float 2.674610e+12
        .float 1.791106e+13
        .float 2.324076e+13
        .float -6.907849e+13
        .float 2.665636e+14
        .float 2.115929e+14
        .float -1.643587e+14
        .float 7.135304e+14
        .float -1.152899e+15
        .float -4.698792e+15
        .float -1.582078e+16
        .float -1.558272e+16
        .float -1.553510e+16
        .float 1.212321e+16
        .float -1.963937e+17
        .float 4.993139e+17
        .float -6.279189e+17
        .float 1.860147e+18
        .float 1.518257e+18
        .float 7.046217e+17
        .float 9.644067e+18
        .float -2.842533e+19
        .float -6.733911e+19
        .float -5.555166e+19
        .float 2.807895e+20
        .float 5.745900e+19
        .float 1.301411e+20
        .float 1.800968e+20
        .float 4.348632e+21
        .float 6.178907e+21
        .float -2.713367e+21
        .float -1.550998e+22
        .float -3.339716e+22
        .float -8.853618e+22
        .float -9.170588e+22
        .float -2.061931e+23
        .float 4.121697e+23
        .float 1.061952e+24
        .float -4.731626e+24
        .float -1.125917e+24
        .float 1.353300e+25
        .float 1.843852e+25
        .float 4.437805e+25
        .float 9.865397e+25
        .float -1.074114e+25
        .float 7.902421e+25
        .float -1.018501e+27
        .float 6.502871e+26
        .float -2.734842e+27
        .float 2.393004e+27
        .float 1.439226e+28
        .float -3.261840e+28
        .float -7.880823e+28
        .float 1.462368e+29
        .float -1.579951e+29
        .float -1.504637e+29
	.ident	"GCC: (Debian 9.3.0-13) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
