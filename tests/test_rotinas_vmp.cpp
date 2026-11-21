#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "rotinas_vmp.h"
#include "rotinas_nbr8800.h"
#include "verificador.h"
#include <cmath>

TEST_CASE("Rotina 1: Cálculo propriedades seção transformada tempo infinito")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();

    auto res = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, aco, 3, -50, false);

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

    auto resNRdPerfil = nbr8800::v2008::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);
    auto resMRdPerfil = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, Lb_mm, Cb, gamma_a1);
    auto resSecaoTransformada = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, cabo.ep, false);

    auto res = nbr8800::v2008::calcularEstimativaP0max(perfil, aco, concreto, cabo, 
        resNRdPerfil.NRd_kN, resMRdPerfil.MRd_kNm, MSd_g_kNm, MSd_q_kNm, gamma_p, 
        beta, psi_1, resSecaoTransformada.ytr_mm, resSecaoTransformada.Atr_mm2, 
        resSecaoTransformada.Wtr_inf_mm3, resSecaoTransformada.Wtr_sup_mm3);
    
    ASSERT_PERCENT(std::abs(res.P0_max_1), 912.57, 0.05);
    ASSERT_PERCENT(std::abs(res.P0_max_2), 1431.63, 0.05);
    ASSERT_PERCENT(res.P0_max_kN, 912.57, 0.05);
}

TEST_CASE("Rotina 3.1 - NBR 8800:2008: Momento positivo VMP")
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

    auto res = nbr8800::v2008::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
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

TEST_CASE("Rotina 3.1 - NBR 8800:2024: Momento positivo VMP (interação completa pressuposta)")
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

    auto res = nbr8800::v2024::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
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

TEST_CASE("Rotina 3.2 - NBR 8800:2008: Momento positivo VMP semicompacta")
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

    auto res = nbr8800::v2008::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
    Ap_mm2, sigma_pd_MPa, gamma_a1, gamma_c);

    ASSERT_PERCENT(res.lbw, 102.67, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01); 
    ASSERT_PERCENT(res.MRd_VMP_kNm, 395.01, 0.5);
}

TEST_CASE("Rotina 3.2 - NBR 8800:2024: Momento positivo VMP semicompacta (limite 0.85*fcd na laje)")
{
    PerfilI perfil = perfil_w360x57_8();
    perfil.tw = 3.0;
    perfil.lbw = 102.67;     // > 90.53 e <= 137.24
    perfil.Ag = 5501.8;
    perfil.Ix = 14321.2e4;

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();
    CaboProtensao cabo = cabo_190RB();

    double Ap_mm2 = 4.2e2;
    double sigma_pd_MPa = 92.38;
    double gamma_a1 = 1.1;
    double gamma_c = 1.4;

    auto res = nbr8800::v2024::calcularMomentoPostivoVMP(perfil, aco, concreto, laje, cabo, 
        Ap_mm2, sigma_pd_MPa, gamma_a1, gamma_c);

    ASSERT_PERCENT(res.lbw, 102.67, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01);
    ASSERT_PERCENT(res.MRd_VMP_kNm, 395.01, 0.5);
}

TEST_CASE("Rotina 3.3 - NBR 8800:2008: Momento negativo VMP")
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

    auto res = nbr8800::v2008::calcularMomentoNegativoVMP(perfil, aco, concreto, laje, cabo, 
        armacaoLaje, MSd_neg, MSd_pos, L_mm, a_vigas_mm, alpha_viga);

    ASSERT_PERCENT(res.MRd_neg_kNm, 354.49, 0.05);
    ASSERT_PERCENT(res.Mrd_dist_kNm, 354.49, 0.05);
    ASSERT_PERCENT(res.MRd_VMP_kNm, 354.49, 0.05);
}

TEST_CASE("Rotina 3.3 - NBR 8800:2024: Momento negativo VMP (nova formulação FLD Item L.6.5.5)")
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

    auto res = nbr8800::v2024::calcularMomentoNegativoVMP(perfil, aco, concreto, laje, cabo, 
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

    auto res = nbr8800::v2008::calcularArmaduraProtensao(cabo, protensao, P0_kN);

    ASSERT_PERCENT(res.sigma_adm_MPa, 1425.8, 2);
    ASSERT_PERCENT(res.Ap_calc, 2.805e2, 2);
    ASSERT_NEAR(res.n_cordoalhas, 3, 0.01);
    ASSERT_PERCENT(res.Ap_real, 4.2e2, 2);
}

