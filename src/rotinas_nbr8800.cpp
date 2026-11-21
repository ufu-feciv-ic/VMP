#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
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

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(const PerfilI& perfil, const Aco& aco, 
    const Concreto& concreto, const Laje& laje, double gamma_a1, double gamma_c)
{
    ResultadoMomentoPositivoVM res;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.7 * sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    if (res.lbw <= res.lbw_p)
    {
        res.Cc_max_kN = 0.85 * fcd * laje.bef_mm * laje.tc_mm * 1e-3;
        res.T_aco_kN = perfil.Ag * fyd * 1e-3;

        if (res.Cc_max_kN >= res.T_aco_kN)
        {
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3) / (0.85 * fcd * laje.bef_mm);

            res.MRd_VM_kNm = res.T_aco_kN * (perfil.d / 2.0 + laje.hF_mm + 
                laje.tc_mm - res.a_linha_neutra_mm / 2.0) * 1e-3;
        }
        else 
        {
            res.C_aco_kN = 0.5 * (perfil.Ag * fyd * 1e-3 - res.Cc_max_kN);
            res.T_aco_kN = res.C_aco_kN + res.Cc_max_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= perfil.bf * perfil.tf * fyd * 1e-3)
            {
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);
                yc = yp / 2.0;

                yt = 
                (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                + (perfil.h * perfil.tw * (perfil.h / 2.0 + perfil.tf))
                + ((perfil.bf / 2.0) * (perfil.tf - yp) * (3.0 * perfil.tf + 2.0 * perfil.h - yp)))
                / ((perfil.bf * perfil.tf) + (perfil.h * perfil.tw) + (perfil.bf * (perfil.tf - yp))));
            }
            else 
            {
                yp = 
                perfil.tf + (perfil.h 
                * (((res.C_aco_kN * 1e3) - (perfil.bf * perfil.tf * fyd))
                / (perfil.h * perfil.tw * fyd)));

                yc = 
                (((perfil.bf * perfil.tf * perfil.tf / 2.0) 
                + (perfil.tw * (yp - perfil.tf) * ((yp + perfil.tf) / 2.0))) 
                / ((perfil.bf * perfil.tf)
                + ((yp - perfil.tf) * perfil.tw)));

                yt = 
                (((perfil.bf * perfil.tf * perfil.tf / 2.0) 
                + (perfil.tw * (perfil.d - yp - perfil.tf) * ((perfil.d - yp + perfil.tf) / 2.0))) 
                / ((perfil.bf * perfil.tf)
                + ((perfil.d - yp - perfil.tf) * perfil.tw)));
            }

            res.y_lnp_mm = yp;
            res.MRd_VM_kNm = (res.C_aco_kN * (perfil.d - yt - yc) 
                + res.Cc_max_kN * (laje.tc_mm + laje.hF_mm + perfil.d - yt)) * 1e-3;
        }
    }
    else if (res.lbw <= res.lbw_r)
    {
        auto secao_tr = nbr8800::calcularSecaoTransformada(perfil, laje, 
            concreto, aco, 1.0, 0.0, true);

        res.MRd_VM_kNm = std::min(secao_tr.Wtr_inf_mm3 * fyd, 
            (1.0 / secao_tr.n_relacao_modular) * secao_tr.Wtr_sup_mm3 * fcd) * 1e-6;
    }
    else 
    {
        res.aplicavel = false;
        res.MRd_VM_kNm = 0.0;
        return res;
    }
    
    return res;
}

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(const PerfilI& perfil, 
    const Aco& aco, const Concreto& concreto, const Laje& laje, 
    const ArmacaoLaje& armacaoLaje, double MSd_neg_kNm, double MSd_pos_kNm, 
    double Lv_mm, double a_vigas_mm, double alpha_viga, double gamma_a1, double gamma_c)
{
    ResultadoMomentoNegativoVM res;

    double lbf = perfil.lbf;
    double lbf_p = 0.38 * sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    double Tds = armacaoLaje.Ads * armacaoLaje.fsd;
    double Tad = 0.0;
    double Cad = 0.0;
    double Ccd = 0.0;
    double yp = 0.0;
    double yc = 0.0;
    double yt = 0.0;
    double d3 = 0.0;
    double d4 = 0.0;
    double d5 = 0.0;

    if (lbf <= lbf_p)
    {
        double lb = 0.0;
        double lb_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa);

        if ((Tds + perfil.bf * perfil.tf * fyd) >= 
        (perfil.h * perfil.tw + perfil.tf * perfil.bf) * fyd)
        {
            Tad = (perfil.Ag * fyd - Tds) / 2;

            yp = Tad / (perfil.bf * fyd);

            lb = 2 * perfil.d1 / perfil.tw;

            if (lb <= lb_p)
            {
                yc = 
                ((perfil.bf * perfil.tf * perfil.tf / 2)
                + perfil.h * perfil.tw * ((perfil.h / 2) + perfil.tf)
                + perfil.bf / 2 * (perfil.tf - yp) * (3 * perfil.tf + 2 * perfil.h - yp))
                / 
                ((perfil.bf * perfil.tf)
                + (perfil.h * perfil.tw)
                + perfil.bf * (perfil.tf - yp));

                yt = yp / 2;

                d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;

                d4 = yt;

                d5 = perfil.d - yp - yc;
                
                res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad)  * d5)*1e-6;
            }
            else
            {
                res.aplicavel = false;
                res.MRd_VM_kNm = 0.0;
            }
        }
        else
        {
            Tad = (perfil.Ag * fyd - Tds) / 2;

            yp = perfil.tf + perfil.h * 
            ((Tad - (perfil.bf * perfil.tf * fyd)) / (perfil.h * perfil.tw * fyd));

            lb = (2 * (perfil.h - yp + perfil.tf - perfil.r)) / (perfil.tw);

            if (lb <= lb_p)
            {
                yc = 
                ((perfil.bf * perfil.tf * perfil.tf / 2)
                + (perfil.d - yp - perfil.tf) * perfil.tw * ((perfil.d - yp + perfil.tf) / 2))
                / 
                ((perfil.bf * perfil.tf)
                + (perfil.d - yp - perfil.tf) * perfil.tw);

                yt = 
                ((perfil.bf * perfil.tf * perfil.tf / 2)
                + (yp - perfil.tf) * perfil.tw * ((yp + perfil.tf) / 2))
                / 
                ((perfil.bf * perfil.tf)
                + (yp - perfil.tf) * perfil.tw);

                d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;

                d4 = yp - yt;

                d5 = perfil.d - yp - yc;

                res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad) * d5)*1e-6;
            }
            else
            {
                res.aplicavel = false;
                res.MRd_VM_kNm = 0.0;
            }
        }
    }
    else 
    {
        res.aplicavel = false;
        res.MRd_VM_kNm = 0.0;
    }

    double h0 = perfil.d - perfil.tf;

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    double I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

    double k1 = alpha_viga * aco.E_MPa * I_laje / a_vigas_mm;

    double k2 = (aco.E_MPa * std::pow(perfil.tw, 3)) / (3.64 * h0);

    double kr = (k1 * k2) / (k1 + k2);

    yc = perfil.d / 2.0 + laje.hF_mm + laje.tc_mm / 2.0;

    double A = perfil.Ag + armacaoLaje.Ads;

    double e = (A * perfil.Ix) / (perfil.Ag * yc * (A - perfil.Ag));

    double ytr = ((perfil.Ag * perfil.d / 2.0) 
    + armacaoLaje.Ads * (perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm))
    / (perfil.Ag + armacaoLaje.Ads);

    double Itr = 
    perfil.Ix 
    + perfil.Ag * std::pow(ytr - (perfil.d / 2.0), 2)
    + armacaoLaje.Ads * std::pow(perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm - ytr, 2);

    double alpha_g = 
    (h0 * Itr / perfil.Ix)
    / (((std::pow(h0, 2) / 4.0) + ((perfil.Ix + perfil.Iy) / perfil.Ag)) / (e + h0));

    double Iaf_y = std::pow(perfil.bf, 3) * perfil.tf / 12.0;

    double psi = MSd_neg_kNm / MSd_pos_kNm;

    double Cdist = nbr8800::v2008::calcularCdist(psi);

    double Mcr = (alpha_g * Cdist / Lv_mm) *
    sqrt(((aco.G_MPa * perfil.It) + (kr * Lv_mm * Lv_mm / (PI * PI))) * aco.E_MPa * Iaf_y);

    double lb_dist = sqrt(MSd_neg_kNm*1e6 / Mcr);

    double X_dist = 0.0;

    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;

    res.MRd_VM_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

} // namespace nbr8800::v2008

