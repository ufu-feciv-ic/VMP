#pragma once
#include "modelos.h"


namespace nbr8800 {
inline namespace v2008 {
// ============================================================================
// ROTINAS NORMATIVAS BÁSICAS (ABNT NBR 8800:2008 / NBR 6118)
// ============================================================================

// ----------------------------------------------------------------------------
// Rotina A: Compressão Axial do Perfil de Aço (NRd)
// ----------------------------------------------------------------------------
struct ResultadoNormalResistentePerfil {
    double Nex_kN;
    double Ney_kN;
    double Ne_kN;
    double Qs;
    double Qa;
    double Q;
    double lb0;
    double chi;
    double NRd_kN;
};

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Lv_mm,
    double kx,
    double ky,
    double gamma_a1 = 1.10
);

// ----------------------------------------------------------------------------
// Rotina B: Momento Fletor Resistente do Perfil de Aço (MRd)
// ----------------------------------------------------------------------------
struct ResultadoMomentoResistentePerfil {
    // double lb;
    // double lb_p;
    // double lb_r;
    double Mpl_kNm;
    double Mr_kNm;
    double MRd_FLT_kNm;
    double MRd_FLM_kNm;
    double MRd_FLA_kNm;
    double MRd_kNm;
};

double calcularMRdFLT (const PerfilI& perfil, const Aco& aco, double lb_mm, double gamma_a1, double Cb, double Mpl, double Mr);
double calcularMRdFLM (const PerfilI& perfil);
double calcularMRdFLA (const PerfilI& perfil);

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Cb = 1.0,
    double gamma_a1 = 1.10
);

// ----------------------------------------------------------------------------
// Rotina D: Força Cortante Resistente do Perfil de Aço (VRd)
// ----------------------------------------------------------------------------
struct ResultadoCortanteResistentePerfil {
    double kv;
    double lbw; // limite de esbeltez da alma
    double lbw_p; // limite de esbeltez plástica da alma
    double lbw_r; // limite de esbeltez elástica da alma
    double Vpl_kN;
    double VRd_kN;
};

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double a_enrijecedor_mm = 0.0,
    double gamma_a1 = 1.10
);

// ----------------------------------------------------------------------------
// Rotina E: Conectores de Cisalhamento Perfil U (QRd, nl, el)
// ----------------------------------------------------------------------------
struct ResultadoRotinaE {
    double beff_mm;
    double QRd_kN;
    double Fhd_kN;
    int nl_conectores;
    double el_espacamento_mm;
};

ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double gamma_cs = 1.25
);

// ----------------------------------------------------------------------------
// Rotina F: Armadura de Costura contra Fendilhamento (As,cos)
// ----------------------------------------------------------------------------
struct ResultadoRotinaF {
    double Hv_Sd_kN_cm;
    double Hv_Rd_max_kN_cm;
    double As_cos_cm2_m;
    double lb_comprimento_cm;
};

ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c = 1.40
);

} // namespace v2008
} // namespace nbr8800