#include "rotinas_vmp.hpp"
#include <cmath>
#include <algorithm>

// ----------------------------------------------------------------------------
// Rotina 1: Seção Transformada
// ----------------------------------------------------------------------------
ResultadoSecaoTransformada calcular_rotina_1_secao_transformada(
    const PerfilI& perfil,
    const Laje& laje,
    const Concreto& concreto,
    const Aco& aco,
    double multiplicador_fluencia,
    double ep_perfil_inferior_mm
) {
    ResultadoSecaoTransformada res;

    res.n_relacao_modular = multiplicador_fluencia * (aco.E / concreto.Ecs);
    res.btr_mm = laje.largura_efetiva / res.n_relacao_modular;

    double Ac_homog = res.btr_mm * laje.tc;

    double y_aco = perfil.d / 2.0;
    double y_laje = perfil.d + (laje.tc / 2.0);

    res.ytr_mm = (perfil.Ag * y_aco + Ac_homog * y_laje) / (perfil.Ag + Ac_homog);
    res.ay_mm = (perfil.d + laje.tc) - res.ytr_mm;

    res.Atr_mm2 = perfil.Ag + res.btr_mm * std::min(res.ay_mm, laje.tc);

    double I_laje_propria = (res.btr_mm * std::pow(laje.tc, 3.0)) / 12.0;
    double I_laje_steiner = Ac_homog * std::pow(y_laje - res.ytr_mm, 2.0);
    double I_aco_steiner = perfil.Ag * std::pow(res.ytr_mm - y_aco, 2.0);

    res.Itr_mm4 = perfil.Ix + I_aco_steiner + I_laje_propria + I_laje_steiner;

    res.Wtr_inf_mm3 = res.Itr_mm4 / res.ytr_mm;
    res.Wtr_sup_mm3 = res.Itr_mm4 / ((perfil.d + laje.tc) - res.ytr_mm);
    res.ep_tr_mm = res.ytr_mm - (-ep_perfil_inferior_mm);

    return res;
}

// ----------------------------------------------------------------------------
// Rotina 2: Estimativa da Força Máxima de Protensão Inicial (P0)
// ----------------------------------------------------------------------------
ResultadoEstimativaP0 calcular_rotina_2_estimativa_p0(
    const PerfilI& perfil,
    const Aco& aco,
    double NRd_kN,
    double MRd_kNm,
    double ep_I_mm,
    double M_peso_proprio_kNm,
    double P0_6118_estimado_kN
) {
    ResultadoEstimativaP0 res;

    double yg2 = 1.0;
    double beta = 1.1;
    double ep_m = ep_I_mm / 1000.0;

    double termo_N = (yg2 * beta) / NRd_kN;
    double termo_M = (8.0 * yg2 * beta * ep_m) / (9.0 * MRd_kNm);

    res.P0_8800_kN = 1.0 / (termo_N + termo_M);
    res.P0_6118_kN = P0_6118_estimado_kN;
    res.P0_max_kN = std::min(res.P0_8800_kN, res.P0_6118_kN);

    return res;
}

