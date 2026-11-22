#include "rotinas_viga_mista.h"
#include <cmath>
#include <algorithm>
#include <numbers>

const double PI = std::numbers::pi;

namespace viga_mista {

// ----------------------------------------------------------------------------
// 1. Largura de Influência e Largura Efetiva
// ----------------------------------------------------------------------------
double calcularLarguraInfluencia(const GeometriaVigaMista& geo) {
    if (geo.eh_viga_borda) {
        double d_prox = (geo.dist_esq_mm > 0.0) ? geo.dist_esq_mm : geo.dist_dir_mm;
        return geo.dist_borda_mm + (d_prox / 2.0);
    }
    return (geo.dist_esq_mm + geo.dist_dir_mm) / 2.0;
}

double calcularLarguraEfetiva(const GeometriaVigaMista& geo) {
    double l_oitavos = geo.L_vao_mm / 8.0;
    if (geo.eh_viga_borda) {
        double d_prox = (geo.dist_esq_mm > 0.0) ? geo.dist_esq_mm : geo.dist_dir_mm;
        return std::min(l_oitavos, geo.dist_borda_mm) + std::min(l_oitavos, d_prox / 2.0);
    }
    return std::min(l_oitavos, geo.dist_esq_mm / 2.0) + std::min(l_oitavos, geo.dist_dir_mm / 2.0);
}

// ----------------------------------------------------------------------------
// 2. Fase 1 - Perfil de Aço Isolado (ELU)
// ----------------------------------------------------------------------------
ResultadoFase1Perfil calcularFase1Perfil(const VigaMistaConfig& cfg) {
    ResultadoFase1Perfil res;
    double binf_m = calcularLarguraInfluencia(cfg.geometria) / 1000.0;

    // Cargas lineares da Fase 1
    double g_perfil = (cfg.acoes.peso_proprio_perfil_kN_m > 0.0)
        ? cfg.acoes.peso_proprio_perfil_kN_m
        : (cfg.perfil.Ag * 78.5e-6); // kN/m

    double g_laje_fresca = cfg.acoes.q_laje_fresco_kN_m2 * binf_m;
    res.g1_kN_m = g_perfil + g_laje_fresca;
    res.q1_kN_m = cfg.acoes.q_sobrecarga_const_kN_m2 * binf_m;

    res.qSd1_kN_m = cfg.acoes.gamma_g1 * res.g1_kN_m + cfg.acoes.gamma_q1 * res.q1_kN_m;

    double L_m = cfg.geometria.L_vao_mm / 1000.0;
    res.MSd1_kNm = (res.qSd1_kN_m * L_m * L_m) / 8.0;
    res.VSd1_kN = (res.qSd1_kN_m * L_m) / 2.0;

    // Momento Plástico
    res.Mpl_kNm = (cfg.perfil.Zx * cfg.aco.fy_MPa) * 1e-6;

    // Momento Crítico Elástico FLT
    double Cb = 1.00;
    double Lb_mm = cfg.geometria.Lb_mm;
    double E = cfg.aco.E_MPa;
    double Iy = cfg.perfil.Iy;
    double Cw = cfg.perfil.Cw;
    double It = cfg.perfil.It;

    double termo1 = (Cb * PI * PI * E * Iy) / (Lb_mm * Lb_mm);
    double termo2 = std::sqrt((Cw / Iy) * (1.0 + 0.039 * (It * Lb_mm * Lb_mm / Cw)));
    res.Mcr_kNm = (termo1 * termo2) * 1e-6;

    double gamma_a1 = 1.10;

    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        // NBR 8800:2024 Anexo D.2.1
        res.lambda_LT = std::sqrt(res.Mpl_kNm / res.Mcr_kNm);
        if (res.lambda_LT <= 0.40) {
            res.chi_LT = 1.0;
        } else if (res.lambda_LT <= 1.40) {
            res.chi_LT = 1.0 - 0.49 * (res.lambda_LT - 0.40);
        } else {
            res.chi_LT = 1.0 / (res.lambda_LT * res.lambda_LT);
        }
        res.MRd_FLT_kNm = (res.chi_LT * res.Mpl_kNm) / gamma_a1;
    } else {
        // NBR 8800:2008
        double lb = Lb_mm / cfg.perfil.ry;
        double lb_p = 1.76 * std::sqrt(E / cfg.aco.fy_MPa);
        double beta1 = (0.7 * cfg.aco.fy_MPa * cfg.perfil.Wx) / (E * It);
        double lb_r = ((1.38 * std::sqrt(Iy * It)) / (cfg.perfil.ry * It * beta1))
            * std::sqrt(1.0 + std::sqrt(1.0 + ((27.0 * Cw * beta1 * beta1) / Iy)));

        double Mr_kNm = (0.7 * cfg.aco.fy_MPa * cfg.perfil.Wx) * 1e-6;

        if (lb <= lb_p) {
            res.MRd_FLT_kNm = res.Mpl_kNm / gamma_a1;
        } else if (lb <= lb_r) {
            res.MRd_FLT_kNm = (Cb / gamma_a1) * (res.Mpl_kNm - (res.Mpl_kNm - Mr_kNm) * ((lb - lb_p) / (lb_r - lb_p)));
            res.MRd_FLT_kNm = std::min(res.MRd_FLT_kNm, res.Mpl_kNm / gamma_a1);
        } else {
            res.MRd_FLT_kNm = std::min(res.Mcr_kNm / gamma_a1, res.Mpl_kNm / gamma_a1);
        }
    }

