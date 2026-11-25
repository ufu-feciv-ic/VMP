#include "test_framework.h"
#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "verificador.h"
#include "catalogo.h"
#include <cmath>

TEST_CASE("Viga Mista NBR 8800:2024 vs 2008 - Verificador Reutilizando rotinas_nbr8800 e rotinas_vmp")
{
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = LAJE_MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.laje.hF_mm = 0.0;

    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 100.0;
    cfg.conector.fu = 420.0;
    cfg.conector.nt = 1;
    cfg.conector.espacamento_adotado_mm = 0.0; // Interação completa

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;
    cfg.geometria.eh_viga_borda = false;
    cfg.geometria.escorada = false;

    cfg.acoes.g1_kN_m = 5.749;
    cfg.acoes.q1_kN_m = 1.750;
    cfg.acoes.g2_kN_m = 7.000;
    cfg.acoes.q2_kN_m = 5.250;
    cfg.acoes.psi1 = 0.70;
    cfg.acoes.psi2 = 0.40;

    auto rel = verificar_viga_mista(cfg);

    // 1. Largura efetiva
    ASSERT_PERCENT(rel.fase2.bef_mm, 1750.0, 0.01);

    // 2. Fase 1: Perfil de Aço Isolado (Reutiliza calcularMomentoResistentePerfil e calcularCortanteResistentePerfil)
    ASSERT_PERCENT(rel.fase1.g1_kN_m, 5.749, 0.1);
    ASSERT_PERCENT(rel.fase1.q1_kN_m, 1.750, 0.1);
    ASSERT_PERCENT(rel.fase1.qSd1_kN_m, 9.461, 0.1);
    ASSERT_PERCENT(rel.fase1.MSd1_kNm, 57.95, 0.15);
    ASSERT_PERCENT(rel.fase1.flexao_aco.Mcr_kNm, 313.01, 0.15);
    ASSERT_PERCENT(rel.fase1.flexao_aco.MRd_FLT_kNm, 216.985, 0.15);
    ASSERT_PERCENT(rel.fase1.flexao_aco.MRd_kNm, 216.985, 0.15);
    ASSERT_PERCENT(rel.fase1.cortante_aco.VRd_kN, 480.99, 0.15);
    ASSERT_TRUE(rel.fase1.aprovado_flexao);
    ASSERT_TRUE(rel.fase1.aprovado_cortante);

    // 3. Conectores Stud Bolt em Laje Maciça na NBR 8800:2024 (Rp = 0.75 conforme item L.4.2.1.3-a)
    ASSERT_PERCENT(rel.conectores.Acs_mm2, 283.53, 0.05);
    ASSERT_NEAR(rel.conectores.Rg, 1.00, 0.001);
    ASSERT_NEAR(rel.conectores.Rp, 0.75, 0.001);
    ASSERT_PERCENT(rel.conectores.QRd1_concreto_kN, 101.76, 0.1);
    ASSERT_PERCENT(rel.conectores.QRd2_aco_kN, 71.45, 0.1);
    ASSERT_PERCENT(rel.conectores.QRd_kN, 71.45, 0.1);
    ASSERT_PERCENT(rel.conectores.Fhd_kN, 1993.52, 0.1);
    ASSERT_NEAR(rel.conectores.n_semi_vao_calculado, 28, 0.01);
    ASSERT_TRUE(rel.conectores.espacamento_atende);
    ASSERT_TRUE(rel.conectores.grau_interacao_atende);

    // 4. Fase 2: Momento Positivo da Viga Mista (Reutiliza calcularMomentoPositivoVM)
    ASSERT_PERCENT(rel.fase2.qSd2_kN_m, 25.72, 0.1);
    ASSERT_PERCENT(rel.fase2.MSd2_kNm, 157.56, 0.15);
    ASSERT_PERCENT(rel.fase2.VSd2_kN, 90.03, 0.15);
    ASSERT_PERCENT(rel.fase2.momento_misto.a_linha_neutra_mm, 62.5, 0.2);
    ASSERT_PERCENT(rel.fase2.momento_misto.MRd_VM_kNm, 530.73, 0.15);
    ASSERT_PERCENT(rel.fase2.cortante_misto.VRd_kN, 480.99, 0.15);
    ASSERT_TRUE(rel.fase2.aprovado_flexao);
    ASSERT_TRUE(rel.fase2.aprovado_cortante);

    // 5. ELS: Flechas e Seção Transformada (Reutiliza calcularSecaoTransformada de rotinas_vmp)
    ASSERT_TRUE(rel.flechas.aprovado_flecha);
    ASSERT_TRUE(rel.flechas.tensao_servico_atende);
    ASSERT_TRUE(rel.aprovado_global);
}

