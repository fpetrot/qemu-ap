/*
 * Translation routines for the RISC-V variable precision extensions
 *
 * Copyright (c) 2020 Frédéric Pétrot, frederic.petrot@univ-grenoble-alpes.fr
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

static bool trans_susr(DisasContext *ctx, arg_susr *a)
{
    return true;
}

static bool trans_srnd(DisasContext *ctx, arg_srnd *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_srnd(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_susr_due_ess(DisasContext *ctx, arg_susr_due_ess *a)
{
    return true;
}

static bool trans_susr_due_fss(DisasContext *ctx, arg_susr_due_fss *a)
{
    return true;
}

static bool trans_susr_sue_ess(DisasContext *ctx, arg_susr_sue_ess *a)
{
    return true;
}

static bool trans_susr_sue_fss(DisasContext *ctx, arg_susr_sue_fss *a)
{
    return true;
}

static bool trans_sp(DisasContext *ctx, arg_sp *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_sp(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_susr_mbb(DisasContext *ctx, arg_susr_mbb *a)
{
    return true;
}

static bool trans_lusr(DisasContext *ctx, arg_lusr *a)
{
    return true;
}

static bool trans_lrnd(DisasContext *ctx, arg_lrnd *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_lrnd(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_lusr_due_ess(DisasContext *ctx, arg_lusr_due_ess *a)
{
    return true;
}

static bool trans_lusr_due_fss(DisasContext *ctx, arg_lusr_due_fss *a)
{
    return true;
}

static bool trans_lusr_sue_ess(DisasContext *ctx, arg_lusr_sue_ess *a)
{
    return true;
}

static bool trans_lusr_sue_fss(DisasContext *ctx, arg_lusr_sue_fss *a)
{
    return true;
}

static bool trans_lp(DisasContext *ctx, arg_lp *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_lp(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_lusr_mbb(DisasContext *ctx, arg_lusr_mbb *a)
{
    return true;
}

static bool trans_mov_g2g(DisasContext *ctx, arg_mov_g2g *a)
{
    return true;
}

static bool trans_movll_g2g(DisasContext *ctx, arg_movll_g2g *a)
{
    return true;
}

static bool trans_movlr_g2g(DisasContext *ctx, arg_movlr_g2g *a)
{
    return true;
}

static bool trans_movrl_g2g(DisasContext *ctx, arg_movrl_g2g *a)
{
    return true;
}

static bool trans_movrr_g2g(DisasContext *ctx, arg_movrr_g2g *a)
{
    return true;
}

static bool trans_mov_x2g(DisasContext *ctx, arg_mov_x2g *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_mov_x2g(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_mov_g2x(DisasContext *ctx, arg_mov_g2x *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_mov_g2x(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_mov_c2g(DisasContext *ctx, arg_mov_c2g *a)
{
    return true;
}

static bool trans_ldu(DisasContext *ctx, arg_ldu *a)
{
    TCGv     data  = tcg_temp_new();
    TCGv     dest  = tcg_temp_local_new();
    TCGv     src1  = tcg_temp_local_new();
    TCGv     i     = tcg_temp_local_new();
    TCGLabel *loop = gen_new_label();

    tcg_gen_movi_tl(dest, a->rgd);
    gen_get_gpr(src1, a->rgs1);
    tcg_gen_movi_tl(i, 0);

    gen_set_label(loop);
#ifdef TARGET_RISCV64
    tcg_gen_qemu_ld_tl(data, src1, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(src1, src1, 8);
#else
    tcg_gen_qemu_ld_tl(data, src1, ctx->mem_idx, MO_TESL);
    tcg_gen_addi_tl(src1, src1, 4);
#endif
    gen_helper_ldu(cpu_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_brcondi_tl(TCG_COND_LT, i, 32, loop);

    tcg_temp_free(data);
    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(i);
    return true;
}

static bool trans_ldul(DisasContext *ctx, arg_ldul *a)
{
    return true;
}

static bool trans_ldur(DisasContext *ctx, arg_ldur *a)
{
    return true;
}

static bool trans_ldub(DisasContext *ctx, arg_ldub *a)
{
    return true;
}

static bool trans_stul(DisasContext *ctx, arg_stul *a)
{
    return true;
}

static bool trans_stur(DisasContext *ctx, arg_stur *a)
{
    return true;
}

static bool trans_stub(DisasContext *ctx, arg_stub *a)
{
    return true;
}

static bool trans_ldu_next(DisasContext *ctx, arg_ldu_next *a)
{
    return true;
}

static bool trans_ldul_next(DisasContext *ctx, arg_ldul_next *a)
{
    return true;
}

static bool trans_ldur_next(DisasContext *ctx, arg_ldur_next *a)
{
    return true;
}

static bool trans_ldub_next(DisasContext *ctx, arg_ldub_next *a)
{
    return true;
}

static bool trans_stul_next(DisasContext *ctx, arg_stul_next *a)
{
    return true;
}

static bool trans_stur_next(DisasContext *ctx, arg_stur_next *a)
{
    return true;
}

static bool trans_stub_next(DisasContext *ctx, arg_stub_next *a)
{
    return true;
}

static bool trans_ldu_s(DisasContext *ctx, arg_ldu_s *a)
{
    return true;
}

static bool trans_ldul_s(DisasContext *ctx, arg_ldul_s *a)
{
    return true;
}

static bool trans_ldur_s(DisasContext *ctx, arg_ldur_s *a)
{
    return true;
}

static bool trans_ldub_s(DisasContext *ctx, arg_ldub_s *a)
{
    return true;
}

static bool trans_stul_s(DisasContext *ctx, arg_stul_s *a)
{
    return true;
}

static bool trans_stur_s(DisasContext *ctx, arg_stur_s *a)
{
    return true;
}

static bool trans_stub_s(DisasContext *ctx, arg_stub_s *a)
{
    return true;
}

static bool trans_ldu_next_s(DisasContext *ctx, arg_ldu_next_s *a)
{
    return true;
}

static bool trans_ldul_next_s(DisasContext *ctx, arg_ldul_next_s *a)
{
    return true;
}

static bool trans_ldur_next_s(DisasContext *ctx, arg_ldur_next_s *a)
{
    return true;
}

static bool trans_ldub_next_s(DisasContext *ctx, arg_ldub_next_s *a)
{
    return true;
}

static bool trans_stul_next_s(DisasContext *ctx, arg_stul_next_s *a)
{
    return true;
}

static bool trans_stur_next_s(DisasContext *ctx, arg_stur_next_s *a)
{
    return true;
}

static bool trans_stub_next_s(DisasContext *ctx, arg_stub_next_s *a)
{
    return true;
}

static bool trans_ldg(DisasContext *ctx, arg_ldg *a)
{
    return true;
}

static bool trans_stg(DisasContext *ctx, arg_stg *a)
{
    return true;
}

static bool trans_gadd(DisasContext *ctx, arg_gadd *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(src2, a->rgs2);

    gen_helper_gadd(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_gsub(DisasContext *ctx, arg_gsub *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(src2, a->rgs2);

    gen_helper_gsub(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_gguess(DisasContext *ctx, arg_gguess *a)
{
    return true;
}

static bool trans_gradius(DisasContext *ctx, arg_gradius *a)
{
    return true;
}

static bool trans_gaddu(DisasContext *ctx, arg_gaddu *a)
{
    return true;
}

static bool trans_gsubu(DisasContext *ctx, arg_gsubu *a)
{
    return true;
}

static bool trans_gmul(DisasContext *ctx, arg_gmul *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(src2, a->rgs2);

    gen_helper_gmul(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_gmulu(DisasContext *ctx, arg_gmulu *a)
{
    return true;
}

static bool trans_gdiv(DisasContext *ctx, arg_gdiv *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(src2, a->rgs2);

    gen_helper_gdiv(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_gdivu(DisasContext *ctx, arg_gdivu *a)
{
    return true;
}

static bool trans_gabs(DisasContext *ctx, arg_gabs *a)
{
    return true;
}

static bool trans_gsqrt(DisasContext *ctx, arg_gsqrt *a)
{
    return true;
}

static bool trans_gisqrt(DisasContext *ctx, arg_gisqrt *a)
{
    return true;
}

static bool trans_gsqrtu(DisasContext *ctx, arg_gsqrtu *a)
{
    return true;
}

static bool trans_gisqrtu(DisasContext *ctx, arg_gisqrtu *a)
{
    return true;
}

static bool trans_gcmp(DisasContext *ctx, arg_gcmp *a)
{
    return true;
}

static bool trans_gcmp_eq(DisasContext *ctx, arg_gcmp_eq *a)
{
    return true;
}

static bool trans_gcmp_neq(DisasContext *ctx, arg_gcmp_neq *a)
{
    return true;
}

static bool trans_gcmp_nneq(DisasContext *ctx, arg_gcmp_nneq *a)
{
    return true;
}

static bool trans_gcmp_gt(DisasContext *ctx, arg_gcmp_gt *a)
{
    return true;
}

static bool trans_gcmp_lt(DisasContext *ctx, arg_gcmp_lt *a)
{
    return true;
}

static bool trans_gcmp_geq(DisasContext *ctx, arg_gcmp_geq *a)
{
    return true;
}

static bool trans_gcmp_leq(DisasContext *ctx, arg_gcmp_leq *a)
{
    return true;
}

static bool trans_gcmp_gnneq(DisasContext *ctx, arg_gcmp_gnneq *a)
{
    return true;
}

static bool trans_gcmp_lnneq(DisasContext *ctx, arg_gcmp_lnneq *a)
{
    return true;
}

static bool trans_gcmp_ll_eq(DisasContext *ctx, arg_gcmp_ll_eq *a)
{
    return true;
}

static bool trans_gcmp_ll_neq(DisasContext *ctx, arg_gcmp_ll_neq *a)
{
    return true;
}

static bool trans_gcmp_ll_gt(DisasContext *ctx, arg_gcmp_ll_gt *a)
{
    return true;
}

static bool trans_gcmp_ll_lt(DisasContext *ctx, arg_gcmp_ll_lt *a)
{
    return true;
}

static bool trans_gcmp_ll_geq(DisasContext *ctx, arg_gcmp_ll_geq *a)
{
    return true;
}

static bool trans_gcmp_ll_leq(DisasContext *ctx, arg_gcmp_ll_leq *a)
{
    return true;
}

static bool trans_gcmp_lr_eq(DisasContext *ctx, arg_gcmp_lr_eq *a)
{
    return true;
}

static bool trans_gcmp_lr_neq(DisasContext *ctx, arg_gcmp_lr_neq *a)
{
    return true;
}

static bool trans_gcmp_lr_gt(DisasContext *ctx, arg_gcmp_lr_gt *a)
{
    return true;
}

static bool trans_gcmp_lr_lt(DisasContext *ctx, arg_gcmp_lr_lt *a)
{
    return true;
}

static bool trans_gcmp_lr_geq(DisasContext *ctx, arg_gcmp_lr_geq *a)
{
    return true;
}

static bool trans_gcmp_lr_leq(DisasContext *ctx, arg_gcmp_lr_leq *a)
{
    return true;
}

static bool trans_gcmp_rl_eq(DisasContext *ctx, arg_gcmp_rl_eq *a)
{
    return true;
}

static bool trans_gcmp_rl_neq(DisasContext *ctx, arg_gcmp_rl_neq *a)
{
    return true;
}

static bool trans_gcmp_rl_gt(DisasContext *ctx, arg_gcmp_rl_gt *a)
{
    return true;
}

static bool trans_gcmp_rl_lt(DisasContext *ctx, arg_gcmp_rl_lt *a)
{
    return true;
}

static bool trans_gcmp_rl_geq(DisasContext *ctx, arg_gcmp_rl_geq *a)
{
    return true;
}

static bool trans_gcmp_rl_leq(DisasContext *ctx, arg_gcmp_rl_leq *a)
{
    return true;
}

static bool trans_gcmp_rr_eq(DisasContext *ctx, arg_gcmp_rr_eq *a)
{
    return true;
}

static bool trans_gcmp_rr_neq(DisasContext *ctx, arg_gcmp_rr_neq *a)
{
    return true;
}

static bool trans_gcmp_rr_gt(DisasContext *ctx, arg_gcmp_rr_gt *a)
{
    return true;
}

static bool trans_gcmp_rr_lt(DisasContext *ctx, arg_gcmp_rr_lt *a)
{
    return true;
}

static bool trans_gcmp_rr_geq(DisasContext *ctx, arg_gcmp_rr_geq *a)
{
    return true;
}

static bool trans_gcmp_rr_leq(DisasContext *ctx, arg_gcmp_rr_leq *a)
{
    return true;
}

static bool trans_fcvt_b_d(DisasContext *ctx, arg_fcvt_b_d *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_fcvt_b_d(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_fcvt_b_dfpr(DisasContext *ctx, arg_fcvt_b_dfpr *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_fcvt_b_dfpr(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}