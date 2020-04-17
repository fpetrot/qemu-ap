/*
 * vim:list:ts=4:sw=4:noet:
 */
    .equiv  MSTATUS_FS, 0x00006000
	.text
again:
    # Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0
    
    li   x28, 200
    .word 0x320e200b # sp x28 : store the precision into the status register precision
    li   x29, 8
	li   x30, 4
    fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4
    .word 0x2e05008b # fcvt_b_dfpr ux1, fa0
	.word 0x2e05810b # fcvt_b_dfpr ux2, fa1
    .word 0x0c004e8b # lp x29 : load the precision from the status register precision in the x29 register
    j again
