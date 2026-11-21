#include "rotinas_vmp.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <array>
#include <utility>
#include <numbers>

const double PI = std::numbers::pi;

namespace nbr8800::inline v2008 {

ResultadoSecaoTransformada calcularSecaoTransformada(const PerfilI& perfil, const Laje& laje, const Concreto& concreto, 
    const Aco& aco, double multiplicador_fluencia, [[maybe_unused]] double ep_perfil_inferior_mm, bool tempo_imediato)
{
    ResultadoSecaoTransformada res;

    if (tempo_imediato) res.n_relacao_modular = concreto.Ecs_MPa / aco.E_MPa;
    else res.n_relacao_modular = concreto.Ecs_MPa / (3 * aco.E_MPa);
    
    res.btr_mm = laje.bef_mm * res.n_relacao_modular;

    res.ytr_mm = ((res.btr_mm * laje.tc_mm * (laje.tc_mm / 2 + laje.hF_mm + perfil.d)) + 
    (perfil.Ag * perfil.d / 2)) / (res.btr_mm * laje.tc_mm + perfil.Ag);

    res.ay_mm = std::min(perfil.d + laje.hF_mm + laje.tc_mm - res.ytr_mm, laje.tc_mm);

    res.Atr_mm2 = (res.btr_mm * res.ay_mm) + (perfil.Ag);

    double dist_cg_conc = (perfil.d + laje.hF_mm + laje.tc_mm - res.ay_mm / 2) - res.ytr_mm;

    res.Itr_mm4 = res.btr_mm * pow(res.ay_mm, 3) / 12
    + res.btr_mm * res.ay_mm * pow(dist_cg_conc, 2)
    + perfil.Ix 
    + perfil.Ag * pow(res.ytr_mm - perfil.d / 2, 2);

    res.Wtr_inf_mm3 = res.Itr_mm4 / res.ytr_mm;

    res.Wtr_sup_mm3 = res.Itr_mm4 / (perfil.d + laje.hF_mm + laje.tc_mm - res.ytr_mm);

    res.ep_tr_mm = res.ytr_mm + ep_perfil_inferior_mm;

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
        res.n_cordoalhas = static_cast<int>(std::ceil(res.Ap_calc / cabo.area_unit));
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

ResultadoTensaoCabo calcularTensaoCaboELU(const PerfilI& perfil, const Laje& laje, 
    const CaboProtensao& cabo, const Concreto& concreto, double P_atuante_lN,
    double ytr_mm, double Atr_mm2, double Itr_mm4, double Lv_mm)
{
    ResultadoTensaoCabo res;

    if (cabo.tipo == TipoProtensao::PRE_TRACAO)
    {
        res.epsilon_pre = (0.9 * P_atuante_lN * 1000) / (cabo.Ep * cabo.Ap);
        res.ro_p = cabo.Ap / Atr_mm2;
    }
    else if (cabo.tipo == TipoProtensao::POS_TRACAO)
    {
        double alpha_p = cabo.Ep / concreto.Ecs_MPa;

        res.ro_p = cabo.Ap / Atr_mm2;

        double eptr = ytr_mm - cabo.ep;

        double eta = 1 + (eptr * eptr * Atr_mm2 / Itr_mm4);

        res.epsilon_pre = ((0.9 * P_atuante_lN * 1000) / (cabo.Ep * cabo.Ap)) * 
        (1 + alpha_p * res.ro_p * eta);
    }

    if ((Lv_mm / (perfil.d + laje.hF_mm + laje.tc_mm)) <= 35)
    {
        res.delta_sigma_MPa = std::min(70 + (concreto.fck_MPa / (100 * res.ro_p)), 420.0);   
    }
    else
    {
        res.delta_sigma_MPa = std::min(70 + (concreto.fck_MPa / (300 * res.ro_p)), 210.0);
    }

    res.sigma_pd_MPa = std::min((((res.epsilon_pre * cabo.Ep) + res.delta_sigma_MPa) / 1.15), cabo.fpyk / 1.15);

    return res;
}

ResultadoPerdaImediataTracadoReto calcularPerdaAncoragemTracadoReto 
    (const CaboProtensao& cabo, double P0_kN, double Lv_mm)
{
    ResultadoPerdaImediataTracadoReto res;

    // delta_P [N] = (delta_cunha * Ep * Ap) / Lv
    // Dividimos por 1000 para converter Newtons (N) em kilonewtons (kN)
    if (cabo.ancoragem == TipoAncoragem::ATIVA_ATIVA)
    {
        res.delta_P_ancoragem_kN = (cabo.delta_cunha * cabo.Ep * cabo.Ap) / (2.0 * Lv_mm * 1000.0);
    }
    else 
    {
        res.delta_P_ancoragem_kN = (cabo.delta_cunha * cabo.Ep * cabo.Ap) / (Lv_mm * 1000.0);
    }

    res.P1_kN = P0_kN - res.delta_P_ancoragem_kN;
    return res;
}

ForcasProtensaoTrechos calcularPerdaImediataTracadoPoligonal(
    const CaboProtensao& cabo, double P0_kN, double Lv_mm,
    [[maybe_unused]] double ep_i_mm, [[maybe_unused]] double ep_f_mm)
{
    // [STAND-BY]: Rotina 5.1 para traçado poligonal.
    // Enquanto em stand-by, utiliza a perda de ancoragem do traçado reto como fallback.
    auto reto = calcularPerdaAncoragemTracadoReto(cabo, P0_kN, Lv_mm);
    return { reto.P1_kN, reto.P1_kN, reto.P1_kN };
}

double calcularPsi1000 (double sigma_P0_MPa, double fptk_MPa, 
    TipoElementoProtensao elemento, TipoRelaxacao relaxacao)
{
    double chi = sigma_P0_MPa / fptk_MPa;

    if (chi <= 0.5) return 0.0;

    std::array<double, 4> y_pct;

    if (elemento == TipoElementoProtensao::BARRA) 
    {
        y_pct = { 0.0, 1.5, 4.0, 7.0 };
    }
    else if (elemento == TipoElementoProtensao::CORDOALHA)
    {
        if (relaxacao == TipoRelaxacao::RELAXACAO_BAIXA)
            y_pct = { 0.0, 1.3, 2.5, 3.5 }; // RB
        else 
            y_pct = { 0.0, 3.5, 7.0, 12.0 }; // RN
    }
    else 
    {
        if (relaxacao == TipoRelaxacao::RELAXACAO_BAIXA)
            y_pct = { 0.0, 1.0, 2.0, 3.0 }; // Fio RB
        else
            y_pct = { 0.0, 2.5, 5.0, 8.5 }; // Fio RN
    }

    constexpr std::array<double, 4> x = { 0.5, 0.6, 0.7, 0.8 };

    for (size_t i = 0; i < x.size() - 1; ++i)
    {
        if (chi <= x[i+1])
        {
            double psi_pct = y_pct[i] + (chi - x[i]) * (y_pct[i+1] - y_pct[i]) / (x[i+1] - x[i]);
            return psi_pct / 100.0; // Converte de % para valor decimal
        }
    }

    double psi_pct = y_pct[3] + (chi - x[3]) * (y_pct[3] - y_pct[2]) / (x[3] - x[2]);
    return psi_pct / 100.0;
}

ResultadoPerdaRelaxacaoTracadoReto calcularPerdaRelaxacaoTracadoReto(const CaboProtensao& cabo, double P_atuante_kN)
{
    ResultadoPerdaRelaxacaoTracadoReto res;
    
    res.sigma_P0_MPa = (P_atuante_kN * 1000) / cabo.Ap;

    res.psi_1000 = calcularPsi1000(res.sigma_P0_MPa, cabo.fpk, cabo.elemento, cabo.relaxacao);

    res.delta_P_relaxacao_kN = 2.5 * res.psi_1000 * P_atuante_kN;
    
    res.P3_kN = P_atuante_kN - res.delta_P_relaxacao_kN;

    return res;
};

double calcularIncrementoDpTracadoReto(double q_kN_m, double Eg_MPa, double Ep_MPa, 
    double Lv_mm, double ep_mm, double Ix_mm4, double Ag_mm2, double Ap_mm2)
{
    double delta_P_kN = (q_kN_m * 1e-3 * ep_mm * Lv_mm * Lv_mm) / 
    (12*((ep_mm * ep_mm) + (Ix_mm4 / Ag_mm2) + ((Eg_MPa * Ix_mm4) / (Ep_MPa * Ap_mm2))));

    return delta_P_kN;
}

double calcularIncrementoDpTracadoPoligonal(
    double q_kN_m, double Eg_MPa, double Ep_MPa,
    double Lv_mm, [[maybe_unused]] double X_ep_mm,
    [[maybe_unused]] double ep_i_mm, double ep_f_mm,
    double Ix_mm4, double Ag_mm2, double Ap_mm2)
{
    // [STAND-BY]: Rotina 6.2 para traçado poligonal.
    // Enquanto em stand-by, utiliza o incremento do traçado reto com ep_f_mm como fallback.
    return calcularIncrementoDpTracadoReto(q_kN_m, Eg_MPa, Ep_MPa, Lv_mm, ep_f_mm, Ix_mm4, Ag_mm2, Ap_mm2);
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
            res.C_aco_kN = 0.5 * ((perfil.Ag * fyd * 1e-3) + res.T_cabo_kN - res.Cc_max_kN);

            res.T_aco_kN = res.Cc_max_kN + res.C_aco_kN - res.T_cabo_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= (perfil.bf * perfil.tf * fyd * 1e-3))
            {
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);

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
                (((res.C_aco_kN * 1e3) - perfil.bf * perfil.tf * fyd) / (perfil.h * perfil.tw * fyd));

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
            (res.Cc_max_kN * (laje.tc_mm/2 + laje.hF_mm + yp) + res.C_aco_kN * (yp - yc)
            + res.T_aco_kN * (perfil.d - yp - yt)
            + res.T_cabo_kN * (perfil.d + cabo.ep - yp)) * 1e-3;
        }
    }
    else if (res.lbw <= res.lbw_r)
    {
        auto secao_tr = 
        calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, 0.0, true);

        res.MRd_VMP_kNm = std::min(secao_tr.Wtr_inf_mm3 * fyd, 
            (1/secao_tr.n_relacao_modular) * secao_tr.Wtr_sup_mm3 * fcd) * 1e-6;
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
    const Concreto& concreto, const Laje& laje, const CaboProtensao& cabo, 
    const ArmacaoLaje& armacaoLaje, double MSd_neg_kNm, double MSd_pos_kNm, 
    double Lv_mm, double a_vigas_mm, double alpha_viga, double gamma_a1, double gamma_c)
{
    ResultadoMomentoNegativoVMP res;

    double lbf = perfil.lbf;
    double lbf_p = 0.38 * sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

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

                    res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + Cad * d5 - Tpd * d6)*1e-6;
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

    double h0 = perfil.d - perfil.tf;

    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;

    double I_laje = calcularInerciaLaje(laje, armacaoLaje, alpha_e);

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

    double psi = (MSd_pos_kNm > 0.0) ? (std::abs(MSd_neg_kNm) / MSd_pos_kNm) : 0.0;

    double Cdist = calcularCdist(psi);

    double Mcr = (alpha_g * Cdist / Lv_mm) *
    sqrt(((aco.G_MPa * perfil.It) + (kr * Lv_mm * Lv_mm / (PI * PI))) * aco.E_MPa * Iaf_y);

    double lb_dist = (Mcr > 0.0) ? sqrt(std::abs(MSd_neg_kNm)*1e6 / Mcr) : 0.0;

    double X_dist = 0.0;

    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;

    res.MRd_VMP_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

