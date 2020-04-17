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

    # Test precision et rounding mode
    li   x28, 200
    .word 0x320e200b # sp x28 : store the precision into the status register precision
    li  x5, MPFR_RNDD
    .word 0x3602a00b # srnd x5 : store the rounding mode in the rounding mode register

    li   x29, 8
	li   x30, 4
    fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4
    .word 0x2e05008b # fcvt_b_dfpr ux1, fa0
	.word 0x2e05810b # fcvt_b_dfpr ux2, fa1

    .word 0x0c004e8b # lp x29 : load the precision from the status register precision in the x29 register
    .word 0x1000430b # lrnd x6 : load the rounding mode in x28
    j again
