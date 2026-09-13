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

    // Perfil soldado - verificacao de kc
    double kc = RotinaA::calcular_kc(300.0, 6.0);
    ASSERT_TRUE(kc >= 0.35 && kc <= 0.76);
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
