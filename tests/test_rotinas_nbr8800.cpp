#include "test_framework.hpp"
#include "modelos.hpp"
#include "catalogo.hpp"
#include "rotinas_nbr8800.hpp"
#include <cmath>

TEST_CASE("Rotina A - Compressao Axial W360x57.8 (Turini Apendice B)") {
    PerfilI p = perfil_w360x57_8();
    Aco a = aco_a572_gr50();

    ResultadoRotinaA res = calcular_rotina_a(p, a, 3000.0, 12000.0);

    // Na dissertacao de Turini (Ap. B p. 196):
    // Ne = 2212.84 kN (ou ~2210 kN com Ix oficial)
    // Q = 1.0 (secao compacta)
    // chi = 0.623
    // NRd = 1416.61 kN
    ASSERT_NEAR(res.Q, 1.0, 0.01);
    ASSERT_NEAR(res.NRd_kN, 1416.61, 10.0); // Tolerancia de 0.7% pelas diferencas de catalogo
}

TEST_CASE("Rotina B - Flexao FLT W360x57.8 (Turini Apendice B)") {
    PerfilI p = perfil_w360x57_8();
    Aco a = aco_a572_gr50();

    ResultadoRotinaB res = calcular_rotina_b(p, a, 3000.0, 1.06);

    // Na dissertacao (Ap. B p. 196-197):
    // lambda = 76.53, lambda_p = 42.37, lambda_r = 126.69
    // Mpl = 350.11 kN.m
    // MRd = 285.98 kN.m
    ASSERT_NEAR(res.lambda, 76.7, 1.0);
    ASSERT_NEAR(res.lambda_p, 42.37, 0.5);
    ASSERT_NEAR(res.MRd_kNm, 285.98, 5.0);
}

TEST_CASE("Rotina D - Cortante W360x57.8 (Turini Apendice B)") {
    PerfilI p = perfil_w360x57_8();
    Aco a = aco_a572_gr50();

    ResultadoRotinaD res = calcular_rotina_d(p, a, 0.0);

    // Na dissertacao (Ap. B p. 200):
    // kv = 5.0, VRd = 532.21 kN
    ASSERT_NEAR(res.kv, 5.0, 0.01);
    ASSERT_NEAR(res.VRd_kN, 532.21, 1.0);
}

TEST_CASE("Rotina E - Conectores de Cisalhamento Perfil U (Turini Apendice B)") {
    PerfilI p = perfil_w360x57_8();
    Aco a = aco_a572_gr50();
    Laje l;
    l.tc = 130.0;
    l.largura_efetiva = 3000.0;
    Concreto c = concreto_c30();
    ConectorU con;
    con.tf = 4.75;
    con.tw = 4.75;
    con.Lcs = 50.0;

    ResultadoRotinaE res = calcular_rotina_e_perfil_u(p, a, l, c, con, 12000.0);

    // Na dissertacao (Ap. B p. 194):
    // QRd = 76.72 kN
    // nl = 60 a 62 conectores
    ASSERT_NEAR(res.QRd_kN, 76.72, 0.1);
    ASSERT_NEAR(res.el_espacamento_mm, 200.0, 10.0);
}
