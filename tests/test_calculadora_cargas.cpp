#include "test_framework.h"
#include "catalogo_completo.h"
#include "calculadora_cargas.h"
#include <cmath>

TEST_CASE("Catálogo Completo de Perfis - Laminados W e Soldados VS") {
    const auto& perfis = obter_catalogo_perfis_completo();
    ASSERT_TRUE(perfis.size() >= 150);

    // Encontra W 460 x 52,0 (perfil do exemplo da planilha)
    bool encontrou_w460 = false;
    for (const auto& item : perfis) {
        if (std::string(item.nome) == "W460x52,0") {
            encontrou_w460 = true;
            ASSERT_TRUE(item.tipo == LAMINADO);
            ASSERT_NEAR(item.d_mm, 450.0, 1.0);
            ASSERT_NEAR(item.bf_mm, 152.0, 1.0);
            ASSERT_NEAR(item.tw_mm, 7.6, 0.1);
            ASSERT_NEAR(item.tf_mm, 10.8, 0.1);
            ASSERT_NEAR(item.Ix_cm4, 21370.0, 50.0);

            PerfilI p = criar_perfil_do_catalogo(item);
            ASSERT_NEAR(p.d, 450.0, 1.0);
            ASSERT_TRUE(p.Ag > 0.0);
            ASSERT_TRUE(p.Ix > 0.0);
            break;
        }
    }
    ASSERT_TRUE(encontrou_w460);

    // Encontra VS 450x60
    bool encontrou_vs = false;
    for (const auto& item : perfis) {
        if (std::string(item.nome) == "VS 450x60") {
            encontrou_vs = true;
            ASSERT_TRUE(item.tipo == SOLDADO);
            ASSERT_NEAR(item.d_mm, 450.0, 1.0);
            break;
        }
    }
    ASSERT_TRUE(encontrou_vs);
}

TEST_CASE("Catálogo Steel Deck Metform (MF-50 e MF-75)") {
    const auto& decks = obter_catalogo_steel_deck();
    ASSERT_TRUE(decks.size() >= 40);

    // Verifica MF-75|140|80 (exemplo da planilha)
    bool encontrou_mf75 = false;
    for (const auto& d : decks) {
        if (std::string(d.chave) == "MF-75|140|80") {
            encontrou_mf75 = true;
            ASSERT_NEAR(d.h_total_mm, 140.0, 0.1);
            ASSERT_NEAR(d.espessura_mm, 0.80, 0.01);
            ASSERT_NEAR(d.hF_mm, 75.0, 0.1);
            ASSERT_NEAR(d.passo_eF_mm, 274.0, 0.1);
            ASSERT_NEAR(d.bF_medio_mm, 137.0, 0.1);
            ASSERT_NEAR(d.peso_proprio_laje_kN_m2, 2.50, 0.2);
            ASSERT_NEAR(d.consumo_concreto_m3_m2, 0.1025, 0.005);
            break;
        }
    }
    ASSERT_TRUE(encontrou_mf75);
}

TEST_CASE("Calculadora de Cargas NBR 6120 - Calibração com Planilha de Referência") {
    nbr6120::AssistenteCargasConfig cfg;
    cfg.idx_ocupacao = 0; // Edif. comerciais/escritórios 2.50 kN/m²
    cfg.idx_divisoria = 2; // 0.75 kN/m²
    cfg.idx_revestimento = 1; // 1.00 kN/m²
    cfg.idx_forro = 1; // 0.25 kN/m²
    cfg.idx_instalacao = 1; // 0.30 kN/m²
    cfg.contrapiso_espessura_cm = 0.0;
    cfg.idx_parede = 0; // Nenhuma parede
    cfg.qc_construcao_kN_m2 = 1.00;

    double binf = 2.80; // m
    double peso_perfil_kg_m = 52.0; // W460x52
    bool eh_deck = true;
    double peso_deck_laje = 2.50; // kN/m²
    double consumo_conc = 0.1025; // m³/m²

    auto res = nbr6120::calcular_cargas_assistente(cfg, binf, peso_perfil_kg_m, eh_deck, peso_deck_laje, consumo_conc);

    // Carga de uso total: 2.50 + 0.75 = 3.25 kN/m²
    ASSERT_NEAR(res.q_total_area_kN_m2, 3.25, 0.01);
    // q2 linear: 3.25 * 2.80 = 9.10 kN/m
    ASSERT_NEAR(res.q2_linear_kN_m, 9.10, 0.05);

    // Permanentes após cura: 1.00 + 0.25 + 0.30 = 1.55 kN/m²
    ASSERT_NEAR(res.g2_area_total_kN_m2, 1.55, 0.01);
    // g2 linear: 1.55 * 2.80 = 4.34 kN/m
    ASSERT_NEAR(res.g2_linear_kN_m, 4.34, 0.05);

    // Peso do perfil: 52 * 9.80665 / 1000 ~= 0.51 kN/m
    ASSERT_NEAR(res.g_perfil_linear_kN_m, 0.51, 0.02);

    // Fatores psi da categoria C
    ASSERT_TRUE(res.categoria_psi == 'C');
    ASSERT_NEAR(res.psi0, 0.7, 0.01);
    ASSERT_NEAR(res.psi1, 0.6, 0.01);
    ASSERT_NEAR(res.psi2, 0.4, 0.01);
}