TEST_CASE("Rotina 5: Perda imediata por acomodação de ancoragem (Exemplo 1, t = 0)")
{
    CaboProtensao cabo = cabo_190RB();
    cabo.Ap = 4.2e2;

    double P0_kN = 400;
    double Lv_mm = 12000;

    auto res = nbr8800::v2008::calcularPerdaAncoragemTracadoReto(cabo, P0_kN, Lv_mm);

    ASSERT_PERCENT(res.delta_P_ancoragem_kN, 42.0, 0.01);                                                                                                     
    ASSERT_PERCENT(res.P1_kN, 358.0, 0.01);
}

TEST_CASE("Rotina 5: Perda progressiva por relaxação do aço (Exemplo 1, t = infinito)")                                                                          
{                                                                                                                                                                
    CaboProtensao cabo = cabo_190RB();
    cabo.Ap = 4.2e2;
    
    // No Exemplo 1 (pág. 209), a força atuante ao final da fase 2 é P2 = 448.42 kN                                                                              
    double P2_kN = 448.42;                                                                                                                                    
    
    auto res = nbr8800::v2008::calcularPerdaRelaxacaoTracadoReto(cabo, P2_kN);

    // Na dissertação (pág. 209):
    // sigma_p0 = 106.76 kN/cm² (1067.6 MPa)
    // psi_1000 = 0.00946 (0.946%)
    // P3 = 448.42 * (1 - 2.5 * 0.00946) = 437.81 kN
    ASSERT_PERCENT(res.sigma_P0_MPa, 1067.6, 0.1);                                                                                                            
    ASSERT_PERCENT(res.psi_1000, 0.00946, 0.2);                                                                                                                  
    ASSERT_PERCENT(res.P3_kN, 437.81, 0.05);                                                                                                                     
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

    double delta_P_kN = nbr8800::v2008::calcularIncrementoDpTracadoReto(q_kN_m, Eg_MPa, Ep_MPa, 
        Lv_mm, epCG_mm, Ix_mm4, Ag_mm2, Ap_mm2);

    ASSERT_PERCENT(delta_P_kN, 35.65, 0.05);
}

TEST_CASE("Rotina G: Cálculo do momento de inércia de laje por metro (exemplo 1)")
{
    Laje laje = laje_macica_130();
    ArmacaoLaje armacaoLaje = laje_5cm_m();
    Concreto concreto = concreto_c30();
    Aco aco = aco_a572_gr50();

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    double I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

    ASSERT_PERCENT(I_laje, 431.15e4, 0.05);
}

TEST_CASE("Rotina G: Cálculo do momento de inércia de laje por metro (exemplo 2)")
{
    Laje laje = laje_mista_130();
    ArmacaoLaje armacaoLaje = laje_5cm_m();
    Concreto concreto = concreto_c30();
    Aco aco = aco_a572_gr50();

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    auto I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

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
    
    auto secao_tr = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, 
        aco, 1.0, cabo.ep, true);

    auto res = nbr8800::v2008::calcularTensaoCaboELU(perfil, laje, cabo, concreto, 
        P_atuante_kN, secao_tr.ytr_mm, secao_tr.Atr_mm2, secao_tr.Itr_mm4, Lv_mm);

    ASSERT_PERCENT(res.epsilon_pre, 4.8e-3, 0.2);
    ASSERT_PERCENT(res.ro_p, 9.26e-3, 0.05);
    ASSERT_PERCENT(res.delta_sigma_MPa, 102.39, 0.05);
    ASSERT_PERCENT(res.sigma_pd_MPa, 92.38e1, 0.2);
}

