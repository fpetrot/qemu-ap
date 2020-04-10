/*
 * vim:list:ts=4:sw=4:noet:
 */
	.text
again:
    li   x29, 8
	li   x30, 4
	li   x28, 1023
	slli x28, x28, 52 # 0x3ff0000000000000 = 1,0
	li   x6, 0x3FC4000000000000 # 0,15625

    .word 0x33ea08b  # movx2g ux1, x29, 19
	.word 0x27f210b  # movx2g ux2, x30, 7
	.word 0xc20818b  # gadd   ux3, ux1, ux2
	.word 0xe20818b  # gsub   ux3, ux1, ux2
	.word 0x1620818b # gmul   ux3, ux1, ux2
	.word 0x1e20818b # gdiv   ux3, ux1, ux2
	.word 0x2d1cf8b  # movg2x x31, ux3,  13
	.word 0x240e220b # fcvt_d_b ux4, x28
	.word 0x2403220b # fcvt_d_b ux4, x6
    j again
