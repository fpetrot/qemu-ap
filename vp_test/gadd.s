/*
 * vim:list:ts=4:sw=4:noet:
 */
	.text
again:
	la x28, unum1
	li x29, 1234
	li x30, 4321

	/*
	   Patterns as I understand them, and implemented as such in
	   qemu :
	   gadd 0000110 ..... ..... 000 ..... 0001011
	   x2g  0000001 ..... ..... 010 ..... 0001011
	   g2x  0000001 ..... ..... 100 ..... 0001011
	   0x33ea08b = movx2g ux1, x29, 19
	   0x27f210b = movx2g ux2, x30, 7
	   0xc20818b = gadd   ux3, ux1, ux2
	   0x2d1cf8b = movg2x x31, u3,  13
	*/
	.word 0x33ea08b
	.word 0x27f210b
	.word 0xc20818b
	.word 0x2d1cf8b
	.word 0x40e200b # ldu ux0, x28
	# Result is now in x31
	j again

	.data
unum1: .word 0xdeadbeef
       .word 0xdeadbeee
       .word 0xdeadbeed
       .word 0xdeadbeec
       .word 0xdeadbeeb
       .word 0xdeadbeea
       .word 0xdeadbee9
       .word 0xdeadbee8
       .word 0xdeadbee7
       .word 0xdeadbee6
       .word 0xdeadbee5
       .word 0xdeadbee4
       .word 0xdeadbee3
       .word 0xdeadbee2
       .word 0xdeadbee1
       .word 0xdeadbee0
       .word 0xdeadbedf
       .word 0xdeadbede
       .word 0xdeadbedd
       .word 0xdeadbedc
       .word 0xdeadbedb
       .word 0xdeadbeda
       .word 0xdeadbed9
       .word 0xdeadbed8
       .word 0xdeadbed7
       .word 0xdeadbed6
       .word 0xdeadbed5
       .word 0xdeadbed4
       .word 0xdeadbed3
       .word 0xdeadbed2
       .word 0xdeadbed1
       .word 0xdeadbed0

