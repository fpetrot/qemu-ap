/*
 * vim:list:ts=4:sw=4:noet:
 */
	.equiv  MSTATUS_FS, 0x00006000
	.text
again:
	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

    li   x29, 8
	li   x30, 4
	li   x28, 1023
	slli x28, x28, 52 # 0x3ff0000000000000 = 1,0
	li   x6, 0x3FC4000000000000 # 0,15625
	
	fcvt.d.lu fa0,x29 # 8.0 <- 8
	fcvt.d.lu fa1,x30 # 4.0 <- 4
	.word 0x2e05008b # fcvt_b_dfpr ux1, fa0
	.word 0x2e05810b # fcvt_b_dfpr ux2, fa1

	.word 0xc20818b  # gadd   ux3, ux1, ux2
	.word 0xe20818b  # gsub   ux3, ux1, ux2
	.word 0x1620818b # gmul   ux3, ux1, ux2
	.word 0x1e20818b # gdiv   ux3, ux1, ux2
	.word 0x2d1cf8b  # movg2x x31, ux3,  13
	.word 0x240e220b # fcvt_b_d ux4, x28
	.word 0x2403220b # fcvt_b_d ux4, x6

	li	t1, 1
	fcvt.d.lu fa0,t1 # 1.0 <- 1
	.word 0x2e05000b # fcvt_b_dfpr ux0, fa0
    j again
