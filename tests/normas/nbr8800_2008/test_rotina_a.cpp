#include "test_framework.hpp"
#include "nucleo/material.hpp"
#include "nucleo/secao_i.hpp"
#include "normas/nbr8800_2008/rotina_a.hpp"
#include <cmath>

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina A - Flambagem Global de Euler (Nex, Ney, Ne)") {
    double E = 200000.0;
    double Ix = 16067.0 * 1e4;
    double Iy = 1118.0 * 1e4;
    double Lv = 12000.0;
    double Lb = 3000.0;

    double nex = RotinaA::calcular_euler_nex(E, Ix, 1.0, Lv);
    double ney = RotinaA::calcular_euler_ney(E, Iy, 1.0, Lb);
    double ne = std::min(nex, ney);

    double nex_kn = nex / 1000.0;
    double ney_kn = ney / 1000.0;

    ASSERT_TRUE(nex_kn > 2200.0 && nex_kn < 2205.0);
    ASSERT_TRUE(ney_kn > 2450.0 && ney_kn < 2455.0);
    ASSERT_TRUE(ne == nex);
}

TEST_CASE("Rotina A - Fator de Mesa Qs (Laminado e Soldado)") {
    double E = 200000.0;
    double fy = 345.0;

    // Perfil laminado com mesa compacta (W 360x57.8: bf = 172, tf = 13.1)
    double qs_compacto = RotinaA::calcular_qs_laminado(172.0, 13.1, E, fy);
    ASSERT_EQ(qs_compacto, 1.0);

    // Perfil laminado com mesa esbelta customizada (bf = 300, tf = 6.0 -> lambda = 25)
    double qs_esbelto = RotinaA::calcular_qs_laminado(300.0, 6.0, E, fy);
    ASSERT_TRUE(qs_esbelto < 1.0);
    ASSERT_TRUE(qs_esbelto > 0.0);

    // Perfil soldado - verificacao de kc e seus limites normativos [0.35, 0.76]
    double kc_normal = RotinaA::calcular_kc(300.0, 6.0); // 4 / sqrt(50) = 0.5657
    ASSERT_TRUE(kc_normal >= 0.35 && kc_normal <= 0.76);
    ASSERT_TRUE(std::abs(kc_normal - 0.5657) < 1e-3);

    double kc_min = RotinaA::calcular_kc(1500.0, 10.0); // 4 / sqrt(150) = 0.326 -> clamp 0.35
    ASSERT_EQ(kc_min, 0.35);

    double kc_max = RotinaA::calcular_kc(160.0, 10.0); // 4 / sqrt(16) = 1.0 -> clamp 0.76
    ASSERT_EQ(kc_max, 0.76);

    // Perfil soldado - calculo de Qs (compacto vs esbelto)
    double qs_sold_comp = RotinaA::calcular_qs_soldado(200.0, 16.0, 468.0, 8.0, E, fy);
    ASSERT_EQ(qs_sold_comp, 1.0);

    double qs_sold_esb = RotinaA::calcular_qs_soldado(350.0, 8.0, 484.0, 8.0, E, fy);
    ASSERT_TRUE(qs_sold_esb < 1.0);
    ASSERT_TRUE(qs_sold_esb > 0.0);
}

