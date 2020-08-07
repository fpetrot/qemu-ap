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


/* 32-bit */
static bool trans_fcvt_p_f(DisasContext *ctx, arg_fcvt_p_f *a)
{
    TCGv dest = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_f(cpu_env, dest, cpu_fpr[a->rs1], rm);

    tcg_temp_free(dest);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_f_p(DisasContext *ctx, arg_fcvt_f_p *a)
{
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_f_p(cpu_fpr[a->rd], cpu_env, src1, rm);

    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_p_w(DisasContext *ctx, arg_fcvt_p_w *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    gen_get_gpr(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_w(cpu_env, dest, src1, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_w_p(DisasContext *ctx, arg_fcvt_w_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_w_p(dest, cpu_env, src1, rm);
    gen_set_gpr(a->rd, dest);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_p_wu(DisasContext *ctx, arg_fcvt_p_wu *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    gen_get_gpr(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_wu(cpu_env, dest, src1, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_wu_p(DisasContext *ctx, arg_fcvt_wu_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_wu_p(dest, cpu_env, src1, rm);
    gen_set_gpr(a->rd, dest);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}


/* 64-bit */
static bool trans_flp(DisasContext *ctx, arg_flp *a)
{
    TCGv     data  = tcg_temp_local_new();
    TCGv     dest  = tcg_temp_local_new();
    TCGv     src1  = tcg_temp_local_new();
    TCGv     i     = tcg_temp_local_new();
    TCGv     nb    = tcg_temp_local_new();
    TCGv     limb  = tcg_temp_local_new();
    TCGLabel *loop = gen_new_label();

    // On récupère l'adresse de la data en mémoire
    gen_get_gpr(src1, a->rs1);
    tcg_gen_addi_tl(src1, src1, a->imm);

    // On récupère le n° du vpr où stocker la data
    tcg_gen_movi_tl(dest, a->rd);

    // Index qui caractérise l'accès
    tcg_gen_movi_tl(i, 1);
    // On récupère les 3 premiers champs, boucle déroulée
    // Ajout d'une dépendance sur i, incrémenté dans le helper, sinon qemu
    // semble ignorer ces appels, et ça devrait de plus optimiser le temps d'exécution
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(i, cpu_env, dest, i, data);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(i, cpu_env, dest, i, data);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(i, cpu_env, dest, i, data);
    tcg_gen_addi_tl(src1, src1, 8);

    // On s'occupe du pointeur sur les limbs
    // D'abord on récupère le nombre de blocks limb qu'on met dans la variable limb
    gen_helper_flp(limb, cpu_env, dest, i, data);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(nb, limb, 5); // nb est notre borne max pour la boucle sur les limbs

    // Puis on load les limbs
    gen_set_label(loop);
    tcg_gen_qemu_ld_i64(data, src1, ctx->mem_idx, MO_TEQ);
    gen_helper_flp(i, cpu_env, dest, i, data);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop);

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
    TCGv src1 = tcg_temp_local_new();
    TCGv src2 = tcg_temp_local_new();
    TCGv res  = tcg_temp_local_new();
    TCGv nb   = tcg_temp_local_new();
    TCGv i    = tcg_temp_local_new(); // i va nous permettre de boucler sur les arg de la struct mpfr
    TCGLabel *loop = gen_new_label();

    // On récupère l'adresse qui est dans le gpr src1 et on l'avance de l'imm
    gen_get_gpr(src1, a->rs1);
    tcg_gen_addi_tl(src1, src1, a->imm);

    // On récupère le numéro du vpr source qui contient la data
    tcg_gen_movi_tl(src2, a->rs2);

    tcg_gen_movi_tl(i, 1);
    // On appelle le helper et le store qemu sur chaque elt du mpfr_t (sauf limb)
    // On déroule la boucle, il n'y a que 3 tours
    gen_helper_fsp(res, cpu_env, src2, i);
    tcg_gen_qemu_st_i64(res, src1, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);
    gen_helper_fsp(res, cpu_env, src2, i);
    tcg_gen_qemu_st_i64(res, src1, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);
    gen_helper_fsp(res, cpu_env, src2, i);
    tcg_gen_qemu_st_i64(res, src1, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);

    // On s'occupe maintenant de store le dernier champ de la struct
    // On récupère d'abord le nombre de blocks limb que stocke le pointeur mp_limb_t *_mpfr_d
    gen_helper_fsp(res, cpu_env, src2, i);
    tcg_gen_addi_tl(i, i, 1);

    // Puis on boucle pour récupérer tous les blocks limb et les storer
    // nb représente la borne sup pour la boucle avec res qui vaut le nombre de blocks
    tcg_gen_movi_tl(nb, 5);
    tcg_gen_add_tl(nb, nb, res);

    gen_set_label(loop);
    gen_helper_fsp(res, cpu_env, src2, i);
    tcg_gen_qemu_st_i64(res, src1, ctx->mem_idx, MO_TEQ);
    tcg_gen_addi_tl(i, i, 1);
    tcg_gen_addi_tl(src1, src1, 8);
    tcg_gen_brcond_tl(TCG_COND_LT, i, nb, loop);

    tcg_temp_free(src1);
    tcg_temp_free(src2);
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

static bool trans_fmadd_p(DisasContext *ctx, arg_fmadd_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fmadd_p(cpu_env, dest, src1, src2, src3, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(src3);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fnmadd_p(DisasContext *ctx, arg_fnmadd_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fnmadd_p(cpu_env, dest, src1, src2, src3, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(src3);
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

static bool trans_fmsub_p(DisasContext *ctx, arg_fmsub_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fmsub_p(cpu_env, dest, src1, src2, src3, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(src3);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fnmsub_p(DisasContext *ctx, arg_fnmsub_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();
    TCGv src3 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);
    tcg_gen_movi_tl(src3, a->rs3);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fnmsub_p(cpu_env, dest, src1, src2, src3, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    tcg_temp_free(src3);
    tcg_temp_free(rm);
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

static bool trans_feq_p(DisasContext *ctx, arg_feq_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_feq_p(dest, cpu_env, src1, src2);
    gen_set_gpr(a->rd, dest);

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

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fle_p(dest, cpu_env, src1, src2);
    gen_set_gpr(a->rd, dest);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_flt_p(DisasContext *ctx, arg_flt_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_flt_p(dest, cpu_env, src1, src2);
    gen_set_gpr(a->rd, dest);

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

static bool trans_fcvt_p_d(DisasContext *ctx, arg_fcvt_p_d *a)
{
    TCGv dest = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_d(cpu_env, dest, cpu_fpr[a->rs1], rm);

    tcg_temp_free(dest);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_d_p(DisasContext *ctx, arg_fcvt_d_p *a)
{
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_d_p(cpu_fpr[a->rd], cpu_env, src1, rm);

    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_p_l(DisasContext *ctx, arg_fcvt_p_l *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    gen_get_gpr(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_l(cpu_env, dest, src1, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_l_p(DisasContext *ctx, arg_fcvt_l_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_l_p(dest, cpu_env, src1, rm);
    gen_set_gpr(a->rd, dest);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_p_lu(DisasContext *ctx, arg_fcvt_p_lu *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    gen_get_gpr(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_p_lu(cpu_env, dest, src1, rm);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fcvt_lu_p(DisasContext *ctx, arg_fcvt_lu_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv rm = tcg_temp_new();

    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(rm, a->rm);

    gen_helper_fcvt_lu_p(dest, cpu_env, src1, rm);
    gen_set_gpr(a->rd, dest);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(rm);
    return true;
}

static bool trans_fsgnj_p(DisasContext *ctx, arg_fsgnj_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fsgnj_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_fsgnjn_p(DisasContext *ctx, arg_fsgnjn_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fsgnjn_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}

static bool trans_fsgnjx_p(DisasContext *ctx, arg_fsgnjx_p *a)
{
    TCGv dest = tcg_temp_new();
    TCGv src1 = tcg_temp_new();
    TCGv src2 = tcg_temp_new();

    tcg_gen_movi_tl(dest, a->rd);
    tcg_gen_movi_tl(src1, a->rs1);
    tcg_gen_movi_tl(src2, a->rs2);

    gen_helper_fsgnjx_p(cpu_env, dest, src1, src2);

    tcg_temp_free(dest);
    tcg_temp_free(src1);
    tcg_temp_free(src2);
    return true;
}