TEST_CASE("Viga Mista com Steel Deck - Verificacao de Coeficientes Rg e Rp")
{
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = LAJE_STEEL_DECK;
    cfg.laje.tc_mm = 65.0; // capa sobre a fôrma
    cfg.laje.hF_mm = 50.0;
    cfg.laje.bF_mm = 130.0;
    cfg.laje.eF_mm = 305.0;
    cfg.laje.nervura_paralela = false; // Perpendicular

    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 100.0;
    cfg.conector.fu = 420.0;
    cfg.conector.nt = 1;

    cfg.geometria.L_vao_mm = 8000.0;
    cfg.geometria.Lb_mm = 4000.0;
    cfg.geometria.dist_esq_mm = 2000.0;
    cfg.geometria.dist_dir_mm = 2000.0;

    auto conn1 = calcularConectoresVigaMista(cfg, 2000.0);
    ASSERT_NEAR(conn1.Rg, 1.0, 0.001);
    ASSERT_NEAR(conn1.Rp, 0.75, 0.001);

    // 2 conectores por nervura
    cfg.conector.nt = 2;
    auto conn2 = calcularConectoresVigaMista(cfg, 2000.0);
    ASSERT_NEAR(conn2.Rg, 0.85, 0.001);
    ASSERT_NEAR(conn2.Rp, 0.75, 0.001);

    ASSERT_NEAR(conn2.h_min_normativo_mm, 90.0, 0.01);
    ASSERT_TRUE(conn2.altura_atende);
    ASSERT_NEAR(conn2.cobrimento_topo_mm, 15.0, 0.01);
    ASSERT_TRUE(conn2.cobrimento_atende);
    ASSERT_TRUE(conn2.diametro_mesa_atende);
}

TEST_CASE("Viga Mista - Interacao Incompleta / Parcial e LNP no Aco")
{
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = LAJE_MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 100.0;
    cfg.conector.fu = 420.0;
    cfg.conector.nt = 1;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;

    // Adotando espaçamento maior (250 mm) para forçar interação parcial
    cfg.conector.espacamento_adotado_mm = 250.0;

    RelatorioVigaMista rel = verificar_viga_mista(cfg);

    ASSERT_TRUE(rel.conectores.grau_interacao_eta < 0.99);
    ASSERT_TRUE(rel.conectores.grau_interacao_eta >= rel.conectores.grau_interacao_minimo);

    ASSERT_TRUE(!rel.fase2.momento_misto.lnp_na_laje);
    ASSERT_TRUE(rel.fase2.momento_misto.C_aco_kN > 0.0);
    ASSERT_TRUE(rel.fase2.momento_misto.y_lnp_mm > 0.0);

    ASSERT_TRUE(rel.fase2.momento_misto.MRd_VM_kNm > 380.0);
    ASSERT_TRUE(rel.fase2.momento_misto.MRd_VM_kNm < 530.73);
}