TEST_CASE("Verificador Completo: Exemplo 1 (Pré-tração, Traçado Reto, X seções)")
{
    VigaVMP viga;
    viga.perfil = perfil_w360x57_8();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_macica_130();
    viga.armacao_laje = laje_5cm_m();
    viga.cabo = cabo_190RB();

    viga.L = 12000.0;
    viga.Lb = 3000.0;
    viga.a_vigas_mm = 3000.0;
    viga.alpha_viga = 3.0;
    viga.Cb_fase1 = 1.06;
    viga.P0_adotado = 400.0;
    viga.num_secoes = 5; // x = 0, 3000, 6000, 9000, 12000 mm

    viga.cargas.q1_perfil = 0.56;       // 72.5 / 10000 * 77 = 0.56 kN/m
    viga.cargas.q2_concreto = 9.75;     // 3.0 * 0.13 * 25 = 9.75 kN/m
    viga.cargas.qs_construcao = 3.00;   // 1 kN/m² * 3.0 m = 3.0 kN/m
    viga.cargas.q3_revestimento = 6.00; // 2 kN/m² * 3.0 m = 6.0 kN/m
    viga.cargas.q4_uso = 15.00;         // 5 kN/m² * 3.0 m = 15.0 kN/m
    viga.cargas.psi1 = 0.6;
    viga.cargas.psi2 = 0.4;

    RelatorioVMP rel = verificar_viga_vmp(viga);

    // Malha de 5 seções geradas ao longo da viga [0, L]
    ASSERT_TRUE(rel.malha_secoes.size() == 5);
    ASSERT_NEAR(rel.malha_secoes[0].x_mm, 0.0, 0.01);
    ASSERT_NEAR(rel.malha_secoes[1].x_mm, 3000.0, 0.01);
    ASSERT_NEAR(rel.malha_secoes[2].x_mm, 6000.0, 0.01);
    ASSERT_NEAR(rel.malha_secoes[3].x_mm, 9000.0, 0.01);
    ASSERT_NEAR(rel.malha_secoes[4].x_mm, 12000.0, 0.01);

    // Tabela 12: Força máxima de protensão e armadura
    ASSERT_PERCENT(rel.compressao_aco.NRd_kN, 1416.61, 0.15);
    ASSERT_PERCENT(rel.flexao_aco.MRd_kNm, 285.98, 0.15);
    ASSERT_PERCENT(rel.estimativa_p0.P0_max_kN, 912.57, 0.1);
    ASSERT_PERCENT(rel.armadura_protensao.Ap_real, 420.0, 0.1);

    // Tabela 13: ELU 1 (Construção - Pré-Tração)
    const auto* f1_meio = rel.fase1_elu.secaoMeioVao();
    const auto* f1_apoio = rel.fase1_elu.secaoApoioEsquerdo();
    ASSERT_TRUE(f1_meio != nullptr && f1_apoio != nullptr);
    ASSERT_PERCENT(f1_meio->P_atuante_kN, 404.62, 0.05);
    ASSERT_PERCENT(f1_meio->ep_LNE_mm, 129.0, 0.05);
    ASSERT_PERCENT(f1_meio->MSd_pos_kNm, 259.09, 0.1);
    ASSERT_PERCENT(f1_meio->MSd_neg_kNm, 46.98, 0.1);
    ASSERT_PERCENT(f1_meio->VSd_max_kN, 102.00, 0.1);
    ASSERT_PERCENT(f1_meio->VRd_kN, 532.21, 0.1);
    ASSERT_NEAR(f1_apoio->MSd_pos_kNm, 0.0, 0.01);

    // Tabela 14: ELU 2 (VMP t = 0)
    const auto* f2_meio = rel.fase2_elu.secaoMeioVao();
    const auto* f2_quarto = &rel.fase2_elu.secoes[1]; // x = L/4 = 3000 mm
    ASSERT_TRUE(f2_meio != nullptr);
    ASSERT_PERCENT(f2_meio->P_atuante_kN, 448.41, 0.05);
    ASSERT_PERCENT(f2_meio->ep_LNE_mm, 343.3, 0.05);
    ASSERT_PERCENT(f2_meio->MSd_pos_kNm, 789.01, 0.1);
    ASSERT_PERCENT(f2_meio->MSd_neg_kNm, 138.68, 0.15);
    ASSERT_PERCENT(f2_meio->VSd_max_kN, 263.00, 0.1);
    ASSERT_PERCENT(f2_meio->MRd_pos_kNm, 807.86, 0.1);
    ASSERT_PERCENT(f2_meio->MRd_neg_kNm, 354.49, 0.1);
    // Em x = L/4, o momento positivo parabólico deve ser 75% do momento máximo no meio do vão
    ASSERT_PERCENT(f2_quarto->MSd_pos_kNm, 0.75 * 789.01, 0.1);

    // Tabela 15: ELU 3 (VMP t = infinito)
    const auto* f3_meio = rel.fase3_elu.secaoMeioVao();
    ASSERT_TRUE(f3_meio != nullptr);
    ASSERT_PERCENT(f3_meio->P_atuante_kN, 437.81, 0.05);
    ASSERT_PERCENT(f3_meio->ep_LNE_mm, 301.3, 0.05);
    ASSERT_PERCENT(f3_meio->MSd_pos_kNm, 789.01, 0.1);
    ASSERT_PERCENT(f3_meio->MSd_neg_kNm, 118.75, 0.15);
    ASSERT_PERCENT(f3_meio->VSd_max_kN, 263.00, 0.1);
    ASSERT_PERCENT(f3_meio->MRd_pos_kNm, 802.53, 0.1);
    ASSERT_PERCENT(f3_meio->MRd_neg_kNm, 358.60, 0.1);
    ASSERT_TRUE(rel.aprovado_global);
}