double calcularCdist(double psi)
{
    static const std::vector<std::pair<double, double>> tabela = {
        {0.50, 21.9}, {0.75, 13.9}, {1.00, 11.0},
        {1.25, 9.6}, {1.50, 8.8}, {1.75, 8.3},
        {2.00, 8.0}, {2.25, 7.8}, {2.50, 7.6} 
    };

    if (psi <= 0.50) return 21.9;
    if (psi >= 2.50) return 7.6;

    for (size_t i = 0; i < tabela.size() - 1; ++i) {
        if (psi >= tabela[i].first && psi <= tabela[i+1].first) {
            double x0 = tabela[i].first, y0 = tabela[i].second;
            double x1 = tabela[i+1].first, y1 = tabela[i+1].second;
            return y0 + (psi - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    return 21.9;
}

double calcularInerciaLaje(const Laje& laje, const ArmacaoLaje& armacaoLaje, double alpha_e)
{
    double tc_cm = laje.tc_mm / 10.0;
    double hF_cm = laje.hF_mm / 10.0;
    double bF_cm = laje.bF_mm / 10.0;
    double eF_cm = laje.eF_mm / 10.0;
    double d_linha_cm = armacaoLaje.d_linha_mm / 10.0;
    double As_cm2 = armacaoLaje.As / 100.0; // mm² para cm²

    double delta = 0.0;
    double xln = 0.0;
    double I_laje_cm4 = 0.0;

    if (laje.tipo == TipoLaje::LAJE_MACICA)
        {
            delta = (As_cm2 * As_cm2) + (200.0 * As_cm2 * (tc_cm - d_linha_cm) / alpha_e);
            xln = (-As_cm2 + std::sqrt(delta)) / (100.0 / alpha_e);

            I_laje_cm4 = ((100.0 * std::pow(xln, 3)) / (3.0 * alpha_e))
                + As_cm2 * std::pow(tc_cm - d_linha_cm - xln, 2);
        }
        else // LAJE_STEEL_DECK
        {
            delta = (As_cm2 * As_cm2) + (2.0 * bF_cm * As_cm2 * (tc_cm + hF_cm - d_linha_cm)) / (alpha_e * eF_cm);
            xln = ((-As_cm2 + std::sqrt(delta)) * alpha_e * eF_cm) / (bF_cm / 2.0);

            if (xln <= hF_cm)
            {
                I_laje_cm4 = ((bF_cm * std::pow(xln, 3)) / (3.0 * alpha_e * eF_cm))
                    + As_cm2 * std::pow(tc_cm + hF_cm - d_linha_cm - xln, 2);
            }
            else 
            {
                double termA = As_cm2 + (bF_cm * hF_cm) / (alpha_e * eF_cm) - (100.0 * hF_cm / alpha_e);
                double bracket = (50.0 * hF_cm * hF_cm / alpha_e)
                    - (bF_cm * hF_cm * hF_cm / (2.0 * alpha_e * eF_cm))
                    - (As_cm2 * (tc_cm + hF_cm - d_linha_cm));

                delta = (termA * termA) - (200.0 / alpha_e) * bracket;
                xln = (-termA + std::sqrt(delta)) / (100.0 / alpha_e);

                I_laje_cm4 =
                    ((bF_cm * std::pow(hF_cm, 3)) / (12.0 * alpha_e * eF_cm))
                    + (((bF_cm * hF_cm) / (alpha_e * eF_cm)) * std::pow(xln - hF_cm / 2.0, 2))
                    + ((100.0 * std::pow(xln - hF_cm, 3)) / (3.0 * alpha_e))
                    + (As_cm2 * std::pow(tc_cm + hF_cm - d_linha_cm - xln, 2));
            }
        }

        // Retorna em mm⁴/m (1 cm⁴ = 10000 mm⁴)
        return I_laje_cm4 * 1e4;
}

// ----------------------------------------------------------------------------
// Fase 4: ELS Deformação Excessiva (Flechas) - Turini (2021)
// ----------------------------------------------------------------------------
ResultadoELSDeformacao calcularELSDeformacao(
    const VigaVMP& viga,
    const PerfilI& perfil,
    const ResultadoSecaoTransformada& secao_t0,
    const ResultadoSecaoTransformada& secao_t_inf,
    double P1_kN,
    double P3_kN
) {
    ResultadoELSDeformacao res;
    double L_m = viga.L / 1000.0;
    double L_mm = viga.L;
    double E_a = viga.aco.E_MPa;
    double fy_MPa = viga.aco.fy_MPa;
    res.fy_MPa = fy_MPa;

    // Excentricidades dos cabos
    // e_p,I: excentricidade em relação ao centro de gravidade do perfil de aço isolado
    double ep_I_mm = (perfil.d / 2.0) - std::abs(viga.cabo.ep);
    // e_p,tr,inf: excentricidade em relação à LNE da seção transformada em t=infinito
    double ep_tr_inf_mm = secao_t_inf.ytr_mm - std::abs(viga.cabo.ep);

    // 1. Verificação de regime elástico
    // Momento atuante no perfil isolado (M_Ga,Sd)
    double q_Ga = viga.cargas.q1_perfil;
    if (!viga.cargas.escorada) {
        q_Ga += viga.cargas.q2_concreto;
    }
    double M_Ga_kNm = (q_Ga * L_m * L_m) / 8.0;
    if (viga.cabo.tipo == PRE_TRACAO) {
        M_Ga_kNm -= (ep_I_mm / 1000.0) * P1_kN;
    }
    res.M_Ga_Sd_kNm = M_Ga_kNm;

    // Momento atuante na seção mista (M_L,Sd)
    double q_L = viga.cargas.q3_revestimento + viga.cargas.q4_uso;
    if (viga.cargas.escorada) {
        q_L += viga.cargas.q2_concreto;
    }
    double M_L_kNm = (q_L * L_m * L_m) / 8.0 - (ep_tr_inf_mm / 1000.0) * P3_kN;
    res.M_L_Sd_kNm = M_L_kNm;

    // Módulos resistentes elásticos
    double W_a_mm3 = perfil.Wx; // mm³
    double W_ef_mm3 = secao_t_inf.Wtr_inf_mm3; // mm³

    // Tensão na fibra inferior do perfil de aço (MPa)
    double sigma_Ga_MPa = (W_a_mm3 > 0.0) ? (M_Ga_kNm * 1e6 / W_a_mm3) : 0.0;
    double sigma_L_MPa  = (W_ef_mm3 > 0.0) ? (M_L_kNm * 1e6 / W_ef_mm3) : 0.0;
    res.sigma_inf_MPa = sigma_Ga_MPa + sigma_L_MPa;
    res.regime_elastico = (res.sigma_inf_MPa <= fy_MPa);

    // 2. Cálculo das parcelas de flecha (em cm)
    auto flecha_q_cm = [](double q_kNm, double L_mm, double E_MPa, double I_mm4) -> double {
        if (E_MPa <= 0.0 || I_mm4 <= 0.0) return 0.0;
        double delta_mm = (5.0 / 384.0) * (q_kNm * std::pow(L_mm, 4.0)) / (E_MPa * I_mm4);
        return delta_mm / 10.0;
    };

    auto flecha_prot_cm = [](double P_kN, double ep_mm, double L_mm, double E_MPa, double I_mm4) -> double {
        if (E_MPa <= 0.0 || I_mm4 <= 0.0) return 0.0;
        double delta_mm = -0.125 * (P_kN * 1000.0 * ep_mm * L_mm * L_mm) / (E_MPa * I_mm4);
        return delta_mm / 10.0;
    };

    res.delta_PP_cm = flecha_q_cm(viga.cargas.q1_perfil, L_mm, E_a, perfil.Ix);

    if (!viga.cargas.escorada) {
        res.delta_conc_cm = flecha_q_cm(viga.cargas.q2_concreto, L_mm, E_a, perfil.Ix);
    } else {
        res.delta_conc_cm = flecha_q_cm(viga.cargas.q2_concreto, L_mm, E_a, secao_t_inf.Itr_mm4);
    }

    res.delta_CPN_cm = flecha_q_cm(viga.cargas.q3_revestimento, L_mm, E_a, secao_t_inf.Itr_mm4);

    double q4_curta = (1.0 - viga.cargas.psi2) * viga.cargas.q4_uso;
    double q4_longa = viga.cargas.psi2 * viga.cargas.q4_uso;
    res.delta_CVN_curta_cm = flecha_q_cm(q4_curta, L_mm, E_a, secao_t0.Itr_mm4);
    res.delta_CVN_longa_cm = flecha_q_cm(q4_longa, L_mm, E_a, secao_t_inf.Itr_mm4);

    if (viga.cabo.tipo == PRE_TRACAO) {
        res.delta_protensao_cm = flecha_prot_cm(P1_kN, ep_I_mm, L_mm, E_a, perfil.Ix);
    } else {
        res.delta_protensao_cm = flecha_prot_cm(P3_kN, ep_tr_inf_mm, L_mm, E_a, secao_t_inf.Itr_mm4);
    }

    // Contraflecha executiva (anula ações permanentes)
    double delta_perm = res.delta_PP_cm + res.delta_conc_cm + res.delta_CPN_cm;
    double delta_sem_cf = delta_perm + res.delta_CVN_curta_cm + res.delta_CVN_longa_cm + res.delta_protensao_cm;

    if (delta_perm > delta_sem_cf && delta_sem_cf > 0.0) {
        res.delta_contraflecha_cm = -delta_sem_cf;
        res.delta_total_cm = 0.0;
    } else {
        res.delta_contraflecha_cm = -delta_perm;
        res.delta_total_cm = delta_sem_cf - delta_perm;
    }

    res.delta_lim_cm = (L_mm / 350.0) / 10.0;
    res.aprovado = res.regime_elastico && (res.delta_total_cm <= res.delta_lim_cm);

    return res;
}

// ----------------------------------------------------------------------------
// Fase 5: ELS Vibração no Piso - Turini (2021) / NBR 8800 Anexo L
// ----------------------------------------------------------------------------
ResultadoELSVibracao calcularELSVibracao(
    const VigaVMP& viga,
    const PerfilI& perfil,
    double P_atuante_kN
) {
    ResultadoELSVibracao res;
    double L_mm = viga.L;
    double E_a = viga.aco.E_MPa;

    // Regra da largura colaborante para vibração (NBR 8800 Anexo L): b_ef <= Lv / 5
    double dist_vigas_mm = (viga.a_vigas_mm > 0.0) ? viga.a_vigas_mm : viga.laje.bef_mm;
    double b_ef_semi = std::min(L_mm / 5.0, dist_vigas_mm / 2.0);
    res.bef_vib_mm = 2.0 * b_ef_semi;

    // Acréscimo dinâmico de 35% no módulo de elasticidade do concreto para laje mista
    double Ecs_base = viga.concreto.Ecs_MPa;
    if (viga.laje.tipo == LAJE_STEEL_DECK || viga.laje.hF_mm > 0.0) {
        res.Ecs_vib_MPa = 1.35 * Ecs_base;
    } else {
        res.Ecs_vib_MPa = Ecs_base;
    }

    // Calcula seção transformada para vibração (t=0, tempo imediato)
    Laje laje_vib = viga.laje;
    laje_vib.bef_mm = res.bef_vib_mm;

    Concreto conc_vib = viga.concreto;
    conc_vib.Ecs_MPa = res.Ecs_vib_MPa;

    res.secao_vib = calcularSecaoTransformada(
        perfil, laje_vib, conc_vib, viga.aco, 1.0, viga.cabo.ep, true
    );

    double ep_tr_vib_mm = res.secao_vib.ytr_mm - std::abs(viga.cabo.ep);

    auto flecha_q_cm = [](double q_kNm, double L_mm, double E_MPa, double I_mm4) -> double {
        if (E_MPa <= 0.0 || I_mm4 <= 0.0) return 0.0;
        double delta_mm = (5.0 / 384.0) * (q_kNm * std::pow(L_mm, 4.0)) / (E_MPa * I_mm4);
        return delta_mm / 10.0;
    };

    auto flecha_prot_cm = [](double P_kN, double ep_mm, double L_mm, double E_MPa, double I_mm4) -> double {
        if (E_MPa <= 0.0 || I_mm4 <= 0.0) return 0.0;
        double delta_mm = -0.125 * (P_kN * 1000.0 * ep_mm * L_mm * L_mm) / (E_MPa * I_mm4);
        return delta_mm / 10.0;
    };

    // No modelo simplificado de vibração, todas as cargas atuam sobre o sistema dinâmico da seção mista
    res.delta_PP_cm = flecha_q_cm(viga.cargas.q1_perfil, L_mm, E_a, res.secao_vib.Itr_mm4);
    res.delta_conc_cm = flecha_q_cm(viga.cargas.q2_concreto, L_mm, E_a, res.secao_vib.Itr_mm4);
    res.delta_CPN_cm = flecha_q_cm(viga.cargas.q3_revestimento, L_mm, E_a, res.secao_vib.Itr_mm4);
    res.delta_CVN_cm = flecha_q_cm(viga.cargas.psi1 * viga.cargas.q4_uso, L_mm, E_a, res.secao_vib.Itr_mm4);
    res.delta_protensao_cm = flecha_prot_cm(P_atuante_kN, ep_tr_vib_mm, L_mm, E_a, res.secao_vib.Itr_mm4);

    // Na vibração não se aplica contraflecha executiva
    res.delta_total_cm = res.delta_PP_cm + res.delta_conc_cm + res.delta_CPN_cm + res.delta_CVN_cm + res.delta_protensao_cm;
    res.delta_lim_cm = 2.0; // 20 mm para uso residencial/comercial (NBR 8800 Anexo L)

    res.aprovado = (res.delta_total_cm <= res.delta_lim_cm);
    return res;
}

} // inline namespace v2008

namespace nbr8800::v2024 {

double calcularCdist(double psi)
{
    // ABNT NBR 8800:2024 — Tabela L.1 (p. 214): Coeficiente C_dist para momentos
    // de extremidade Psi * M0 e carga uniformemente distribuída
    static const std::vector<std::pair<double, double>> tabela = {
        {0.50, 1.92}, {0.75, 1.67}, {1.00, 1.53},
        {1.25, 1.44}, {1.50, 1.38}, {1.75, 1.33},
        {2.00, 1.30}, {2.25, 1.27}, {2.50, 1.24}
    };

    if (psi <= 0.50) return 1.92;
    if (psi >= 2.50) return 1.24;

    for (size_t i = 0; i < tabela.size() - 1; ++i) {
        if (psi >= tabela[i].first && psi <= tabela[i + 1].first) {
            double x0 = tabela[i].first, y0 = tabela[i].second;
            double x1 = tabela[i + 1].first, y1 = tabela[i + 1].second;
            return y0 + (psi - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    return 1.92;
}

ResultadoMomentoPositivoVMP calcularMomentoPostivoVMP(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const CaboProtensao& cabo,
    double Ap_mm2,
    double sigma_pd_MPa,
    double gamma_a1,
    double gamma_c
) {
    ResultadoMomentoPositivoVMP res{};

    // 1. Fator de fragilidade do concreto eta_c = (40/fck)^(1/3) <= 1.0 (NBR 8800:2024 Item L.5.3.4 / L.6.3.1.1.1, p. 197 e 202)
    double eta_c = (concreto.fck_MPa > 40.0)
        ? std::min(1.0, std::cbrt(40.0 / concreto.fck_MPa))
        : 1.0;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    if (res.lbw <= res.lbw_p) {
        // Interação completa pressuposta para VMP (ABNT NBR 8800:2024, Item L.6.3.1.1.1, pp. 202-203)
        res.Cc_max_kN = (0.85 * eta_c * fcd * laje.bef_mm * laje.tc_mm) * 1e-3;
        res.T_aco_kN = perfil.Ag * fyd * 1e-3;
        res.T_cabo_kN = Ap_mm2 * sigma_pd_MPa * 1e1 * 1e-3;

        if (res.Cc_max_kN >= (res.T_aco_kN + res.T_cabo_kN)) {
            // Item L.6.3.1.1.1-a (p. 202): Interação completa com LNP na laje de concreto
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3 + res.T_cabo_kN * 1e3) / (0.85 * eta_c * fcd * laje.bef_mm);

            res.MRd_VMP_kNm = (res.T_aco_kN * (perfil.d / 2.0 + laje.hF_mm + laje.tc_mm - res.a_linha_neutra_mm / 2.0)
                + res.T_cabo_kN * (perfil.d + cabo.ep + laje.hF_mm + laje.tc_mm - res.a_linha_neutra_mm / 2.0)) * 1e-3;
        } else {
            // Item L.6.3.1.1.1-b (p. 203): Interação completa com LNP no perfil de aço
            res.C_aco_kN = 0.5 * ((perfil.Ag * fyd * 1e-3) + res.T_cabo_kN - res.Cc_max_kN);
            res.T_aco_kN = res.Cc_max_kN + res.C_aco_kN - res.T_cabo_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= (perfil.bf * perfil.tf * fyd * 1e-3)) {
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);
                yc = yp / 2.0;
                yt = (((perfil.bf * perfil.tf * perfil.tf) / 2.0)
                    + (perfil.h * perfil.tw * (perfil.h / 2.0 + perfil.tf))
                    + (perfil.bf / 2.0 * (perfil.tf - yp) * (3.0 * perfil.tf + 2.0 * perfil.h - yp)))
                    / (perfil.bf * perfil.tf + perfil.h * perfil.tw + perfil.bf * (perfil.tf - yp));
            } else {
                yp = perfil.tf + perfil.h * (((res.C_aco_kN * 1e3) - perfil.bf * perfil.tf * fyd) / (perfil.h * perfil.tw * fyd));
                yc = ((perfil.bf / 2.0 * perfil.tf * perfil.tf)
                    + ((yp - perfil.tf) * perfil.tw) * ((yp + perfil.tf) / 2.0))
                    / ((perfil.bf * perfil.tf) + (yp - perfil.tf) * perfil.tw);
                yt = ((perfil.bf / 2.0 * perfil.tf * perfil.tf)
                    + ((perfil.d - yp - perfil.tf) * perfil.tw) * ((perfil.d - yp + perfil.tf) / 2.0))
                    / ((perfil.bf * perfil.tf) + (perfil.d - yp - perfil.tf) * perfil.tw);
            }

            res.y_lnp_mm = yp;

            // Na ABNT NBR 8800:2024 (Item L.6.3.1.1.1-b, p. 203), sob interação completa com LNP no perfil de aço,
            // a parcela resistida pelo binário interno inclui o somatório dos conectores sum_QRd (= Ccd):
            // MRd = [ sum_QRd * (d - yt - yc) + Ccd * (tc/2 + hF + d - yt) + Tpd * (ep + yt) ]
            double soma_QRd_kN = res.Cc_max_kN;
            res.MRd_VMP_kNm = (soma_QRd_kN * (perfil.d - yt - yc)
                + res.Cc_max_kN * (laje.tc_mm / 2.0 + laje.hF_mm + perfil.d - yt)
                + res.T_cabo_kN * (cabo.ep + yt)) * 1e-3;
        }
    } else if (res.lbw <= res.lbw_r) {
        // Item L.6.3.1.2-a (p. 207 da NBR 8800:2024): Seção semicompacta sob interação completa
        // Tensão limite de compressão no concreto reduzida de 1.00 * fcd (2008) para 0.85 * fcd (2024)
        auto secao_tr = nbr8800::v2008::calcularSecaoTransformada(perfil, laje, concreto, aco, 1.0, 0.0, true);
        res.MRd_VMP_kNm = std::min(secao_tr.Wtr_inf_mm3 * fyd,
            (1.0 / secao_tr.n_relacao_modular) * secao_tr.Wtr_sup_mm3 * (0.85 * fcd)) * 1e-6;
    } else {
        res.aplicavel = false;
        res.MRd_VMP_kNm = 0.0;
    }

    return res;
}

ResultadoMomentoNegativoVMP calcularMomentoNegativoVMP(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const CaboProtensao& cabo,
    const ArmacaoLaje& armacaoLaje,
    double MSd_neg_kNm,
    double MSd_pos_kNm,
    double Lv_mm,
    double a_vigas_mm,
    double alpha_viga,
    double gamma_a1,
    double gamma_c
) {
    // 1. Capacidade resistente plástica da seção mista protendida sob momento negativo
    ResultadoMomentoNegativoVMP res = nbr8800::v2008::calcularMomentoNegativoVMP(
        perfil, aco, concreto, laje, cabo, armacaoLaje,
        MSd_neg_kNm, MSd_pos_kNm, Lv_mm, a_vigas_mm, alpha_viga, gamma_a1, gamma_c
    );

    // 2. Nova formulação de Flambagem Lateral com Distorção (FLD) da ABNT NBR 8800:2024 (Item L.6.5.5 e L.6.5.6, pp. 212-215)
    double h0 = perfil.d - perfil.tf;
    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;
    double I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);
    double k1 = alpha_viga * aco.E_MPa * I_laje / a_vigas_mm;
    double k2 = (aco.E_MPa * std::pow(perfil.tw, 3.0)) / (3.64 * h0);
    double kr = (k1 * k2) / (k1 + k2);

    double ytr = ((perfil.Ag * perfil.d / 2.0)
        + armacaoLaje.Ads * (perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm))
        / (perfil.Ag + armacaoLaje.Ads);

    double Itr = perfil.Ix
        + perfil.Ag * std::pow(ytr - (perfil.d / 2.0), 2.0)
        + armacaoLaje.Ads * std::pow(perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm - ytr, 2.0);

    // Distância y_barra entre o centroide da seção mista e o centroide do perfil de aço (Item L.6.5.5, p. 213 e Figura L.9, p. 211)
    double y_barra = std::abs(ytr - (perfil.d / 2.0));
    double kappa_g = (0.31 + 0.69 * std::pow(0.05, y_barra / h0)) * (Itr / perfil.Ix);

    double Iaf_y = std::pow(perfil.bf, 3.0) * perfil.tf / 12.0;

    // Parâmetro de rigidez rotacional eta_b e número de semiondas n (Item L.6.5.5 e L.6.5.6, p. 213)
    double eta_b = std::sqrt((kr * std::pow(Lv_mm, 4.0)) / (aco.E_MPa * Iaf_y * h0 * h0));
    double n_id = std::sqrt(eta_b) / PI;
    int n1 = std::max(1, static_cast<int>(std::floor(n_id)));
    int n2 = std::max(1, static_cast<int>(std::ceil(n_id)));
    int n_ondas = (n_id <= std::sqrt(static_cast<double>(n1) * static_cast<double>(n2))) ? n1 : n2;

    double n_pi = static_cast<double>(n_ondas) * PI;
    double termo_ondas = (n_pi * n_pi) + std::pow(eta_b / n_pi, 2.0);

    double psi = (MSd_pos_kNm > 0.0) ? (std::abs(MSd_neg_kNm) / MSd_pos_kNm) : 0.0;
    double Cdist = nbr8800::v2024::calcularCdist(psi);

    double Mcr = Cdist * (kappa_g / h0) * (
        (aco.G_MPa * perfil.It) + ((aco.E_MPa * Iaf_y * h0 * h0) / (Lv_mm * Lv_mm)) * termo_ondas
    );

    double lb_dist = (Mcr > 0.0) ? std::sqrt(std::abs(MSd_neg_kNm) * 1e6 / Mcr) : 0.0;

    double X_dist = 0.0;
    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;
    res.MRd_VMP_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

} // namespace nbr8800::v2024