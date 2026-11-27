#include "test_framework.h"
#include "estudo_caso.h"
#include "verificador.h"
#include "vibracao.h"
#include "geometria.h"

// ============================================================================
// Calibração ponta a ponta contra a planilha de referência (caso V1).
// Valores esperados: referencias/Viga_Mista_NBR8800-2024.xlsx, aba Memorial.
// ============================================================================
TEST_CASE("Estudo de caso V1 - ELU e conectores vs planilha (Memorial)")
{
    auto rel = verificar_viga_mista(estudo_caso_v1::configuracao());

    // Fase 1 (Memorial E91, E118, E247)
    ASSERT_PERCENT(rel.fase1.MSd1_kNm, 136.6001, 0.01);
    ASSERT_PERCENT(rel.fase1.flexao_aco.MRd_kNm, 343.7141, 0.01);
    ASSERT_PERCENT(rel.fase1.cortante_aco.VRd_kN, 643.5818, 0.01);

    // Conectores (E135, E164, E146-E148, E166, E168)
    ASSERT_PERCENT(rel.conectores.QRd_kN, 70.5987, 0.01);
    ASSERT_PERCENT(rel.conectores.Fhd_kN, 2088.818, 0.01);
    ASSERT_NEAR(rel.conectores.n_linhas_meio_vao, 16, 0);
    ASSERT_NEAR(rel.conectores.n_total_adotado, 32, 0);
    ASSERT_NEAR(rel.conectores.n_semi_vao_calculado, 30, 0);
    ASSERT_PERCENT(rel.conectores.grau_interacao_eta, 0.540774, 0.01);

    // Fase 2 (E88, E90, E207, E224) e grau mínimo (E235, E236)
    ASSERT_PERCENT(rel.fase2.MSd2_kNm, 311.9069, 0.01);
    ASSERT_PERCENT(rel.fase2.VSd2_kN, 138.6253, 0.01);
    ASSERT_PERCENT(rel.fase2.momento_misto.C_aco_kN, 479.6198, 0.01);
    ASSERT_PERCENT(rel.fase2.momento_misto.MRd_VM_kNm, 607.7327, 0.01);
    ASSERT_PERCENT(rel.conectores.grau_interacao_minimo, 0.28, 0.01);
    ASSERT_NEAR(rel.conectores.n_semi_vao_minimo, 9, 0);

    ASSERT_TRUE(rel.aprovado_global);
}

TEST_CASE("Estudo de caso V1 - ELS (flechas, tensão) vs planilha (Memorial)")
{
    auto rel = verificar_viga_mista(estudo_caso_v1::configuracao());
    const auto& f = rel.flechas;

    // Seções homogeneizadas e inércias efetivas (E180, E190, E195-E198)
    ASSERT_PERCENT(f.Itr_cd_cm4, 76609.25, 0.01);
    ASSERT_PERCENT(f.Itr_ld_cm4, 57546.09, 0.01);
    ASSERT_PERCENT(f.Ief_cd_cm4, 61991.47, 0.01);
    ASSERT_PERCENT(f.Ief_ld_cm4, 47972.93, 0.01);
    ASSERT_PERCENT(f.Wef_inf_cd_cm3, 1446.384, 0.01);
    ASSERT_PERCENT(f.Wef_inf_ld_cm3, 1343.830, 0.01);

    // Parcelas de flecha (E258-E278, em mm)
    ASSERT_PERCENT(f.flecha_g1_mm, 15.8756, 0.01);
    ASSERT_PERCENT(f.flecha_perm_curta_mm, 18.8661, 0.01);
    ASSERT_PERCENT(f.flecha_fluencia_mm, 0.873862, 0.01);
    ASSERT_PERCENT(f.flecha_retracao_mm, 4.56420, 0.01);
    ASSERT_PERCENT(f.flecha_var_max_mm, 3.24104, 0.01);
    ASSERT_PERCENT(f.flecha_var_dano_mm, 7.00322, 0.01);
    ASSERT_PERCENT(f.flecha_total_bruta_mm, 27.5452, 0.01);
    ASSERT_NEAR(f.contraflecha_mm, 15.0, 1e-9);
    ASSERT_PERCENT(f.flecha_total_mm, 12.5452, 0.01);
    ASSERT_PERCENT(f.flecha_dano_mm, 12.4413, 0.01);
    ASSERT_PERCENT(f.limite_total_mm, 25.7143, 0.01);
    ASSERT_NEAR(f.contraflecha_sugerida_mm, 15.0, 1e-9);
    ASSERT_TRUE(f.aprovado_flecha);
    ASSERT_TRUE(f.aprovado_flecha_dano);

    // Tensão na face inferior em serviço (E280)
    ASSERT_PERCENT(f.tensao_mesa_inferior_MPa, 181.0695, 0.01);
}

