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

    auto res = nbr8800::v2008::calcularNormalResistentePerfil(perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1);

    ASSERT_PERCENT(res.Nex_kN, 2212.84, 0.1);
    ASSERT_PERCENT(res.Ney_kN, 2441.08, 0.1);
    ASSERT_NEAR(res.Qs, 1.0, 0.001);
    ASSERT_NEAR(res.Qa, 1.0, 0.001);
    ASSERT_NEAR(res.Q, 1.0, 0.001);
    ASSERT_PERCENT(res.lb0, 1.06, 0.5);
    ASSERT_PERCENT(res.chi, 0.623, 0.5);
    ASSERT_PERCENT(res.NRd_kN, 1416.61, 0.15);
}

TEST_CASE("Rotina B - NBR 8800:2008: Momento resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();

    Aco aco = aco_a572_gr50();

    double Lb = 3000; // mm
    double Cb = 1.06; // a favor da segurança
    double gamma_a1 = 1.10;

    auto res = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, Lb, Cb, gamma_a1);

    ASSERT_PERCENT(res.Mpl_kNm, 350.106, 0.1);
    ASSERT_PERCENT(res.Mr_kNm, 217.78, 0.1);
    ASSERT_PERCENT(res.MRd_FLT_kNm, 285.98, 0.15);
    ASSERT_PERCENT(res.MRd_FLM_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_FLA_kNm, 318.28, 0.1);
    ASSERT_PERCENT(res.MRd_kNm, 285.98, 0.15);
    ASSERT_TRUE(!res.perfil_esbelto);
}

TEST_CASE("Rotina C.1 - NBR 8800:2008: Momento positivo viga mista (Exemplo 2)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 44.09, 0.1);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.1);
    ASSERT_PERCENT(res.Cc_max_kN, 4735.71, 0.05);
    ASSERT_PERCENT(res.T_aco_kN, 3578.59, 0.05);
    ASSERT_PERCENT(res.a_linha_neutra_mm, 98.23, 0.1);
    ASSERT_PERCENT(res.MRd_VM_kNm, 1296.81, 0.05);
}

TEST_CASE("Rotina C.2 - NBR 8800:2008: Momento positivo viga mista semicompacta (Exemplo 2 adaptado)")
{
    PerfilI perfil = perfil_w460x89();
    perfil.tw = 4.0;
    perfil.lbw = 463.0 / 4.0; // 115.75 (> 90.53 e <= 137.24)
    perfil.Ag = 8630.6;       // Área atualizada com tw = 4.0 mm
    perfil.Ix = 36870.1e4;    // Inércia atualizada com tw = 4.0 mm

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 115.75, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01);
    ASSERT_PERCENT(res.MRd_VM_kNm, 768.90, 0.1);
}

TEST_CASE("Rotina C.2 - NBR 8800:2008: Momento positivo viga mista semicompacta (Exemplo 1 adaptado)")
{
    PerfilI perfil = perfil_w360x57_8();
    perfil.tw = 3.0;
    perfil.lbw = 102.67;     // > 90.53 e <= 137.24
    perfil.Ag = 5501.8;      // Área atualizada com tw = 3.0 mm
    perfil.Ix = 14321.2e4;   // Inércia atualizada com tw = 3.0 mm

    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();

    auto res = nbr8800::v2008::calcularMomentoPositivoVM(perfil, aco, concreto, laje);

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.lbw, 102.67, 0.01);
    ASSERT_PERCENT(res.lbw_p, 90.53, 0.01);
    ASSERT_PERCENT(res.lbw_r, 137.23, 0.01);
    ASSERT_PERCENT(res.MRd_VM_kNm, 395.01, 0.1);
}

TEST_CASE("Rotina C.3 - NBR 8800:2008: Momento negativo viga mista (Exemplo 2)")
{
    PerfilI perfil = perfil_w460x89();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_mista_exemplo2();
    ArmacaoLaje armacaoLaje = laje_exemplo2_armacao();

    double MSd_neg_kNm = 56.45;
    double MSd_pos_kNm = 854.18;
    double Lv_mm = 15000.0;
    double a_vigas_mm = 2000.0;
    double alpha_viga = 3.0;

    auto res = nbr8800::v2008::calcularMomentoNegativoVM(
        perfil, aco, concreto, laje, armacaoLaje,
        MSd_neg_kNm, MSd_pos_kNm, Lv_mm, a_vigas_mm, alpha_viga
    );

    ASSERT_TRUE(res.aplicavel);
    ASSERT_PERCENT(res.MRd_neg_kNm, 789.50, 0.1);
    ASSERT_PERCENT(res.MRd_VM_kNm, 789.50, 0.1);
}

