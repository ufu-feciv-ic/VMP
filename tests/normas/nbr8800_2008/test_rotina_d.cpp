#include "test_framework.hpp"
#include "nucleo/material.hpp"
#include "nucleo/secao_i.hpp"
#include "normas/nbr8800_2008/rotina_d.hpp"
#include <cmath>

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina D - Coeficiente de Flambagem por Cisalhamento (kv)") {
    double h = 500.0;
    double tw = 6.0; // h / tw = 83.33 -> 260 / (h/tw) = 3.12 -> limite (a/h) = 9.73

    // 1. Sem enrijecedores (dist_a = 0) -> kv = 5.0
    double kv_sem_enrij = RotinaD::calcular_kv(0.0, h, tw);
    ASSERT_EQ(kv_sem_enrij, 5.0);

    // 2. Protecao para entrada negativa
    double kv_negativo = RotinaD::calcular_kv(-100.0, h, tw);
    ASSERT_EQ(kv_negativo, 5.0);

    // 3. Com enrijecedores onde a/h > 3.0 -> kv = 5.0 (NBR 8800 item 5.4.3.1.1)
    double kv_muito_espacado = RotinaD::calcular_kv(1600.0, h, tw); // a/h = 3.2 > 3.0
    ASSERT_EQ(kv_muito_espacado, 5.0);

    // 4. Limite de esbeltez da alma: a/h > [260 / (h/tw)]^2 -> kv = 5.0
    // Se a alma for muito fina: h = 1000, tw = 5 -> h/tw = 200.
    // [260 / 200]^2 = 1.3^2 = 1.69. Se a = 2000 (a/h = 2.0 <= 3, mas 2.0 > 1.69):
    double kv_alma_esbelta = RotinaD::calcular_kv(2000.0, 1000.0, 5.0);
    ASSERT_EQ(kv_alma_esbelta, 5.0);

    // 5. Enrijecedores atuantes: kv = 5 + 5 / (a/h)^2
    // Caso a = h (a/h = 1.0) -> kv = 5 + 5 / 1^2 = 10.0
    double kv_quadrado = RotinaD::calcular_kv(500.0, 500.0, tw);
    ASSERT_EQ(kv_quadrado, 10.0);

    // Caso a = 0.5 * h (a/h = 0.5) -> kv = 5 + 5 / 0.25 = 25.0
    double kv_denso = RotinaD::calcular_kv(250.0, 500.0, tw);
    ASSERT_EQ(kv_denso, 25.0);

    // Caso a = 2.0 * h (a/h = 2.0) -> kv = 5 + 5 / 4 = 6.25
    double kv_largo = RotinaD::calcular_kv(1000.0, 500.0, tw);
    ASSERT_EQ(kv_largo, 6.25);
}

TEST_CASE("Rotina D - Calculo Isolado de VRd nos 3 Regimes Normativos") {
    double Vpl = 1100000.0; // 1100 kN
    double gamma_a1 = 1.10;
    double Vrd_plastico = Vpl / gamma_a1; // 1000 kN = 1000000 N

    double lambda_p = 50.0;
    double lambda_r = 75.0;

    // 1. Regime 1: Plastificacao total da alma (lambda <= lambda_p)
    double vrd_regime1 = RotinaD::calcular_VRd(Vpl, 35.0, lambda_p, lambda_r, gamma_a1);
    ASSERT_EQ(vrd_regime1, Vrd_plastico);

    // Fronteira exata lambda == lambda_p
    double vrd_fronteira_p = RotinaD::calcular_VRd(Vpl, lambda_p, lambda_p, lambda_r, gamma_a1);
    ASSERT_EQ(vrd_fronteira_p, Vrd_plastico);

    // 2. Regime 2: Flambagem inelastica por cisalhamento (lambda_p < lambda <= lambda_r)
    // Formula: (lambda_p / lambda) * (Vpl / gamma_a1)
    double lambda_inel = 60.0;
    double vrd_regime2 = RotinaD::calcular_VRd(Vpl, lambda_inel, lambda_p, lambda_r, gamma_a1);
    double esperado_regime2 = (50.0 / 60.0) * Vrd_plastico;
    ASSERT_TRUE(std::abs(vrd_regime2 - esperado_regime2) < 1e-4);

    // Fronteira exata lambda == lambda_r (pelo ramo inelastico)
    double vrd_fronteira_r_inel = RotinaD::calcular_VRd(Vpl, lambda_r, lambda_p, lambda_r, gamma_a1);
    double esperado_fronteira_r = (50.0 / 75.0) * Vrd_plastico;
    ASSERT_TRUE(std::abs(vrd_fronteira_r_inel - esperado_fronteira_r) < 1e-4);

    // 3. Regime 3: Flambagem elastica por cisalhamento (lambda > lambda_r)
    // Formula: 1.24 * (lambda_p / lambda)^2 * (Vpl / gamma_a1)
    double lambda_elast = 100.0;
    double vrd_regime3 = RotinaD::calcular_VRd(Vpl, lambda_elast, lambda_p, lambda_r, gamma_a1);
    double esperado_regime3 = 1.24 * std::pow(50.0 / 100.0, 2.0) * Vrd_plastico;
    ASSERT_TRUE(std::abs(vrd_regime3 - esperado_regime3) < 1e-4);

    // Validacao fisica: o valor resistente deve decrescer com o aumento da esbeltez
    ASSERT_TRUE(vrd_regime1 > vrd_regime2);
    ASSERT_TRUE(vrd_regime2 > vrd_regime3);
}