    // FLM e FLA
    double lbf_p = 0.38 * std::sqrt(E / cfg.aco.fy_MPa);
    res.MRd_FLM_kNm = (cfg.perfil.lbf <= lbf_p) ? (res.Mpl_kNm / gamma_a1) : (0.9 * res.Mpl_kNm / gamma_a1);

    double lbw_p = 3.76 * std::sqrt(E / cfg.aco.fy_MPa);
    res.MRd_FLA_kNm = (cfg.perfil.lbw <= lbw_p) ? (res.Mpl_kNm / gamma_a1) : (0.9 * res.Mpl_kNm / gamma_a1);

    res.MRd_aco_kNm = std::min({res.MRd_FLT_kNm, res.MRd_FLM_kNm, res.MRd_FLA_kNm});

    // Cortante
    res.kv = (cfg.norma == NormaEdicao::NBR8800_2024) ? 5.34 : 5.0;
    double lbw_cort_p = 1.10 * std::sqrt(res.kv * E / cfg.aco.fy_MPa);
    res.Vpl_kN = (0.6 * cfg.aco.fy_MPa * cfg.perfil.d * cfg.perfil.tw) * 1e-3;

    if (cfg.perfil.lbw <= lbw_cort_p) {
        res.VRd_kN = res.Vpl_kN / gamma_a1;
    } else {
        res.VRd_kN = (lbw_cort_p / cfg.perfil.lbw) * (res.Vpl_kN / gamma_a1);
    }

    res.taxa_flexao = res.MSd1_kNm / res.MRd_aco_kNm;
    res.taxa_cortante = res.VSd1_kN / res.VRd_kN;
    res.aprovado_flexao = (res.taxa_flexao <= 1.00);
    res.aprovado_cortante = (res.taxa_cortante <= 1.00);

    return res;
}

