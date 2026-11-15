#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "rotinas_vmp.h"
#include "rotinas_nbr8800.h"
#include <cmath>

TEST_CASE("Rotina 1: Cálculo propriedades seção transformada tempo infinito")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();

    auto res = nbr8800::calcularSecaoTransformada(perfil, laje, concreto, aco, 3, -50, false);

    // Comparação percentual para absorver arredondamentos intermediários da dissertação (tolerância de 0.1%)
    ASSERT_PERCENT(res.n_relacao_modular, 0.04473, 0.05);
    ASSERT_PERCENT(res.btr_mm, 13.42e1, 0.1);
    ASSERT_PERCENT(res.Atr_mm2, 246.96e2, 0.1);
    ASSERT_PERCENT(res.ytr_mm, 351.3, 0.1);
    ASSERT_NEAR(res.ay_mm, 130.0, 0.01);
    ASSERT_PERCENT(res.Itr_mm4, 49091.19e4, 0.1);
    ASSERT_PERCENT(res.Wtr_inf_mm3, 1397.41e3, 0.1);
    ASSERT_PERCENT(res.Wtr_sup_mm3, 3592.87e3, 0.1);
}

TEST_CASE("Rotina 2: Força máxima protensão")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();

    double Lv_mm = 12000;
    double Lb_mm = 3000;
    double Cb = 1.06;
    double kx = 1.0;
    double ky = 1.0;
    double gamma_a1 = 1.10;
    double gamma_p = 0.9;
    double beta = 1.1;
    double psi_1 = 0.6;
    double MSd_g_kNm = 293.58;
    double MSd_q_kNm = 270.00;

    auto resNRdPerfil = nbr8800::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);
    auto resMRdPerfil = nbr8800::calcularMomentoResistentePerfil(perfil, aco, Lb_mm, Cb, gamma_a1);
    auto resSecaoTransformada = nbr8800::calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, cabo.ep, false);

    auto res = nbr8800::calcularEstimativaP0max(perfil, aco, concreto, cabo, 
        resNRdPerfil.NRd_kN, resMRdPerfil.MRd_kNm, MSd_g_kNm, MSd_q_kNm, gamma_p, 
        beta, psi_1, resSecaoTransformada.ytr_mm, resSecaoTransformada.Atr_mm2, 
        resSecaoTransformada.Wtr_inf_mm3, resSecaoTransformada.Wtr_sup_mm3);
    
    ASSERT_PERCENT(std::abs(res.P0_max_1), 912.57, 0.05);
    ASSERT_PERCENT(std::abs(res.P0_max_2), 1431.63, 0.05);
    ASSERT_PERCENT(res.P0_max_kN, 912.57, 0.05);
}

TEST_CASE("Rotina 3.1: Momento positivo VMP")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();

    double Ap_mm2 = 4.2e2;
    double sigma_pd_MPa = 92.38;
    double gamma_a1 = 1.1;
    double gamma_c = 1.4;

    auto res = nbr8800::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
    Ap_mm2, sigma_pd_MPa, gamma_a1, gamma_c);

    ASSERT_PERCENT(res.lbw, 38.98, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.1);
    ASSERT_PERCENT(res.Cc_max_kN, 7103.53, 0.05);
    ASSERT_PERCENT(res.T_aco_kN, 2272.86, 0.05);
    ASSERT_PERCENT(res.T_cabo_kN, 387.99, 0.05);
    ASSERT_PERCENT(res.a_linha_neutra_mm, 4.87e1, 0.05);
    ASSERT_PERCENT(res.MRd_VMP_kNm, 807.86, 0.05);
}

TEST_CASE("Rotina 3.2: Momento positivo VMP semicompacta")
{
    PerfilI perfil = perfil_w360x57_8();
    perfil.tw = 3.0;                                                                                                                                             
    perfil.lbw = 102.67;     // > 90.53 e <= 137.24                                                                                                              
    perfil.Ag = 5501.8;      // Área atualizada com tw = 3.0 mm                                                                                                  
    perfil.Ix = 14321.2e4;   // Inércia atualizada com tw = 3.0 mm 

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();

    double Ap_mm2 = 4.2e2;
    double sigma_pd_MPa = 92.38;
    double gamma_a1 = 1.1;
    double gamma_c = 1.4;

    auto res = nbr8800::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
    Ap_mm2, sigma_pd_MPa, gamma_a1, gamma_c);

    ASSERT_PERCENT(res.lbw, 102.67, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01); 
    ASSERT_PERCENT(res.MRd_VMP_kNm, 395.01, 0.5);
}