TEST_CASE("Verificador Completo: Pós-tração (Fase 1 sem protensão e Fases 2/3 com protensão)")
{
    VigaVMP viga;
    viga.perfil = perfil_w460x89();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_mista_exemplo2();
    viga.armacao_laje = laje_exemplo2_armacao();
    viga.cabo = cabo_exemplo2();
    viga.cabo.tracado = CABO_RETO; // Poligonal em stand-by; valida fluxo de pós-tração

    viga.L = 15000.0;
    viga.Lb = 3000.0;
    viga.a_vigas_mm = 2000.0;
    viga.alpha_viga = 3.0;
    viga.num_secoes = 5;

    viga.cargas.q1_perfil = 0.88;       // 114.1 / 10000 * 77 = 0.88 kN/m
    viga.cargas.q2_concreto = 6.81;     // (13 + 0.63) * 200 * 25 / 10000 = 6.81 kN/m
    viga.cargas.qs_construcao = 2.00;   // 1 kN/m² * 2.0 m = 2.0 kN/m
    viga.cargas.q3_revestimento = 4.00; // 2 kN/m² * 2.0 m = 4.0 kN/m
    viga.cargas.q4_uso = 10.00;         // 5 kN/m² * 2.0 m = 10.0 kN/m
    viga.cargas.psi1 = 0.6;
    viga.cargas.psi2 = 0.4;

    RelatorioVMP rel = verificar_viga_vmp(viga);

    // Tabela 20: Estimativa P0,max na Pós-tração (apenas P0,6118 governa)
    ASSERT_PERCENT(rel.estimativa_p0.P0_max_kN, 1165.50, 0.1);
    ASSERT_PERCENT(rel.armadura_protensao.Ap_real, 840.0, 0.1);

    // Tabela 21: ELU 1 na Pós-tração (Viga Mista sem protensão via Rotina C.1)
    const auto* f1_meio = rel.fase1_elu.secaoMeioVao();
    ASSERT_TRUE(f1_meio != nullptr);
    ASSERT_NEAR(f1_meio->P_atuante_kN, 0.0, 0.001);
    ASSERT_PERCENT(f1_meio->MSd_pos_kNm, 348.66, 0.1);
    ASSERT_PERCENT(f1_meio->MRd_pos_kNm, 1296.81, 0.1);
    ASSERT_PERCENT(f1_meio->VSd_max_kN, 92.97, 0.1);
    ASSERT_PERCENT(f1_meio->VRd_kN, 914.84, 0.1);

    // ELU 2 e ELU 3: protensão aplicada na Fase 2 e relaxada na Fase 3
    const auto* f2_meio = rel.fase2_elu.secaoMeioVao();
    const auto* f3_meio = rel.fase3_elu.secaoMeioVao();
    ASSERT_TRUE(f2_meio != nullptr && f3_meio != nullptr);
    ASSERT_TRUE(f2_meio->P_atuante_kN > 1000.0);
    ASSERT_TRUE(f3_meio->P_atuante_kN < f2_meio->P_atuante_kN);
    ASSERT_PERCENT(f2_meio->MSd_pos_kNm, 854.18, 0.1);
    ASSERT_PERCENT(f2_meio->MRd_pos_kNm, 1878.46, 0.1);
    ASSERT_PERCENT(f2_meio->MRd_neg_kNm, 379.45, 0.1);
    ASSERT_PERCENT(f3_meio->MRd_pos_kNm, 1878.46, 0.1);
    ASSERT_PERCENT(f3_meio->MRd_neg_kNm, 379.45, 0.1);

    // No Exemplo 2 com P0 = P0_max (1165.5 kN), ELU 1 e ELU 3 passam,
    // mas no ELU 2 o momento negativo da protensão (~451 kN.m em cabo reto / 437.8 kN.m em poligonal)
    // supera MRd,VMP- (379.45 kN.m), sendo reprovado na Fase 2:
    ASSERT_TRUE(rel.fase1_elu.aprovado_fase);
    ASSERT_TRUE(!rel.fase2_elu.aprovado_fase);
    ASSERT_TRUE(rel.fase3_elu.aprovado_fase);
}

