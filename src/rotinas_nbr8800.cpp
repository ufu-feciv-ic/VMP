#include "rotinas_nbr8800.hpp"
#include <cmath>
#include <algorithm>

const double PI = 3.14159265358979323846;

// ----------------------------------------------------------------------------
// Rotina A: Compressão Axial
// ----------------------------------------------------------------------------
ResultadoRotinaA calcular_rotina_a(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Lv_mm,
    double Kx,
    double Ky,
    double gamma_a1
) {
    ResultadoRotinaA res;

    // Euler global x-x e y-y
    res.Nex_kN = (PI * PI * aco.E * perfil.Ix) / std::pow(Kx * Lv_mm, 2.0) / 1000.0;
    res.Ney_kN = (PI * PI * aco.E * perfil.Iy) / std::pow(Ky * Lb_mm, 2.0) / 1000.0;
    res.Ne_kN = std::min(res.Nex_kN, res.Ney_kN);

    // Mesa (Elemento AL)
    double lambda_mesa = perfil.bf / (2.0 * perfil.tf);
    double lambda_p_mesa = 0.56 * std::sqrt(aco.E / aco.fy);
    double lambda_r_mesa = 1.03 * std::sqrt(aco.E / aco.fy); // laminados
    if (lambda_mesa <= lambda_p_mesa) {
        res.Qs = 1.0;
    } else if (lambda_mesa <= lambda_r_mesa) {
        res.Qs = 1.415 - 0.74 * lambda_mesa * std::sqrt(aco.fy / aco.E);
    } else {
        res.Qs = (0.69 * aco.E) / (aco.fy * lambda_mesa * lambda_mesa);
    }

    // Alma (Elemento AA)
    double d1 = (perfil.d1 > 0.0) ? perfil.d1 : (perfil.d - 2.0 * perfil.tf);
    double lambda_alma = d1 / perfil.tw;
    double lambda_p_alma = 1.49 * std::sqrt(aco.E / aco.fy);

    if (lambda_alma <= lambda_p_alma) {
        res.Qa = 1.0;
    } else {
        double lambda_0_alma = std::sqrt(perfil.Ag * aco.fy / (res.Ne_kN * 1000.0));
        double chi_alma = (lambda_0_alma <= 1.5) ? std::pow(0.658, lambda_0_alma * lambda_0_alma) : (0.877 / (lambda_0_alma * lambda_0_alma));
        double sigma = chi_alma * aco.fy;
        double befa = 1.92 * perfil.tw * std::sqrt(aco.E / sigma) * (1.0 - (0.34 / lambda_alma) * std::sqrt(aco.E / sigma));
        befa = std::min(befa, d1);
        double Aef = perfil.Ag - (d1 - befa) * perfil.tw;
        res.Qa = Aef / perfil.Ag;
    }

    res.Q = res.Qs * res.Qa;

    // Redução global chi
    res.lambda_0 = std::sqrt(res.Q * perfil.Ag * aco.fy / (res.Ne_kN * 1000.0));
    if (res.lambda_0 <= 1.5) {
        res.chi = std::pow(0.658, res.lambda_0 * res.lambda_0);
    } else {
        res.chi = 0.877 / (res.lambda_0 * res.lambda_0);
    }

    res.NRd_kN = (res.chi * res.Q * perfil.Ag * aco.fy / gamma_a1) / 1000.0;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina B: Flexão (FLT)
// ----------------------------------------------------------------------------
ResultadoRotinaB calcular_rotina_b(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Cb,
    double gamma_a1
) {
    ResultadoRotinaB res;

    res.lambda = Lb_mm / perfil.ry;
    res.lambda_p = 1.76 * std::sqrt(aco.E / aco.fy);

    double beta1 = (0.7 * aco.fy * perfil.Wx) / (aco.E * perfil.It);
    res.lambda_r = ((1.38 * std::sqrt(perfil.Iy * perfil.It)) / (perfil.ry * perfil.It * beta1))
                   * std::sqrt(1.0 + std::sqrt(1.0 + (27.0 * perfil.Cw * beta1 * beta1 / perfil.Iy)));

    double Mpl_Nmm = std::min(perfil.Zx * aco.fy, 1.5 * perfil.Wx * aco.fy);
    double Mr_Nmm = 0.7 * aco.fy * perfil.Wx;

    res.Mpl_kNm = Mpl_Nmm / 1e6;
    res.Mr_kNm = Mr_Nmm / 1e6;

    double MRd_Nmm = 0.0;
    if (res.lambda <= res.lambda_p) {
        MRd_Nmm = Mpl_Nmm / gamma_a1;
    } else if (res.lambda > res.lambda_p && res.lambda <= res.lambda_r) {
        MRd_Nmm = (Cb / gamma_a1) * (Mpl_Nmm - (Mpl_Nmm - Mr_Nmm) * ((res.lambda - res.lambda_p) / (res.lambda_r - res.lambda_p)));
        MRd_Nmm = std::min(MRd_Nmm, Mpl_Nmm / gamma_a1);
    } else {
        double Mcr_Nmm = ((Cb * PI * PI * aco.E * perfil.Iy) / (Lb_mm * Lb_mm))
                         * std::sqrt((perfil.Cw / perfil.Iy) * (1.0 + 0.039 * perfil.It * Lb_mm * Lb_mm / perfil.Cw));
        MRd_Nmm = std::min(Mcr_Nmm / gamma_a1, Mpl_Nmm / gamma_a1);
    }

    res.MRd_kNm = MRd_Nmm / 1e6;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina D: Cortante
// ----------------------------------------------------------------------------
ResultadoRotinaD calcular_rotina_d(
    const PerfilI& perfil,
    const Aco& aco,
    double a_enrijecedor_mm,
    double gamma_a1
) {
    ResultadoRotinaD res;
    double h = perfil.d - 2.0 * perfil.tf;
    res.lambda_alma = h / perfil.tw;

    if (a_enrijecedor_mm <= 0.0 || (a_enrijecedor_mm / h) > 3.0) {
        res.kv = 5.0;
    } else {
        res.kv = 5.0 + 5.0 / std::pow(a_enrijecedor_mm / h, 2.0);
    }

    res.lambda_p = 1.10 * std::sqrt(res.kv * aco.E / aco.fy);
    res.lambda_r = 1.37 * std::sqrt(res.kv * aco.E / aco.fy);

    double Aw = perfil.d * perfil.tw;
    double Vpl_N = 0.60 * Aw * aco.fy;
    res.Vpl_kN = Vpl_N / 1000.0;

    double VRd_N = 0.0;
    if (res.lambda_alma <= res.lambda_p) {
        VRd_N = Vpl_N / gamma_a1;
    } else if (res.lambda_alma <= res.lambda_r) {
        VRd_N = (res.lambda_p / res.lambda_alma) * (Vpl_N / gamma_a1);
    } else {
        VRd_N = 1.24 * std::pow(res.lambda_p / res.lambda_alma, 2.0) * (Vpl_N / gamma_a1);
    }

    res.VRd_kN = VRd_N / 1000.0;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina E: Conectores Perfil U
// ----------------------------------------------------------------------------
ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorU& conector,
    double Lv_mm,
    double gamma_a1,
    double gamma_c,
    double gamma_cs
) {
    ResultadoRotinaE res;

    res.beff_mm = std::min(Lv_mm / 4.0, laje.largura_efetiva);

    double tf_cm = conector.tf / 10.0;
    double tw_cm = conector.tw / 10.0;
    double Lcs_cm = conector.Lcs / 10.0;
    double fck_kN_cm2 = concreto.fck / 10.0;
    double Ecs_kN_cm2 = concreto.Ecs / 10.0;

    res.QRd_kN = 0.3 * (tf_cm + 0.5 * tw_cm) * Lcs_cm * std::sqrt(fck_kN_cm2 * Ecs_kN_cm2) / gamma_cs;

    double Fhd_aco = (perfil.Ag * aco.fy / gamma_a1) / 1000.0;
    double Fhd_conc = (0.85 * (concreto.fck / gamma_c) * res.beff_mm * laje.tc) / 1000.0;
    res.Fhd_kN = std::min(Fhd_aco, Fhd_conc);

    int n_metade = static_cast<int>(std::ceil(res.Fhd_kN / res.QRd_kN));
    res.nl_conectores = 2 * n_metade;
    res.el_espacamento_mm = Lv_mm / res.nl_conectores;
    return res;
}

// ----------------------------------------------------------------------------
// Rotina F: Armadura de Costura
// ----------------------------------------------------------------------------
ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c
) {
    ResultadoRotinaF res;

    double beff_metade_cm = (laje.largura_efetiva / 2.0) / 10.0;
    double beff_total_cm = laje.largura_efetiva / 10.0;
    double Lv_metade_cm = (Lv_mm / 2.0) / 10.0;
    double tc_cm = laje.tc / 10.0;

    res.Hv_Sd_kN_cm = (Fhd_kN * beff_metade_cm) / (beff_total_cm * Lv_metade_cm);

    double fck_kN_cm2 = concreto.fck / 10.0;
    res.Hv_Rd_max_kN_cm = (0.2 * tc_cm * fck_kN_cm2) / gamma_c;
    res.As_cos_cm2_m = 0.2 * tc_cm;
    res.lb_comprimento_cm = 144.0;
    return res;
}