TEST_CASE("Rotina D - Afericao Perfil Laminado W 360x57.8 (Dissertacao Tabela 13)") {
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50();

    ResultadoRotinaD res = RotinaD::calcular(secao, material, 1.10, 0.0);

    // 1. Verificacao de parametros geometricos
    // Aw = d * tw = 358.0 * 7.9 = 2828.2 mm²
    ASSERT_EQ(res.Aw, 358.0 * 7.9);

    // 2. Forca de plastificacao Vpl = 0.6 * Aw * fy = 0.6 * 2828.2 * 345 = 585437.4 N
    double vpl_esperado = 0.6 * (358.0 * 7.9) * 345.0;
    ASSERT_TRUE(std::abs(res.Vpl - vpl_esperado) < 1e-3);

    // 3. Coeficiente kv sem enrijecedores
    ASSERT_EQ(res.kv, 5.0);

    // 4. Esbeltez da alma h / tw = 331.8 / 7.9 = 42.0
    ASSERT_TRUE(std::abs(res.lambda_alma - (331.8 / 7.9)) < 1e-4);

    // 5. Limite de plastificacao lambda_p = 1.10 * sqrt(5.0 * 200000 / 345) = 59.22
    ASSERT_TRUE(res.lambda_p_alma > 59.2 && res.lambda_p_alma < 59.3);

    // Como lambda_alma (42.0) <= lambda_p (59.22), alma plastifica totalmente por cisalhamento!
    ASSERT_TRUE(res.lambda_alma <= res.lambda_p_alma);

    // 6. Resistencia VRd = Vpl / gamma_a1 = 585437.4 / 1.10 = 532215.82 N = 532.22 kN
    double vrd_kn_esperado = 532.22;
    double diff_pct = std::abs(res.VRd_kN - vrd_kn_esperado) / vrd_kn_esperado * 100.0;
    ASSERT_TRUE(diff_pct < 0.01); // Menor que 0.01% de diferenca com o valor da Tabela 13

    // Coerencia entre N e kN
    ASSERT_TRUE(std::abs(res.VRd_N - res.VRd_kN * 1000.0) < 1e-4);
}

TEST_CASE("Rotina D - Viga de Alma Esbelta: Sem Enrijecedores vs Com Enrijecedores") {
    // Viga soldada esbelta com h = 1000 mm e tw = 8 mm
    // lambda = 1000 / 8 = 125.0
    SecaoI viga_alta(
        "VS 1040x250",
        TipoFabricacao::Soldado,
        1040.0, // d
        250.0,  // bf
        20.0,   // tf (h = 1040 - 40 = 1000 mm)
        8.0     // tw
    );
    Aco aco = Aco::a572_gr50();

    // 1. Sem enrijecedores transversais
    ResultadoRotinaD res_sem_enrij = RotinaD::calcular(viga_alta, aco, 1.10, 0.0);

    ASSERT_EQ(res_sem_enrij.kv, 5.0);
    // lambda (125) > lambda_r (~73.76) -> Flambagem elastica governa
    ASSERT_TRUE(res_sem_enrij.lambda_alma > res_sem_enrij.lambda_r_alma);
    
    // Resistencia reduzida por flambagem elastica
    double vrd_sem_enrij = res_sem_enrij.VRd_kN;
    double vrd_plastico_max = (res_sem_enrij.Vpl / 1.10) / 1000.0;
    ASSERT_TRUE(vrd_sem_enrij < vrd_plastico_max * 0.5); // Perde mais de 50% de capacidade por flambagem

    // 2. Adicionando enrijecedores transversais a cada 500 mm (a = 500 mm, a/h = 0.5)
    ResultadoRotinaD res_com_enrij = RotinaD::calcular(viga_alta, aco, 1.10, 500.0);

    // kv aumenta significativamente para 25.0
    ASSERT_EQ(res_com_enrij.kv, 25.0);

    // Com kv = 25, lambda_p sobe para ~132.42 > lambda (125.0)
    ASSERT_TRUE(res_com_enrij.lambda_alma <= res_com_enrij.lambda_p_alma);

    // Como atingiu plastificacao plena, VRd passa a ser o valor plastico maximo!
    ASSERT_TRUE(std::abs(res_com_enrij.VRd_kN - vrd_plastico_max) < 1e-2);

    // Ganho estrutural expressivo
    ASSERT_TRUE(res_com_enrij.VRd_kN > res_sem_enrij.VRd_kN * 2.0);
}
