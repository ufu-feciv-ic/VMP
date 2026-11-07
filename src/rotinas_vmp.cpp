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
    
    return res;
}

}