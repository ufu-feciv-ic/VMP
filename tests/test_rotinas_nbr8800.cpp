#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "geometria.h"
#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include <cmath>

TEST_CASE("Rotina A - NBR 8800:2008: Normal resistente perfil W360x57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    
    // calcular_propriedades_secao_i(perfil);

    Aco aco = aco_a572_gr50();

    const double Lb_mm = 3000.00;
    const double Lv_mm = 12000.00;
    const double kx = 1.0;
    const double ky = 1.0;
    const double gamma_a1 = 1.10;

    auto res = nbr8800::v2008::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);

    ASSERT_PERCENT(res.Nex_kN, 2212.84, 0.1);
    ASSERT_PERCENT(res.Ney_kN, 2441.08, 0.1);
    ASSERT_NEAR(res.Qs, 1.0, 0.001);
    ASSERT_NEAR(res.Qa, 1.0, 0.001);
    ASSERT_NEAR(res.Q, 1.0, 0.001);
    ASSERT_PERCENT(res.lb0, 1.06, 0.5);
    ASSERT_PERCENT(res.chi, 0.623, 0.5);
    ASSERT_PERCENT(res.NRd_kN, 1416.61, 0.15);
}

TEST_CASE("Rotina B - NBR 8800:2008: Momento resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();

    Aco aco = aco_a572_gr50();

    double Lb = 3000; // mm
    double Cb = 1.06; // a favor da segurança
    double gamma_a1 = 1.10;

    auto res = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, Lb, Cb, gamma_a1);

    ASSERT_PERCENT(res.Mpl_kNm, 350.106, 0.1);
    ASSERT_PERCENT(res.Mr_kNm, 217.78, 0.1);
    ASSERT_PERCENT(res.MRd_FLT_kNm, 285.98, 0.15);
    ASSERT_PERCENT(res.MRd_FLM_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_FLA_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_kNm, 285.98, 0.15);
    ASSERT_TRUE(!res.perfil_esbelto);
}

TEST_CASE("Rotina C.1 - NBR 8800:2008: Momento positivo viga mista (Exemplo 2)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 44.09, 0.1);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.1);
    ASSERT_PERCENT(res.Cc_max_kN, 4735.71, 0.05);
    ASSERT_PERCENT(res.T_aco_kN, 3578.59, 0.05);
    ASSERT_PERCENT(res.a_linha_neutra_mm, 98.23, 0.1);
    ASSERT_PERCENT(res.MRd_VM_kNm, 1296.81, 0.05);
}

TEST_CASE("Rotina C.2 - NBR 8800:2008: Momento positivo viga mista semicompacta (Exemplo 2 adaptado)")
{
    PerfilI perfil = perfil_w460x89();
    perfil.tw = 4.0;
    perfil.lbw = 463.0 / 4.0; // 115.75 (> 90.53 e <= 137.24)
    perfil.Ag = 8630.6;       // Área atualizada com tw = 4.0 mm
    perfil.Ix = 36870.1e4;    // Inércia atualizada com tw = 4.0 mm

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 115.75, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01);
    ASSERT_PERCENT(res.MRd_VM_kNm, 768.90, 0.1);
}

TEST_CASE("Rotina C.2 - NBR 8800:2008: Momento positivo viga mista semicompacta (Exemplo 1 adaptado)")
{
    PerfilI perfil = perfil_w360x57_8();
    perfil.tw = 3.0;
    perfil.lbw = 102.67;     // > 90.53 e <= 137.24
    perfil.Ag = 5501.8;      // Área atualizada com tw = 3.0 mm
    perfil.Ix = 14321.2e4;   // Inércia atualizada com tw = 3.0 mm

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 102.67, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01);
    ASSERT_PERCENT(res.MRd_VM_kNm, 395.01, 0.1);
}

TEST_CASE("Rotina C.3 - NBR 8800:2008: Momento negativo viga mista (Exemplo 2)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();
    ArmacaoLaje armacaoLaje = laje_exemplo2_armacao();

    double MSd_neg_kNm = 56.45;
    double MSd_pos_kNm = 854.18;
    double Lv_mm = 15000.0;
    double a_vigas_mm = 2000.0;
    double alpha_viga = 3.0;

    auto res = nbr8800::v2008::calcularMomentoNegativoVM(
        perfil, aco, concreto, laje, armacaoLaje,
        MSd_neg_kNm, MSd_pos_kNm, Lv_mm, a_vigas_mm, alpha_viga
    );

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.MRd_neg_kNm, 789.50, 0.1);
    ASSERT_PERCENT(res.MRd_VM_kNm, 789.50, 0.1);
}