// ----------------------------------------------------------------------------
// 3. Conexão de Cisalhamento - Stud Bolt (ELU)
// ----------------------------------------------------------------------------
ResultadoConectoresStud calcularConectoresStud(const VigaMistaConfig& cfg, double bef_mm) {
    ResultadoConectoresStud res;
    double d = cfg.conector.d_cs_mm;
    res.Acs_mm2 = (PI * d * d) / 4.0;

    // Coeficientes Rg e Rp
    if (cfg.laje.tipo == TipoLajeMista::MACICA) {
        res.Rg = 1.0;
        res.Rp = (cfg.norma == NormaEdicao::NBR8800_2024) ? 1.0 : 0.75;
    } else { // STEEL_DECK
        if (cfg.laje.orientacao == OrientacaoDeck::PERPENDICULAR_A_VIGA) {
            if (cfg.conector.n_por_linha == 1) res.Rg = 1.0;
            else if (cfg.conector.n_por_linha == 2) res.Rg = 0.85;
            else res.Rg = 0.70;

            double altura_livre = cfg.conector.h_cs_mm - cfg.laje.hF_deck_mm;
            res.Rp = (altura_livre >= 50.0) ? 0.75 : 0.60;
        } else { // PARALELA
            res.Rg = 1.0;
            double relacao = (cfg.laje.hF_deck_mm > 0.0) ? (cfg.laje.bF_deck_mm / cfg.laje.hF_deck_mm) : 2.0;
            res.Rp = (relacao >= 1.5) ? 0.75 : 0.60;
        }
    }

    double gamma_cs = 1.25;
    double fck = cfg.concreto.fck_MPa;
    double Ecs = cfg.concreto.Ecs_MPa;

    // Ruptura do concreto
    res.QRd1_concreto_kN = (res.Acs_mm2 * std::sqrt(fck * Ecs)) / (2.0 * gamma_cs * 1000.0);

    // Ruptura do aço do pino
    res.QRd2_aco_kN = (res.Rg * res.Rp * res.Acs_mm2 * cfg.conector.fu_MPa) / (gamma_cs * 1000.0);

    res.QRd_adotado_kN = std::min(res.QRd1_concreto_kN, res.QRd2_aco_kN);

    // Força horizontal rasante de cálculo
    double fyd = cfg.aco.fy_MPa / 1.10;
    double fcd = cfg.concreto.fck_MPa / 1.40;
    double eta_c = 1.00;

    double Fa = (cfg.perfil.Ag * fyd) * 1e-3; // kN
    double Fc = (0.85 * eta_c * fcd * bef_mm * cfg.laje.tc_mm) * 1e-3; // kN

    res.Fhd_interface_kN = std::min(Fa, Fc);

    // Quantidade por semi-vão e total
    res.n_semi_vao_calculado = static_cast<int>(std::ceil(res.Fhd_interface_kN / res.QRd_adotado_kN));
    res.n_total_calculado = 2 * res.n_semi_vao_calculado;

    // Espaçamento e quantidade adotada
    res.s_min_long_mm = 6.0 * d;
    double h_total_laje = cfg.laje.tc_mm + cfg.laje.hF_deck_mm;
    res.s_max_long_mm = (cfg.norma == NormaEdicao::NBR8800_2024 && cfg.laje.tipo == TipoLajeMista::MACICA)
        ? (8.0 * h_total_laje)
        : std::min(8.0 * h_total_laje, 800.0);

    if (cfg.conector.espacamento_adotado_mm > 0.0) {
        res.espacamento_adotado_mm = cfg.conector.espacamento_adotado_mm;
        int n_linhas = static_cast<int>(std::floor(cfg.geometria.L_vao_mm / res.espacamento_adotado_mm)) + 1;
        res.n_total_adotado = n_linhas * std::max(1, cfg.conector.n_por_linha);
    } else {
        res.n_total_adotado = res.n_total_calculado;
        int n_linhas = std::max(2, res.n_total_adotado / std::max(1, cfg.conector.n_por_linha));
        res.espacamento_adotado_mm = cfg.geometria.L_vao_mm / (n_linhas - 1);
    }

    // Grau de interação (relação entre a capacidade dos conectores adotados e a força rasante)
    double n_semi_adotado = res.n_total_adotado / 2.0;
    res.grau_interacao_eta = std::min(1.00, (n_semi_adotado * res.QRd_adotado_kN) / res.Fhd_interface_kN);

    // Grau mínimo de interação (L <= 10m -> 0.40; L > 10m -> 0.50)
    res.grau_interacao_minimo = (cfg.geometria.L_vao_mm <= 10000.0) ? 0.40 : 0.50;

    res.espacamento_atende = (res.espacamento_adotado_mm >= res.s_min_long_mm && res.espacamento_adotado_mm <= res.s_max_long_mm);
    res.grau_interacao_atende = (res.grau_interacao_eta >= res.grau_interacao_minimo);

    // Verificações geométricas do pino Stud Bolt (NBR 8800 L.8.1, L.6.6 e L.8.4)
    double h_min_base = 4.0 * d; // Comprimento mínimo de 4 diâmetros após soldado
    double h_min_deck = (cfg.laje.tipo == TipoLajeMista::STEEL_DECK) ? (cfg.laje.hF_deck_mm + 40.0) : 0.0; // Penetração mínima de 40mm na capa
    res.h_min_normativo_mm = std::max(h_min_base, h_min_deck);
    res.altura_atende = (cfg.conector.h_cs_mm >= res.h_min_normativo_mm);

    res.cobrimento_topo_mm = h_total_laje - cfg.conector.h_cs_mm;
    res.cobrimento_min_mm = 10.0; // Cobrimento superior mínimo da cabeça do pino (NBR 8800 L.8.1)
    res.cobrimento_atende = (res.cobrimento_topo_mm >= res.cobrimento_min_mm);

    res.d_max_mesa_mm = 2.5 * cfg.perfil.tf; // Diâmetro máximo do pino soldado à mesa (NBR 8800 L.8.1)
    res.diametro_mesa_atende = (d <= res.d_max_mesa_mm);

    return res;
}