TEST_CASE("Diferenciacao por Namespaces nbr8800::v2008 vs nbr8800::v2024")
{
    PerfilI perfil = perfil_w360x51();
    Aco aco = aco_a572_gr50();
    Concreto conc50 = concreto_c30();
    conc50.fck_MPa = 50.0; // Concreto C50 (> 40 MPa) para testar eta_c

    Laje laje;
    laje.tipo = LAJE_MACICA;
    laje.tc_mm = 120.0;
    laje.bef_mm = 1750.0;

    ConectorCisalhamento stud;
    stud.tipo = CONECTOR_STUD_BOLT;
    stud.d = 19.0;
    stud.h = 100.0;
    stud.fu = 420.0;
    stud.nt = 1;

    // 1. Coeficiente kv de cortante (5.00 em v2008 vs 5.34 em v2024)
    auto cort_2008 = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, aco);
    auto cort_2024 = nbr8800::v2024::calcularCortanteResistentePerfil(perfil, aco);
    ASSERT_NEAR(cort_2008.kv, 5.00, 0.01);
    ASSERT_NEAR(cort_2024.kv, 5.34, 0.01);

    // 2. Coeficiente de fragilidade do concreto eta_c para fck = 50 MPa (1.00 em v2008 vs (40/50)^(1/3) = 0.928 em v2024)
    ASSERT_NEAR(nbr8800::v2008::calcularEtaC(50.0), 1.000, 0.001);
    ASSERT_NEAR(nbr8800::v2024::calcularEtaC(50.0), std::cbrt(40.0 / 50.0), 0.001);

    // 3. Fator Rp para pino soldado diretamente no perfil em laje maciça (1.00 em v2008 vs 0.75 em v2024)
    auto conn_2008 = nbr8800::v2008::calcularConectoresCisalhamento(perfil, aco, laje, conc50, stud, 7000.0, false);
    auto conn_2024 = nbr8800::v2024::calcularConectoresCisalhamento(perfil, aco, laje, conc50, stud, 7000.0, false);
    ASSERT_NEAR(conn_2008.Rp, 1.00, 0.001);
    ASSERT_NEAR(conn_2024.Rp, 0.75, 0.001);
    ASSERT_TRUE(conn_2008.QRd_kN > conn_2024.QRd_kN);

    // 4. Grau de interação mínimo (O.2.3.1.1.2 em v2008 vs L.2.3.1.1.2 com cp e cr em v2024)
    double eta_min_2008 = nbr8800::v2008::calcularGrauInteracaoMinimo(perfil, aco, 9000.0, false, 311.9, 607.7);
    double eta_min_2024 = nbr8800::v2024::calcularGrauInteracaoMinimo(perfil, aco, 9000.0, false, 311.9, 607.7);
    ASSERT_PERCENT(eta_min_2008, 0.5186, 0.5);
    ASSERT_PERCENT(eta_min_2024, 0.2800, 0.5);

    // 5. Normal resistente (Método Q=Qs*Qa em v2008 vs Método da Largura Efetiva Unificado Aef com c1,c2 em v2024)
    PerfilI p_esbelto = perfil;
    p_esbelto.tw = 3.5;
    p_esbelto.lbw = 308.0 / 3.5; // 88.0 > 1.49*sqrt(E/fy) = 35.88 (alma esbelta à compressão)
    auto nrd_2008 = nbr8800::v2008::calcularNormalResistentePerfil(p_esbelto, aco, 3000.0, 7000.0, 1.0, 1.0);
    auto nrd_2024 = nbr8800::v2024::calcularNormalResistentePerfil(p_esbelto, aco, 3000.0, 7000.0, 1.0, 1.0);
    ASSERT_TRUE(std::abs(nrd_2008.NRd_kN - nrd_2024.NRd_kN) > 1.0);

    // 6. FLT com Cb > 1.0 (Cb fora de lambda_r em v2008 vs Cb dentro de lambda_r em v2024 D.2.8-a)
    auto mrd_2008 = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, 3500.0, 1.25);
    auto mrd_2024 = nbr8800::v2024::calcularMomentoResistentePerfil(perfil, aco, 3500.0, 1.25);
    ASSERT_TRUE(std::abs(mrd_2008.MRd_FLT_kNm - mrd_2024.MRd_FLT_kNm) > 1.0);

    // 7. FLA em alma semicompacta (Mr = 0.7*fy*Wx em v2008 Tabela G.1 vs Mr = fy*Wx em v2024 Tabela D.1)
    PerfilI p_semi = perfil;
    p_semi.lbw = 110.0; // Entre 90.53 e 137.23
    auto fla_2008 = nbr8800::v2008::calcularMomentoResistentePerfil(p_semi, aco, 0.0, 1.0);
    auto fla_2024 = nbr8800::v2024::calcularMomentoResistentePerfil(p_semi, aco, 0.0, 1.0);
    ASSERT_TRUE(fla_2024.MRd_FLA_kNm > fla_2008.MRd_FLA_kNm);

    // 8. Momento Positivo VM com concreto C50 (fck = 50 MPa > 40 MPa -> eta_c = 0.928 em v2024 vs 1.00 em v2008)
    auto vm_pos_2008 = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, conc50, laje);
    auto vm_pos_2024 = nbr8800::v2024::calcularMomentoPositivoVM(perfil, aco, conc50, laje);
    ASSERT_TRUE(vm_pos_2008.Cc_max_kN > vm_pos_2024.Cc_max_kN);
}

