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

#include "mpfr-impl.h"
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


/* Helpers 32-bit */
void helper_fcvt_vp_f(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_VP_F \n");

    struct FloatIEEE *f = unpack_float(env->gpr[src1]);
    float m = 1.0;

#if defined(TARGET_RISCV32)
    for (int i = 1; i < 24; i++) {
        m += powf(2.0, -(float)i) * ((f->frac >> (23-i)) & 1);
    }
#endif

    // Result
    float res = f->sign * m * pow(2, f->exp);

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_set_flt(x, res, rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fcvt_vp_ffpr(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_VP_FFPR \n");

    struct FloatIEEE *f = unpack_float(env->fpr[src1]);
    float m = 1.0;

#if defined(TARGET_RISCV32)
    for (int i = 1; i < 24; i++) {
        m += powf(2.0, -(float)i) * ((f->frac >> (23-i)) & 1);
    }
#endif

    // Result
    float res = f->sign * m * pow(2, f->exp);

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_set_flt(x, res, rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fcvt_f_vp(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_F_VP \n");

#if defined(TARGET_RISCV32)
    target_ulong res;
    float nb_double = mpfr_get_flt(env->vpr[src1], rm);
    memcpy(&res, &nb_double, sizeof(target_ulong));
    env->gpr[dest] = res;
#endif
}


void helper_fcvt_ffpr_vp(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_FFPR_VP \n");

#if defined(TARGET_RISCV32)
    target_ulong res;
    float nb_double = mpfr_get_flt(env->vpr[src1], rm);
    memcpy(&res, &nb_double, sizeof(target_ulong));
    env->fpr[dest] = res;
#endif
}


/* Helpers 64-bit */

void helper_ldu(CPURISCVState *env, target_ulong dest, target_ulong idx, target_ulong data)
{
    // TODO
    //env->vpr[dest] = NULL;
    //env->vpr[dest][idx] = data; /* TODO: inline that to gain some time when refactoring */
}


/* For now dumb operations that should end up as calls to the mpfr library */
void helper_fadd_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    printf("TEST FADD_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_add(x, env->vpr[src1], env->vpr[src2], rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fsub_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    printf("TEST FSUB_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_sub(x, env->vpr[src1], env->vpr[src2], rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fmul_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    printf("TEST FMUL_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_mul(x, env->vpr[src1], env->vpr[src2], rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fdiv_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    printf("TEST FDIV_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_div(x, env->vpr[src1], env->vpr[src2], rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fsqrt_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FSQRT_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_sqrt(x, env->vpr[src1], rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_feq_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST FEQ_P \n");

    if (mpfr_cmp(env->vpr[src1], env->vpr[src2]) == 0) {
        env->gpr[dest] = 1;
    } else {
        env->gpr[dest] = 0;
    }
}


void helper_flt_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST FLT_P \n");

    if (mpfr_cmp(env->vpr[src1], env->vpr[src2]) < 0) {
        env->gpr[dest] = 1;
    } else {
        env->gpr[dest] = 0;
    }
}


void helper_fle_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST FLE_P \n");

    if (mpfr_cmp(env->vpr[src1], env->vpr[src2]) <= 0) {
        env->gpr[dest] = 1;
    } else {
        env->gpr[dest] = 0;
    }
}


void helper_fmin_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST FMIN_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_min(x, env->vpr[src1], env->vpr[src2], env->rounding_mode);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fmax_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    printf("TEST FMAX_P \n");

    mpfr_t x;
    mpfr_init2(x, env->precision);
    mpfr_max(x, env->vpr[src1], env->vpr[src2], env->rounding_mode);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_fcvt_vp_d(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_VP_D \n");

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
    mpfr_init2(x, env->precision);
    mpfr_set_d(x, res, rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);  
}


void helper_fcvt_vp_dfpr(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_VP_DFPR \n");

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
    mpfr_init2(x, env->precision);
    mpfr_set_d(x, res, rm);
    memcpy(env->vpr[dest], x, sizeof(mpfr_t));

    mpfr_printf("%.128Rf\n", env->vpr[dest]);
}


void helper_lpre(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST LOAD PRECISION : %ld\n", env->precision);
    env->gpr[dest] = env->precision;
}


void helper_spre(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST STORE PRECISION : %ld\n", env->gpr[src1]);
    env->precision = env->gpr[src1];
}


void helper_lrnd(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST LOAD ROUNDING MODE : %ld\n", env->rounding_mode);
    env->gpr[dest] = env->rounding_mode;
}


void helper_srnd(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    printf("TEST STORE ROUNDING MODE : %ld\n", env->gpr[src1]);
    env->rounding_mode = env->gpr[src1];
}


void helper_fcvt_d_vp(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_D_VP \n");

#if defined(TARGET_RISCV64)
    target_ulong res;
    double nb_double = mpfr_get_d(env->vpr[src1], rm);
    memcpy(&res, &nb_double, sizeof(target_ulong));
    env->gpr[dest] = res;
#endif
}


void helper_fcvt_dfpr_vp(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    printf("TEST FCVT_DFPR_VP \n");

#if defined(TARGET_RISCV64)
    target_ulong res;
    double nb_double = mpfr_get_d(env->vpr[src1], rm);
    memcpy(&res, &nb_double, sizeof(target_ulong));
    env->fpr[dest] = res;
#endif
}


uint64_t helper_flp(CPURISCVState *env, target_ulong dest, target_ulong idx, uint64_t data)
{
    printf("TEST LOAD %lu\n", idx);
    uint64_t res;
    uint64_t nb_limb;
    switch(idx)
    {
        case 0:
            // On regarde si la précision a changé
            if (env->precision != (env->vpr[dest])->_mpfr_prec) {
                mpfr_prec_round(env->vpr[dest], env->precision, env->rounding_mode);
            }
            mpfr_printf("%.128Rf\n", env->vpr[dest]);
            res = 1;
            break;
        case 1:
            mpfr_init2(env->vpr[dest], data);
            res = 1;
            break;
        case 2:
            (env->vpr[dest])->_mpfr_sign = data;
            res = 1;
            break;
        case 3:
            (env->vpr[dest])->_mpfr_exp = data;
            res = 1;
            break;
        case 4:
            // On doit calculer le nombre de limb
            nb_limb = ceil((double)((env->vpr[dest])->_mpfr_prec) / (double)mp_bits_per_limb);
            (env->vpr[dest])->_mpfr_d = (mp_limb_t *) malloc(sizeof(mp_limb_t) * nb_limb);
            res = nb_limb;
            break;
        default:
            // On charge chaque limb
            ((env->vpr[dest])->_mpfr_d)[idx - 5] = data;
            res = 1;
            break;
    }
    return res;
}


uint64_t helper_fsp(CPURISCVState *env, target_ulong src1, uint64_t idx)
{
    printf("TEST STORE %lu\n", idx);
    uint64_t res = 0;
    switch(idx)
    {
        case 1:
            res = (env->vpr[src1])->_mpfr_prec;
            break;
        case 2:
            res = (env->vpr[src1])->_mpfr_sign;
            break;
        case 3:
            res = (env->vpr[src1])->_mpfr_exp;
            break;
        case 4:
            res = MPFR_LIMB_SIZE(env->vpr[src1]);
            break;
        default:
            // On store chaque limb
            res = ((env->vpr[src1])->_mpfr_d)[idx - 5];
            break;
    }
    return res;
}