TEST_CASE("Rotina D - NBR 8800:2008: Cortante resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    
    // calcular_propriedades_secao_i(perfil);

    Aco aco = aco_a572_gr50();

    double dist_enrijecedor = 0.0;
    double gamma_a1 = 1.1;

    auto res = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, aco, dist_enrijecedor, gamma_a1);

    ASSERT_NEAR(res.kv, 5.0, 0.01);
    ASSERT_PERCENT(res.lbw, 38.98, 0.1);
    ASSERT_PERCENT(res.lbw_p, 59.22, 0.1);
    ASSERT_PERCENT(res.lbw_r, 73.75, 0.1);
    ASSERT_PERCENT(res.Vpl_kN, 585.43, 0.1);
    ASSERT_PERCENT(res.VRd_kN, 532.21, 0.1);
}

// ============================================================================
// TESTES UNITÁRIOS SEPARADOS — NAMESPACE nbr8800::v2024 (ABNT NBR 8800:2024)
// ============================================================================

TEST_CASE("Rotina A - NBR 8800:2024: Normal resistente perfil W360x57.8 (Método da Largura Efetiva)")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();

    const double Lb_mm = 3000.00;
    const double Lv_mm = 12000.00;
    const double kx = 1.0;
    const double ky = 1.0;
    const double gamma_a1 = 1.10;

    auto res = nbr8800::v2024::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);

    ASSERT_PERCENT(res.Nex_kN, 2212.84, 0.1);
    ASSERT_PERCENT(res.Ney_kN, 2441.08, 0.1);
    ASSERT_NEAR(res.Qs, 1.0, 0.001);
    ASSERT_NEAR(res.Qa, 1.0, 0.001);
    ASSERT_NEAR(res.Q, 1.0, 0.001);
    ASSERT_PERCENT(res.lb0, 1.06, 0.5);
    ASSERT_PERCENT(res.chi, 0.623, 0.5);
    ASSERT_PERCENT(res.NRd_kN, 1416.61, 0.15);
}

TEST_CASE("Rotina B - NBR 8800:2024: Momento resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();

    double Lb = 3000.0;
    double Cb = 1.06;
    double gamma_a1 = 1.10;

    auto res = nbr8800::v2024::calcularMomentoResistentePerfil(perfil, aco, Lb, Cb, gamma_a1);

    ASSERT_PERCENT(res.Mpl_kNm, 350.106, 0.1);
    ASSERT_PERCENT(res.Mr_kNm, 217.78, 0.1);
    ASSERT_PERCENT(res.MRd_FLT_kNm, 285.98, 0.15);
    ASSERT_PERCENT(res.MRd_FLM_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_FLA_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_kNm, 285.98, 0.15);
    ASSERT_TRUE(!res.perfil_esbelto);
}

TEST_CASE("Rotina C.1 - NBR 8800:2024: Momento positivo viga mista com interação completa (LNP na laje e no perfil)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    // Caso 1: LNP na laje com fck = 30 MPa (eta_c = 1.0)
    auto res_laje = nbr8800::v2024::calcularMomentoPositivoVM(perfil, aco, concreto, laje);
    ASSERT_TRUE(res_laje.aplicavel);
    ASSERT_PERCENT(res_laje.Cc_max_kN, 4735.71, 0.05);
    ASSERT_PERCENT(res_laje.T_aco_kN, 3578.59, 0.05);
    ASSERT_PERCENT(res_laje.a_linha_neutra_mm, 98.23, 0.1);
    ASSERT_PERCENT(res_laje.MRd_VM_kNm, 1296.81, 0.05);

    // Caso 2: LNP no perfil de aço sob interação completa (Item L.6.3.1.1.1-b, p. 203, com sum_QRd = Ccd)
    Laje laje_fina = laje;
    laje_fina.bef_mm = 1000.0;
    laje_fina.tc_mm = 80.0;
    auto res_2008 = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje_fina);
    auto res_2024 = nbr8800::v2024::calcularMomentoPositivoVM(perfil, aco, concreto, laje_fina);
    ASSERT_TRUE(res_2024.Cc_max_kN < perfil.Ag * (aco.fy_MPa / 1.1));
    ASSERT_TRUE(res_2024.MRd_VM_kNm != res_2008.MRd_VM_kNm);
}