TEST_CASE("Rotina 3.3: Momento negativo VMP")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();
    ArmacaoLaje armacaoLaje = laje_5cm_m();

    cabo.Ap = 4.2e2;
    cabo.sigma_Pd = 92.38e1;
    double L_mm = 3000;
    double a_vigas_mm = 3000;
    double alpha_viga = 3.0;

    double MSd_neg = 138.68;
    double MSd_pos = 798.01;

    auto res = nbr8800::calcularMomentoNegativoVMP(perfil, aco, concreto, laje, cabo, 
        armacaoLaje, MSd_neg, MSd_pos, L_mm, a_vigas_mm, alpha_viga);

    ASSERT_PERCENT(res.MRd_neg_kNm, 354.49, 0.05);
    ASSERT_PERCENT(res.Mrd_dist_kNm, 354.49, 0.05);
    ASSERT_PERCENT(res.MRd_VMP_kNm, 354.49, 0.05);
}

TEST_CASE("Rotina 4: Cálculo da armadura de protensão")
{
    const CaboProtensao cabo = cabo_190RB();
    TipoProtensao protensao = TipoProtensao::PRE_TRACAO;
    double P0_kN = 400;

    auto res = nbr8800::calcularArmaduraProtensao(cabo, protensao, P0_kN);

    ASSERT_PERCENT(res.sigma_adm_MPa, 1425.8, 2);
    ASSERT_PERCENT(res.Ap_calc, 2.805e2, 2);
    ASSERT_NEAR(res.n_cordoalhas, 3, 0.01);
    ASSERT_PERCENT(res.Ap_real, 4.2e2, 2);
}

TEST_CASE("Rotina 6: Incremento força protensão traçado reto")
{
    PerfilI perfil = perfil_w360x57_8();
    CaboProtensao cabo = cabo_190RB();

    double q_kN_m = 9.75; // qconc = bef * tc / 1e4 * 25 [kN/m]
    double Eg_MPa = 200000; // Módulo elasticidade aço
    double Ep_MPa = 200000; // Módulo elasticidade cabo protensão
    double Lv_mm = 12000; // Comprimento vão biapoiado
    double epCG_mm = (perfil.d / 2) + cabo.ep; // Excentricidade em relação ao CG cabo protensão (negativa se acima da mesa inferior)
    double Ix_mm4 = 16143e4;
    double Ag_mm2 = 72.5e2;
    double Ap_mm2 = 4.2e2;

    auto res = nbr8800::calcularIncrementoDpTraçadoReto(q_kN_m, Eg_MPa, Ep_MPa, 
        Lv_mm, epCG_mm, Ix_mm4, Ag_mm2, Ap_mm2);

    ASSERT_PERCENT(res.delta_P_kN, 35.65, 0.05);
}

TEST_CASE("Rotina G: Cálculo do momento de inércia de laje por metro (exemplo 1)")
{
    Laje laje = laje_macica_130();
    ArmacaoLaje armacaoLaje = laje_5cm_m();
    Concreto concreto = concreto_c30();
    Aco aco = aco_a572_gr50();

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    double I_laje = nbr8800::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

    ASSERT_PERCENT(I_laje, 431.15e4, 0.05);
}

TEST_CASE("Rotina G: Cálculo do momento de inércia de laje por metro (exemplo 2)")
{
    Laje laje = laje_mista_130();
    ArmacaoLaje armacaoLaje = laje_5cm_m();
    Concreto concreto = concreto_c30();
    Aco aco = aco_a572_gr50();

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    auto I_laje = nbr8800::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

    ASSERT_PERCENT(I_laje, 438.86e4, 0.05);
}

TEST_CASE("Rotina H: Cálculo da tensão de cálculo no cabo para ELU da VMP")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();

    cabo.Ap = 4.2e2; // mm²

    double P_atuante_kN = 448.84;
    double Lv_mm = 12000;
    
    auto secao_tr = nbr8800::calcularSecaoTransformada(perfil, laje, concreto, 
        aco, 1.0, cabo.ep, true);

    auto res = nbr8800::calcularTensaoCaboELU(perfil, laje, cabo, concreto, 
        P_atuante_kN, secao_tr.ytr_mm, secao_tr.Atr_mm2, secao_tr.Itr_mm4, Lv_mm);

    ASSERT_PERCENT(res.epsilon_pre, 4.8e-3, 0.2);
    ASSERT_PERCENT(res.ro_p, 9.26e-3, 0.05);
    ASSERT_PERCENT(res.delta_sigma_MPa, 102.39, 0.05);
    ASSERT_PERCENT(res.sigma_pd_MPa, 92.38e1, 0.2);
}