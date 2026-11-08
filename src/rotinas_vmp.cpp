#include "rotinas_vmp.h"
#include <cmath>
#include <algorithm>

namespace nbr8800::inline v2008 {

ResultadoSecaoTransformada calcularSecaoTransformada(const PerfilI& perfil, const Laje& laje, const Concreto& concreto, 
    const Aco& aco, double multiplicador_fluencia, double ep_perfil_inferior_mm, bool tempo_imediato)
{
    ResultadoSecaoTransformada res;

    if (tempo_imediato) res.n_relacao_modular = concreto.Ecs_MPa / aco.E_MPa;
    else res.n_relacao_modular = concreto.Ecs_MPa / (3 * aco.E_MPa);
    
    res.btr_mm = laje.bef_mm * res.n_relacao_modular;
    res.Atr_mm2 = (res.btr_mm * laje.tc_mm) + (perfil.Ag);

    res.ytr_mm = ((res.btr_mm * laje.tc_mm * (laje.tc_mm / 2 + laje.hF_mm + perfil.d)) + (perfil.Ag * perfil.d / 2)) / (res.Atr_mm2);

    res.ay_mm = std::min(perfil.d + laje.hF_mm + laje.tc_mm - res.ytr_mm, laje.tc_mm);

    res.Itr_mm4 = res.btr_mm * pow(res.ay_mm, 3) / 12
    + res.btr_mm * res.ay_mm * pow(perfil.d + laje.hF_mm + res.ay_mm / 2 - res.ytr_mm, 2)
    + perfil.Ix 
    + perfil.Ag * pow(res.ytr_mm - perfil.d / 2, 2);

    res.Wtr_inf_mm3 = res.Itr_mm4 / res.ytr_mm;

    res.Wtr_sup_mm3 = res.Itr_mm4 / (perfil.d + laje.hF_mm + laje.tc_mm - res.ytr_mm);

    return res;
}

ResultadoEstimativaP0 calcularEstimativaP0max(const PerfilI& perfil, const Aco& aco, const Concreto& concreto,
    const CaboProtensao& cabo, double NRd_kN, double MRd_kN, double MSd_g_kN, double MSd_q_kN, 
    double gamma_p, double beta, double psi_1, double ytr_mm, double Atr_mm, double Wtr_inf_mm, double Wtr_sup_mm)
{
    double P0_max_1 = 0.0;
    double ep_I = perfil.d / 2 - cabo.ep;
    double ep_tr = ytr_mm - cabo.ep;

    if (cabo.tipo == TipoProtensao::PRE_TRACAO)
    {
        P0_max_1 = -1 / (gamma_p * beta * (1/ NRd_kN + 8*ep_I / 9*MRd_kN));

        P0_max_1 = (P0_max_1 / NRd_kN >= 0.2) ? P0_max_1 : (-1 / 
            (gamma_p * beta * (1 / (2*NRd_kN) + ep_I / MRd_kN)));
    }
    else if (cabo.tipo == TipoProtensao::POS_TRACAO)
    {
        P0_max_1 = std::numeric_limits<double>::infinity();
    }

    double P0_max_2 = ((-MSd_g_kN/Wtr_inf_mm) + (-MSd_q_kN / Wtr_inf_mm)) / 
    ((1.1 / Atr_mm)+(1.1 * ep_tr / Wtr_inf_mm));

    double sigma_c = (MSd_g_kN / Wtr_sup_mm) + (MSd_q_kN / Wtr_sup_mm)
    + (P0_max_2 / Atr_mm) + (P0_max_2 * ep_tr / Wtr_sup_mm);

    double alpha_E = 3 * aco.E_MPa / concreto.Ecs_MPa;

    P0_max_2 = ((sigma_c / alpha_E) <= (-0.6 / concreto.fck_MPa)) ? P0_max_2 : 
    ((-0.6 * concreto.fck_MPa) + (((-MSd_g_kN/Wtr_inf_mm) + (-MSd_q_kN * psi_1 / Wtr_inf_mm)) / 
    ((1.1 / Atr_mm)+(1.1 * ep_tr / Wtr_inf_mm))));
    
    ResultadoEstimativaP0 res;

    res.P0_max_kN = std::min(P0_max_1, P0_max_2);

    return res;
}

}