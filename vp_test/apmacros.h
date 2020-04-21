/*
 * vim:list:ts=8:sw=8:noet:
 * Macros to simplify asm writing for arbitrary precision arithmetic
 * These macros use the .insn directive specific to the riscv target
 * of the gnu assembler
 */

/*
 * Let's define our own register names, just to make things clear
 * FIXME: change the names sometimes
 */
#define  gx0  x0
#define  gx1  x1
#define  gx2  x2
#define  gx3  x3
#define  gx4  x4
#define  gx5  x5
#define  gx6  x6
#define  gx7  x7
#define  gx8  x8
#define  gx9  x9
#define gx10 x10
#define gx11 x11
#define gx12 x12
#define gx13 x13
#define gx14 x14
#define gx15 x15
#define gx16 x16
#define gx17 x17
#define gx18 x18
#define gx19 x19
#define gx20 x20
#define gx21 x21
#define gx22 x22
#define gx23 x23
#define gx24 x24
#define gx25 x25
#define gx26 x26
#define gx27 x27
#define gx28 x28
#define gx29 x29
#define gx30 x30
#define gx31 x31

/*
 * Values copied form mpfr.h that we need in asm
 */
#define MPFR_RNDN    0
#define MPFR_RNDZ    1
#define MPFR_RNDU    2
#define MPFR_RNDD    3
#define MPFR_RNDA    4
#define MPFR_RNDF    5
#define MPFR_RNDNA  -1

/*
 * Values copied from riscv spec headers that we need in asm
 */
#define MSTATUS_FS   0x00006000
#define MSTATUS_FS   0x00006000

/*
 * Definition of our own asm opcode to avoid writing everything
 * in hexadecimal
 * We rely on the .insn directive for that.
 * use the "r" for the arithmetic and conversion operations
 *'R type: .insn r opcode, func3, func7, rd, rs1, rs2'
 * +-------+-----+-----+-------+----+-------------+
 * | func7 | rs2 | rs1 | func3 | rd |      opcode |
 * +-------+-----+-----+-------+----+-------------+
 * 31      25    20    15      12   7             0
 */
#define fcvt_d_g(aprd, rs1)      .insn r 0x0b, 2, 0x12, aprd, rs1,   x0
#define gmul(aprd, aprs1, aprs2) .insn r 0x0b, 0, 0xb,  aprd, aprs1, aprs2
#define gdiv(aprd, aprs1, aprs2) .insn r 0x0b, 0, 0x0f, aprd, aprs1, aprs2
#define gadd(aprd, aprs1, aprs2) .insn r 0x0b, 0, 0x06, aprd, aprs1, aprs2
#define spre(rs1)                .insn r 0x0b, 2, 0x19, x0,   rs1,   x0
#define srnd(rs1)                .insn r 0x0b, 2, 0x1b, x0,   rs1,   x0