// ----------------------------------------------------------------------------
// 4. Momento Resistente da Viga Mista (ELU Fase 2)
// ----------------------------------------------------------------------------
ResultadoMomentoMisto calcularMomentoMistoELU(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    const ResultadoConectoresStud& conn
) {
    ResultadoMomentoMisto res;
    res.bef_mm = bef_mm;
    res.binf_mm = binf_mm;

    double binf_m = binf_mm / 1000.0;
    double L_m = cfg.geometria.L_vao_mm / 1000.0;

    // Cargas da Fase 2
    res.g2_kN_m = (cfg.acoes.g_revestimento_kN_m2 * binf_m) + cfg.acoes.g_linear_adicional_kN_m;
    res.q2_kN_m = cfg.acoes.q_uso_kN_m2 * binf_m;

    double g_perfil = (cfg.acoes.peso_proprio_perfil_kN_m > 0.0)
        ? cfg.acoes.peso_proprio_perfil_kN_m
        : (cfg.perfil.Ag * 78.5e-6);
    double g1_kN_m = g_perfil + (cfg.acoes.q_laje_fresco_kN_m2 * binf_m);

    res.qSd2_kN_m = cfg.acoes.gamma_g2 * (g1_kN_m + res.g2_kN_m) + cfg.acoes.gamma_q2 * res.q2_kN_m;
    res.MSd2_kNm = (res.qSd2_kN_m * L_m * L_m) / 8.0;
    res.VSd2_kN = (res.qSd2_kN_m * L_m) / 2.0;

    res.fyd_MPa = cfg.aco.fy_MPa / 1.10;
    res.fcd_MPa = cfg.concreto.fck_MPa / 1.40;
    double eta_c = 1.00;

    res.Fa_kN = (cfg.perfil.Ag * res.fyd_MPa) * 1e-3;
    res.Fc_max_kN = (0.85 * eta_c * res.fcd_MPa * bef_mm * cfg.laje.tc_mm) * 1e-3;

    // Força máxima na laje considerando o grau de interação dos conectores
    res.Ccd_kN = std::min(conn.grau_interacao_eta * conn.Fhd_interface_kN, res.Fc_max_kN);

    double beta_vm = 1.00; // Viga biapoiada

    if (res.Ccd_kN >= res.Fa_kN) {
        // LNP na laje (interação completa)
        res.lnp_na_laje = true;
        res.Cad_kN = 0.0;
        res.a_bloco_concreto_mm = (res.Fa_kN * 1000.0) / (0.85 * eta_c * res.fcd_MPa * bef_mm);

        double braco_z = (cfg.perfil.d / 2.0) + cfg.laje.hF_deck_mm + cfg.laje.tc_mm - (res.a_bloco_concreto_mm / 2.0);
        res.MRd_VM_kNm = beta_vm * res.Fa_kN * braco_z * 1e-3;
    } else {
        // LNP no perfil de aço ou interação parcial
        res.lnp_na_laje = false;
        res.a_bloco_concreto_mm = (res.Ccd_kN * 1000.0) / (0.85 * eta_c * res.fcd_MPa * bef_mm);
        res.Cad_kN = 0.5 * (res.Fa_kN - res.Ccd_kN);

        double forca_mesa = (cfg.perfil.bf * cfg.perfil.tf * res.fyd_MPa) * 1e-3;

        if (res.Cad_kN <= forca_mesa) {
            // LNP na mesa superior do perfil (NBR 8800 L.6.3.1.1.1-b)
            res.yp_lnp_aco_mm = (res.Cad_kN * 1000.0) / (cfg.perfil.bf * res.fyd_MPa);
            double yc = res.yp_lnp_aco_mm / 2.0;
            double d_restante = cfg.perfil.d - res.yp_lnp_aco_mm;
            double yt = res.yp_lnp_aco_mm + d_restante / 2.0;

            double braco_conc = (cfg.perfil.d / 2.0) + cfg.laje.hF_deck_mm + cfg.laje.tc_mm - (res.a_bloco_concreto_mm / 2.0);
            double braco_aco = (cfg.perfil.d / 2.0) - yc;
            res.MRd_VM_kNm = beta_vm * (res.Ccd_kN * braco_conc + res.Cad_kN * braco_aco) * 1e-3;
        } else {
            // LNP na alma do perfil (NBR 8800 L.6.3.1.1.1-b e c)
            double forca_alma_comp = res.Cad_kN - forca_mesa;
            double h_wc = (forca_alma_comp * 1000.0) / (cfg.perfil.tw * res.fyd_MPa);
            res.yp_lnp_aco_mm = cfg.perfil.tf + h_wc;

            // CG da compressão no aço (mesa superior + trecho comprimido da alma)
            double y_ms = cfg.perfil.tf / 2.0;
            double y_wc = cfg.perfil.tf + h_wc / 2.0;
            double yc = (forca_mesa * y_ms + forca_alma_comp * y_wc) / res.Cad_kN;

            // CG da tração no aço (trecho tracionado da alma + mesa inferior)
            double h_wt = cfg.perfil.d - cfg.perfil.tf - res.yp_lnp_aco_mm;
            double forca_alma_trac = (cfg.perfil.tw * h_wt * res.fyd_MPa) * 1e-3;
            double forca_mesa_inf = forca_mesa;
            double Tad_total = forca_alma_trac + forca_mesa_inf;

            double z_mi = cfg.perfil.tf / 2.0;
            double z_wt = cfg.perfil.tf + h_wt / 2.0;
            double yt = (forca_mesa_inf * z_mi + forca_alma_trac * z_wt) / Tad_total; // medido a partir da borda inferior

            double braco_conc = (cfg.perfil.d - yt) + cfg.laje.hF_deck_mm + cfg.laje.tc_mm - (res.a_bloco_concreto_mm / 2.0);
            double braco_aco = (cfg.perfil.d - yt) - yc;
            res.MRd_VM_kNm = beta_vm * (res.Ccd_kN * braco_conc + res.Cad_kN * braco_aco) * 1e-3;
        }
    }

    // Cortante resistente na fase mista (dado pela alma do perfil)
    double gamma_a1 = 1.10;
    double kv = (cfg.norma == NormaEdicao::NBR8800_2024) ? 5.34 : 5.0;
    double lbw_cort_p = 1.10 * std::sqrt(kv * cfg.aco.E_MPa / cfg.aco.fy_MPa);
    double Vpl_kN = (0.6 * cfg.aco.fy_MPa * cfg.perfil.d * cfg.perfil.tw) * 1e-3;

    if (cfg.perfil.lbw <= lbw_cort_p) {
        res.VRd_VM_kN = Vpl_kN / gamma_a1;
    } else {
        res.VRd_VM_kN = (lbw_cort_p / cfg.perfil.lbw) * (Vpl_kN / gamma_a1);
    }

    // Verificação de tensões em construção não escorada (L.6.3.2)
    double M_Ga_S = (g1_kN_m * L_m * L_m) / 8.0;
    double M_L_S = ((res.g2_kN_m + res.q2_kN_m) * L_m * L_m) / 8.0;
    double n = cfg.aco.E_MPa / cfg.concreto.Ecs_MPa;
    double Ac = bef_mm * cfg.laje.tc_mm;
    double ya = cfg.perfil.d / 2.0;
    double yc = cfg.perfil.d + cfg.laje.hF_deck_mm + cfg.laje.tc_mm / 2.0;
    double ybar = (cfg.perfil.Ag * ya + (Ac / n) * yc) / (cfg.perfil.Ag + Ac / n);
    double Itr = cfg.perfil.Ix + cfg.perfil.Ag * std::pow(ya - ybar, 2)
        + (1.0 / n) * ((bef_mm * std::pow(cfg.laje.tc_mm, 3)) / 12.0 + Ac * std::pow(yc - ybar, 2));
    double Wef_inf = Itr / ybar;

    res.tensao_mesa_inferior_MPa = (M_Ga_S * 1e6 / cfg.perfil.Wx) + (M_L_S * 1e6 / Wef_inf);
    res.limite_tensao_MPa = cfg.aco.fy_MPa;
    res.tensao_servico_atende = (res.tensao_mesa_inferior_MPa <= res.limite_tensao_MPa);

    res.taxa_flexao = res.MSd2_kNm / res.MRd_VM_kNm;
    res.taxa_cortante = res.VSd2_kN / res.VRd_VM_kN;
    res.aprovado_flexao = (res.taxa_flexao <= 1.00);
    res.aprovado_cortante = (res.taxa_cortante <= 1.00);

    return res;
}

