# vim:list:ts=8:sw=8:noet:
###########################################################
# Définition de synonymes pour nos nouveaux registres
	#include "apregs.h"

	.equiv  MPFR_RNDD, 3
	.equiv  MSTATUS_FS, 0x00006000

	.text
	.align	1
	.globl	e_approx
	.type	e_approx, @function

e_approx:

	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0
	# Spécifie l'arrondi pour les opérations suivantes
	li	t0, MPFR_RNDD
	# srnd	t0
	# met la représentation double de 1.0 dans t0
	li	t0,1023  # 0x3ff
	slli	t0,t0,52 # 0x3ff0000000000000
	# et charge là dans gx0 (notre registre vpr[0])
	# qui est notre variable t dans le prog mpfr
	#
	# Utilisation de la macro .insn pour éviter
	# la construction hexa de l'instruction
	#'R type: .insn r opcode, func3, func7, rd, rs1, rs2'
	# +-------+-----+-----+-------+----+-------------+
	# | func7 | rs2 | rs1 | func3 | rd |      opcode |
	# +-------+-----+-----+-------+----+-------------+
	# 31      25    20    15      12   7             0
	# .word 0x2402a00b # fcvt_d_g gx0, t0
	.insn	r 0x0b, 2, 0x12, gx0, t0, x0 # le x0 ne sert à rien mais est obligatoire, ...
	# et aussi dans gx1 qui est s
	# .word 0x2402a08b # fcvt_d_g gx1, t0
	.insn	r 0x0b, 2, 0x12, gx1, t0, x0

	li	t1, 1		# indice de boucle, i
	li	t2, 100		# borne de la boucle
loop:
	fcvt.d.lu	fa0,t1 # 1 => 1.0
	fmv.x.d	t3,fa0 	 # 1.0 => 0x3ff0000000000000
	# .word 0x240e218b # fcvt_d_g  gx3, t3	# gx3 est notre indice en vp
	.insn	r 0x0b, 2, 0x12, gx3, t3, x0
	# .word 0x1630000b # gmul	gx0, gx0, gx3
	.insn	r 0x0b, 0, 0xb, gx0, gx0, gx3
	# .word 0x2402a10b # fcvt_d_g gx2, t0	# gx2 est u, t0 est 1.0
	.insn	r 0x0b, 2, 0x12, gx2, t0, x0
	# .word 0x1e01010b # gdiv	gx2, gx2, gx0
	.insn	r 0x0b, 0, 0x0f, gx2, gx2, gx0
	# .word 0x0c20808b # gadd	gx1, gx1, gx2
	.insn	r 0x0b, 0, 0x06, gx1, gx1, gx2

	addi	t1, t1, 1
	ble	t1, t2, loop
	la	t0, room_for_vpr
	ret

	.data
room_for_vpr: .space 1024
