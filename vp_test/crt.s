.section .text.init,"ax",@progbits
.globl _start
_start:
    # We shall not fail!
    csrw    mtvec, x0
    .equiv  MPFR_RNDD, 3
    .equiv  MSTATUS_FS, 0x00006000

    # But we shall use the FPU
    li  t0, MSTATUS_FS
    csrs    mstatus, t0

    # We're in bare metal, so set stack at the end of physical memory
    la      sp, _memory_end
    jal     main

die:
    wfi
    j       die