// ----------------------------------------------------------------------------
// 5. Armadura de Costura contra Fendilhamento (L.5.3)
// ----------------------------------------------------------------------------
ResultadoArmaduraCostura calcularArmaduraCostura(
    const VigaMistaConfig& cfg,
    double bef_mm,
    const ResultadoConectoresStud& conn
) {
    ResultadoArmaduraCostura res;
    double Lm_m = (cfg.geometria.L_vao_mm / 2.0) / 1000.0;
    double sigma_Q = std::min(conn.Fhd_interface_kN, (conn.n_total_adotado / 2.0) * conn.QRd_adotado_kN);

    // Superfície a-a: corte plano longitudinal
    res.nu_Sd_kN_m = sigma_Q / Lm_m;

    double fcd = cfg.concreto.fck_MPa / 1.40;
    double hf = cfg.laje.tc_mm;
    // Capacidade máxima de bielas comprimidas (NBR 8800 L.5.3.3)
    res.nu_Rd_max_kN_m = 0.20 * fcd * hf * 1000.0 * 1e-3;
    res.atende_bielas = (res.nu_Sd_kN_m <= res.nu_Rd_max_kN_m);

    // Armadura de costura necessária (cm²/m)
    double fyd = 500.0 / 1.15; // CA-50
    double As_mm2_m = (res.nu_Sd_kN_m * 1000.0) / fyd;
    res.As_costura_cm2_m = As_mm2_m / 100.0;

    return res;
}

