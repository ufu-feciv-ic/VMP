#include "test_framework.h"
#include "rotinas_viga_mista.h"
#include "catalogo.h"
#include <cmath>

using namespace viga_mista;

TEST_CASE("Viga Mista NBR 8800:2024 - Estudo de Referencia (W360x51, Vao 7m, Garagem)")
{
    viga_mista::VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = TipoLajeMista::MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.laje.hF_deck_mm = 0.0;

    cfg.conector.d_cs_mm = 19.0;
    cfg.conector.h_cs_mm = 100.0;
    cfg.conector.fu_MPa = 420.0;
    cfg.conector.espacamento_adotado_mm = 150.0;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;
    cfg.geometria.eh_viga_borda = false;
    cfg.geometria.escorada = false;

    // Cargas conforme estudo
    cfg.acoes.peso_proprio_perfil_kN_m = 0.499;
    cfg.acoes.q_laje_fresco_kN_m2 = 25.0 * 0.120; // 3.0 kN/m²
    cfg.acoes.q_sobrecarga_const_kN_m2 = 1.0;
    cfg.acoes.g_revestimento_kN_m2 = 4.0;
    cfg.acoes.q_uso_kN_m2 = 3.0;
    cfg.acoes.ocupacao = "Garagens";
    cfg.acoes.psi1 = 0.70;
    cfg.acoes.psi2 = 0.40;
    cfg.acoes.psi_vib = 0.00;
    cfg.acoes.beta_amortecimento = 0.025;

    auto rel = viga_mista::verificar_viga_mista(cfg);

    // 1. Largura efetiva e de influência
    ASSERT_PERCENT(rel.fase2.binf_mm, 1750.0, 0.01);
    ASSERT_PERCENT(rel.fase2.bef_mm, 1750.0, 0.01);

    // 2. Fase 1: Perfil de Aço Isolado
    ASSERT_PERCENT(rel.fase1.g1_kN_m, 5.749, 0.1);
    ASSERT_PERCENT(rel.fase1.q1_kN_m, 1.750, 0.1);
    ASSERT_PERCENT(rel.fase1.qSd1_kN_m, 9.461, 0.1); // 1.25*5.749 + 1.30*1.75
    ASSERT_PERCENT(rel.fase1.MSd1_kNm, 57.95, 0.15);
    ASSERT_PERCENT(rel.fase1.Mcr_kNm, 313.01, 0.15);
    ASSERT_PERCENT(rel.fase1.MRd_FLT_kNm, 199.46, 0.15);
    ASSERT_PERCENT(rel.fase1.MRd_aco_kNm, 199.46, 0.15);
    ASSERT_PERCENT(rel.fase1.VRd_kN, 480.99, 0.15);
    ASSERT_TRUE(rel.fase1.aprovado_flexao);
    ASSERT_TRUE(rel.fase1.aprovado_cortante);

    // 3. Conectores de Cisalhamento Stud Bolt
    ASSERT_PERCENT(rel.conectores.Acs_mm2, 283.53, 0.05);
    ASSERT_PERCENT(rel.conectores.QRd1_concreto_kN, 101.76, 0.1);
    ASSERT_PERCENT(rel.conectores.QRd2_aco_kN, 95.27, 0.1);
    ASSERT_PERCENT(rel.conectores.QRd_adotado_kN, 95.27, 0.1);
    ASSERT_PERCENT(rel.conectores.Fhd_interface_kN, 1993.52, 0.1); // Fa = Aa * fyd = 6356 * 313.64 = 1993.5 kN
    ASSERT_NEAR(rel.conectores.n_semi_vao_calculado, 21, 0.01);
    ASSERT_TRUE(rel.conectores.espacamento_atende);
    ASSERT_TRUE(rel.conectores.grau_interacao_atende);

    // 4. Fase 2: Flexão e Cortante Misto
    ASSERT_PERCENT(rel.fase2.qSd2_kN_m, 25.72, 0.1); // 1.4*(5.749 + 7.0) + 1.5*5.25 = 17.8486 + 7.875 = 25.72
    ASSERT_PERCENT(rel.fase2.MSd2_kNm, 157.56, 0.15);
    ASSERT_PERCENT(rel.fase2.VSd2_kN, 90.03, 0.15);
    ASSERT_PERCENT(rel.fase2.a_bloco_concreto_mm, 62.5, 0.2); // 62.5 mm
    ASSERT_PERCENT(rel.fase2.MRd_VM_kNm, 530.73, 0.15);
    ASSERT_PERCENT(rel.fase2.VRd_VM_kN, 480.99, 0.15);
    ASSERT_TRUE(rel.fase2.aprovado_flexao);
    ASSERT_TRUE(rel.fase2.aprovado_cortante);

    // 5. ELS: Flechas com Fluência
    ASSERT_PERCENT(rel.flechas.Itr_cd_cm4, 46857.95, 0.5);
    ASSERT_PERCENT(rel.flechas.Itr_ld_cm4, 36732.64, 0.5);
    ASSERT_PERCENT(rel.flechas.flecha_total_mm, 13.17, 1.0);
    ASSERT_TRUE(rel.flechas.aprovado_flecha);

    // 6. ELS: Vibração no Piso (NBR 8800:2024 Anexo I)
    ASSERT_PERCENT(rel.vibracao.Itr_din_cm4, 49936.12, 0.5);
    ASSERT_PERCENT(rel.vibracao.f0_Hz, 8.97, 1.0);
    ASSERT_PERCENT(rel.vibracao.Bb_m, 7.79, 0.5);
    ASSERT_PERCENT(rel.vibracao.massa_modal_kg, 20249.0, 1.0);
    ASSERT_PERCENT(rel.vibracao.ap_m_s2, 0.0176, 2.0);
    ASSERT_TRUE(rel.vibracao.aprovado_vibracao);

    // Diagnóstico Global
    ASSERT_TRUE(rel.aprovado_global);
}

