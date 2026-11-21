#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "geometria.h"
#include "rotinas_nbr8800.h"
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