#include "rotinas_nbr8800.h"
#include <cmath>
#include <algorithm>
#include <numbers>

const double PI = std::numbers::pi;

namespace nbr8800::inline v2008 {

ResultadoRotinaA calcular_rotina_a(const PerfilI& perfil, const Aco& aco, double Lb_mm, double lv_mm, double kx, double ky, double gamma_a1)
{
    ResultadoRotinaA res;

    // Cálculo cargas críticas 
    res.Nex_kN = ((PI * PI * aco.E * perfil.Ix) / std::pow(kx * lv_mm, 2)) / 1000;
    res.Ney_kN = ((PI * PI * aco.E * perfil.Iy) / std::pow(ky * Lb_mm, 2)) / 1000;
    res.Ne_kN = std::min(res.Nex_kN, res.Ney_kN);

    if (perfil.tipo == TipoFabricacao::LAMINADO)
    {
        double lbf = perfil.lbf;
        double lbf_p = 0.56 * sqrt(aco.E / aco.fy);
        double lbf_r = 1.03 * sqrt(aco.E / aco.fy);

        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.74 * lbf * sqrt(aco.fy / aco.E);
        else res.Qs = 0.69 * aco.E / aco.fy * lbf * lbf;
    }
    else if (perfil.tipo == TipoFabricacao::SOLDADO)
    {
        double lbw = perfil.lbw;
        double kc = 4 / sqrt(lbw);
        
        double lbf = perfil.lbf;
        double lbf_p = 0.64 * sqrt(aco.E / (aco.fy / kc));
        double lbf_r = 1.17 * sqrt(aco.E / (aco.fy / kc));
        
        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.65 * lbf * sqrt(aco.fy / aco.E * kc);
        else res.Qs = 0.9 * aco.E * kc / aco.fy * lbf * lbf;
    }
    
    double lbw = perfil.lbw;
    double lbw_p = 1.49 * sqrt(aco.E / aco.fy);
    
    if (lbw <= lbw_p) res.Qa = 1.0;
    else
    {
        double lb0_1 = sqrt(perfil.Ag * aco.fy / (res.Ne_kN * 1000));
        
        double chi = (lb0_1 <= 1.5) ? std::pow(0.658, lb0_1 * lb0_1) : (0.877 / (lb0_1 * lb0_1));

        double sigma = chi * aco.fy;

        double bef_a = 1.92 * perfil.tw * sqrt(aco.E / sigma) * 
                        (1 - (0.34/lbw) * (sqrt(aco.E / sigma)));
        
        bef_a = std::min(bef_a, perfil.d1);

        double Aef = perfil.d1 * perfil.tw - (perfil.d1 - bef_a) * perfil.tw;

        res.Qa = Aef / perfil.d1 * perfil.tw;
    }

    res.Q = res.Qs * res.Qa;

    res.lb0 = sqrt((res.Q * perfil.Ag * aco.fy) / (res.Ne_kN * 1000));

    res.chi = (res.lb0 <= 1.5) ? std::pow(0.658, res.lb0 * res.lb0) : (0.877 / (res.lb0 * res.lb0));

    res.NRd_kN = ((res.chi * res.Q * aco.fy * perfil.Ag) / gamma_a1) / 1000;

    return res;
}

} // namespace nbr8800::v2008