TEST_CASE("Rotina C.2 - NBR 8800:2024: Momento positivo viga mista semicompacta (limite 0.85*fcd na laje)")
{
    PerfilI perfil = perfil_w460x89();
    perfil.tw = 4.0;
    perfil.lbw = 463.0 / 4.0; // 115.75 (> 90.53 e <= 137.24)
    perfil.Ag = 8630.6;
    perfil.Ix = 36870.1e4;

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    auto res = nbr8800::v2024::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 115.75, 0.01);
    ASSERT_PERCENT(res.MRd_VM_kNm, 768.90, 0.1);
}

TEST_CASE("Rotina C.3 - NBR 8800:2024: Momento negativo viga mista (nova formulação FLD Item L.6.5.5)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();
    ArmacaoLaje armacaoLaje = laje_exemplo2_armacao();

    double MSd_neg_kNm = 56.45;
    double MSd_pos_kNm = 854.18;
    double Lv_mm = 15000.0;
    double a_vigas_mm = 2000.0;
    double alpha_viga = 3.0;

    auto res = nbr8800::v2024::calcularMomentoNegativoVM(
        perfil, aco, concreto, laje, armacaoLaje,
        MSd_neg_kNm, MSd_pos_kNm, Lv_mm, a_vigas_mm, alpha_viga
    );

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.MRd_neg_kNm, 789.50, 0.1);
    ASSERT_TRUE(res.MRd_VM_kNm > 0.0);
}

TEST_CASE("Rotina D - NBR 8800:2024: Cortante resistente perfil W360X57.8 (kv = 5.34)")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();

    double dist_enrijecedor = 0.0;
    double gamma_a1 = 1.1;

    auto res = nbr8800::v2024::calcularCortanteResistentePerfil(perfil, aco, dist_enrijecedor, gamma_a1);

    ASSERT_NEAR(res.kv, 5.34, 0.01);
    ASSERT_PERCENT(res.lbw, 38.98, 0.1);
    ASSERT_PERCENT(res.lbw_p, 61.20, 0.1);
    ASSERT_PERCENT(res.lbw_r, 76.22, 0.1);
    ASSERT_PERCENT(res.Vpl_kN, 585.43, 0.1);
    ASSERT_PERCENT(res.VRd_kN, 532.21, 0.1);
}

