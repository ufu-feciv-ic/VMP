#include "test_framework.hpp"
#include "nucleo/material.hpp"
#include "nucleo/secao_i.hpp"
#include "normas/nbr8800_2008/rotina_b.hpp"
#include <cmath>

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina B.1 - FLT no Regime Plastico")
{
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50(); // fy = 345 MPa, E = 200000 MPa
    double gamma_a1 = 1.10;

    // Se Lb = 500 mm, lambda = Lb / ry = 500 / 39.1 = 12.78 <= lambda_p = 1,76*raiz(E/fy) = 42.38
    double Lb = 500.0;
    ResultadoFLT res = RotinaB::calcular_FLT(secao, material, Lb, 1.0, gamma_a1);

    // Primeira verificação: regime deve ser plástico
    ASSERT_TRUE(res.regime == RegimeEsbeltez::Plastico);
    
    // Segunda verificação: MRd deve ser Mpl/gamma_a1
    double Mpl_esperado = std::min(secao.Zx * material.fy, 1.5 * secao.Wx * material.fy);
    double MRd_esperado = Mpl_esperado / gamma_a1;

    ASSERT_TRUE(std::abs(res.MRd_Nmm - MRd_esperado) < 1e-4);
}

