# vim:ts=8:sw=8:noet:ai
	.file	"e.s"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16

#Pris dans mpfr.h
#typedef enum {
#  MPFR_RNDN=0,  /* round to nearest, with ties to even */
#  MPFR_RNDZ,    /* round toward zero */
#  MPFR_RNDU,    /* round toward +Inf */
#  MPFR_RNDD,    /* round toward -Inf */
#  MPFR_RNDA,    /* round away from zero */
#  MPFR_RNDF,    /* faithful rounding */
#  MPFR_RNDNA=-1 /* round to nearest, with ties away from zero (mpfr_round) */
#} mpfr_rnd_t;


	.equiv  MPFR_RNDD, 3
	.text
	.align	1
	.globl	e_approx
	.type	e_approx, @function

e_approx:
   
	# Spécifie l'arrondi pour les opérations suivantes
	li	t0, MPFR_RNDD
	#srnd	t0
	# met la représentation double de 1.0 dans t0
	li	t0,1023  # 0x3ff
        slli	t0,t0,52 # 0x3ff0000000000000
	# et charge là dans gx0 (notre registre vpr[0])
	# qui est notre variable t dans le prog mpfr
	fcvt.d.g gx0, t0
	# et aussi dans gx1 qui est s
	fcvt.d.g gx1, t0

	li	t1, 1		# indice de boucle, i
	li	t2, 100		# borne de la boucle
loop:	
	fcvt.d.lu fa0, t1	# 1 => 1.0
	fmv.x.d   t3,  fa0	# 1.0 => 0x3ff0000000000000
	fcvt.d.g  gx3, t3	# gx3 est notre indice en vp

	gmul	gx0, gx0, gx3 
	fcvt.d.g gx2, t0	# gx2 est u, t0 est 1.0
	gdiv	gx2, gx2, gx0
	gadd	gx1, gx1, gx2

	addi	t1, t1, 1
	ble	t1, t2, loop
	la	t0, room_for_vpr
	stg	gx1, t0
	ret

	.data
room_for_vpr: .space 1024
