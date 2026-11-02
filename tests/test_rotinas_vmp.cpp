#include "test_framework.hpp"
#include "modelos.hpp"
#include "catalogo.hpp"
#include "rotinas_vmp.hpp"
#include <cmath>

TEST_CASE("Rotina 1 - Secao Transformada t=0 e t=inf (Turini Apendice B)") {
    PerfilI p = perfil_w360x57_8();
    Aco a = aco_a572_gr50();
    Laje l;
    l.tc = 130.0;
    l.largura_efetiva = 3000.0;
    Concreto c = concreto_c30();

    // Curta duracao t=0
    ResultadoSecaoTransformada t0 = calcular_rotina_1_secao_transformada(p, l, c, a, 1.0, -50.0);
    // Na dissertacao: ytr = 39.33 cm = 393.3 mm, Itr = 60834 cm4 = 6.08e8 mm4
    ASSERT_NEAR(t0.ytr_mm, 393.3, 2.0);
    ASSERT_NEAR(t0.Itr_mm4 / 1e4, 60834.0, 1500.0); // Diferenca minima pelo Ix nominal

    // Longa duracao t=infinito
    ResultadoSecaoTransformada t_inf = calcular_rotina_1_secao_transformada(p, l, c, a, 3.0, -50.0);
    // Na dissertacao: ytr = 35.13 cm = 351.3 mm, Itr = 49091 cm4
    ASSERT_NEAR(t_inf.ytr_mm, 351.3, 3.0);
}

TEST_CASE("Rotina 5 - Perdas Imediatas por Acomodacao de Cunha") {
    // P0 = 400 kN, delta_cunha = 6 mm, Ep = 200 GPa, Ap = 420 mm², L = 12000 mm
    // delta_P = (6 * 200000 * 420) / 12000 = 42000 N = 42 kN
    ResultadoPerdas res = calcular_rotina_5_perdas_imediatas(400.0, 6.0, 200000.0, 420.0, 12000.0);
    ASSERT_NEAR(res.delta_P_ancoragem_kN, 42.0, 0.01);
    ASSERT_NEAR(res.P_efetiva_kN, 358.0, 0.01);
}

TEST_CASE("Rotina 6 - Incremento Hiperestatico delta_P (Turini Apendice B)") {
    // Para q_conc = 9.75 kN/m, ep = 129 mm, Ix = 161430000 mm4, Ag = 7250 mm2, Ap = 420 mm2, L = 12000 mm
    // Na dissertacao: delta_P_conc = 35.65 kN
    ResultadoIncrementoDP res = calcular_rotina_6_incremento_dp(
        9.75, 12000.0, 129.0, 161430000.0, 7250.0, 420.0
    );
    ASSERT_NEAR(res.delta_P_kN, 35.65, 0.5);
}
