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


/*
 * In the following functions we use MPFR_DECL_INIT followed by mpfr_set
 * when assigning a result because the destination register may also be
 * a source register, in which case changing its precision/exponent as
 * source leads to errors.
 * This might not be optimal in terms of performance, depending on the
 * cost of the mpfr_set_prec and mpfr_set functions, and checking if a srcx
 * register is the dest register could spare some time, ..., or not
 */
static inline void check_attributes(CPURISCVState *env, target_ulong dest)
{
   if (mpfr_get_prec(env->apr[dest]) != env->fprec)
      mpfr_set_prec(env->apr[dest], env->fprec);
   if (mpfr_get_exp(env->apr[dest]) != env->fexp)
      mpfr_set_exp(env->apr[dest], env->fexp);
}

/* 
 * Printing helper, cause we must unfortunately debug, as we are only
 * humans, ...
 */

#define MPFR_OUT(x) \
    mpfr_printf("%-10s %.128Rf\n", &__func__[sizeof("helper")], x);

#define OTHR_OUT(fmt, x) \
    printf("%-10s %" #fmt "\n", &__func__[sizeof("helper")], x);

/* Helpers 32-bit */
void helper_fcvt_p_f(CPURISCVState *env, target_ulong dest, uint64_t src1, target_ulong rm)
{
    struct FloatIEEE *f = unpack_float(src1);
    float m = 1.0;

#if defined(TARGET_RISCV32)
    for (int i = 1; i < 24; i++) {
        m += powf(2.0, -(float)i) * ((f->frac >> (23-i)) & 1);
    }
#endif

    // Result
    float res = f->sign * m * pow(2, f->exp);

    check_attributes(env, dest);
    mpfr_set_flt(env->apr[dest], res, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


target_ulong helper_fcvt_f_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV32)
    union {
        target_ulong u;
        float        f;
    } res;
    res.f = mpfr_get_flt(env->apr[src1], rm == 7 ? env->frm : rm);
    return res.u;
#else
    return 0xdeadbeef;
#endif
}


void helper_fcvt_p_w(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV32)
    check_attributes(env, dest);
    mpfr_set_si(env->apr[dest], src1, rm == 7 ? env->frm : rm);
#endif
}


target_ulong helper_fcvt_w_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV32)
    return mpfr_get_si(env->apr[src1], rm == 7 ? env->frm : rm);
#else
    return 0xdeadbeef;
#endif
}


void helper_fcvt_p_wu(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV32)
    check_attributes(env, dest);
    mpfr_set_ui(env->apr[dest], src1, rm == 7 ? env->frm : rm);
#endif
}


target_ulong helper_fcvt_wu_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV32)
    return mpfr_get_ui(env->apr[src1], rm == 7 ? env->frm : rm);
#else
    return 0xdeadbeef;
#endif
}

/* Helpers 64-bit */
void helper_fcvt_p_d(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    struct FloatIEEE *f = unpack_float(src1);
    double m = 1.0;

#if defined(TARGET_RISCV64)
    for (int i = 1; i < 53; i++) {
        m += powl(2.0, -(double)i) * ((f->frac >> (52-i)) & 1);
    }
#endif

    // Result
    double res = f->sign * m * pow(2, f->exp);

    check_attributes(env, dest);
    mpfr_set_d(env->apr[dest], res, rm == 7 ? env->frm : rm);

#if 1
    MPFR_OUT(env->apr[dest]);
#endif
}


target_ulong helper_fcvt_d_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV64)
    union {
        target_ulong u;
        double       d;
    } res;
    res.d = mpfr_get_d(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(f, res.d);
    return res.u;
#else
    return 0xdeadbeef;
#endif
}


void helper_fcvt_p_l(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV64)
    check_attributes(env, dest);
    mpfr_set_si(env->apr[dest], src1, rm == 7 ? env->frm : rm);
#if 1
    MPFR_OUT(env->apr[dest]);
#endif
#endif
}


target_ulong helper_fcvt_l_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV64)
    target_ulong res = mpfr_get_si(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(ld, res);
    return res;
#else
    return 0xdeadbeef;
#endif
}


void helper_fcvt_p_lu(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV64)
    check_attributes(env, dest);
    mpfr_set_ui(env->apr[dest], src1, rm == 7 ? env->frm : rm);
#if 1
    MPFR_OUT(env->apr[dest]);
#endif
#endif
}


