/*
 * Variable Precision Emulation Helpers for QEMU, RISCV Target
 *
 * Copyright (c) 2020 Frédéric Pétrot <frederic.petrot@univ-grenoble-alpes.fr>
 * Copyright (c) 2020 Marie Badaroux <marie.badaroux@univ-grenoble-alpes.fr>
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

/* QEMU compile flags do not allow undef #ifdef */
#define HAVE_LDOUBLE_IEEE_EXT_LITTLE 0
#include "mpfr-impl.h"
#include "qemu/osdep.h"
#include "cpu.h"
#include "qemu/host-utils.h"
#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* 
 * Printing helper, cause we must unfortunately debug, as we are only
 * humans, ...
 */
#define MPFR_DEBUG 1

#define MPFR_OUT(x) \
    if (MPFR_DEBUG) mpfr_printf("%-10s %.128Rf\n", &__func__[sizeof("helper")], x);

#define OTHR_OUT(fmt, x) \
    if (MPFR_DEBUG) printf("%-10s %" #fmt "\n", &__func__[sizeof("helper")], x);

/*
 * In the following functions we use MPFR_DECL_INIT followed by mpfr_set
 * when assigning a result because the destination register may also be
 * a source register, in which case changing its precision/exponent as
 * source leads to errors.
 * This might not be optimal in terms of performance, depending on the
 * cost of the mpfr_set_prec and mpfr_set functions, and checking if a srcx
 * register is the dest register could spare some time, ..., or not
 *
 * For the conversion functions, we use type punning when usable, as
 * authorized per rule s6.5 of the C99 standard.
 *
 * Final note: mpfr defines an mp_limb_t type that is most of the time 64 bits
 * on 64-bit machine, so we make this assumption, otherwise nothing works!
 */

/* Single precision conversion helpers */
void helper_fcvt_a_s(CPURISCVState *env, target_ulong dest, uint64_t src1,
                     target_ulong rm)
{
    union {
        target_ulong u;
        float        f;
    } res;
    res.u = src1;
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_flt(env->apr[dest], res.f, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}


target_ulong helper_fcvt_s_a(CPURISCVState *env, target_ulong src1,
                             target_ulong rm)
{
    union {
        target_ulong u;
        float        f;
    } res;
    res.f = mpfr_get_flt(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(f, res.f);
    return res.u;
}

/* 32-bit integer conversion helpers */
void helper_fcvt_a_w(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong rm)
{
    int64_t res = ((int64_t)src1 << 32) >> 32; 
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_si(env->apr[dest], res, rm == 7 ? env->frm : rm);
    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_fcvt_w_a(CPURISCVState *env, target_ulong src1,
                             target_ulong rm)
{
    uint32_t res = mpfr_get_si(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(d, res);
    return res;
}

void helper_fcvt_a_wu(CPURISCVState *env, target_ulong dest, target_ulong src1,
                      target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_ui(env->apr[dest], src1 & 0xffffffff, rm == 7 ? env->frm : rm);
    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_fcvt_wu_a(CPURISCVState *env, target_ulong src1,
                              target_ulong rm)
{
    uint32_t res = mpfr_get_ui(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(u, res);
    return res;
}

/* Double precision conversion helpers */
void helper_fcvt_a_d(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong rm)
{
    union {
        target_ulong u;
        double       d;
    } res;
    res.u = src1;
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_d(env->apr[dest], res.d, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_fcvt_d_a(CPURISCVState *env, target_ulong src1,
                             target_ulong rm)
{
    union {
        target_ulong u;
        double       d;
    } res;
    res.d = mpfr_get_d(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(f, res.d);
    return res.u;
}

/* 64-bit integer conversion helpers */
void helper_fcvt_a_l(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_si(env->apr[dest], src1, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_fcvt_l_a(CPURISCVState *env, target_ulong src1,
                             target_ulong rm)
{
    target_ulong res = mpfr_get_si(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(ld, res);
    return res;
}

void helper_fcvt_a_lu(CPURISCVState *env, target_ulong dest, target_ulong src1,
                      target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec)
        mpfr_set_prec(env->apr[dest], env->fprec);
    mpfr_set_ui(env->apr[dest], src1, rm == 7 ? env->frm : rm);

    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_fcvt_lu_a(CPURISCVState *env, target_ulong src1,
                              target_ulong rm)
{
    target_ulong res = mpfr_get_ui(env->apr[src1], rm == 7 ? env->frm : rm);
    OTHR_OUT(lu, res);
    return res;
}

/*
 * This new version of the functions assumes:
 * a) mpfr_set_* takes quite more time than testing the precision field size
 * b) changes in precision do not occur often
 */
void helper_fadd_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2, target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_add(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_add(env->apr[dest], env->apr[src1], env->apr[src2],
                 rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fmadd_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                    target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        mpfr_add(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_add(env->apr[dest], x, env->apr[src3], rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fnmadd_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    MPFR_CHANGE_SIGN(x);
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        mpfr_sub(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_sub(env->apr[dest], x, env->apr[src3], rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fsub_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2, target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_sub(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_sub(env->apr[dest], env->apr[src1], env->apr[src2],
                 rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}


void helper_fmsub_a(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        mpfr_sub(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_sub(env->apr[dest], x, env->apr[src3], rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fnmsub_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong src2, target_ulong src3, target_ulong rm)
{
    MPFR_DECL_INIT(x, env->fprec);
    mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
    MPFR_CHANGE_SIGN(x);
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        mpfr_add(x, x, env->apr[src3], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_add(env->apr[dest], x, env->apr[src3], rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fmul_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2, target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_mul(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_mul(env->apr[dest], env->apr[src1], env->apr[src2],
                 rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fdiv_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2, target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_div(x, env->apr[src1], env->apr[src2], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_div(env->apr[dest], env->apr[src1], env->apr[src2],
                 rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fsqrt_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                    target_ulong rm)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_sqrt(x, env->apr[src1], rm == 7 ? env->frm : rm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, rm == 7 ? env->frm : rm);
    } else {
        mpfr_sqrt(env->apr[dest], env->apr[src1], rm == 7 ? env->frm : rm);
    }

    MPFR_OUT(env->apr[dest]);
}

target_ulong helper_feq_a(CPURISCVState *env, target_ulong src1,
                          target_ulong src2)
{
    target_ulong res = mpfr_equal_p(env->apr[src1], env->apr[src2]);
    OTHR_OUT(ld, res);
    return res;
}

target_ulong helper_flt_a(CPURISCVState *env, target_ulong src1,
                          target_ulong src2)
{
    target_ulong res = mpfr_less_p(env->apr[src1], env->apr[src2]);
    OTHR_OUT(ld, res);
    return res;
}

target_ulong helper_fle_a(CPURISCVState *env, target_ulong src1,
                          target_ulong src2)
{
    target_ulong res = mpfr_lessequal_p(env->apr[src1], env->apr[src2]);
    OTHR_OUT(ld, res);
    return res;
}

void helper_fmin_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_min(x, env->apr[src1], env->apr[src2], env->frm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, env->frm);
    } else {
        mpfr_min(env->apr[dest], env->apr[src1], env->apr[src2], env->frm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fmax_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                   target_ulong src2)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_max(x, env->apr[src1], env->apr[src2], env->frm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, env->frm);
    } else {
        mpfr_max(env->apr[dest], env->apr[src1], env->apr[src2], env->frm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fsgnj_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                    target_ulong src2)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_copysign(x, env->apr[src1], env->apr[src2], env->frm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, env->frm);
    } else {
        mpfr_copysign(env->apr[dest], env->apr[src1], env->apr[src2], env->frm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fsgnjn_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong src2)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_setsign(x, env->apr[src1], !mpfr_signbit(env->apr[src2]), env->frm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, env->frm);
    } else {
        mpfr_setsign(env->apr[dest], env->apr[src1],
                     !mpfr_signbit(env->apr[src2]), env->frm);
    }

    MPFR_OUT(env->apr[dest]);
}

void helper_fsgnjx_a(CPURISCVState *env, target_ulong dest, target_ulong src1,
                     target_ulong src2)
{
    if (mpfr_get_prec(env->apr[dest]) != env->fprec) {
        MPFR_DECL_INIT(x, env->fprec);
        mpfr_setsign(x, env->apr[src1],
                     !!mpfr_signbit(env->apr[src1])
                     ^ !!mpfr_signbit(env->apr[src2]), env->frm);
        mpfr_set_prec(env->apr[dest], env->fprec);
        mpfr_set(env->apr[dest], x, env->frm);
    } else {
        mpfr_setsign(env->apr[dest], env->apr[src1],
                    !!mpfr_signbit(env->apr[src1])
                    ^ !!mpfr_signbit(env->apr[src2]), env->frm);
    }

    MPFR_OUT(env->apr[dest]);
}

/*
 * Load:
 * retrieves one by one all the field of the mpfr data structure
 * (precision, sign, exponent and then all the blocks of the limb)
 */
target_ulong helper_fla(CPURISCVState *env, target_ulong dest,
                        target_ulong idx, uint64_t data)
{
    switch(idx) {
        case 0:
            if (env->fprec != mpfr_get_prec(env->apr[dest])) {
                mpfr_prec_round(env->apr[dest], env->fprec, env->frm);
            }
            return 0;
        case 1:
            mpfr_set_prec(env->apr[dest], data);
            return idx + 1;
        case 2:
            env->apr[dest]->_mpfr_sign = data;
            return idx + 1;
        case 3:
            /* Warning: do not use mpfr_set_exp(env->apr[dest], data), as
             * it doesn't do what you naïvely expect! */
            env->apr[dest]->_mpfr_exp = data;
            return idx + 1;
        case 4:
            return MPFR_LIMB_SIZE(env->apr[dest]);
        default:
            env->apr[dest]->_mpfr_d[idx - 5] = data;
#ifdef MPFR_DEBUG
            if (MPFR_LIMB_SIZE(env->apr[dest]) - 1 == idx - 5) {
               MPFR_OUT(env->apr[dest]);
            }
#endif
            return idx + 1;
    }
}

/*
 * Store:
 * stores one by one each field of the mpfr data structure
 */
target_ulong helper_fsa(CPURISCVState *env, target_ulong src1, target_ulong idx)
{
    switch(idx) {
        case 1:
            return mpfr_get_prec(env->apr[src1]);
        case 2:
            return env->apr[src1]->_mpfr_sign;
        case 3:
            return mpfr_get_exp(env->apr[src1]);
        case 4:
            return MPFR_LIMB_SIZE(env->apr[src1]);
        default:
#ifdef MPFR_DEBUG
            /* Not so useful, as what really matters is what is in memory,
             * and I have no access to that */
            if (MPFR_LIMB_SIZE(env->apr[src1]) - 1 == idx - 5) {
               MPFR_OUT(env->apr[src1]);
            }
#endif
            return env->apr[src1]->_mpfr_d[idx - 5];
    }
}
