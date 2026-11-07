#include "rotinas_nbr8800.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numbers>

const double PI = std::numbers::pi;

namespace nbr8800::inline v2008 {

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(const PerfilI& perfil, const Aco& aco, double Lb_mm, double lv_mm, double kx, double ky, double gamma_a1)
{
    ResultadoNormalResistentePerfil res;

    // Cálculo cargas críticas 
    res.Nex_kN = ((PI * PI * aco.E_MPa * perfil.Ix) / std::pow(kx * lv_mm, 2)) / 1000;
    res.Ney_kN = ((PI * PI * aco.E_MPa * perfil.Iy) / std::pow(ky * Lb_mm, 2)) / 1000;
    res.Ne_kN = std::min(res.Nex_kN, res.Ney_kN);

    if (perfil.tipo == TipoFabricacao::LAMINADO)
    {
        double lbf = perfil.lbf;
        double lbf_p = 0.56 * sqrt(aco.E_MPa / aco.fy_MPa);
        double lbf_r = 1.03 * sqrt(aco.E_MPa / aco.fy_MPa);

        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.74 * lbf * sqrt(aco.fy_MPa / aco.E_MPa);
        else res.Qs = 0.69 * aco.E_MPa / aco.fy_MPa * lbf * lbf;
    }
    else if (perfil.tipo == TipoFabricacao::SOLDADO)
    {
        double lbw = perfil.lbw;
        double kc = 4 / sqrt(lbw);
        
        double lbf = perfil.lbf;
        double lbf_p = 0.64 * sqrt(aco.E_MPa / (aco.fy_MPa / kc));
        double lbf_r = 1.17 * sqrt(aco.E_MPa / (aco.fy_MPa / kc));
        
        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.65 * lbf * sqrt(aco.fy_MPa / aco.E_MPa * kc);
        else res.Qs = 0.9 * aco.E_MPa * kc / aco.fy_MPa * lbf * lbf;
    }
    
    double lbw = perfil.lbw;
    double lbw_p = 1.49 * sqrt(aco.E_MPa / aco.fy_MPa);
    
    if (lbw <= lbw_p) res.Qa = 1.0;
    else
    {
        double lb0_1 = sqrt(perfil.Ag * aco.fy_MPa / (res.Ne_kN * 1000));
        
        double chi = (lb0_1 <= 1.5) ? std::pow(0.658, lb0_1 * lb0_1) : (0.877 / (lb0_1 * lb0_1));

        double sigma = chi * aco.fy_MPa/10; // unidades em kN/cm²

        double bef_a = 1.92 * (perfil.tw/10) * sqrt(aco.E_MPa/10 / sigma) 
        * (1 - ((0.34/lbw) * (sqrt(aco.E_MPa/10 / sigma)))); // unidades em cm
        
        bef_a = std::min(bef_a*10, perfil.d1);

        double Aef = perfil.d1 * (perfil.tw) - ((perfil.d1) - bef_a) * (perfil.tw); // unidades em cm

        res.Qa = Aef / (perfil.d1 * perfil.tw);
    }

    res.Q = res.Qs * res.Qa;

    res.lb0 = sqrt((res.Q * perfil.Ag * aco.fy_MPa) / (res.Ne_kN * 1000));

    res.chi = (res.lb0 <= 1.5) ? std::pow(0.658, res.lb0 * res.lb0) : (0.877 / (res.lb0 * res.lb0));

    res.NRd_kN = ((res.chi * res.Q * aco.fy_MPa * perfil.Ag) / gamma_a1) / 1000;

    return res;
}

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(const PerfilI& perfil, const Aco& aco, double a_enrikecedor_mm, double gamma_a1)
{
    ResultadoCortanteResistentePerfil res;

    res.kv = (a_enrikecedor_mm <= 0.0 || (a_enrikecedor_mm / perfil.h) || ((a_enrikecedor_mm / perfil.h) > std::pow(260.0 / (perfil.h / perfil.tw), 2.0))) 
    ? 5.0 : 5.0 + 5.0 / std::pow(a_enrikecedor_mm / perfil.h, 2.0);

    res.Vpl_kN = (0.6 * perfil.d * perfil.tw * aco.fy_MPa) / 1000;

    res.lbw = perfil.lbw;
    res.lbw_p = 1.1 * sqrt(res.kv * aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 1.37 * sqrt(res.kv * aco.E_MPa / aco.fy_MPa);

    if (res.lbw <= res.lbw_p) res.VRd_kN = res.Vpl_kN / gamma_a1;
    else if (res.lbw <= res.lbw_r) res.VRd_kN = (res.lbw_p / res.lbw) * (res.Vpl_kN / gamma_a1);
    else res.VRd_kN = 1.24 * std::pow(res.lbw_p / res.lbw_r, 2.0) * res.Vpl_kN / gamma_a1;

    return res;
}

double calcularMRdFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double gamma_a1, double Cb, double Mpl, double Mr)
{
    double lb = Lb_mm / perfil.ry;

    double lb_p = 1.76 * sqrt(aco.E_MPa / aco.fy_MPa);
    
    double beta1 = (0.7 * aco.fy_MPa * perfil.Wx) / (aco.E_MPa * perfil.It);

    double lb_r = ((1.38 * sqrt(perfil.Iy * perfil.It)) / (perfil.ry * perfil.It * beta1)) 
    * sqrt(1 + sqrt(1 + ((27 * perfil.Cw * beta1 * beta1) / perfil.Iy)));

    double Mcr = (((Cb * PI * PI * aco.E_MPa * perfil.Iy)/(Lb_mm * Lb_mm)) * 
    (sqrt((perfil.Cw / perfil.Iy)*(1 + 0.039*(perfil.It * Lb_mm * Lb_mm / perfil.Cw)))))/1E6;

    double MRd_kNm = 0.0;

    if (lb <= lb_p) MRd_kNm = Mpl / gamma_a1;
    else if (lb <= lb_r) 
    {
        MRd_kNm = (Cb / gamma_a1) * (Mpl - (Mpl - Mr)*((lb - lb_p)/(lb_r - lb_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl/gamma_a1); 
    }
    else 
    {
        MRd_kNm = Mcr / gamma_a1;
        MRd_kNm = std::min(MRd_kNm, Mpl/gamma_a1);
    }

    return MRd_kNm ;
}

double calcularMRdFLM(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr)
{
    double lbf = perfil.lbf;
    double lbf_p = 0.38 * sqrt(aco.E_MPa / aco.fy_MPa);
    
    double lbf_r = 0.0;
    double kc = 0.0;

    if (perfil.tipo == TipoFabricacao::LAMINADO) lbf_r = 0.83 * sqrt(aco.E_MPa / 0.7 * aco.fy_MPa);
    else if (perfil.tipo == TipoFabricacao::SOLDADO) 
    {
        kc = 4 / (sqrt(perfil.h / perfil.tw));
        kc = std::clamp(kc, 0.35, 0.76);

        lbf_r = 0.85 * sqrt(aco.E_MPa / 0.7 * (aco.fy_MPa / kc));
    }

    double MRd_kNm = 0.0;

    if (lbf <= lbf_p) MRd_kNm = Mpl / gamma_a1;
    else if (lbf <= lbf_r) 
    {
        MRd_kNm = (1 / gamma_a1) * (Mpl - (Mpl - Mr)*((lbf - lbf_p)/(lbf_r - lbf_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl/gamma_a1);
    }
    else 
    {
        if (perfil.tipo == TipoFabricacao::LAMINADO) 
        {
            double Mcr = (0.69 * aco.E_MPa * perfil.Wx / (lbf * lbf)) / 1e6;
            MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
        }
        else if (perfil.tipo == TipoFabricacao::SOLDADO)
        {
            double Mcr = (0.9 * kc * aco.E_MPa * perfil.Wx / (lbf * lbf)) / 1e6;
            MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
        }
    }

    return MRd_kNm;
}

double calcularMRdFLA(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr)
{
    double lbw = perfil.lbw;
    double lbw_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa);
    double lbw_r = 5.7 * sqrt(aco.E_MPa / aco.fy_MPa);

    double MRd_kNm = 0.0;

    if (lbw <= lbw_p) MRd_kNm = Mpl / gamma_a1;
    else if (lbw <= lbw_r) 
    {
        MRd_kNm = (1 / gamma_a1) * (Mpl - (Mpl - Mr)*((lbw - lbw_p)/(lbw_r - lbw_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl/gamma_a1);
    }
    else 
    {
        MRd_kNm = 0.0;
    }

    return MRd_kNm;
}

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb, double gamma_a1)
{
    ResultadoMomentoResistentePerfil res;

    res.Mpl_kNm = std::min(((perfil.Zx * aco.fy_MPa)/1E6), ((1.5 * perfil.Wx * aco.fy_MPa)/1E6));
    res.Mr_kNm = (0.7 * aco.fy_MPa * perfil.Wx) / 1E6;

    res.MRd_FLT_kNm = nbr8800::calcularMRdFLT(perfil, aco, Lb_mm, gamma_a1, Cb, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLM_kNm = nbr8800::calcularMRdFLM(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLA_kNm = nbr8800::calcularMRdFLA(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);

    if ((res.MRd_FLT_kNm || res.MRd_FLM_kNm || res.MRd_FLA_kNm) == 0.0) 
    {
        res.perfil_esbelto = true; 
        res.MRd_kNm = 0.0;
    }

    res.MRd_kNm = std::min({res.MRd_FLT_kNm, res.MRd_FLM_kNm, res.MRd_FLA_kNm});

    return res;
}


} // namespace nbr8800::v2008