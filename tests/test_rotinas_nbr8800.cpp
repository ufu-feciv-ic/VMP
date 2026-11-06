#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "geometria.h"
#include "rotinas_nbr8800.h"
#include <cmath>

TEST_CASE("Rotina A - NBR 8800:2008: Normal resistente perfil W360x57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    
    // calcular_propriedades_secao_i(perfil);

    Aco aco = aco_a572_gr50();

    const double Lb_mm = 3000.00;
    const double Lv_mm = 12000.00;
    const double kx = 1.0;
    const double ky = 1.0;
    const double gamma_a1 = 1.10;

    auto res = nbr8800::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);

    ASSERT_NEAR(res.Nex_kN, 2212.84, 0.01);
    ASSERT_NEAR(res.Ney_kN, 2441.08, 0.01);
    ASSERT_NEAR(res.Qs, 1.0, 0.01);
    ASSERT_NEAR(res.Qa, 1.0, 0.01);
    ASSERT_NEAR(res.Q, 1.0, 0.01);
    ASSERT_NEAR(res.lb0, 1.06, 0.01);
    ASSERT_NEAR(res.chi, 0.623, 0.01);
    ASSERT_NEAR(res.NRd_kN, 1416.61, 0.2);
}

TEST_CASE("Rotina B - NBR 8800:2008: Momento resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();

    Aco aco = aco_a572_gr50();

    double Lb = 3000; // mm
    double Cb = 1.06; // a favor da segurança
    double gamma_a1 = 1.10;

    auto res = nbr8800::calcularMomentoResistentePerfil(perfil, aco, Lb, Cb, gamma_a1);

    ASSERT_NEAR(res.Mpl_kNm, 350.106, 0.01);
    ASSERT_NEAR(res.Mr_kNm, 217.78, 0.01);
    ASSERT_NEAR(res.MRd_FLT_kNm, 285.98, 0.3);
    ASSERT_NEAR(res.MRd_FLM_kNm, 318.28, 0.01);
    ASSERT_NEAR(res.MRd_FLA_kNm, 318.28, 0.01);
    ASSERT_NEAR(res.MRd_kNm, 285.98, 0.3);
    ASSERT_TRUE(!res.perfil_esbelto);
}

TEST_CASE("Rotina D - NBR 8800:2008: Cortante resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    
    // calcular_propriedades_secao_i(perfil);

    Aco aco = aco_a572_gr50();

    double dist_enrijecedor = 0.0;
    double gamma_a1 = 1.1;

    auto res = nbr8800::calcularCortanteResistentePerfil(perfil, aco, dist_enrijecedor, gamma_a1);

    ASSERT_NEAR(res.kv, 5.0, 0.01);
    ASSERT_NEAR(res.lbw, 38.98, 0.01);
    ASSERT_NEAR(res.lbw_p, 59.22, 0.01);
    ASSERT_NEAR(res.lbw_r, 73.75, 0.01);
    ASSERT_NEAR(res.Vpl_kN, 585.43, 0.01);
    ASSERT_NEAR(res.VRd_kN, 532.21, 0.01);
}