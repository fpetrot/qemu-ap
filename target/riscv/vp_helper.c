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
/* Should probably include mpfr.h here, ... */

void helper_ldu(CPURISCVState *env, target_ulong dest, target_ulong idx, target_ulong data)
{
    env->vpr[dest][idx] = data; /* TODO: inline that to gain some time when refactoring */
}

/* For now dumb operations that should end up as calls to the mpfr library */
void helper_gadd(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong src2)
{
    /* Call mpfr instead of doing an addition here */
    env->vpr[dest][0] = env->vpr[src1][0] + env->vpr[src2][0];
}

void helper_mov_x2g(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    /* Do whatever is necessary to copy stuff into an mpfr buffer */
    env->vpr[dest][imm] =  env->gpr[src1];
}

void helper_mov_g2x(CPURISCVState *env, target_ulong dest, target_ulong src1, target_ulong imm)
{
    /* Copy back into the general purpose register the partial value in the unum reg,
     * but only if it is not zero (and we hope so!) */
    if (dest != 0)
        env->gpr[dest] = env->vpr[src1][imm];
}