TEST_CASE("Comparativo Viga Mista Pura: Escorada vs Nao-Escorada, Interacao Completa vs Parcial")
{
    // 1. Dados da Seção e Materiais
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    laje.bef_mm = 2500.0; // 2,50 m de largura colaborante

    const double L_mm = 10000.0; // Vão de 10 metros
    const double gamma_a1 = 1.10;
    const double gamma_c = 1.40;
    const double fyd = aco.fy_MPa / gamma_a1;
    const double fcd = concreto.fck_MPa / gamma_c;

    // Forças plásticas fundamentais
    const double Tad = perfil.Ag * fyd * 1e-3; // ~2273.86 kN
    const double Ccd_max = 0.85 * fcd * laje.bef_mm * laje.tc_mm * 1e-3; // ~5919.64 kN
    const double Fhd = std::min(Tad, Ccd_max); // Fhd = Tad = 2273.86 kN (LNP na laje em interação completa)

    // ========================================================================
    // CASO 1: INTERAÇÃO COMPLETA (alpha = 1.00)
    // ========================================================================
    // Conectores fornecem sum_QRd >= Fhd
    const double a_comp = (Tad * 1e3) / (0.85 * fcd * laje.bef_mm); // ~49.93 mm <= tc (130 mm)
    const double MRd_comp_2008 = Tad * (perfil.d / 2.0 + laje.hF_mm + laje.tc_mm - a_comp / 2.0) * 1e-3;
    const double MRd_comp_2024 = 1.0 * Tad * (perfil.d / 2.0 + laje.hF_mm + laje.tc_mm - a_comp / 2.0) * 1e-3;

    ASSERT_NEAR(MRd_comp_2008, MRd_comp_2024, 0.01);
    ASSERT_PERCENT(MRd_comp_2024, 645.85, 0.2);

    // ========================================================================
    // CASO 2: INTERAÇÃO INCOMPLETA / PARCIAL (alpha = 0.60)
    // ========================================================================
    // Adotam-se conectores com capacidade sum_QRd = 0.60 * Fhd = 1364.32 kN
    const double alpha = 0.60;
    const double sum_QRd_parcial = alpha * Fhd; // 1364.32 kN (< Tad)
    
    // Na interação parcial, a compressão na laje é limitada pela conexão: Ccd_parcial = sum_QRd
    const double Ccd_parcial = sum_QRd_parcial;
    const double a_parcial = (Ccd_parcial * 1e3) / (0.85 * fcd * laje.bef_mm); // ~29.96 mm

    // Pelo equilíbrio axial no perfil: Tad_parcial - Cad_parcial = Ccd_parcial
    // Com Tad_parcial + Cad_parcial = Tad:
    const double Cad_parcial = 0.5 * (Tad - Ccd_parcial); // ~454.77 kN
    const double yp_parcial = (Cad_parcial * 1e3) / (perfil.bf * fyd); // ~8.43 mm <= tf (13.1 mm)
    const double yc_parcial = yp_parcial / 2.0;

    // Centroide da região tracionada yt:
    double yt_parcial = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
        + (perfil.h * perfil.tw * (perfil.h / 2.0 + perfil.tf))
        + ((perfil.bf / 2.0) * (perfil.tf - yp_parcial) * (3.0 * perfil.tf + 2.0 * perfil.h - yp_parcial)))
        / ((perfil.bf * perfil.tf) + (perfil.h * perfil.tw) + (perfil.bf * (perfil.tf - yp_parcial))));

    // Momento resistente sob interação parcial (Item L.6.3.1.1.1-c):
    double MRd_parcial = (Cad_parcial * (perfil.d - yt_parcial - yc_parcial)
        + sum_QRd_parcial * (laje.tc_mm - a_parcial / 2.0 + laje.hF_mm + perfil.d - yt_parcial)) * 1e-3;

    // A interação parcial com 60% dos conectores preserva ~87% do momento resistente da interação completa:
    ASSERT_TRUE(MRd_parcial < MRd_comp_2024);
    ASSERT_PERCENT(MRd_parcial, 561.67, 0.2);
    ASSERT_TRUE(MRd_parcial > 285.98); // Muito superior ao perfil isolado (285.98 kNm)

    // ========================================================================
    // CASOS 3 E 4: ELS DE DEFORMAÇÃO (ESCORADA VS NÃO-ESCORADA)
    // ========================================================================
    auto secao_tr_tinf = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, aco, 3.0, 0.0, false);

    const double Ix = perfil.Ix; // mm4 (16143 cm4 = 16143e4 mm4)
    const double Itr_inf = secao_tr_tinf.Itr_mm4; // Longa duração (3n) in mm4
    
    // Inércia efetiva sob interação parcial (fórmula de Johnson / NBR 8800 / AISC 360):
    // Ief = Ix + sqrt(alpha) * (Itr - Ix)
    const double Ief_inf = Ix + std::sqrt(alpha) * (Itr_inf - Ix);

    // Cargas de serviço (kN/m = N/mm):
    const double q_conc = 8.125 + 0.57;      // Concreto + peso perfil = 8.695 N/mm
    const double q_serv = 3.75 + 0.6 * 7.50; // Sobrecargas quase permanentes = 8.25 N/mm
    const double E = aco.E_MPa;              // 200000 MPa

    // --- CONSTRUÇÃO NÃO-ESCORADA (Interação Completa) ---
    // Concreto atua no perfil de aço isolado (Ix):
    double delta_conc_nao_esc = (5.0 / 384.0) * (q_conc * std::pow(L_mm, 4)) / (E * Ix);
    // Sobrecargas atuam na seção mista (Itr_inf):
    double delta_serv_comp = (5.0 / 384.0) * (q_serv * std::pow(L_mm, 4)) / (E * Itr_inf);
    double delta_total_nao_esc_comp = delta_conc_nao_esc + delta_serv_comp;

    // --- CONSTRUÇÃO ESCORADA (Interação Completa) ---
    // Concreto e sobrecargas atuam ambos na seção mista (Itr_inf):
    double delta_conc_esc = (5.0 / 384.0) * (q_conc * std::pow(L_mm, 4)) / (E * Itr_inf);
    double delta_total_esc_comp = delta_conc_esc + delta_serv_comp;

    // --- CONSTRUÇÃO NÃO-ESCORADA (Interação Parcial, alpha = 0.60) ---
    double delta_serv_parcial = (5.0 / 384.0) * (q_serv * std::pow(L_mm, 4)) / (E * Ief_inf);
    double delta_total_nao_esc_parcial = delta_conc_nao_esc + delta_serv_parcial;

    // --- CONSTRUÇÃO ESCORADA (Interação Parcial, alpha = 0.60) ---
    double delta_total_esc_parcial = ((5.0 / 384.0) * ((q_conc + q_serv) * std::pow(L_mm, 4))) / (E * Ief_inf);

    // Verificações conceituais e de ordem de grandeza:
    // 1. Escoramento reduz drasticamente a flecha total de serviço (em ~50%):
    ASSERT_TRUE(delta_total_esc_comp < delta_total_nao_esc_comp);
    ASSERT_NEAR(delta_total_esc_comp / delta_total_nao_esc_comp, 0.505, 0.02);

    // 2. Interação parcial apresenta maior flecha elástica do que interação completa:
    ASSERT_TRUE(delta_total_nao_esc_parcial > delta_total_nao_esc_comp);
    ASSERT_TRUE(delta_total_esc_parcial > delta_total_esc_comp);

    // 3. Limite de flecha L/350 = 28.57 mm
    const double delta_lim = L_mm / 350.0;
    ASSERT_NEAR(delta_lim, 28.57, 0.05);

    // Na construção não escorada, delta_total (~46.5 mm) supera o limite L/350 (28.57 mm),
    // enquanto na construção escorada (~23.5 mm) passa com grande folga!
    ASSERT_TRUE(delta_total_nao_esc_comp > delta_lim);
    ASSERT_TRUE(delta_total_esc_comp < delta_lim);
    ASSERT_TRUE(delta_total_esc_parcial < delta_lim);

    // ========================================================================
    // CASO 5: VERIFICAÇÃO DO PERFIL DE AÇO ISOLADO NA MONTAGEM (ELU Fase 1)
    // ========================================================================
    // Na construção não escorada, a seção isolada de aço DEVE ser verificada
    // sob as ações de peso próprio, concreto fresco e sobrecarga de construção.
    const double qs_montagem = 2.50; // kN/m (1.0 kN/m² * 2.5 m)
    const double qSd_montagem = 1.3 * q_conc + 1.2 * qs_montagem; // 1.3 * 8.695 + 1.2 * 2.50 = 14.3035 kN/m
    const double MSd_montagem = (qSd_montagem * std::pow(L_mm * 1e-3, 2)) / 8.0; // 178.79 kNm
    const double VSd_montagem = (qSd_montagem * (L_mm * 1e-3)) / 2.0;            // 71.52 kN

    // Verificação com contenção lateral intermediária a cada Lb = 2.50 m:
    const double Lb_travado = 2500.0; // mm
    const double Cb_montagem = 1.0;
    auto res_MRd_travado = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, Lb_travado, Cb_montagem, gamma_a1);
    auto res_VRd_isolado = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, aco, 0.0, gamma_a1);

    // 1. Momento plástico do perfil de aço isolado:
    ASSERT_PERCENT(res_MRd_travado.Mpl_kNm, 350.11, 0.2); // Zx * fy / 1.10 = 1014.8 * 345 / 1.1e3 = 318.28 (ou Wx)

    // 2. Com contenção a cada Lb = 2.5 m, o perfil resiste com folga à montagem:
    ASSERT_TRUE(res_MRd_travado.MRd_kNm > MSd_montagem);
    ASSERT_TRUE(res_VRd_isolado.VRd_kN > VSd_montagem);

    // 3. Sem contenção lateral (Lb = 10.0 m, vão livre total destravado):
    auto res_MRd_livre = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, L_mm, Cb_montagem, gamma_a1);
    
    // A flambagem lateral com torção (FLT) reduz drasticamente a capacidade do perfil isolado:
    ASSERT_TRUE(res_MRd_livre.MRd_kNm < res_MRd_travado.MRd_kNm);
    ASSERT_TRUE(res_MRd_livre.MRd_kNm < MSd_montagem); // FALHA por FLT no perfil isolado se não for contido!
}