TEST_CASE("Viga Mista com Steel Deck - Verificacao de Coeficientes Rg e Rp")
{
    viga_mista::VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = TipoLajeMista::STEEL_DECK;
    cfg.laje.tc_mm = 65.0; // capa sobre a fôrma
    cfg.laje.hF_deck_mm = 50.0;
    cfg.laje.bF_deck_mm = 130.0;
    cfg.laje.passo_deck_mm = 305.0;
    cfg.laje.orientacao = OrientacaoDeck::PERPENDICULAR_A_VIGA;

    cfg.conector.d_cs_mm = 19.0;
    cfg.conector.h_cs_mm = 100.0;
    cfg.conector.fu_MPa = 420.0;
    cfg.conector.n_por_linha = 1;

    cfg.geometria.L_vao_mm = 8000.0;
    cfg.geometria.Lb_mm = 4000.0;
    cfg.geometria.dist_esq_mm = 2000.0;
    cfg.geometria.dist_dir_mm = 2000.0;

    auto conn1 = viga_mista::calcularConectoresStud(cfg, 2000.0);
    ASSERT_NEAR(conn1.Rg, 1.0, 0.001);
    ASSERT_NEAR(conn1.Rp, 0.75, 0.001); // altura livre 100 - 50 = 50 mm >= 50 mm

    // 2 conectores por nervura
    cfg.conector.n_por_linha = 2;
    auto conn2 = viga_mista::calcularConectoresStud(cfg, 2000.0);
    ASSERT_NEAR(conn2.Rg, 0.85, 0.001);
    ASSERT_NEAR(conn2.Rp, 0.75, 0.001);

    // Verificações geométricas do pino no Steel Deck
    // h_min = max(4*19=76, 50+40=90) = 90 mm -> h_cs = 100 mm (Atende)
    ASSERT_NEAR(conn2.h_min_normativo_mm, 90.0, 0.01);
    ASSERT_TRUE(conn2.altura_atende);
    // Cobrimento: (65 + 50) - 100 = 15 mm >= 10 mm (Atende)
    ASSERT_NEAR(conn2.cobrimento_topo_mm, 15.0, 0.01);
    ASSERT_TRUE(conn2.cobrimento_atende);
    ASSERT_TRUE(conn2.diametro_mesa_atende);
}

TEST_CASE("Viga Mista - Interacao Incompleta / Parcial e LNP no Aco")
{
    viga_mista::VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = TipoLajeMista::MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.conector.d_cs_mm = 19.0;
    cfg.conector.h_cs_mm = 100.0;
    cfg.conector.fu_MPa = 420.0;
    cfg.conector.n_por_linha = 1;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;

    // Adotando espaçamento maior (280 mm) para forçar interação parcial
    cfg.conector.espacamento_adotado_mm = 280.0;

    RelatorioVigaMista rel = verificar_viga_mista(cfg);

    // O grau de interação deve ser parcial (eta < 1.0)
    ASSERT_TRUE(rel.conectores.grau_interacao_eta < 0.99);
    ASSERT_TRUE(rel.conectores.grau_interacao_eta >= rel.conectores.grau_interacao_minimo);

    // A LNP deve ter descido para a mesa de aço (LNP no aço)
    ASSERT_TRUE(!rel.fase2.lnp_na_laje);
    ASSERT_TRUE(rel.fase2.Cad_kN > 0.0);
    ASSERT_TRUE(rel.fase2.yp_lnp_aco_mm > 0.0);

    // Momento resistente calculado com interação parcial
    ASSERT_TRUE(rel.fase2.MRd_VM_kNm > 400.0);
    ASSERT_TRUE(rel.fase2.MRd_VM_kNm < 535.0); // Menor que o momento de 100% (530.7 kNm)
}

TEST_CASE("Comparativo NBR 8800:2008 vs NBR 8800:2024 - Cortante e FLT")
{
    viga_mista::VigaMistaConfig cfg;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;

    // 2024
    cfg.norma = NormaEdicao::NBR8800_2024;
    auto f1_2024 = viga_mista::calcularFase1Perfil(cfg);
    ASSERT_NEAR(f1_2024.kv, 5.34, 0.01);

    // 2008
    cfg.norma = NormaEdicao::NBR8800_2008;
    auto f1_2008 = viga_mista::calcularFase1Perfil(cfg);
    ASSERT_NEAR(f1_2008.kv, 5.00, 0.01);
}
