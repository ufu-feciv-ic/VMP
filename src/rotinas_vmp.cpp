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
    const CaboProtensao& cabo, double NRd_kN, double MRd_kNm, double MSd_g_kNm, double MSd_q_kNm, 
    double gamma_p, double beta, double psi_1, double ytr_mm, double Atr_mm, double Wtr_inf_mm, double Wtr_sup_mm)
{
    ResultadoEstimativaP0 res;
    
    res.P0_max_1 = 0.0;
    double ep_I = perfil.d / 2 + cabo.ep;
    double ep_tr = ytr_mm + cabo.ep;

    if (cabo.tipo == TipoProtensao::PRE_TRACAO)
    {
        res.P0_max_1 = -1 / (gamma_p * beta * ((1 / NRd_kN) + ((8 * ep_I) / (9 * MRd_kNm * 1e3)))); // Unidades kN.mm

        res.P0_max_1 = (std::abs(res.P0_max_1) / NRd_kN >= 0.2) ? res.P0_max_1 : (-1 / 
            (gamma_p * beta * (1 / (2*NRd_kN) + (ep_I / (MRd_kNm * 1e3))))); // Unidades kN.mm
    }
    else if (cabo.tipo == TipoProtensao::POS_TRACAO)
    {
        res.P0_max_1 = std::numeric_limits<double>::infinity(); 
    }

    res.P0_max_2 = ((-MSd_g_kNm * 1e3 /Wtr_inf_mm) + (-MSd_q_kNm * 1e3 / Wtr_inf_mm)) / 
    ((1.1 / Atr_mm)+(1.1 * ep_tr / Wtr_inf_mm)); // Unidades kN.mm

    double sigma_c = (-MSd_g_kNm * 1e6 / Wtr_sup_mm) + (-MSd_q_kNm * 1e6 / Wtr_sup_mm)
    + (res.P0_max_2 * 1e3 / Atr_mm) + (-res.P0_max_2 * 1e3 * ep_tr / Wtr_sup_mm); // Unidades N.mm² = MPa

    double alpha_E = 3 * aco.E_MPa / concreto.Ecs_MPa;

    res.P0_max_2 = (std::abs(sigma_c / alpha_E) <= std::abs(-0.6 * concreto.fck_MPa)) ? res.P0_max_2 : 
    ((-0.6 * concreto.fck_MPa) + (((-MSd_g_kNm * 1e3 / Wtr_inf_mm) + (-MSd_q_kNm * 1e3 * psi_1 / Wtr_inf_mm)) / 
    ((1.1 / Atr_mm)+(1.1 * ep_tr / Wtr_inf_mm))));

    res.P0_max_kN = std::min(std::abs(res.P0_max_1), std::abs(res.P0_max_2));

    return res;
}

ResultadoArmaduraProtensao calcularArmaduraProtensao(const CaboProtensao& cabo, TipoProtensao& protensao, double P0_kN)
{
    ResultadoArmaduraProtensao res;

    if (protensao == TipoProtensao::PRE_TRACAO)
    {
        res.sigma_adm_MPa = std::min(0.77 * cabo.fpk, 0.85 * cabo.fpyk);
        res.Ap_calc = (P0_kN * 1000) / res.sigma_adm_MPa;
        res.n_cordoalhas = res.n_cordoalhas = static_cast<int>(std::ceil(res.Ap_calc / cabo.area_unit));
        res.Ap_real = cabo.area_unit * res.n_cordoalhas;
        return res;
    }
    else if (protensao == TipoProtensao::POS_TRACAO)
    {
        res.sigma_adm_MPa = std::min(0.8 * cabo.fpk, 0.88 * cabo.fpyk);
        res.Ap_calc = (P0_kN * 1000) / res.sigma_adm_MPa;
        res.n_cordoalhas = res.n_cordoalhas = static_cast<int>(std::ceil(res.Ap_calc / cabo.area_unit));
        res.Ap_real = cabo.area_unit * res.n_cordoalhas;
        return res;
    }

    return res;
}

ResultadoIncrementoDP calcularIncrementoDpTraçadoReto(double q_kN_m, double Eg_MPa, double Ep_MPa, 
    double Lv_mm, double ep_mm, double Ix_mm4, double Ag_mm2, double Ap_mm2)
{
    ResultadoIncrementoDP res;

    res.delta_P_kN = (q_kN_m * 1e-3 * ep_mm * Lv_mm * Lv_mm) / 
    (12*((ep_mm * ep_mm) + (Ix_mm4 / Ag_mm2) + ((Eg_MPa * Ix_mm4) / (Ep_MPa * Ap_mm2))));

    return res;
}