// ============================================================================
// IMPLEMENTAÇÃO DO MÓDULO ABNT NBR 8800:2024 (namespace nbr8800::v2024)
// ============================================================================
namespace nbr8800::v2024 {

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double lv_mm, double kx, double ky, double gamma_a1
) {
    ResultadoNormalResistentePerfil res{};

    // 1. Cargas críticas elásticas de Euler (eixos X e Y)
    res.Nex_kN = ((PI * PI * aco.E_MPa * perfil.Ix) / std::pow(kx * lv_mm, 2)) / 1000.0;
    res.Ney_kN = ((PI * PI * aco.E_MPa * perfil.Iy) / std::pow(ky * Lb_mm, 2)) / 1000.0;
    res.Ne_kN = std::min(res.Nex_kN, res.Ney_kN);

    // 2. Na ABNT NBR 8800:2024, o índice de esbeltez reduzido global lambda_0
    // é calculado diretamente com a área bruta Ag, e a flambagem local é tratada
    // pelo Método da Largura/Área Efetiva (Aef) unificado (c1, c2).
    res.lb0 = std::sqrt((perfil.Ag * aco.fy_MPa) / (res.Ne_kN * 1000.0));
    res.chi = (res.lb0 <= 1.5) ? std::pow(0.658, res.lb0 * res.lb0) : (0.877 / (res.lb0 * res.lb0));

    double fn_MPa = res.chi * aco.fy_MPa; // Tensão crítica global fn = chi * fy

    // 3. Verificação de flambagem local das mesas (elementos AL) pela curva unificada de Winter
    double lbf = perfil.lbf;
    double lbf_r = 0.56 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    if (perfil.tipo == TipoFabricacao::SOLDADO) {
        double kc = std::clamp(4.0 / std::sqrt(perfil.lbw), 0.35, 0.76);
        lbf_r = 0.64 * std::sqrt(kc * aco.E_MPa / aco.fy_MPa);
    }

    double delta_A_mesas = 0.0;
    double lbf_ef = lbf_r * std::sqrt(aco.fy_MPa / fn_MPa);
    if (lbf > lbf_ef) {
        // Coeficientes unificados c1 = 0.22, c2 = 1.49 para elementos AL
        constexpr double c1_f = 0.22;
        constexpr double c2_f = 1.49;
        double fel_f = std::pow(c2_f * lbf_r / lbf, 2.0) * aco.fy_MPa;
        double razao_f = std::sqrt(fel_f / fn_MPa);
        double b_meia_mesa = perfil.bf / 2.0;
        double bef_meia_mesa = b_meia_mesa * (1.0 - c1_f * razao_f) * razao_f;
        bef_meia_mesa = std::min(bef_meia_mesa, b_meia_mesa);
        delta_A_mesas = 4.0 * (b_meia_mesa - bef_meia_mesa) * perfil.tf;
    }

    // 4. Verificação de flambagem local da alma (elemento AA) pela curva unificada de Winter
    double lbw = perfil.lbw;
    double lbw_r = 1.49 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double lbw_ef = lbw_r * std::sqrt(aco.fy_MPa / fn_MPa);

    double delta_A_alma = 0.0;
    if (lbw > lbw_ef) {
        // Coeficientes unificados c1 = 0.18, c2 = 1.31 para elementos AA
        constexpr double c1_w = 0.18;
        constexpr double c2_w = 1.31;
        double fel_w = std::pow(c2_w * lbw_r / lbw, 2.0) * aco.fy_MPa;
        double razao_w = std::sqrt(fel_w / fn_MPa);
        double hef_alma = perfil.d1 * (1.0 - c1_w * razao_w) * razao_w;
        hef_alma = std::min(hef_alma, perfil.d1);
        delta_A_alma = (perfil.d1 - hef_alma) * perfil.tw;
    }

    double Aef_mm2 = std::max(0.0, perfil.Ag - delta_A_mesas - delta_A_alma);

    // Preenche Qs, Qa e Q equivalentes (Aef / Ag) para manter rastreabilidade comparativa com 2008
    res.Qs = (perfil.Ag > 0.0) ? ((perfil.Ag - delta_A_mesas) / perfil.Ag) : 1.0;
    res.Qa = (perfil.Ag > 0.0) ? ((perfil.Ag - delta_A_alma) / perfil.Ag) : 1.0;
    res.Q  = (perfil.Ag > 0.0) ? (Aef_mm2 / perfil.Ag) : 1.0;

    res.NRd_kN = ((res.chi * Aef_mm2 * aco.fy_MPa) / gamma_a1) / 1000.0;
    return res;
}

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double a_enrijecedor_mm, double gamma_a1
) {
    ResultadoCortanteResistentePerfil res{};

    // Na revisão alinhada ao AISC 360 / NBR 8800:2024, almas de perfis I sem enrijecedores
    // transversais adotam kv = 5.34 (em vez de 5.0 na edição 2008)
    bool sem_enrijecedor = (a_enrijecedor_mm <= 0.0)
        || ((a_enrijecedor_mm / perfil.h) > 3.0)
        || ((a_enrijecedor_mm / perfil.h) > std::pow(260.0 / (perfil.h / perfil.tw), 2.0));

    res.kv = sem_enrijecedor ? 5.34 : (5.0 + 5.0 / std::pow(a_enrijecedor_mm / perfil.h, 2.0));
    res.Vpl_kN = (0.60 * perfil.d * perfil.tw * aco.fy_MPa) / 1000.0;

    res.lbw = perfil.lbw;
    res.lbw_p = 1.10 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 1.37 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);

    if (res.lbw <= res.lbw_p) {
        res.VRd_kN = res.Vpl_kN / gamma_a1;
    } else if (res.lbw <= res.lbw_r) {
        res.VRd_kN = (res.lbw_p / res.lbw) * (res.Vpl_kN / gamma_a1);
    } else {
        // Regime de flambagem elástica por cisalhamento: 1.24 * (lambda_p / lambda_w)^2 * Vpl / gamma_a1
        res.VRd_kN = 1.24 * std::pow(res.lbw_p / res.lbw, 2.0) * (res.Vpl_kN / gamma_a1);
    }

    return res;
}

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb, double gamma_a1
) {
    return nbr8800::v2008::calcularMomentoResistentePerfil(perfil, aco, Lb_mm, Cb, gamma_a1);
}

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(
    const PerfilI& perfil, const Aco& aco, const Concreto& concreto, const Laje& laje,
    double gamma_a1, double gamma_c
) {
    ResultadoMomentoPositivoVM res{};

    // Fator de fragilidade do concreto na ABNT NBR 8800:2024 (Anexo L, Item L.5.3.4 / L.6.3.1.1.1, p. 197 e 202):
    // eta_c = (40 / fck)^(1/3) <= 1.0
    double eta_c = (concreto.fck_MPa > 40.0)
        ? std::min(1.0, std::cbrt(40.0 / concreto.fck_MPa))
        : 1.0;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    if (res.lbw <= res.lbw_p) {
        // Interação completa pressuposta (ABNT NBR 8800:2024, Item L.6.3.1.1.1, pp. 202-203)
        res.Cc_max_kN = 0.85 * eta_c * fcd * laje.bef_mm * laje.tc_mm * 1e-3;
        res.T_aco_kN = perfil.Ag * fyd * 1e-3;

        if (res.Cc_max_kN >= res.T_aco_kN) {
            // Item L.6.3.1.1.1-a (p. 202): LNP na laje de concreto
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3) / (0.85 * eta_c * fcd * laje.bef_mm);
            res.MRd_VM_kNm = res.T_aco_kN * (perfil.d / 2.0 + laje.hF_mm + laje.tc_mm - res.a_linha_neutra_mm / 2.0) * 1e-3;
        } else {
            // Item L.6.3.1.1.1-b (p. 203): Interação completa com LNP no perfil de aço
            res.C_aco_kN = 0.5 * (perfil.Ag * fyd * 1e-3 - res.Cc_max_kN);
            res.T_aco_kN = res.C_aco_kN + res.Cc_max_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= perfil.bf * perfil.tf * fyd * 1e-3) {
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);
                yc = yp / 2.0;
                yt = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.h * perfil.tw * (perfil.h / 2.0 + perfil.tf))
                    + ((perfil.bf / 2.0) * (perfil.tf - yp) * (3.0 * perfil.tf + 2.0 * perfil.h - yp)))
                    / ((perfil.bf * perfil.tf) + (perfil.h * perfil.tw) + (perfil.bf * (perfil.tf - yp))));
            } else {
                yp = perfil.tf + (perfil.h * (((res.C_aco_kN * 1e3) - (perfil.bf * perfil.tf * fyd)) / (perfil.h * perfil.tw * fyd)));
                yc = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.tw * (yp - perfil.tf) * ((yp + perfil.tf) / 2.0)))
                    / ((perfil.bf * perfil.tf) + ((yp - perfil.tf) * perfil.tw)));
                yt = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.tw * (perfil.d - yp - perfil.tf) * ((perfil.d - yp + perfil.tf) / 2.0)))
                    / ((perfil.bf * perfil.tf) + ((perfil.d - yp - perfil.tf) * perfil.tw)));
            }

            res.y_lnp_mm = yp;

            // Na ABNT NBR 8800:2024 (Item L.6.3.1.1.1-b, p. 203), para interação completa com LNP no perfil de aço:
            // MRd = beta_vm * [ sum_QRd * (d - yt - yc) + Ccd * (tc/2 + hF + d - yt) ]
            // onde sob interação completa com LNP no perfil de aço: sum_QRd = 0.85 * eta_c * fcd * b * tc = Ccd
            double soma_QRd_kN = res.Cc_max_kN;
            res.MRd_VM_kNm = (soma_QRd_kN * (perfil.d - yt - yc)
                + res.Cc_max_kN * (laje.tc_mm / 2.0 + laje.hF_mm + perfil.d - yt)) * 1e-3;
        }
    } else if (res.lbw <= res.lbw_r) {
        // Item L.6.3.1.2-a (p. 207 da NBR 8800:2024): Seção semicompacta sob interação completa
        // A tensão limite na fibra superior da laje de concreto passou de 1.00 * fcd (2008) para 0.85 * fcd (2024)
        auto secao_tr = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, 0.0, true);
        res.MRd_VM_kNm = std::min(secao_tr.Wtr_inf_mm3 * fyd,
            (1.0 / secao_tr.n_relacao_modular) * secao_tr.Wtr_sup_mm3 * (0.85 * fcd)) * 1e-6;
    } else {
        res.aplicavel = false;
        res.MRd_VM_kNm = 0.0;
    }

    return res;
}

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(
    const PerfilI& perfil, const Aco& aco, const Concreto& concreto, const Laje& laje,
    const ArmacaoLaje& armacaoLaje, double MSd_neg_kNm, double MSd_pos_kNm,
    double Lv_mm, double a_vigas_mm, double alpha_viga, double gamma_a1, double gamma_c
) {
    // 1. Parte plastificada da seção mista sob momento negativo
    ResultadoMomentoNegativoVM res = nbr8800::v2008::calcularMomentoNegativoVM(
        perfil, aco, concreto, laje, armacaoLaje, MSd_neg_kNm, MSd_pos_kNm,
        Lv_mm, a_vigas_mm, alpha_viga, gamma_a1, gamma_c
    );

    // 2. Nova formulação de Flambagem Lateral com Distorção (FLD) da ABNT NBR 8800:2024 (Item L.6.5.5 e L.6.5.6, pp. 212-215)
    double h0 = perfil.d - perfil.tf;
    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;
    double I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);
    double k1 = alpha_viga * aco.E_MPa * I_laje / a_vigas_mm;
    double k2 = (aco.E_MPa * std::pow(perfil.tw, 3.0)) / (3.64 * h0);
    double kr = (k1 * k2) / (k1 + k2);

    // Posição do centroide da seção mista fissurada (aço + armadura da laje)
    double ytr = ((perfil.Ag * perfil.d / 2.0)
        + armacaoLaje.Ads * (perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm))
        / (perfil.Ag + armacaoLaje.Ads);

    double Itr = perfil.Ix
        + perfil.Ag * std::pow(ytr - (perfil.d / 2.0), 2.0)
        + armacaoLaje.Ads * std::pow(perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm - ytr, 2.0);

    // Distância y_barra entre o centroide da seção mista e o centroide do perfil de aço (Item L.6.5.5, p. 213 e Figura L.9, p. 211)
    double y_barra = std::abs(ytr - (perfil.d / 2.0));

    // Novo fator geométrico kappa_g para seções duplamente simétricas (NBR 8800:2024 Item L.6.5.5, p. 213)
    double kappa_g = (0.31 + 0.69 * std::pow(0.05, y_barra / h0)) * (Itr / perfil.Ix);

    double Iaf_y = std::pow(perfil.bf, 3.0) * perfil.tf / 12.0;

    // Novo parâmetro adimensional de rigidez rotacional eta_b e número de semiondas n (Item L.6.5.5 e L.6.5.6, p. 213)
    double eta_b = std::sqrt((kr * std::pow(Lv_mm, 4.0)) / (aco.E_MPa * Iaf_y * h0 * h0));
    double n_id = std::sqrt(eta_b) / PI;
    int n1 = std::max(1, static_cast<int>(std::floor(n_id)));
    int n2 = std::max(1, static_cast<int>(std::ceil(n_id)));
    int n_ondas = (n_id <= std::sqrt(static_cast<double>(n1) * static_cast<double>(n2))) ? n1 : n2;

    double n_pi = static_cast<double>(n_ondas) * PI;
    double termo_ondas = (n_pi * n_pi) + std::pow(eta_b / n_pi, 2.0);

    double psi = (MSd_pos_kNm > 0.0) ? (std::abs(MSd_neg_kNm) / MSd_pos_kNm) : 0.0;
    double Cdist = nbr8800::v2024::calcularCdist(psi);

    // Momento crítico elástico Mcr na NBR 8800:2024 (N.mm)
    double Mcr = Cdist * (kappa_g / h0) * (
        (aco.G_MPa * perfil.It) + ((aco.E_MPa * Iaf_y * h0 * h0) / (Lv_mm * Lv_mm)) * termo_ondas
    );

    double lb_dist = (Mcr > 0.0) ? std::sqrt(std::abs(MSd_neg_kNm) * 1e6 / Mcr) : 0.0;

    double X_dist = 0.0;
    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;
    res.MRd_VM_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

} // namespace nbr8800::v2024