TEST_CASE("Verificacao de Geometria Transversal dos Conectores (Mesa e Steel Deck)")
{
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51(); // bf = 171.0 mm
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();
    cfg.laje.tipo = LAJE_MACICA;
    cfg.laje.tc_mm = 120.0;

    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 100.0;
    cfg.conector.nt = 1;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;

    // 1. nt = 1 conector de 19 mm: st_min = 76 mm, bf_min = 50 mm (atende bf = 171 mm)
    auto rel1 = verificar_viga_mista(cfg);
    ASSERT_NEAR(rel1.conectores.s_transv_min_mm, 76.0, 0.01);
    ASSERT_NEAR(rel1.conectores.bf_min_necessario_mm, 50.0, 0.01);
    ASSERT_TRUE(rel1.conectores.largura_mesa_atende);
    ASSERT_TRUE(rel1.conectores.geometria_transversal_atende);

    // 2. nt = 2 conectores de 19 mm em W360x51: bf_min = 76 + 50 = 126 mm (atende bf = 171 mm)
    cfg.conector.nt = 2;
    auto rel2 = verificar_viga_mista(cfg);
    ASSERT_NEAR(rel2.conectores.bf_min_necessario_mm, 126.0, 0.01);
    ASSERT_TRUE(rel2.conectores.largura_mesa_atende);
    ASSERT_TRUE(rel2.conectores.geometria_transversal_atende);

    // 3. nt = 2 conectores em perfil estreito (ex: mesa bf = 100 mm): deve falhar
    cfg.perfil.bf = 100.0;
    auto rel3 = verificar_viga_mista(cfg);
    ASSERT_NEAR(rel3.conectores.bf_min_necessario_mm, 126.0, 0.01);
    ASSERT_TRUE(!rel3.conectores.largura_mesa_atende);
    ASSERT_TRUE(!rel3.conectores.geometria_transversal_atende);
    ASSERT_TRUE(!rel3.aprovado_global);

    // 4. nt = 3 conectores em perfil com mesa bf = 171 mm: bf_min = 2*76 + 50 = 202 mm > 171 mm -> deve falhar
    cfg.perfil.bf = 171.0;
    cfg.conector.nt = 3;
    auto rel4 = verificar_viga_mista(cfg);
    ASSERT_NEAR(rel4.conectores.bf_min_necessario_mm, 202.0, 0.01);
    ASSERT_TRUE(!rel4.conectores.largura_mesa_atende);
    ASSERT_TRUE(!rel4.conectores.geometria_transversal_atende);

    // 5. Steel Deck com nervuras paralelas e calha estreita (nt = 2 pinos precisam de 126 mm, calha tem 70 mm)
    cfg.perfil.bf = 205.0; // Mesa comporta 126 mm
    cfg.conector.nt = 2;
    cfg.laje.tipo = LAJE_STEEL_DECK;
    cfg.laje.tc_mm = 65.0;
    cfg.laje.hF_mm = 50.0;
    cfg.laje.bF_mm = 90.0; // w_fundo = 90 - 0.4*50 = 70 mm < 126 mm
    cfg.laje.nervura_paralela = true;
    auto rel5 = verificar_viga_mista(cfg);
    ASSERT_TRUE(rel5.conectores.largura_mesa_atende);
    ASSERT_TRUE(!rel5.conectores.nervura_deck_atende);
    ASSERT_TRUE(!rel5.conectores.geometria_transversal_atende);
    ASSERT_TRUE(!rel5.aprovado_global);
}
