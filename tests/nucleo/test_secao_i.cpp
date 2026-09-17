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

TEST_CASE("Secao I - Perfil Soldado (r = 0, d1, Ag, Ix, Iy e cota k)") {
    // Perfil soldado sem especificar d1, Ag, Ix, Iy manuais (calculo analitico automatico)
    // d = 500, bf = 200, tf = 16, tw = 8, r = 0 (padrao para perfil soldado)
    SecaoI secao(
        "VS 500x86 Custom",
        TipoFabricacao::Soldado,
        500.0,
        200.0,
        16.0,
        8.0
    );

    // Verificacoes de tipo e raio
    ASSERT_TRUE(secao.eh_soldado());
    ASSERT_FALSE(secao.eh_laminado());
    ASSERT_EQ(secao.r, 0.0);

    // Altura livre entre mesas: d - 2*tf = 500 - 32 = 468 mm
    ASSERT_EQ(secao.altura_entre_mesas(), 468.0);

    // Para soldados, d1 coincide exatamente com a altura entre mesas (d - 2*tf)
    ASSERT_EQ(secao.d1, 468.0);

    // Cota k para soldados e estritamente a espessura da mesa tf: tf + r = 16 + 0 = 16 mm
    ASSERT_EQ(secao.cota_k(), 16.0);

    // Area dos cantos curvos de concordancia deve ser nula
    ASSERT_EQ(secao.area_cantos_raio(), 0.0);

    // Area bruta: soma pura dos 3 retangulos sem adicao de cantos
    // 2 * (200 * 16) + (468 * 8) = 6400 + 3744 = 10144 mm²
    ASSERT_EQ(secao.Ag, 10144.0);

    // Razoes de esbeltez locais
    // Mesa: bf / (2*tf) = 200 / 32 = 6.25
    ASSERT_EQ(secao.esbeltez_mesa(), 6.25);
    // Alma: d1 / tw = 468 / 8 = 58.5
    ASSERT_EQ(secao.esbeltez_alma(), 58.5);

    // Inercia Ix calculada analiticamente pelas partes retangulares
    // Alma: (8 * 468^3) / 12 = 68309408 mm⁴
    // Mesas: 2 * [(200 * 16^3)/12 + (200 * 16) * ((500-16)/2)^2] = 374946133.33 mm⁴
    double h_alma = 468.0;
    double y_mesa = (500.0 - 16.0) / 2.0;
    double ix_esperado = (8.0 * std::pow(h_alma, 3)) / 12.0 +
                         2.0 * ((200.0 * std::pow(16.0, 3)) / 12.0 + (200.0 * 16.0) * std::pow(y_mesa, 2));
    ASSERT_TRUE(std::abs(secao.Ix - ix_esperado) < 1e-4);

    // Inercia Iy calculada analiticamente pelas partes retangulares
    // Mesas: 2 * (16 * 200^3) / 12 = 21333333.33 mm⁴
    // Alma: (468 * 8^3) / 12 = 19968 mm⁴
    double iy_esperado = 2.0 * ((16.0 * std::pow(200.0, 3)) / 12.0) +
                         (468.0 * std::pow(8.0, 3)) / 12.0;
    ASSERT_TRUE(std::abs(secao.Iy - iy_esperado) < 1e-4);
}

