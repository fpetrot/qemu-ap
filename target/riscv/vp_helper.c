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
    if (dest != 0) {
        printf("TEST MOV_G2X \n");
        mpfr_printf("%.128Rf\n", env->vpr[src1]);
        env->gpr[dest] = mpfr_get_ui(env->vpr[src1], MPFR_RNDD);
    }
}


void helper_fcvt_d_b(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST FCVT_D_B \n");

    /* We assume for the moment that we are in 64 bits */
    // Signe
    int64_t signe = (env->gpr[src1] >> 63) == 0?1:-1;

    // Exponant
    int64_t exponant = (env->gpr[src1] & 0x7ff0000000000000) >> 52;
    exponant -= 1023;

    // Mantissa
    uint64_t mantissa = (env->gpr[src1] & 0x000fffffffffffff);
    double m = 1.0;
    for (int i = 1; i < 53; i++) {
        m += powl(2.0, -(double)i) * ((mantissa >> (52-i)) & 1);
    }

    // Result
    double res = signe * m * pow(2, exponant);

    mpfr_t x;
    mpfr_init2(x, 200);
    mpfr_set_d(x, res, MPFR_RNDD);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}