// ----------------------------------------------------------------------------
// 6. Estados Limites de Serviço - Flechas com Fluência (L.5.2)
// ----------------------------------------------------------------------------
ResultadoFlechasVigaMista calcularFlechasVigaMista(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    double grau_interacao_eta
) {
    ResultadoFlechasVigaMista res;
    double E = cfg.aco.E_MPa;
    double Ia = cfg.perfil.Ix;
    double L = cfg.geometria.L_vao_mm;
    double binf_m = binf_mm / 1000.0;

    res.n_curta = E / cfg.concreto.Ecs_MPa;
    res.n_longa = 3.0 * res.n_curta;

    double Ac = bef_mm * cfg.laje.tc_mm;
    double ya = cfg.perfil.d / 2.0;
    double yc = cfg.perfil.d + cfg.laje.hF_deck_mm + cfg.laje.tc_mm / 2.0;

    // Curta duração
    double ybar_cd = (cfg.perfil.Ag * ya + (Ac / res.n_curta) * yc) / (cfg.perfil.Ag + Ac / res.n_curta);
    double Itr_cd = cfg.perfil.Ix + cfg.perfil.Ag * std::pow(ya - ybar_cd, 2)
        + (1.0 / res.n_curta) * ((bef_mm * std::pow(cfg.laje.tc_mm, 3)) / 12.0 + Ac * std::pow(yc - ybar_cd, 2));

    // Longa duração
    double ybar_ld = (cfg.perfil.Ag * ya + (Ac / res.n_longa) * yc) / (cfg.perfil.Ag + Ac / res.n_longa);
    double Itr_ld = cfg.perfil.Ix + cfg.perfil.Ag * std::pow(ya - ybar_ld, 2)
        + (1.0 / res.n_longa) * ((bef_mm * std::pow(cfg.laje.tc_mm, 3)) / 12.0 + Ac * std::pow(yc - ybar_ld, 2));

    res.Itr_cd_cm4 = Itr_cd / 1e4;
    res.Itr_ld_cm4 = Itr_ld / 1e4;

    // Inércias efetivas com interação parcial
    double Ief_cd = Ia + grau_interacao_eta * (Itr_cd - Ia);
    double Ief_ld = Ia + grau_interacao_eta * (Itr_ld - Ia);
    res.Ief_cd_cm4 = Ief_cd / 1e4;
    res.Ief_ld_cm4 = Ief_ld / 1e4;

    // Cargas de serviço (em N/mm)
    double g_perfil = (cfg.acoes.peso_proprio_perfil_kN_m > 0.0)
        ? cfg.acoes.peso_proprio_perfil_kN_m
        : (cfg.perfil.Ag * 78.5e-6);
    double g1_N_mm = (g_perfil + cfg.acoes.q_laje_fresco_kN_m2 * binf_m);
    double q1_N_mm = (cfg.acoes.q_sobrecarga_const_kN_m2 * binf_m);
    double g2_N_mm = (cfg.acoes.g_revestimento_kN_m2 * binf_m + cfg.acoes.g_linear_adicional_kN_m);
    double q2_N_mm = (cfg.acoes.q_uso_kN_m2 * binf_m);

    // Flechas
    res.flecha_fase1_mm = (5.0 * (g1_N_mm + q1_N_mm) * std::pow(L, 4)) / (384.0 * E * Ia);
    res.flecha_perm_ld_mm = (5.0 * g2_N_mm * std::pow(L, 4)) / (384.0 * E * Ief_ld);
    res.flecha_var_cd_mm = (5.0 * (1.0 - cfg.acoes.psi2) * q2_N_mm * std::pow(L, 4)) / (384.0 * E * Ief_cd);
    res.flecha_var_ld_mm = (5.0 * (cfg.acoes.psi2 * q2_N_mm) * std::pow(L, 4)) / (384.0 * E * Ief_ld);

    res.flecha_total_mm = res.flecha_fase1_mm + res.flecha_perm_ld_mm + res.flecha_var_cd_mm + res.flecha_var_ld_mm;
    res.flecha_ativa_mm = res.flecha_perm_ld_mm + res.flecha_var_cd_mm + res.flecha_var_ld_mm;

    // Limites de flecha normativos
    res.limite_total_mm = L / 250.0; // Adotado L/250 a L/500
    res.limite_ativa_mm = L / 350.0;

    res.taxa_flecha_total = res.flecha_total_mm / res.limite_total_mm;
    res.aprovado_flecha = (res.taxa_flecha_total <= 1.00);

    return res;
}

