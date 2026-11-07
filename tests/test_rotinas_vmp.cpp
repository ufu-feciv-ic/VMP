#include "test_framework.h"
#include "modelos.h"
#include "catalogo.h"
#include "rotinas_vmp.h"
#include <cmath>

TEST_CASE("Rotina 1: Cálculo propriedades seção transformada tempo infinito")
{
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();
    Concreto concreto = concreto_c30();
    Laje laje = laje_macica_130();

    auto res = nbr8800::calcularSecaoTransformada(perfil, laje, concreto, aco, 3, -50, false);

    ASSERT_NEAR(res.n_relacao_modular, 0.04473, 0.00001);
    ASSERT_NEAR(res.Atr_mm2, 246.96e2, 0.01);
    ASSERT_NEAR(res.ytr_mm, 351.3, 0.1);
    ASSERT_NEAR(res.ay_mm, 130, 0.0);
    ASSERT_NEAR(res.Itr_mm4, 49091.19e4, 0.01);
}
