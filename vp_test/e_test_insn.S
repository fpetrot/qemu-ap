/*
 * vim:list:ts=4:sw=4:noet:
 */
#include "apmacros.h"
	.text
	.align	1
	.globl	e_approx
	.type	e_approx, @function

e_approx:

	# Indique que l'on veut utiliser la FPU
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	# Precision et rounding mode
    li   x28, 200
	spre(x28)
	li   x5, MPFR_RNDD
	srnd(x5)
	# met la représentation double de 1.0 dans t0
	li	t0,1023  # 0x3ff
    slli	t0,t0,52 # 0x3ff0000000000000
	# et charge là dans gx0 (notre registre vpr[0])
	# qui est notre variable t dans le prog mpfr
	fcvt_d_g(gx0, t0)
	# et aussi dans gx1 qui est s
	fcvt_d_g(gx1, t0)

	li	t1, 1		# indice de boucle, i
	li	t2, 100		# borne de la boucle
loop:	
	fcvt.d.lu fa0,t1 # 1 => 1.0
	fmv.x.d t3,fa0 	 # 1.0 => 0x3ff0000000000000
	fcvt_d_g  (gx3, t3)
	gmul	  (gx0, gx0, gx3)
	fcvt_d_g  (gx2, t0)
	gdiv	  (gx2, gx2, gx0)
	gadd	  (gx1, gx1, gx2)

	addi	t1, t1, 1
	ble	t1, t2, loop
	la	t0, room_for_vpr
	ret

	.data
room_for_vpr: .space 1024