// ----------------------------------------------------------------------------
// 7. Estados Limites de Serviço - Vibração no Piso (NBR 8800:2024 Anexo I)
// ----------------------------------------------------------------------------
ResultadoVibracaoPiso calcularVibracaoPiso(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    double grau_interacao_eta
) {
    ResultadoVibracaoPiso res;
    double E = cfg.aco.E_MPa;
    double Ia = cfg.perfil.Ix;
    double L = cfg.geometria.L_vao_mm;
    double b = binf_mm;
    double binf_m = binf_mm / 1000.0;

    res.Ec_din_MPa = 38000.0;
    res.ndin = E / res.Ec_din_MPa;

    double Ac = bef_mm * cfg.laje.tc_mm;
    double ya = cfg.perfil.d / 2.0;
    double yc = cfg.perfil.d + cfg.laje.hF_deck_mm + cfg.laje.tc_mm / 2.0;
    double ybar_din = (cfg.perfil.Ag * ya + (Ac / res.ndin) * yc) / (cfg.perfil.Ag + Ac / res.ndin);

    double Itr_din = cfg.perfil.Ix + cfg.perfil.Ag * std::pow(ya - ybar_din, 2)
        + (1.0 / res.ndin) * ((bef_mm * std::pow(cfg.laje.tc_mm, 3)) / 12.0 + Ac * std::pow(yc - ybar_din, 2));

    double Ief_din = Ia + grau_interacao_eta * (Itr_din - Ia);
    res.Itr_din_cm4 = Itr_din / 1e4;
    res.Ief_din_cm4 = Ief_din / 1e4;

    // Carga de vibração
    double g_perfil = (cfg.acoes.peso_proprio_perfil_kN_m > 0.0)
        ? cfg.acoes.peso_proprio_perfil_kN_m
        : (cfg.perfil.Ag * 78.5e-6);
    double g1_kN_m = g_perfil + (cfg.acoes.q_laje_fresco_kN_m2 * binf_m);
    double g2_kN_m = (cfg.acoes.g_revestimento_kN_m2 * binf_m + cfg.acoes.g_linear_adicional_kN_m);
    double q2_kN_m = (cfg.acoes.q_uso_kN_m2 * binf_m);

    res.wvib_kN_m = (g1_kN_m + g2_kN_m) + cfg.acoes.psi_vib * q2_kN_m;

    // Rigidez transversal da laje
    res.Is_laje_mm3 = std::pow(cfg.laje.tc_mm, 3) / (12.0 * res.ndin);

    // Deflexão da viga e flexibilidade da laje
    res.delta_viga_mm = (5.0 * res.wvib_kN_m * std::pow(L, 4)) / (384.0 * E * Ief_din);
    res.delta_laje_mm = (res.wvib_kN_m * std::pow(b, 3)) / (384.0 * E * res.Is_laje_mm3);
    res.delta_din_total_mm = res.delta_viga_mm + res.delta_laje_mm;

    // Frequência natural
    res.f0_Hz = 18.0 / std::sqrt(res.delta_din_total_mm);

    // Massa modal Modo A (I.6.3)
    double relacao_rigidez = (res.Is_laje_mm3 * b) / Ief_din;
    double Bb_rig = cfg.acoes.kb_vibracao * (L / 1000.0) * std::pow(relacao_rigidez, 0.25);
    double Bb_lim = (2.0 / 3.0) * cfg.acoes.np_vaos * (cfg.acoes.Lp_vao_principal_mm / 1000.0);
    res.Bb_m = std::min(Bb_rig, Bb_lim);

    double m_kg_m2 = (res.wvib_kN_m * 1000.0) / (9.81 * binf_m);
    res.massa_modal_kg = 0.5 * m_kg_m2 * (L / 1000.0) * res.Bb_m;

    // Aceleração de pico (NBR 8800:2024 Item I.6.4: ap = 0.55 * P0 * exp(-0.35 * f0) / (2 * M * beta))
    res.beta_amort = cfg.acoes.beta_amortecimento;
    double P0 = 750.0; // Peso da pessoa (N)
    res.ap_m_s2 = (0.55 * P0 * std::exp(-0.35 * res.f0_Hz)) / (2.0 * res.massa_modal_kg * res.beta_amort);
    res.ap_percent_g = (res.ap_m_s2 / 9.81) * 100.0;

    // Limites de conforto da Tabela I.3 da NBR 8800:2024
    if (cfg.acoes.ocupacao == "Garagens") {
        res.ap_lim_m_s2 = 0.150;
        res.R_max_limite = 24.0;
    } else if (cfg.acoes.ocupacao == "Residências" || cfg.acoes.ocupacao == "Escritórios") {
        res.ap_lim_m_s2 = 0.050;
        res.R_max_limite = 8.0;
    } else {
        res.ap_lim_m_s2 = 0.150;
        res.R_max_limite = 24.0;
    }

    res.R_fator_resposta = (res.ap_m_s2 * 0.707) / 0.005; // Fator de resposta aproximado
    res.taxa_aceleracao = res.ap_m_s2 / res.ap_lim_m_s2;
    res.aprovado_vibracao = (res.taxa_aceleracao <= 1.00);

    return res;
}