TEST_CASE("Estudo de caso V1 - Vibração Anexo I vs planilha (Memorial E286-E312)")
{
    auto rel = verificar_viga_mista(estudo_caso_v1::configuracao());
    const auto& v = rel.vibracao;

    ASSERT_TRUE(v.calculado);
    ASSERT_PERCENT(v.alpha_din, 5.263158, 0.01);
    ASSERT_PERCENT(v.Ib_mm4 / 1e4, 81743.27, 0.01);
    ASSERT_PERCENT(v.w_vib_kN_m, 13.19250, 0.01);
    ASSERT_PERCENT(v.m_kg_m2, 480.2860, 0.01);
    ASSERT_PERCENT(v.delta_mm, 7.180043, 0.01);
    ASSERT_PERCENT(v.f0_Hz, 6.717520, 0.01);
    ASSERT_PERCENT(v.Bb_m, 8.295555, 0.01);
    ASSERT_PERCENT(v.M_kg, 17929.07, 0.01);
    ASSERT_NEAR(v.beta, 0.030, 1e-12);
    ASSERT_PERCENT(v.ap_m_s2, 0.0365284, 0.01);
    ASSERT_NEAR(v.W, 1.0, 1e-12);
    ASSERT_NEAR(v.zeta, 0.10, 1e-12);
    ASSERT_PERCENT(v.aw_rms_m_s2, 0.0492989, 0.01);
    ASSERT_PERCENT(v.R, 9.859778, 0.01);
    ASSERT_PERCENT(v.utilizacao, 0.730569, 0.01);
    ASSERT_TRUE(v.aprovado);
}

// ============================================================================
// Segundo gabarito de vibração: exemplo "Dimensionamento de uma viga mista de piso"
// (referencias/dimensionamento_de_viga_mista_de_piso.md, seção 7): W360x51, L = 7 m,
// laje maciça 120 mm, garagem, b = 1,75 m, np·Lp = 3 x 7 m.
// ============================================================================
TEST_CASE("Vibração Anexo I - Exemplo laje maciça (garagem) vs memorial de referência")
{
    PerfilI perfil = perfil_w360x51();
    Aco aco = aco_a572_gr50();
    Laje laje;
    laje.tipo = LAJE_MACICA;
    laje.tc_mm = 120.0;
    laje.hF_mm = 0.0;
    laje.bef_mm = 1750.0;

    ParametrosVibracao p;
    p.Ec_din_MPa = 38000.0;
    p.forro_instalacoes_suspensos = true;
    p.largura_piso_m = 21.0;
    p.viga_interna = true;
    const auto& tab = nbr8800::obter_tabela_ocupacoes_vibracao();
    nbr8800::aplicar_ocupacao_vibracao(p, tab[5]); // Garagens

    auto v = nbr8800::calcularVibracaoPisoAnexoI(perfil, aco, laje, 7000.0, 1750.0, 5.749, 7.000, 5.25, p);

    ASSERT_NEAR(v.w_vib_kN_m, 12.749, 1e-9);           // psi_vib = 0 em garagens
    ASSERT_PERCENT(v.Ib_mm4 / 1e4, 49936.12, 0.05);
    ASSERT_PERCENT(v.Is_mm4_por_mm, 27360.0, 0.01);
    ASSERT_PERCENT(v.delta_viga_mm, 3.991, 0.05);
    ASSERT_PERCENT(v.delta_mm, 4.023, 0.05);
    ASSERT_PERCENT(v.f0_Hz, 8.97, 0.1);
    ASSERT_PERCENT(v.Bb_m, 7.790, 0.05);
    ASSERT_PERCENT(v.M_kg, 20248.8, 0.05);
    ASSERT_NEAR(v.beta, 0.025, 1e-12);
    ASSERT_PERCENT(v.ap_m_s2, 0.0176, 0.5);
    ASSERT_PERCENT(v.W, 0.89, 0.5);
    ASSERT_PERCENT(v.aw_rms_m_s2, 0.0467, 0.5);
    ASSERT_PERCENT(v.R, 9.34, 0.5);
    ASSERT_TRUE(v.aprovado);
}