ResultadoMomentoPositivoVMP calcularMomentoPostivoVMP(const PerfilI& perfil, const Aco& aco, 
const Concreto& concreto, const Laje& laje, const CaboProtensao& cabo, double Ap_mm2, 
double sigma_pd_MPa, double gamma_a1, double gamma_c)
{
    ResultadoMomentoPositivoVMP res;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.7 * sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    if (res.lbw <= res.lbw_p)
    {
        res.Cc_max_kN = (0.85 * fcd * laje.bef_mm * laje.tc_mm) * 1e-3;

        res.T_aco_kN = perfil.Ag * fyd * 1e-3;

        res.T_cabo_kN = Ap_mm2 * sigma_pd_MPa * 1e1 * 1e-3;

        if (res.Cc_max_kN >= (res.T_aco_kN + res.T_cabo_kN))
        {
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3  + res.T_cabo_kN * 1e3) / ((0.85 * fcd * laje.bef_mm));

            res.MRd_VMP_kNm = (res.T_aco_kN * (perfil.d / 2 + laje.hF_mm + laje.tc_mm - res.a_linha_neutra_mm / 2)
            + res.T_cabo_kN * (perfil.d + cabo.ep + laje.hF_mm + laje.tc_mm - res.a_linha_neutra_mm / 2)) * 1e-3;
        }
        else 
        {
            res.C_aco_kN = 0.5 * ((perfil.Ag * fyd * 1e3) + res.T_cabo_kN - res.Cc_max_kN);

            res.T_aco_kN = res.Cc_max_kN + res.C_aco_kN - res.T_cabo_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= (perfil.bf * perfil.tf * fyd))
            {
                yp = res.C_aco_kN / (perfil.bf * fyd);

                yc = yp / 2;

                yt = 
                (((perfil.bf * perfil.tf * perfil.tf) / 2) 
                + (perfil.h * perfil.tw * (perfil.h/2 + perfil.tf)) 
                + (perfil.bf / 2 * (perfil.tf - yp) * (3 * perfil.tf + 2 * perfil.h - yp)))
                / (perfil.bf * perfil.tf + perfil.h * perfil.tw + perfil.bf*(perfil.tf - yp));
            }
            else
            {
                yp = perfil.tf + perfil.h * 
                ((res.C_aco_kN - perfil.bf * perfil.tf * fyd) / (perfil.h * perfil.tw * fyd));

                yc = 
                ((perfil.bf/2 * perfil.tf * perfil.tf)
                + ((yp - perfil.tf) * perfil.tw) * ((yp + perfil.tf) / 2))
                / ((perfil.bf * perfil.tf) + (yp - perfil.tf) * perfil.tw);

                yt = 
                ((perfil.bf/2 * perfil.tf * perfil.tf)
                + ((perfil.d - yp - perfil.tf) * perfil.tw) * ((perfil.d - yp + perfil.tf) / 2))
                / ((perfil.bf * perfil.tf) + (perfil.d - yp - perfil.tf) * perfil.tw);
            }

            res.MRd_VMP_kNm = 
            (res.Cc_max_kN * (laje.tc_mm / 2 + laje.hF_mm + yp)+ res.C_aco_kN * (yp - yc)
            + res.T_aco_kN * (perfil.d - yp - yt)
            + res.T_cabo_kN * (perfil.d + cabo.ep - yp)) * 1e-3;
        }
    }
    else if (res.lbw <= res.lbw_r)
    {
        auto secao_tr = 
        nbr8800::calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, cabo.ep, true);

        res.MRd_VMP_kNm = std::min(secao_tr.Wtr_inf_mm3 * fyd, (1/secao_tr.n_relacao_modular) * secao_tr.Wtr_sup_mm3 * fcd);
    }
    else 
    {
        res.aplicavel = false;
        res.MRd_VMP_kNm = 0.0;
        return res;
    }

    return res;
}

ResultadoMomentoNegativoVMP calcularMomentoNegativoVMP(const PerfilI& perfil, const Aco& aco, 
    const Concreto& concreto, const Laje& laje, const CaboProtensao& cabo, const ArmacaoLaje& armacaoLaje)
{
    ResultadoMomentoNegativoVMP res;

    double lbf = perfil.lbf;
    double lbf_p = 0.38 * sqrt(aco.E_MPa / aco.fy_MPa);
    
    double lbw = perfil.lbw;
    double lbw_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / 1.4;
    double fyd = aco.fy_MPa / 1.1;
    double Tpd = cabo.Ap * cabo.sigma_Pd;
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
    double d6 = 0.0;

    if (lbf <= lbf_p)
    {
        if (Tds + Tpd >= (perfil.Ag * fyd))
        {
            double ay = (Tpd + Tds - perfil.Ag * fyd) / (0.85 * laje.bef_mm * fcd);
            ay = std::min(ay, laje.tc_mm - perfil.d1);

            Ccd = Tpd + Tds - perfil.Ag * fyd;

            d3 = laje.tc_mm - armacaoLaje.d_linha_mm - ay;

            d4 = ay / 2;

            d5 = perfil.d / 2 + laje.hF_mm + ay;

            d6 = perfil.d + cabo.ep + laje.hF_mm + ay;

            res.MRd_neg_kNm = Tds * d3 + Ccd * d4 + Tad * d5 + Tpd * d6;
        }
        else 
        {
            Tad = ((perfil.Ag * fyd) - Tds - Tpd) / 2;
            double lb = 0.0;
            double lb_p = 3.76 * sqrt(aco.E_MPa / aco.fy_MPa); 

            if (Tad <= perfil.bf * perfil.tf * fyd)
            {
                Cad = (perfil.Ag * fyd) - Tad;
                yp = Tad / (perfil.bf * fyd);
                lb = (2 * perfil.d1) / perfil.tw;

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

                    d6 = perfil.d - yp + cabo.ep;
                    
                    res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + Cad * d5 - Tpd * d6)*1e-6;
                }
                else
                {
                    res.aplicavel = false;
                    res.MRd_VMP_kNm = 0.0;
                }
            }
            else 
            {
                Cad = (perfil.Ag * fyd) - Tad;
                
                yp = perfil.tf + perfil.h * 
                ((Tad - (perfil.bf * perfil.tf * fyd)) / (perfil.h * perfil.tw * fyd));

                lb = (2 * (perfil.h - yp + perfil.tf + perfil.r)) / (perfil.tw);

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

                    d6 = perfil.d - yp + cabo.ep;

                    res.MRd_neg_kNm = Tds * d3 + Tad * d4 + Cad * d5 - Tpd * d6;
                }
                else
                {
                    res.aplicavel = false;
                    res.MRd_VMP_kNm = 0.0;
                }
            }
        }
    }
    else
    {
        res.aplicavel = false;
        res.MRd_VMP_kNm = 0.0;
    }

    res.MRd_VMP_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

}