// ----------------------------------------------------------------------------
// 8. Verificador Consolidado
// ----------------------------------------------------------------------------
RelatorioVigaMista verificar_viga_mista(const VigaMistaConfig& cfg) {
    RelatorioVigaMista rel;
    rel.aprovado_global = true;

    double bef_mm = calcularLarguraEfetiva(cfg.geometria);
    double binf_mm = calcularLarguraInfluencia(cfg.geometria);

    // 1. Fase 1
    rel.fase1 = calcularFase1Perfil(cfg);
    rel.itens.push_back({
        "Flexão Fase 1 (Perfil Isolado)",
        "Flambagem Lateral com Torção e Flambagens Locais",
        rel.fase1.MSd1_kNm,
        rel.fase1.MRd_aco_kNm,
        "kN.m",
        rel.fase1.taxa_flexao,
        rel.fase1.aprovado_flexao
    });
    rel.itens.push_back({
        "Cortante Fase 1 (Perfil Isolado)",
        "Resistência da Alma ao Cortante",
        rel.fase1.VSd1_kN,
        rel.fase1.VRd_kN,
        "kN",
        rel.fase1.taxa_cortante,
        rel.fase1.aprovado_cortante
    });

    // 2. Conectores de Cisalhamento
    rel.conectores = calcularConectoresStud(cfg, bef_mm);
    bool conn_geom_ok = rel.conectores.altura_atende && rel.conectores.cobrimento_atende && rel.conectores.diametro_mesa_atende;
    rel.itens.push_back({
        "Conectores - Grau de Interação",
        "Grau de interação eta atingido vs mínimo normativo",
        rel.conectores.grau_interacao_minimo,
        rel.conectores.grau_interacao_eta,
        "-",
        rel.conectores.grau_interacao_minimo / std::max(0.001, rel.conectores.grau_interacao_eta),
        rel.conectores.grau_interacao_atende && rel.conectores.espacamento_atende
    });
    rel.itens.push_back({
        "Conectores - Altura e Cobrimento",
        "Altura mínima exigida e cobrimento superior da cabeça",
        rel.conectores.h_min_normativo_mm,
        cfg.conector.h_cs_mm,
        "mm",
        rel.conectores.h_min_normativo_mm / std::max(1.0, cfg.conector.h_cs_mm),
        conn_geom_ok
    });

    // 3. Fase 2
    rel.fase2 = calcularMomentoMistoELU(cfg, bef_mm, binf_mm, rel.conectores);
    rel.itens.push_back({
        "Flexão Fase 2 (Viga Mista)",
        "Momento Resistente Plástico da Seção Mista",
        rel.fase2.MSd2_kNm,
        rel.fase2.MRd_VM_kNm,
        "kN.m",
        rel.fase2.taxa_flexao,
        rel.fase2.aprovado_flexao
    });
    rel.itens.push_back({
        "Cortante Fase 2 (Viga Mista)",
        "Resistência ao Cortante em Serviço",
        rel.fase2.VSd2_kN,
        rel.fase2.VRd_VM_kN,
        "kN",
        rel.fase2.taxa_cortante,
        rel.fase2.aprovado_cortante
    });

    // 4. Armadura de Costura
    rel.costura = calcularArmaduraCostura(cfg, bef_mm, rel.conectores);

    // 5. ELS Flechas
    rel.flechas = calcularFlechasVigaMista(cfg, bef_mm, binf_mm, rel.conectores.grau_interacao_eta);
    rel.itens.push_back({
        "ELS Flecha Total",
        "Flecha de Serviço Considerando Fluência (L/350)",
        rel.flechas.flecha_total_mm,
        rel.flechas.limite_total_mm,
        "mm",
        rel.flechas.taxa_flecha_total,
        rel.flechas.aprovado_flecha
    });
    rel.itens.push_back({
        "ELS Flecha Ativa (Pós-Cura)",
        "Flecha Adicional após a Construção do Piso",
        rel.flechas.flecha_ativa_mm,
        rel.flechas.limite_ativa_mm,
        "mm",
        rel.flechas.flecha_ativa_mm / std::max(0.001, rel.flechas.limite_ativa_mm),
        rel.flechas.flecha_ativa_mm <= rel.flechas.limite_ativa_mm
    });

    // Opcional: cálculo dinâmico interno preservado no objeto rel
    rel.vibracao = calcularVibracaoPiso(cfg, bef_mm, binf_mm, rel.conectores.grau_interacao_eta);

    for (const auto& item : rel.itens) {
        if (!item.aprovado) {
            rel.aprovado_global = false;
            break;
        }
    }

    return rel;
}

} // namespace viga_mista
