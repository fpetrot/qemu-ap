/*
 * Variable Precision Emulation Helpers for QEMU, RISCV Target
 *
 * Copyright (c) 2020-2020 Frédéric Pétrot 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "qemu/host-utils.h"
#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

struct FloatIEEE
{
    int8_t sign;
    uint64_t frac;
    int64_t exp;
};

static struct FloatIEEE *unpack_float(target_ulong num)
{
    struct FloatIEEE *f = malloc(sizeof(struct FloatIEEE));

#if defined(TARGET_RISCV64)
    f->sign = (num >> 63) == 0?1:-1;

    f->exp = (num & 0x7ff0000000000000) >> 52;
    f->exp -= 1023;

    f->frac = (num & 0x000fffffffffffff);
#endif

#if defined(TARGET_RISCV32)
    f->sign = (num >> 31) == 0?1:-1;

    f->exp = (num & 0x7f800000) >> 23;
    f->exp -= 127;

    f->frac = (num & 0x007fffff);
#endif
    return f;
}

/* Helpers */

void helper_ldu(CPURISCVState *env, target_ulong dest, target_ulong idx, target_ulong data)
{
    // TODO
    //env->vpr[dest] = NULL;
    //env->vpr[dest][idx] = data; /* TODO: inline that to gain some time when refactoring */
}


/* For now dumb operations that should end up as calls to the mpfr library */
void helper_gadd(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST ADD \n");

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_add(x, env->vpr[src1], env->vpr[src2], MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_gsub(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST SUB \n");

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_sub(x, env->vpr[src1], env->vpr[src2], MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_gmul(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST MUL \n");

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_mul(x, env->vpr[src1], env->vpr[src2], MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_gdiv(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST DIV \n");

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_div(x, env->vpr[src1], env->vpr[src2], MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_mov_x2g(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST MOV_X2G \n");
    // TODO : Modify
    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_set_ui(x, env->gpr[src1], MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_mov_g2x(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    /* Copy back into the general purpose register the partial value in the unum reg,
     * but only if it is not zero (and we hope so!) */
    // TODO : Modify
    if (dest != 0) {
        printf("TEST MOV_G2X \n");
        mpfr_printf("%.128Rf\n", env->vpr[src1]);
        env->gpr[dest] = mpfr_get_ui(env->vpr[src1], MPFR_RNDD);
    }
}


void helper_fcvt_b_d(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST FCVT_B_D \n");

    struct FloatIEEE *f = unpack_float(env->gpr[src1]);
    double m = 1.0;

#if defined(TARGET_RISCV64)
    for (int i = 1; i < 53; i++) {
        m += powl(2.0, -(double)i) * ((f->frac >> (52-i)) & 1);
    }
#endif

    // Result
    double res = f->sign * m * pow(2, f->exp);

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_set_d(x, res, MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);  
}


void helper_fcvt_b_dfpr(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST FCVT_B_DFPR \n");

    struct FloatIEEE *f = unpack_float(env->fpr[src1]);
    double m = 1.0;

#if defined(TARGET_RISCV64)
    for (int i = 1; i < 53; i++) {
        m += powl(2.0, -(double)i) * ((f->frac >> (52-i)) & 1);
    }
#endif

    // Result
    double res = f->sign * m * pow(2, f->exp);

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_set_d(x, res, MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}