TEST_CASE("Comparativo ABNT NBR 8800:2008 vs ABNT NBR 8800:2024")
{
    VigaVMP viga;
    viga.perfil = perfil_w360x57_8();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_macica_130();
    viga.armacao_laje = laje_5cm_m();
    viga.cabo = cabo_190RB();
    viga.L = 12000.0;
    viga.Lb = 3000.0;
    viga.a_vigas_mm = 3000.0;
    viga.alpha_viga = 3.0;
    viga.Cb_fase1 = 1.06;
    viga.P0_adotado = 400.0;
    viga.num_secoes = 5;
    viga.cargas.q1_perfil = 0.56;
    viga.cargas.q2_concreto = 9.75;
    viga.cargas.qs_construcao = 3.00;
    viga.cargas.q3_revestimento = 6.00;
    viga.cargas.q4_uso = 15.00;

    viga.norma = NBR8800_2008;
    RelatorioVMP rel_2008 = verificar_viga_vmp(viga);

    viga.norma = NBR8800_2024;
    RelatorioVMP rel_2024 = verificar_viga_vmp(viga);

    // Para perfil compacto W360x57.8, NRd e MRd coincidem, e kv passa de 5.0 (2008) para 5.34 (2024)
    ASSERT_NEAR(rel_2008.compressao_aco.NRd_kN, rel_2024.compressao_aco.NRd_kN, 0.01);
    ASSERT_NEAR(rel_2008.cortante_aco.kv, 5.0, 0.01);
    ASSERT_NEAR(rel_2024.cortante_aco.kv, 5.34, 0.01);
    ASSERT_TRUE(rel_2024.cortante_aco.lbw_p > rel_2008.cortante_aco.lbw_p);
    ASSERT_TRUE(rel_2024.aprovado_global);

    // 1. Momento positivo resistente sob interação completa com LNP no perfil de aço (Item L.6.3.1.1.1-b, p. 203):
    //    Na NBR 8800:2024, a equação para LNP no perfil de aço sob interação completa substitui Cad*(d - yt - yc)
    //    por sum_QRd*(d - yt - yc), onde sum_QRd = Ccd = 0.85 * eta_c * fcd * b * tc.
    Laje laje_fina = viga.laje;
    laje_fina.bef_mm = 1000.0;
    laje_fina.tc_mm = 80.0; // Força LNP no perfil de aço (Ccd < Tad + Tpd)
    auto m_lnp_aco_2008 = nbr8800::v2008::calcularMomentoPostivoVMP(
        viga.perfil, viga.aco, viga.concreto, laje_fina, viga.cabo, 420.0, 92.38, 1.1, 1.4
    );
    auto m_lnp_aco_2024 = nbr8800::v2024::calcularMomentoPostivoVMP(
        viga.perfil, viga.aco, viga.concreto, laje_fina, viga.cabo, 420.0, 92.38, 1.1, 1.4
    );
    ASSERT_TRUE(m_lnp_aco_2024.MRd_VMP_kNm != m_lnp_aco_2008.MRd_VMP_kNm);

    // 2. Fator de fragilidade do concreto eta_c = (40/fck)^(1/3) <= 1.0 para fck = 50 MPa na NBR 8800:2024
    Concreto c50 = viga.concreto;
    c50.fck_MPa = 50.0;
    auto m_pos_2008_c50 = nbr8800::v2008::calcularMomentoPostivoVMP(
        viga.perfil, viga.aco, c50, viga.laje, viga.cabo, 420.0, 92.38, 1.1, 1.4
    );
    auto m_pos_2024_c50 = nbr8800::v2024::calcularMomentoPostivoVMP(
        viga.perfil, viga.aco, c50, viga.laje, viga.cabo, 420.0, 92.38, 1.1, 1.4
    );
    ASSERT_TRUE(m_pos_2024_c50.Cc_max_kN < m_pos_2008_c50.Cc_max_kN);
    ASSERT_TRUE(m_pos_2024_c50.MRd_VMP_kNm < m_pos_2008_c50.MRd_VMP_kNm);

    // 3. Momento negativo resistente com nova formulação de FLD e Tabela L.1 (C_dist) da NBR 8800:2024
    ASSERT_NEAR(nbr8800::v2008::calcularCdist(1.0), 11.0, 0.01);
    ASSERT_NEAR(nbr8800::v2024::calcularCdist(1.0), 1.53, 0.01);
    auto m_neg_2008 = nbr8800::v2008::calcularMomentoNegativoVMP(
        viga.perfil, viga.aco, viga.concreto, viga.laje, viga.cabo, viga.armacao_laje,
        800.0, 500.0, 6000.0, 3000.0, 3.0, 1.1, 1.4
    );
    auto m_neg_2024 = nbr8800::v2024::calcularMomentoNegativoVMP(
        viga.perfil, viga.aco, viga.concreto, viga.laje, viga.cabo, viga.armacao_laje,
        800.0, 500.0, 6000.0, 3000.0, 3.0, 1.1, 1.4
    );
    ASSERT_TRUE(m_neg_2024.Mrd_dist_kNm != m_neg_2008.Mrd_dist_kNm);
}