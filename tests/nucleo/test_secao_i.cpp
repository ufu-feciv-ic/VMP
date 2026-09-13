#include "test_framework.hpp"
#include "nucleo/material.hpp"
#include "nucleo/secao_i.hpp"
#include <cmath>

using namespace vmp::nucleo;

TEST_CASE("Material - Criacao de Aco ASTM A572 Grau 50") {
    Aco aco = Aco::a572_gr50();
    ASSERT_EQ(aco.fy, 345.0);
    ASSERT_EQ(aco.fu, 450.0);
    ASSERT_EQ(aco.E, 200000.0);
}

TEST_CASE("Secao I - Propriedades e esbeltez do perfil W 360x57.8") {
    SecaoI secao = SecaoI::w360x57_8();
    
    ASSERT_TRUE(secao.eh_laminado());
    ASSERT_FALSE(secao.eh_soldado());
    ASSERT_EQ(secao.d, 358.0);
    ASSERT_EQ(secao.bf, 172.0);
    ASSERT_EQ(secao.tf, 13.1);
    ASSERT_EQ(secao.tw, 7.9);
    ASSERT_EQ(secao.r, 10.2);
    ASSERT_EQ(secao.Ag, 7320.0);
    
    // bf / (2 * tf) = 172 / (2 * 13.1) = 6.56488...
    double esb_mesa = secao.esbeltez_mesa();
    ASSERT_TRUE(esb_mesa > 6.56 && esb_mesa < 6.57);

    // Altura entre mesas: h = d - 2*tf = 358 - 26.2 = 331.8 mm
    ASSERT_TRUE(std::abs(secao.altura_entre_mesas() - 331.8) < 1e-4);

    // Cota k para dispersao de forcas concentradas: tf + r = 13.1 + 10.2 = 23.3 mm
    ASSERT_TRUE(std::abs(secao.cota_k() - 23.3) < 1e-4);
}

TEST_CASE("Secao I - Calculo automatico de d1 e Ag considerando o raio r") {
    // Cria perfil laminado sem especificar d1 e Ag manuais
    // d = 400, bf = 200, tf = 15, tw = 10, r = 12
    SecaoI secao(
        "Perfil Customizado",
        TipoFabricacao::Laminado,
        400.0,
        200.0,
        15.0,
        10.0,
        12.0 // r = 12 mm
    );

    // d1 automatico para laminado: d - 2*(tf + r) = 400 - 2*(15 + 12) = 400 - 54 = 346 mm
    ASSERT_TRUE(std::abs(secao.d1 - 346.0) < 1e-4);

    // Altura livre entre mesas: d - 2*tf = 400 - 30 = 370 mm
    ASSERT_TRUE(std::abs(secao.altura_entre_mesas() - 370.0) < 1e-4);

    // Cota k: tf + r = 15 + 12 = 27 mm
    ASSERT_TRUE(std::abs(secao.cota_k() - 27.0) < 1e-4);

    // Esbeltez da alma considerando raio: 346 / 10 = 34.6
    ASSERT_TRUE(std::abs(secao.esbeltez_alma() - 34.6) < 1e-4);

    // Area: retangulos (2*200*15 + 370*10 = 6000 + 3700 = 9700 mm²)
    // + cantos de concordancia: (4 - pi) * 12² = 0.8584 * 144 = 123.61 mm²
    double area_esperada = 9700.0 + (4.0 - 3.14159265358979323846) * 144.0;
    ASSERT_TRUE(std::abs(secao.Ag - area_esperada) < 1e-2);
}
