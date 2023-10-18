/*
 * Translation routines for the RISC-V variable precision extensions
 *
 * Copyright (c) 2020-2024
*                Frédéric Pétrot <frederic.petrot@univ-grenoble-alpes.fr>
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

/*
 * Some doc regarding the tcg insns is here:
 * https://wiki.qemu.org/Documentation/TCG/frontend-ops 
 */

static bool trans_fla(DisasContext *ctx, arg_fla *a)
{
    TCGv data = tcg_temp_new();
    TCGv dest = tcg_temp_new();
    TCGv addr = get_address(ctx, a->rs1, a->imm);
    TCGv i = tcg_temp_new();
    TCGv nb = tcg_temp_new();
    TCGv limb = tcg_temp_new();
    TCGLabel *loop = gen_new_label();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(i, 1);
    tcg_gen_qemu_ld_i64(data, addr, ctx->mem_idx, MO_TEUQ);
    gen_helper_fla(i, tcg_env, dest, i, data);
    tcg_gen_addi_tl(addr, addr, 8);
    tcg_gen_qemu_ld_i64(data, addr, ctx->mem_idx, MO_TEUQ);
    gen_helper_fla(i, tcg_env, dest, i, data);
    tcg_gen_addi_tl(addr, addr, 8);
    tcg_gen_qemu_ld_i64(data, addr, ctx->mem_idx, MO_TEUQ);
    gen_helper_fla(i, tcg_env, dest, i, data);
    tcg_gen_addi_tl(addr, addr, 8);

    gen_helper_fla(limb, tcg_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(nb, limb, 5);

    gen_set_label(loop);
    tcg_gen_qemu_ld_i64(data, addr, ctx->mem_idx, MO_TEUQ);
    gen_helper_fla(i, tcg_env, dest, i, data);
    tcg_gen_addi_tl(addr, addr, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop);

    tcg_gen_movi_tl(i, 0);
    gen_helper_fla(limb, tcg_env, dest, i, data);
    return true;
}

static bool trans_fsa(DisasContext *ctx, arg_fsa *a)
{
    TCGv addr = get_address(ctx, a->rs1, a->imm);
    TCGv src2 = tcg_temp_new();
    TCGv res = tcg_temp_new();
    TCGv nb = tcg_temp_new();
    TCGv i = tcg_temp_new();
    TCGLabel *loop = gen_new_label();

    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(i, 1);
    gen_helper_fsa(res, tcg_env, src2, i);
    tcg_gen_qemu_st_i64(res, addr, ctx->mem_idx, MO_TEUQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(addr, addr, 8);
    gen_helper_fsa(res, tcg_env, src2, i);
    tcg_gen_qemu_st_i64(res, addr, ctx->mem_idx, MO_TEUQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(addr, addr, 8);
    gen_helper_fsa(res, tcg_env, src2, i);
    tcg_gen_qemu_st_i64(res, addr, ctx->mem_idx, MO_TEUQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(addr, addr, 8);

    gen_helper_fsa(res, tcg_env, src2, i);
    tcg_gen_addi_tl(i, i, 1);

    tcg_gen_movi_tl(nb, 5);
    tcg_gen_add_tl(nb, nb, res);

    gen_set_label(loop);
    gen_helper_fsa(res, tcg_env, src2, i);
    tcg_gen_qemu_st_i64(res, addr, ctx->mem_idx, MO_TEUQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(addr, addr, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop);
    return true;
}

/* Single precision conversions */
static bool trans_fcvt_a_s(DisasContext *ctx, arg_fcvt_a_s *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = get_fpr_hs(ctx, a->rs1);

    gen_helper_fcvt_a_s(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_s_a(DisasContext *ctx, arg_fcvt_s_a *a)
{
    TCGv dest = dest_fpr(ctx, a->rd);
    TCGv src1 = get_fpr_hs(ctx, a->rs1);

    gen_helper_fcvt_s_a(dest, tcg_env, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_a_w(DisasContext *ctx, arg_fcvt_a_w *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = get_gpr(ctx, a->rs1, EXT_NONE);

    tcg_gen_movi_tl(dest, a->rd);
    gen_helper_fcvt_a_w(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_w_a(DisasContext *ctx, arg_fcvt_w_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    gen_helper_fcvt_w_a(dest, tcg_env, src1, tcg_constant_tl(a->rm));
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fcvt_a_wu(DisasContext *ctx, arg_fcvt_a_wu *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = get_gpr(ctx, a->rs1, EXT_NONE);

    tcg_gen_movi_tl(dest, a->rd);
    gen_helper_fcvt_a_wu(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_wu_a(DisasContext *ctx, arg_fcvt_wu_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    gen_helper_fcvt_wu_a(dest, tcg_env, src1, tcg_constant_tl(a->rm));
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fcvt_a_d(DisasContext *ctx, arg_fcvt_a_d *a)
{
    TCGv dest = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    gen_helper_fcvt_a_d(tcg_env, dest, cpu_fpr[a->rs1], tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_d_a(DisasContext *ctx, arg_fcvt_l_a *a)
{
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    gen_helper_fcvt_d_a(cpu_fpr[a->rd], tcg_env, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_a_l(DisasContext *ctx, arg_fcvt_a_l *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = get_gpr(ctx, a->rs1, EXT_NONE);

    tcg_gen_movi_tl(dest, a->rd);
    gen_helper_fcvt_a_l(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_l_a(DisasContext *ctx, arg_fcvt_l_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    gen_helper_fcvt_l_a(dest, tcg_env, src1, tcg_constant_tl(a->rm));
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fcvt_a_lu(DisasContext *ctx, arg_fcvt_a_lu *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = get_gpr(ctx, a->rs1, EXT_NONE);

    tcg_gen_movi_tl(dest, a->rd);
    gen_helper_fcvt_a_lu(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fcvt_lu_a(DisasContext *ctx, arg_fcvt_lu_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    gen_helper_fcvt_lu_a(dest, tcg_env, src1, tcg_constant_tl(a->rm));
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fadd_a(DisasContext *ctx, arg_fadd_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fadd_a(tcg_env, dest, src1, src2, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fmadd_a(DisasContext *ctx, arg_fmadd_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);

    gen_helper_fmadd_a(tcg_env, dest, src1, src2, src3, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fnmadd_a(DisasContext *ctx, arg_fnmadd_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);

    gen_helper_fnmadd_a(tcg_env, dest, src1, src2, src3, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fsub_a(DisasContext *ctx, arg_fsub_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fsub_a(tcg_env, dest, src1, src2, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fmsub_a(DisasContext *ctx, arg_fmsub_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);

    gen_helper_fmsub_a(tcg_env, dest, src1, src2, src3, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fnmsub_a(DisasContext *ctx, arg_fnmsub_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);

    gen_helper_fnmsub_a(tcg_env, dest, src1, src2, src3, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fmul_a(DisasContext *ctx, arg_fmul_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fmul_a(tcg_env, dest, src1, src2, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fdiv_a(DisasContext *ctx, arg_fdiv_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fdiv_a(tcg_env, dest, src1, src2, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_fsqrt_a(DisasContext *ctx, arg_fsqrt_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);

    gen_helper_fsqrt_a(tcg_env, dest, src1, tcg_constant_tl(a->rm));
    return true;
}

static bool trans_feq_a(DisasContext *ctx, arg_feq_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_feq_a(dest, tcg_env, src1, src2);
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fle_a(DisasContext *ctx, arg_fle_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fle_a(dest, tcg_env, src1, src2);
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_flt_a(DisasContext *ctx, arg_flt_a *a)
{
    TCGv dest = dest_gpr(ctx, a->rd);
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_flt_a(dest, tcg_env, src1, src2);
    gen_set_gpr(ctx, a->rd, dest);
    return true;
}

static bool trans_fmin_a(DisasContext *ctx, arg_fmin_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_fmin_a(tcg_env, dest, src1, src2);
    return true;
}

static bool trans_fmax_a(DisasContext *ctx, arg_fmax_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_fmax_a(tcg_env, dest, src1, src2);
    return true;
}

static bool trans_fsgnj_a(DisasContext *ctx, arg_fsgnj_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_fsgnj_a(tcg_env, dest, src1, src2);
    return true;
}

static bool trans_fsgnjn_a(DisasContext *ctx, arg_fsgnjn_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_fsgnjn_a(tcg_env, dest, src1, src2);
    return true;
}

static bool trans_fsgnjx_a(DisasContext *ctx, arg_fsgnjx_a *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    gen_helper_fsgnjx_a(tcg_env, dest, src1, src2);
    return true;
}
