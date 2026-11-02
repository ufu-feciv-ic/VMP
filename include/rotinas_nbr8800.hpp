#pragma once
#include "modelos.hpp"

// ============================================================================
// ROTINAS NORMATIVAS BÁSICAS (ABNT NBR 8800:2008 / NBR 6118)
// Funções puras -> DTOs de resultados.
// ============================================================================

// ----------------------------------------------------------------------------
// Rotina A: Compressão Axial do Perfil de Aço (NRd)
// ----------------------------------------------------------------------------
struct ResultadoRotinaA {
    double Nex_kN;
    double Ney_kN;
    double Ne_kN;
    double Qs;
    double Qa;
    double Q;
    double lambda_0;
    double chi;
    double NRd_kN;
};

ResultadoRotinaA calcular_rotina_a(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Lv_mm,
    double Kx = 1.0,
    double Ky = 1.0,
    double gamma_a1 = 1.10
);

// ----------------------------------------------------------------------------
// Rotina B: Momento Fletor Resistente do Perfil de Aço (MRd)
// ----------------------------------------------------------------------------
struct ResultadoRotinaB {
    double lambda;
    double lambda_p;
    double lambda_r;
    double Mpl_kNm;
    double Mr_kNm;
    double MRd_kNm;
};

ResultadoRotinaB calcular_rotina_b(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Cb = 1.0,
    double gamma_a1 = 1.10
);

// ----------------------------------------------------------------------------
// Rotina D: Força Cortante Resistente do Perfil de Aço (VRd)
// ----------------------------------------------------------------------------
struct ResultadoRotinaD {
    double kv;
    double lambda_alma;
    double lambda_p;
    double lambda_r;
    double Vpl_kN;
    double VRd_kN;
};

ResultadoRotinaD calcular_rotina_d(
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
    const ConectorU& conector,
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
