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

static bool trans_spre(DisasContext *ctx, arg_spre *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_spre(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
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

static bool trans_lpre(DisasContext *ctx, arg_lpre *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_lpre(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
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

static bool trans_flp(DisasContext *ctx, arg_flp *a)
{
    TCGv     data  = tcg_temp_local_new();
    TCGv     dest  = tcg_temp_local_new();
    TCGv     src1  = tcg_temp_local_new();
    TCGv     i     = tcg_temp_local_new();
    TCGv     nb     = tcg_temp_local_new();
    TCGv     limb = tcg_temp_local_new();
    TCGLabel *loop = gen_new_label();
    TCGLabel *loop_limb = gen_new_label();
    tcg_gen_movi_tl(i, 1);

    // On récupère l'adresse de la data en mémoire
    gen_get_gpr(src1, a->rs1);
    tcg_gen_addi_tl(src1, src1, a->imm);

    // On récupère le n° du vpr où stocker la data
    tcg_gen_movi_tl(dest, a->rd);

    // On récupère les 3 premiers champs
    gen_set_label(loop);
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(limb, cpu_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_brcondi_tl(TCG_COND_LT, i, 4, loop);

    // On s'occupe du pointeur sur les limbs
    // D'abord on récupère le nombre de blocks limb qu'on met dans la variable limb
    gen_helper_flp(limb, cpu_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(nb, limb, 5); // nb est notre borne max pour la boucle sur les limbs

    // Puis on load les limbs
    gen_set_label(loop_limb);
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(limb, cpu_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop_limb);

    // Une fois que le nombre est reformé, on regarde si la précision de env a changé ou non
    tcg_gen_movi_tl(i, 0);
    gen_helper_flp(limb, cpu_env, dest, i, data);

    tcg_temp_free(data);
    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(i);
    tcg_temp_free(nb);
    tcg_temp_free(limb);
    return true;
}

static bool trans_fsp(DisasContext *ctx, arg_fsp *a)
{
    TCGv dest = tcg_temp_local_new();
    TCGv src1 = tcg_temp_local_new();
    TCGv res = tcg_temp_local_new();
    TCGLabel *loop = gen_new_label();
    TCGLabel *loop_limb = gen_new_label();
    TCGv i = tcg_temp_local_new(); // i va nous permettre de boucler sur les arg de la struct mpfr
    tcg_gen_movi_tl(i, 1);

    // On récupère l'adresse qui est dans le gpr dest et on l'avance de l'imm
    gen_get_gpr(dest, a->rs1);
    tcg_gen_addi_tl(dest, dest, a->imm);

    // On récupère le numéro du vpr source qui contient la data
    tcg_gen_movi_tl(src1, a->rs2);

    // On appelle le helper et le store qemu sur chaque elt du mpfr_t (sauf limb)
    gen_set_label(loop);
    gen_helper_fsp(res, cpu_env, src1, i);
    tcg_gen_qemu_st_i64(res, dest, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(dest, dest, 8);
    tcg_gen_brcondi_tl(TCG_COND_LT, i, 4, loop);

    // On s'occupe maintenant de store le dernier champ de la struct
    // On récupère d'abord le nombre de blocks limb que stocke le pointeur mp_limb_t *_mpfr_d
    gen_helper_fsp(res, cpu_env, src1, i);
    tcg_gen_addi_tl(i, i, 1);

    // Puis on boucle pour récupérer tous les blocks limb et les storer
    // nb représente la borne sup pour la boucle avec res qui vaut le nombre de blocks
    TCGv nb = tcg_temp_local_new();
    tcg_gen_movi_tl(nb, 5);
    tcg_gen_add_tl(nb, nb, res);

    gen_set_label(loop_limb);
    gen_helper_fsp(res, cpu_env, src1, i);
    tcg_gen_qemu_st_i64(res, dest, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(dest, dest, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop_limb);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(res);
    tcg_temp_free(i);
    tcg_temp_free(nb);
    return true;
}

static bool trans_fadd_p(DisasContext *ctx, arg_fadd_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fadd_p(cpu_env, dest, src1, src2, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fsub_p(DisasContext *ctx, arg_fsub_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fsub_p(cpu_env, dest, src1, src2, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(rm);
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

static bool trans_fmul_p(DisasContext *ctx, arg_fmul_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fmul_p(cpu_env, dest, src1, src2, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(rm);
    return true;
}

static bool trans_gmulu(DisasContext *ctx, arg_gmulu *a)
{
    return true;
}

static bool trans_fdiv_p(DisasContext *ctx, arg_fdiv_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fdiv_p(cpu_env, dest, src1, src2, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(rm);
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

static bool trans_fsqrt_p(DisasContext *ctx, arg_fsqrt_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fsqrt_p(cpu_env, dest, src1, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
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

static bool trans_feq_p(DisasContext *ctx, arg_feq_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_feq_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_fle_p(DisasContext *ctx, arg_fle_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fle_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
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

static bool trans_flt_p(DisasContext *ctx, arg_flt_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_flt_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_fmin_p(DisasContext *ctx, arg_fmin_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fmin_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_fmax_p(DisasContext *ctx, arg_fmax_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fmax_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
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

static bool trans_fcvt_d_b(DisasContext *ctx, arg_fcvt_d_b *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_fcvt_d_b(cpu_env, dest, src1, imm);

    gen_set_gpr(a->rgd, dest);
    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}

static bool trans_fcvt_dfpr_b(DisasContext *ctx, arg_fcvt_dfpr_b *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv imm  = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rgd);
    tcg_gen_movi_tl(src1, a->rgs1);
    tcg_gen_movi_tl(imm, a->imm);

    gen_helper_fcvt_dfpr_b(cpu_env, dest, src1, imm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(imm);
    return true;
}