target_ulong helper_fcvt_lu_p(CPURISCVState *env, target_ulong src1, target_ulong rm)
{
#if defined(TARGET_RISCV64)
    target_ulong res = mpfr_get_ui(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(lu, res);
    return res;
#else
    return 0xdeadbeef;
#endif
}

void helper_fadd_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_add(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fmadd_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    mpfr_add(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fnmadd_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    MPFR_CHANGE_SIGN(env->apr[src1]);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    mpfr_sub(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    MPFR_CHANGE_SIGN(env->apr[src1]);

    MPFR_OUT(env->apr[dest]);
}


void helper_fsub_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_sub(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fmsub_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    mpfr_sub(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fnmsub_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    MPFR_CHANGE_SIGN(env->apr[src1]);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    mpfr_add(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    MPFR_CHANGE_SIGN(env->apr[src1]);

    MPFR_OUT(env->apr[dest]);
}


void helper_fmul_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fdiv_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_div(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fsqrt_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_sqrt(env->apr[dest], env->apr[src1], rm == 7 ? env->frm : rm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


target_ulong helper_feq_p(CPURISCVState *env, target_ulong src1, target_ulong src2)
{
    return mpfr_equal_p(env->apr[src1], env->apr[src2]);
}


target_ulong helper_flt_p(CPURISCVState *env, target_ulong src1, target_ulong src2)
{
    return mpfr_less_p(env->apr[src1], env->apr[src2]);
}


target_ulong helper_fle_p(CPURISCVState *env, target_ulong src1, target_ulong src2)
{
    return mpfr_lessequal_p(env->apr[src1], env->apr[src2]);
}


void helper_fmin_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_min(x, env->apr[src1], env->apr[src2], env->frm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, env->frm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fmax_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_max(x, env->apr[src1], env->apr[src2], env->frm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, env->frm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fsgnj_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_copysign(x, env->apr[src1], env->apr[src2], env->frm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, env->frm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fsgnjn_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_setsign(x, env->apr[src1], !mpfr_signbit(env->apr[src2]), env->frm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, env->frm);

    MPFR_OUT(env->apr[dest]);
}


void helper_fsgnjx_p(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_setsign(x, env->apr[src1], !!mpfr_signbit(env->apr[src1]) ^ !!mpfr_signbit(env->apr[src2]), env->frm);
    check_attributes(env, dest);
    mpfr_set(env->apr[dest], x, env->frm);

    MPFR_OUT(env->apr[dest]);
}


uint64_t helper_flp(CPURISCVState *env, target_ulong dest, target_ulong idx, uint64_t data)
{
    uint64_t res;
    uint64_t nb_limb;
    switch(idx)
    {
        case 0:
            // On regarde si la précision a changé
            if (env->fprec != (env->apr[dest])->_mpfr_prec) {
                mpfr_prec_round(env->apr[dest], env->fprec, env->frm);
            }
            res = 1;
            break;
        case 1:
            mpfr_init2(env->apr[dest], data);
            res = 1;
            break;
        case 2:
            (env->apr[dest])->_mpfr_sign = data;
            res = 1;
            break;
        case 3:
            (env->apr[dest])->_mpfr_exp = data;
            res = 1;
            break;
        case 4:
            // On doit calculer le nombre de limb
            nb_limb = ceil((double)((env->apr[dest])->_mpfr_prec) / (double)mp_bits_per_limb);
            (env->apr[dest])->_mpfr_d = (mp_limb_t *) malloc(sizeof(mp_limb_t) * nb_limb);
            res = nb_limb;
            break;
        default:
            // On charge chaque limb
            ((env->apr[dest])->_mpfr_d)[idx - 5] = data;
            res = 1;
            break;
    }
    return res;
}


uint64_t helper_fsp(CPURISCVState *env, target_ulong src1, uint64_t idx)
{
    uint64_t res = 0;
    switch(idx)
    {
        case 1:
            res = (env->apr[src1])->_mpfr_prec;
            break;
        case 2:
            res = (env->apr[src1])->_mpfr_sign;
            break;
        case 3:
            res = (env->apr[src1])->_mpfr_exp;
            break;
        case 4:
            res = MPFR_LIMB_SIZE(env->apr[src1]);
            break;
        default:
            // On store chaque limb
            res = ((env->apr[src1])->_mpfr_d)[idx - 5];
            break;
    }
    return res;
}
