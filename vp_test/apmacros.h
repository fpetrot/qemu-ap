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
#define  p0  x0
#define  p1  x1
#define  p2  x2
#define  p3  x3
#define  p4  x4
#define  p5  x5
#define  p6  x6
#define  p7  x7
#define  p8  x8
#define  p9  x9
#define p10 x10
#define p11 x11
#define p12 x12
#define p13 x13
#define p14 x14
#define p15 x15
#define p16 x16
#define p17 x17
#define p18 x18
#define p19 x19
#define p20 x20
#define p21 x21
#define p22 x22
#define p23 x23
#define p24 x24
#define p25 x25
#define p26 x26
#define p27 x27
#define p28 x28
#define p29 x29
#define p30 x30
#define p31 x31

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

#define flp(imm, rs1, rd)               .insn i 0b0000111, 0b101, rd, rs1, imm
#define fsp(imm, rs2, rs1)              .insn s 0b0100111, 0b101, rs1, rs2, imm
#define fadd_p(rs2, rs1, rm, rd)        .insn r 0b0001011, rm, 0b0000011, rd, rs1, rs2
#define fsub_p(rs2, rs1, rm, rd)        .insn r 0b0001011, rm, 0b0000111, rd, rs1, rs2
#define fmul_p(rs2, rs1, rm, rd)        .insn r 0b0001011, rm, 0b0001011, rd, rs1, rs2
#define fdiv_p(rs2, rs1, rm, rd)        .insn r 0b0001011, rm, 0b0001111, rd, rs1, rs2
#define fsqrt_p(rs1, rm, rd)            .insn r 0b0001011, rm, 0b0101111, rd, rs1, x0
#define feq_p(rs2, rs1, rd)             .insn r 0b0001011, 0b010, 0b1010011, rd, rs1, rs2
#define flt_p(rs2, rs1, rd)             .insn r 0b0001011, 0b001, 0b1010011, rd, rs1, rs2
#define fle_p(rs2, rs1, rd)             .insn r 0b0001011, 0b000, 0b1010011, rd, rs1, rs2
#define fmin_p(rs2, rs1, rd)            .insn r 0b0001011, 0b000, 0b0010111, rd, rs1, rs2
#define fmax_p(rs2, rs1, rd)            .insn r 0b0001011, 0b001, 0b0010111, rd, rs1, rs2
#define fsgnj_p(rs2, rs1, rd)           .insn r 0b0001011, 0b000, 0b0010011, rd, rs1, rs2
#define fsgnjn_p(rs2, rs1, rd)          .insn r 0b0001011, 0b001, 0b0010011, rd, rs1, rs2
#define fsgnjx_p(rs2, rs1, rd)          .insn r 0b0001011, 0b010, 0b0010011, rd, rs1, rs2
#define fcvt_p_d(rs1, rm, rd)           .insn r 0b0001011, rm, 0b1100011, rd, rs1, x1
#define fcvt_d_p(rs1, rm, rd)           .insn r 0b0001011, rm, 0b1100011, rd, rs1, x2
#define fcvt_p_l(rs1, rm, rd)           .insn r 0b0001011, rm, 0b1100011, rd, rs1, x9
#define fcvt_l_p(rs1, rm, rd)           .insn r 0b0001011, rm, 0b1100011, rd, rs1, x10
#define fcvt_p_lu(rs1, rm, rd)          .insn r 0b0001011, rm, 0b1100011, rd, rs1, x11
#define fcvt_lu_p(rs1, rm, rd)          .insn r 0b0001011, rm, 0b1100011, rd, rs1, x12
#define fmadd_p(rs3, rs2, rs1, rm, rd)  .insn r4 0b0001011, rm, 0b00, rd, rs1, rs2, rs3
#define fmsub_p(rs3, rs2, rs1, rm, rd)  .insn r4 0b0001011, rm, 0b01, rd, rs1, rs2, rs3