TEST_CASE("Rotina A - Dimensionamento Completo de Perfil Soldado (VS 500x86)") {
    // Perfil soldado da serie VS com alma esbelta (Qa < 1.0) e mesa compacta (Qs = 1.0)
    SecaoI secao(
        "VS 500x86 Custom",
        TipoFabricacao::Soldado,
        500.0, // d (mm)
        200.0, // bf (mm)
        16.0,  // tf (mm)
        8.0    // tw (mm)
    );
    Aco material = Aco::a572_gr50(); // fy = 345 MPa, E = 200000 MPa

    ResultadoRotinaA res = RotinaA::calcular(
        secao,
        material,
        10000.0, // Lv = 10 m
        5000.0,  // Lb = 5 m
        1.0,     // Kx
        1.0,     // Ky
        1.10     // gamma_a1
    );

    // 1. Verificacao de tipo e Euler
    ASSERT_TRUE(secao.eh_soldado());
    ASSERT_TRUE(res.Nex > res.Ney); // Iy e menor que Ix, eixo Y governa
    ASSERT_EQ(res.Ne, res.Ney);
    double ne_kn = res.Ne / 1000.0;
    ASSERT_TRUE(std::abs(ne_kn - 1685.99) < 1.0);

    // 2. Flambagem local da mesa (Elemento AL soldado com kc)
    ASSERT_EQ(res.lambda_mesa, 6.25);
    ASSERT_TRUE(res.lambda_p_mesa > 11.1 && res.lambda_p_mesa < 11.2);
    ASSERT_EQ(res.Qs, 1.0); // Mesa compacta

    // 3. Flambagem local da alma (Elemento AA com d1 = 468 mm)
    ASSERT_EQ(res.lambda_alma, 58.5);
    ASSERT_TRUE(res.lambda_alma > res.lambda_p_alma); // Alma esbelta (58.5 > 35.87)
    ASSERT_TRUE(res.Qa < 1.0);
    ASSERT_TRUE(res.Qa > 0.95 && res.Qa < 0.96);
    ASSERT_TRUE(res.befa < secao.d1);

    // 4. Fator Q total
    ASSERT_EQ(res.Q, res.Qs * res.Qa);
    ASSERT_EQ(res.Q, res.Qa);

    // 5. Esbeltez global e resistencia final
    ASSERT_TRUE(res.lambda_0 > 1.40 && res.lambda_0 < 1.42);
    ASSERT_TRUE(res.chi > 0.43 && res.chi < 0.44);
    ASSERT_TRUE(std::abs(res.NRd_kN - 1325.62) < 2.0);
}

TEST_CASE("Rotina A - Perfil Soldado com Mesa Esbelta (Qs < 1.0 e Qa < 1.0)") {
    // Perfil soldado com mesa larga e fina (bf = 350, tf = 8, tw = 8)
    SecaoI secao(
        "VS Mesa Esbelta",
        TipoFabricacao::Soldado,
        500.0,
        350.0,
        8.0,
        8.0
    );
    Aco material = Aco::a572_gr50();

    ResultadoRotinaA res = RotinaA::calcular(
        secao,
        material,
        8000.0,
        4000.0
    );

    // Tanto mesa quanto alma devem ser esbeltas
    ASSERT_TRUE(res.Qs < 1.0);
    ASSERT_TRUE(res.Qa < 1.0);
    ASSERT_TRUE(res.Q < res.Qs);
    ASSERT_TRUE(res.Q < res.Qa);
    ASSERT_TRUE(std::abs(res.NRd_kN - 1103.89) < 2.0);
}

TEST_CASE("Rotina A - Curva de reducao chi por esbeltez global") {
    double chi_baixo = RotinaA::calcular_chi(1.0);
    ASSERT_TRUE(std::abs(chi_baixo - 0.658) < 1e-4);

    double chi_alto = RotinaA::calcular_chi(2.0);
    ASSERT_TRUE(std::abs(chi_alto - 0.21925) < 1e-4);
}

TEST_CASE("Rotina A - Afericao Completa Dissertacao Exemplo 1 (Tabela 12)") {
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50();

    ResultadoRotinaA resultado = RotinaA::calcular(
        secao,
        material,
        12000.0, // Lv (mm)
        3000.0,  // Lb (mm)
        1.0,     // Kx
        1.0,     // Ky
        1.10     // gamma_a1
    );

    ASSERT_EQ(resultado.Qs, 1.0);
    ASSERT_EQ(resultado.Qa, 1.0);
    ASSERT_EQ(resultado.Q, 1.0);

    double ne_kn = resultado.Ne / 1000.0;
    ASSERT_TRUE(ne_kn > 2200.0 && ne_kn < 2205.0);

    // Valor Dissertacao: ~1416.77 kN (analitico detalhado: 1418.68 kN)
    double nrd_kn = resultado.NRd_kN;
    double diff_pct = std::abs(nrd_kn - 1416.77) / 1416.77 * 100.0;
    ASSERT_TRUE(diff_pct < 0.5);
}