TEST_CASE("Seção homogeneizada ELS - LNE no perfil x LNE na laje")
{
    // V1, curta duração: LNE no perfil (planilha E176/E177: y0 = 471,4 mm < d + hF = 525 mm)
    auto cfg = estudo_caso_v1::configuracao();
    cfg.laje.bef_mm = 2250.0;
    calcular_propriedades_secao_i(cfg.perfil);
    auto s = nbr8800::calcularSecaoHomogeneizadaELS(cfg.perfil, cfg.laje, 200000.0 / cfg.concreto.Ecs_MPa);
    ASSERT_TRUE(!s.lne_na_laje);
    ASSERT_PERCENT(s.yLN_mm, 471.4179, 0.01);

    // Mesma seção com laje maciça espessa: LNE na laje -> concreto tracionado desprezado
    Laje macica;
    macica.tipo = LAJE_MACICA;
    macica.tc_mm = 200.0;
    macica.bef_mm = 2250.0;
    auto s2 = nbr8800::calcularSecaoHomogeneizadaELS(cfg.perfil, macica, 7.67);
    auto s2_nao_fiss = nbr8800::calcularSecaoHomogeneizadaELS(cfg.perfil, macica, 7.67, false);
    ASSERT_TRUE(s2.lne_na_laje);
    ASSERT_TRUE(s2.x_mm < macica.tc_mm);
    ASSERT_TRUE(s2.Itr_mm4 < s2_nao_fiss.Itr_mm4);
}

TEST_CASE("ELS - Limite de 15 mm com alvenaria e critério de vibração da NBR 8800:2008")
{
    auto cfg = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008);
    cfg.acoes.alvenaria_solidarizada = true;
    auto rel = verificar_viga_mista(cfg);

    ASSERT_NEAR(rel.flechas.limite_dano_mm, 15.0, 1e-9);
    ASSERT_TRUE(rel.flechas.flecha_vibracao_2008_mm > rel.flechas.flecha_perm_curta_mm);

    bool achou_vibracao = false;
    for (const auto& item : rel.itens) {
        if (item.item == "ELS Vibração do Piso") {
            achou_vibracao = true;
            ASSERT_NEAR(item.resistente, 20.0, 1e-9);
            ASSERT_NEAR(item.solicitante, rel.flechas.flecha_vibracao_2008_mm, 1e-9);
        }
    }
    ASSERT_TRUE(achou_vibracao);
}

TEST_CASE("Conectores - Rp depende só de emh; limites de espaçamento iguais em 2008 e 2024")
{
    auto cfg = estudo_caso_v1::configuracao();
    Laje laje = cfg.laje;
    laje.bef_mm = 2250.0;

    // Pino curto (h − hF = 30 mm < 40 mm): Rp continua 0,75 se emh >= 50 mm (L.4.2.1.3-b);
    // a altura insuficiente é acusada pela verificação geométrica, não por Rp.
    ConectorCisalhamento curto = cfg.conector;
    curto.h = 105.0;
    auto r = nbr8800::v2024::calcularConectoresCisalhamento(cfg.perfil, cfg.aco, laje, cfg.concreto, curto, 9000.0);
    ASSERT_NEAR(r.Rp, 0.75, 1e-12);
    ASSERT_TRUE(!r.altura_atende);

    ConectorCisalhamento emh_baixo = cfg.conector;
    emh_baixo.emh_mm = 40.0;
    r = nbr8800::v2024::calcularConectoresCisalhamento(cfg.perfil, cfg.aco, laje, cfg.concreto, emh_baixo, 9000.0);
    ASSERT_NEAR(r.Rp, 0.60, 1e-12);

    // O.4.3.2/O.4.3.3 e L.4.3.3/L.4.3.4: s_min = 4d com fôrma; s_max = min(8 h, 915 mm) com nervuras perpendiculares
    auto r08 = nbr8800::v2008::calcularConectoresCisalhamento(cfg.perfil, cfg.aco, laje, cfg.concreto, cfg.conector, 9000.0);
    auto r24 = nbr8800::v2024::calcularConectoresCisalhamento(cfg.perfil, cfg.aco, laje, cfg.concreto, cfg.conector, 9000.0);
    ASSERT_NEAR(r08.s_min_long_mm, 76.0, 1e-9);
    ASSERT_NEAR(r08.s_max_long_mm, 915.0, 1e-9);
    ASSERT_NEAR(r24.s_min_long_mm, r08.s_min_long_mm, 1e-9);
    ASSERT_NEAR(r24.s_max_long_mm, r08.s_max_long_mm, 1e-9);

    // Nervuras paralelas com bF/hF < 1,5: Rg = 0,85 (O.4.2.1.2-b2 / L.4.2.1.2-c2)
    Laje paralela = laje;
    paralela.nervura_paralela = true;
    paralela.bF_mm = 100.0; // 100/75 = 1,33
    ConectorCisalhamento dois = cfg.conector;
    dois.nt = 2;
    r = nbr8800::v2024::calcularConectoresCisalhamento(cfg.perfil, cfg.aco, paralela, cfg.concreto, dois, 9000.0);
    ASSERT_NEAR(r.Rg, 0.85, 1e-12);
}
