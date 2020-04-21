/*
 * vim:list:ts=4:sw=4:noet:
 */
	.equiv  MPFR_RNDN, 0
    .equiv  MPFR_RNDZ, 1
    .equiv  MPFR_RNDU, 2
    .equiv  MPFR_RNDD, 3
    .equiv  MPFR_RNDA, 4
    .equiv  MPFR_RNDF, 5
    .equiv  MPFR_RNDNA, -1
	.equiv  MSTATUS_FS, 0x00006000
	.text
again:
	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Precision et rounding mode
    li   x7, 200
    .word 0x3203a00b # sp x7 : store the precision into the status register precision
    li  x5, MPFR_RNDD
    .word 0x3602a00b # srnd x5 : store the rounding mode in the rounding mode register

    li   x29, 8
	li   x30, 4
	li   x28, 1023
	slli x28, x28, 52 # 0x3ff0000000000000 = 1,0
	li   x6, 0x3FC4000000000000 # 0,15625
	
	fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4

	# Test conversions fpr -> mpfr
	.word 0x2e05008b # fcvt_b_dfpr ux1, fa0
	.word 0x2e05810b # fcvt_b_dfpr ux2, fa1

	# Test arithmetic operations
	.word 0xc20818b  # gadd   ux3, ux1, ux2
	.word 0xe20818b  # gsub   ux3, ux1, ux2
	.word 0x1620818b # gmul   ux3, ux1, ux2
	.word 0x1e20818b # gdiv   ux3, ux1, ux2

	# Test conversions gpr -> mpfr
	.word 0x240e220b # fcvt_b_d ux4, x28
	.word 0x2403220b # fcvt_b_d ux4, x6

	# Test conversion mpfr -> gpr
	.word 0x26024f8b # fcvt_d_b x31, ux4

	# Test conversion mpfr -> fpr
	.word 0x3002060b # fcvt_dfpr_b fa2, ux4
    j again