TEST_CASE("Rotina D - NBR 8800:2008: Cortante resistente perfil W360X57.8")
{
    PerfilI perfil = perfil_w360x57_8();
    
    // calcular_propriedades_secao_i(perfil);

    Aco aco = aco_a572_gr50();

    double dist_enrijecedor = 0.0;
    double gamma_a1 = 1.1;

    auto res = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, aco, dist_enrijecedor, gamma_a1);

    ASSERT_NEAR(res.kv, 5.0, 0.01);
    ASSERT_PERCENT(res.lbw, 38.98, 0.1);
    ASSERT_PERCENT(res.lbw_p, 59.22, 0.1);
    ASSERT_PERCENT(res.lbw_r, 73.75, 0.1);
    ASSERT_PERCENT(res.Vpl_kN, 585.43, 0.1);
    ASSERT_PERCENT(res.VRd_kN, 532.21, 0.1);
}

TEST_CASE("Geometria Perfil I - calcular_propriedades_secao_i vs Catalogo Gerdau (W360x57.8 e W460x89)")
{
    // 1. Aferição contra W360x57.8 do catalogo.h fornecendo apenas dimensões nominais (d, bf, tf, tw, r)
    PerfilI cat_w360 = perfil_w360x57_8();
    PerfilI calc_w360;
    calc_w360.tipo = LAMINADO;
    calc_w360.d = cat_w360.d;       // 358.0 mm
    calc_w360.bf = cat_w360.bf;     // 172.0 mm
    calc_w360.tf = cat_w360.tf;     // 13.1 mm
    calc_w360.tw = cat_w360.tw;     // 7.9 mm
    calc_w360.r = 11.9;             // Raio de concordância laminado Gerdau (d1 = 308 mm)

    calcular_propriedades_secao_i(calc_w360);

    ASSERT_PERCENT(calc_w360.h, cat_w360.h, 0.1);
    ASSERT_PERCENT(calc_w360.d1, cat_w360.d1, 0.1);
    ASSERT_PERCENT(calc_w360.Ag, cat_w360.Ag, 0.05);
    ASSERT_PERCENT(calc_w360.Ix, cat_w360.Ix, 0.05);
    ASSERT_PERCENT(calc_w360.Wx, cat_w360.Wx, 0.05);
    ASSERT_PERCENT(calc_w360.Zx, cat_w360.Zx, 0.05);
    ASSERT_PERCENT(calc_w360.Iy, cat_w360.Iy, 0.1);
    ASSERT_PERCENT(calc_w360.Wy, cat_w360.Wy, 0.1);
    ASSERT_PERCENT(calc_w360.ry, cat_w360.ry, 0.1);
    ASSERT_PERCENT(calc_w360.Zy, cat_w360.Zy, 0.05);
    ASSERT_PERCENT(calc_w360.rt, cat_w360.rt, 1.0);
    ASSERT_PERCENT(calc_w360.It, cat_w360.It, 1.0);
    ASSERT_PERCENT(calc_w360.lbf, cat_w360.lbf, 0.1);
    ASSERT_PERCENT(calc_w360.lbw, cat_w360.lbw, 0.1);
    ASSERT_PERCENT(calc_w360.Cw, cat_w360.Cw, 0.05);

    // 2. Aferição contra W460x89 do catalogo.h fornecendo apenas dimensões nominais (d, bf, tf, tw, r)
    PerfilI cat_w460 = perfil_w460x89();
    PerfilI calc_w460;
    calc_w460.tipo = LAMINADO;
    calc_w460.d = cat_w460.d;       // 463.0 mm
    calc_w460.bf = cat_w460.bf;     // 192.0 mm
    calc_w460.tf = cat_w460.tf;     // 17.7 mm
    calc_w460.tw = cat_w460.tw;     // 10.5 mm
    calc_w460.r = 11.8;             // Raio de concordância laminado Gerdau (d1 = 404 mm)

    calcular_propriedades_secao_i(calc_w460);

    ASSERT_PERCENT(calc_w460.h, cat_w460.h, 0.1);
    ASSERT_PERCENT(calc_w460.d1, cat_w460.d1, 0.1);
    ASSERT_PERCENT(calc_w460.Ag, cat_w460.Ag, 0.05);
    ASSERT_PERCENT(calc_w460.Ix, cat_w460.Ix, 0.2);
    ASSERT_PERCENT(calc_w460.Wx, cat_w460.Wx, 0.2);
    ASSERT_PERCENT(calc_w460.rx, cat_w460.rx, 0.1);
    ASSERT_PERCENT(calc_w460.Zx, cat_w460.Zx, 0.2);
    ASSERT_PERCENT(calc_w460.Iy, cat_w460.Iy, 0.3);
    ASSERT_PERCENT(calc_w460.Wy, cat_w460.Wy, 0.3);
    ASSERT_PERCENT(calc_w460.ry, cat_w460.ry, 0.1);
    ASSERT_PERCENT(calc_w460.Zy, cat_w460.Zy, 0.25);
    ASSERT_PERCENT(calc_w460.rt, cat_w460.rt, 0.5);
    ASSERT_PERCENT(calc_w460.It, cat_w460.It, 1.2);
    ASSERT_PERCENT(calc_w460.lbf, cat_w460.lbf, 0.1);
    ASSERT_PERCENT(calc_w460.Cw, cat_w460.Cw, 0.05);
}