// ----------------------------------------------------------------------------
// Rotina 4: Armadura de Protensão
// ----------------------------------------------------------------------------
ResultadoRotina4 calcular_rotina_4_armadura_protensao(
    double P0_kN,
    double fpk_MPa,
    double area_unitaria_cordoalha_mm2
) {
    ResultadoRotina4 res;
    double sigma_p0_MPa = 0.75 * fpk_MPa;

    res.Ap_calculada_mm2 = (P0_kN * 1000.0) / sigma_p0_MPa;
    res.n_cabos = static_cast<int>(std::ceil(res.Ap_calculada_mm2 / area_unitaria_cordoalha_mm2));
    if (res.n_cabos < 1) res.n_cabos = 1;

    res.Ap_adotada_mm2 = res.n_cabos * area_unitaria_cordoalha_mm2;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina 5: Perdas Imediatas
// ----------------------------------------------------------------------------
ResultadoPerdas calcular_rotina_5_perdas_imediatas(
    double P0_kN,
    double delta_cunha_mm,
    double Ep_MPa,
    double Ap_mm2,
    double Lv_mm
) {
    ResultadoPerdas res;
    double delta_P_N = (delta_cunha_mm * Ep_MPa * Ap_mm2) / Lv_mm;
    res.delta_P_ancoragem_kN = delta_P_N / 1000.0;
    res.P_efetiva_kN = P0_kN - res.delta_P_ancoragem_kN;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina 6: Incremento Hiperestático da Força de Protensão (ΔP)
// ----------------------------------------------------------------------------
ResultadoIncrementoDP calcular_rotina_6_incremento_dp(
    double q_kN_m,
    double Lv_mm,
    double ep_mm,
    double I_mm4,
    double A_secao_mm2,
    double Ap_mm2
) {
    ResultadoIncrementoDP res;

    double q_N_mm = q_kN_m;
    double numerador = q_N_mm * std::pow(Lv_mm, 2.0) * ep_mm;
    double denominador = 12.0 * (std::pow(ep_mm, 2.0) + (I_mm4 / Ap_mm2) + (I_mm4 / A_secao_mm2));

    double delta_P_N = numerador / denominador;
    res.delta_P_kN = delta_P_N / 1000.0;

    return res;
}

// ----------------------------------------------------------------------------
// Rotina H: Tensão de Cálculo no Cabo em ELU (sigma_pd)
// ----------------------------------------------------------------------------
ResultadoRotinaH calcular_rotina_h_tensao_cabo(
    double P_atuante_kN,
    double Ap_mm2,
    double Ep_MPa,
    double fpyk_MPa,
    double Atr_mm2,
    double Lv_mm,
    double d_total_mm
) {
    ResultadoRotinaH res;

    double P_N = P_atuante_kN * 1000.0;
    res.epsilon_pre = (0.9 * P_N) / (Ep_MPa * Ap_mm2);

    double rho_p = Ap_mm2 / Atr_mm2;

    if ((Lv_mm / d_total_mm) < 35.0) {
        res.delta_sigma_MPa = 70.0 + (30.0 / (rho_p * 100.0));
        res.delta_sigma_MPa = std::min(res.delta_sigma_MPa, 420.0);
    } else {
        res.delta_sigma_MPa = 70.0;
    }

    double sigma_1 = (res.epsilon_pre * Ep_MPa + res.delta_sigma_MPa) / 1.15;
    double sigma_2 = fpyk_MPa / 1.15;

    res.sigma_pd_MPa = std::min(sigma_1, sigma_2);
    return res;
}

// ----------------------------------------------------------------------------
// Rotina 3: Momento Fletor Resistente de VMP (MRd,VMP+)
// ----------------------------------------------------------------------------
ResultadoMomentoVMP calcular_rotina_3_momento_vmp(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    double Ap_mm2,
    double sigma_pd_MPa,
    double ep_base_perfil_mm,
    double gamma_a1,
    double gamma_c
) {
    ResultadoMomentoVMP res;

    res.T_aco_kN = (perfil.Ag * aco.fy / gamma_a1) / 1000.0;
    res.T_cabo_kN = (Ap_mm2 * sigma_pd_MPa) / 1000.0;
    double T_total_kN = res.T_aco_kN + res.T_cabo_kN;

    double fcd_MPa = concreto.fck / gamma_c;
    res.Cc_max_kN = (0.85 * fcd_MPa * laje.largura_efetiva * laje.tc) / 1000.0;

    if (res.Cc_max_kN >= T_total_kN) {
        res.a_linha_neutra_mm = (T_total_kN * 1000.0) / (0.85 * fcd_MPa * laje.largura_efetiva);

        double braco_laje_m = (perfil.d / 2.0 + laje.tc - (res.a_linha_neutra_mm / 2.0)) / 1000.0;
        double braco_cabo_m = (perfil.d / 2.0 + (-ep_base_perfil_mm)) / 1000.0;

        res.MRd_VMP_kNm = res.T_aco_kN * braco_laje_m + res.T_cabo_kN * (braco_laje_m + braco_cabo_m);
    } else {
        res.a_linha_neutra_mm = laje.tc;
        res.MRd_VMP_kNm = res.Cc_max_kN * ((perfil.d / 2.0 + laje.tc / 2.0) / 1000.0);
    }

    return res;
}
