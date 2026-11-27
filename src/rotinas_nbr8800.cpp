#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numbers>
#include <vector>
#include <utility>

const double PI = std::numbers::pi;

namespace nbr8800 {

// ============================================================================
// FUNÇÃO GEOMÉTRICA DE LARGURA EFETIVA (O.2.2 / L.2.2)
// ============================================================================
double calcularLarguraEfetiva(const GeometriaVigaMista& geo) {
    double l_oitavos = geo.L_vao_mm / 8.0;
    if (geo.eh_viga_borda) {
        double d_prox = (geo.dist_esq_mm > 0.0) ? geo.dist_esq_mm : geo.dist_dir_mm;
        return std::min(l_oitavos, geo.dist_borda_mm) + std::min(l_oitavos, d_prox / 2.0);
    }
    return std::min(l_oitavos, geo.dist_esq_mm / 2.0) + std::min(l_oitavos, geo.dist_dir_mm / 2.0);
}

// ============================================================================
// SEÇÃO HOMOGENEIZADA PARA ELS (O.1.2 / L.1.2)
// ============================================================================
ResultadoSecaoHomogeneizadaELS calcularSecaoHomogeneizadaELS(
    const PerfilI& perfil, const Laje& laje, double razao_modular, bool desprezar_concreto_tracionado
) {
    ResultadoSecaoHomogeneizadaELS res;
    res.razao_modular = razao_modular;
    res.btr_mm = laje.bef_mm / std::max(1e-9, razao_modular);

    const double A = perfil.Ag;
    const double d = perfil.d;
    const double hF = laje.hF_mm;
    const double tc = laje.tc_mm;
    const double bt = res.btr_mm;
    const double topo = d + hF + tc;

    // Linha neutra supondo toda a espessura tc comprimida (medida a partir da face inferior do perfil)
    const double y0 = (A * d / 2.0 + bt * tc * (d + hF + tc / 2.0)) / (A + bt * tc);
    res.lne_na_laje = desprezar_concreto_tracionado && (y0 > d + hF);

    if (res.lne_na_laje) {
        // Equilíbrio: bt·x²/2 = A·(tc + hF + d/2 − x)
        const double D = tc + hF + d / 2.0;
        res.x_mm = (-A + std::sqrt(A * A + 2.0 * bt * A * D)) / bt;
        res.yLN_mm = topo - res.x_mm;
        res.Itr_mm4 = bt * std::pow(res.x_mm, 3.0) / 3.0 + perfil.Ix + A * std::pow(D - res.x_mm, 2.0);
    } else {
        res.x_mm = tc;
        res.yLN_mm = y0;
        res.Itr_mm4 = perfil.Ix + A * std::pow(y0 - d / 2.0, 2.0)
            + bt * std::pow(tc, 3.0) / 12.0
            + bt * tc * std::pow(d + hF + tc / 2.0 - y0, 2.0);
    }

    res.Wtr_inf_mm3 = res.Itr_mm4 / res.yLN_mm;
    res.Wtr_sup_mm3 = res.Itr_mm4 / std::max(1e-9, topo - res.yLN_mm);
    return res;
}

// ============================================================================
// NAMESPACE v2008: ABNT NBR 8800:2008
// ============================================================================
inline namespace v2008 {

double calcularEtaC([[maybe_unused]] double fck_MPa) {
    // NBR 8800:2008 item O.2.3.1.1.1: bloco de compressão é 0.85 * fcd sem redutor eta_c
    return 1.00;
}

double calcularEcsConcreto(double fck_MPa, [[maybe_unused]] double alpha_ag) {
    // NBR 8800:2008 item 4.5.3.1: Eci = 5600 * sqrt(fck), Ecs = 0.85 * Eci = 4760 * sqrt(fck)
    return 4760.0 * std::sqrt(fck_MPa);
}

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double lv_mm, double kx, double ky, double gamma_a1
) {
    ResultadoNormalResistentePerfil res;

    // Cálculo cargas críticas
    res.Nex_kN = ((PI * PI * aco.E_MPa * perfil.Ix) / std::pow(kx * lv_mm, 2)) / 1000.0;
    res.Ney_kN = ((PI * PI * aco.E_MPa * perfil.Iy) / std::pow(ky * Lb_mm, 2)) / 1000.0;
    res.Ne_kN = std::min(res.Nex_kN, res.Ney_kN);

    if (perfil.tipo == TipoFabricacao::LAMINADO) {
        double lbf = perfil.lbf;
        double lbf_p = 0.56 * std::sqrt(aco.E_MPa / aco.fy_MPa);
        double lbf_r = 1.03 * std::sqrt(aco.E_MPa / aco.fy_MPa);

        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.74 * lbf * std::sqrt(aco.fy_MPa / aco.E_MPa);
        else res.Qs = 0.69 * aco.E_MPa / (aco.fy_MPa * lbf * lbf);
    } else if (perfil.tipo == TipoFabricacao::SOLDADO) {
        double kc = std::clamp(4.0 / std::sqrt(perfil.h / perfil.tw), 0.35, 0.76);

        double lbf = perfil.lbf;
        double lbf_p = 0.64 * std::sqrt(aco.E_MPa / (aco.fy_MPa / kc));
        double lbf_r = 1.17 * std::sqrt(aco.E_MPa / (aco.fy_MPa / kc));

        if (lbf <= lbf_p) res.Qs = 1.0;
        else if (lbf <= lbf_r) res.Qs = 1.415 - 0.65 * lbf * std::sqrt(aco.fy_MPa / (kc * aco.E_MPa));
        else res.Qs = 0.90 * aco.E_MPa * kc / (aco.fy_MPa * lbf * lbf);
    }

    double lbw = perfil.lbw;
    double lbw_p = 1.49 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    if (lbw <= lbw_p) {
        res.Qa = 1.0;
    } else {
        double lb0_1 = std::sqrt(perfil.Ag * aco.fy_MPa / (res.Ne_kN * 1000.0));
        double chi = (lb0_1 <= 1.5) ? std::pow(0.658, lb0_1 * lb0_1) : (0.877 / (lb0_1 * lb0_1));
        double sigma = chi * aco.fy_MPa / 10.0; // kN/cm²

        double bef_a = 1.92 * (perfil.tw / 10.0) * std::sqrt(aco.E_MPa / 10.0 / sigma)
            * (1.0 - ((0.34 / lbw) * std::sqrt(aco.E_MPa / 10.0 / sigma))); // cm

        bef_a = std::min(bef_a * 10.0, perfil.d1);
        double Aef = perfil.Ag - (perfil.d1 - bef_a) * perfil.tw; // F.3.1: Aef = Ag − Σ(b − bef)·t
        res.Qa = Aef / perfil.Ag;
    }

    res.Q = res.Qs * res.Qa;
    res.lb0 = std::sqrt((res.Q * perfil.Ag * aco.fy_MPa) / (res.Ne_kN * 1000.0));
    res.chi = (res.lb0 <= 1.5) ? std::pow(0.658, res.lb0 * res.lb0) : (0.877 / (res.lb0 * res.lb0));
    res.NRd_kN = ((res.chi * res.Q * aco.fy_MPa * perfil.Ag) / gamma_a1) / 1000.0;

    return res;
}

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double a_enrijecedor_mm, double gamma_a1
) {
    ResultadoCortanteResistentePerfil res;

    // NBR 8800:2008 item 5.4.3.1.1: kv = 5.00 para almas sem enrijecedores transversais
    bool sem_enrijecedor = (a_enrijecedor_mm <= 0.0)
        || ((a_enrijecedor_mm / perfil.h) > 3.0)
        || ((a_enrijecedor_mm / perfil.h) > std::pow(260.0 / (perfil.h / perfil.tw), 2.0));

    res.kv = sem_enrijecedor ? 5.0 : (5.0 + 5.0 / std::pow(a_enrijecedor_mm / perfil.h, 2.0));
    res.Vpl_kN = (0.6 * perfil.d * perfil.tw * aco.fy_MPa) / 1000.0;

    res.lbw = perfil.lbw;
    res.lbw_p = 1.10 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 1.37 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);

    if (res.lbw <= res.lbw_p) {
        res.VRd_kN = res.Vpl_kN / gamma_a1;
    } else if (res.lbw <= res.lbw_r) {
        res.VRd_kN = (res.lbw_p / res.lbw) * (res.Vpl_kN / gamma_a1);
    } else {
        res.VRd_kN = 1.24 * std::pow(res.lbw_p / res.lbw, 2.0) * (res.Vpl_kN / gamma_a1);
    }

    return res;
}

double calcularMcrFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb) {
    if (Lb_mm <= 0.0) return 1e12;
    double Mcr = (((Cb * PI * PI * aco.E_MPa * perfil.Iy) / (Lb_mm * Lb_mm)) *
        std::sqrt((perfil.Cw / perfil.Iy) * (1.0 + 0.039 * (perfil.It * Lb_mm * Lb_mm / perfil.Cw)))) / 1e6;
    return Mcr;
}

double calcularMRdFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double gamma_a1, double Cb, double Mpl, double Mr) {
    if (Lb_mm <= 0.0) return Mpl / gamma_a1;

    double lb = Lb_mm / perfil.ry;
    double lb_p = 1.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double beta1 = (0.7 * aco.fy_MPa * perfil.Wx) / (aco.E_MPa * perfil.It);

    double lb_r = ((1.38 * std::sqrt(perfil.Iy * perfil.It)) / (perfil.ry * perfil.It * beta1))
        * std::sqrt(1.0 + std::sqrt(1.0 + ((27.0 * perfil.Cw * beta1 * beta1) / perfil.Iy)));

    double Mcr = calcularMcrFLT(perfil, aco, Lb_mm, Cb);
    double MRd_kNm = 0.0;

    if (lb <= lb_p) {
        MRd_kNm = Mpl / gamma_a1;
    } else if (lb <= lb_r) {
        MRd_kNm = (Cb / gamma_a1) * (Mpl - (Mpl - Mr) * ((lb - lb_p) / (lb_r - lb_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl / gamma_a1);
    } else {
        MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
    }

    return MRd_kNm;
}

double calcularMRdFLM(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr) {
    double lbf = perfil.lbf;
    double lbf_p = 0.38 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    double lbf_r = 0.0;
    double kc = 0.0;

    if (perfil.tipo == TipoFabricacao::LAMINADO) {
        lbf_r = 0.83 * std::sqrt(aco.E_MPa / (0.7 * aco.fy_MPa));
    } else if (perfil.tipo == TipoFabricacao::SOLDADO) {
        kc = 4.0 / std::sqrt(perfil.h / perfil.tw);
        kc = std::clamp(kc, 0.35, 0.76);
        lbf_r = 0.95 * std::sqrt(aco.E_MPa / (0.7 * (aco.fy_MPa / kc)));
    }

    double MRd_kNm = 0.0;

    if (lbf <= lbf_p) {
        MRd_kNm = Mpl / gamma_a1;
    } else if (lbf <= lbf_r) {
        MRd_kNm = (1.0 / gamma_a1) * (Mpl - (Mpl - Mr) * ((lbf - lbf_p) / (lbf_r - lbf_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl / gamma_a1);
    } else {
        if (perfil.tipo == TipoFabricacao::LAMINADO) {
            double Mcr = (0.69 * aco.E_MPa * perfil.Wx / (lbf * lbf)) / 1e6;
            MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
        } else if (perfil.tipo == TipoFabricacao::SOLDADO) {
            double Mcr = (0.9 * kc * aco.E_MPa * perfil.Wx / (lbf * lbf)) / 1e6;
            MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
        }
    }

    return MRd_kNm;
}

double calcularMRdFLA(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, [[maybe_unused]] double Mr) {
    // NBR 8800:2008 Tabela G.1 (FLA): Mr = fy·W (sem tensão residual, ao contrário de FLT e FLM)
    double lbw = perfil.lbw;
    double lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double Mr_fla = (aco.fy_MPa * perfil.Wx) / 1e6;

    double MRd_kNm = 0.0;

    if (lbw <= lbw_p) {
        MRd_kNm = Mpl / gamma_a1;
    } else if (lbw <= lbw_r) {
        MRd_kNm = (1.0 / gamma_a1) * (Mpl - (Mpl - Mr_fla) * ((lbw - lbw_p) / (lbw_r - lbw_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl / gamma_a1);
    } else {
        MRd_kNm = 0.0;
    }

    return MRd_kNm;
}

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb, double gamma_a1
) {
    ResultadoMomentoResistentePerfil res;

    res.Mpl_kNm = std::min((perfil.Zx * aco.fy_MPa) / 1e6, (1.5 * perfil.Wx * aco.fy_MPa) / 1e6);
    res.Mr_kNm = (0.7 * aco.fy_MPa * perfil.Wx) / 1e6;
    res.Mcr_kNm = calcularMcrFLT(perfil, aco, Lb_mm, Cb);

    res.MRd_FLT_kNm = calcularMRdFLT(perfil, aco, Lb_mm, gamma_a1, Cb, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLM_kNm = calcularMRdFLM(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLA_kNm = calcularMRdFLA(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);

    if (res.MRd_FLT_kNm == 0.0 || res.MRd_FLM_kNm == 0.0 || res.MRd_FLA_kNm == 0.0) {
        res.perfil_esbelto = true;
    }

    res.MRd_kNm = std::min({res.MRd_FLT_kNm, res.MRd_FLM_kNm, res.MRd_FLA_kNm});
    return res;
}

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(
    const PerfilI& perfil, const Aco& aco,
    const Concreto& concreto, const Laje& laje,
    double gamma_a1, double gamma_c, double Fhd_conectores_kN
) {
    ResultadoMomentoPositivoVM res;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.eta_c = 1.00;

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    res.Cc_max_kN = 0.85 * fcd * laje.bef_mm * laje.tc_mm * 1e-3;
    double Fa_max_kN = perfil.Ag * fyd * 1e-3;
    double Fhd_max_kN = std::min(res.Cc_max_kN, Fa_max_kN);

    bool interacao_parcial = (Fhd_conectores_kN >= 0.0 && Fhd_conectores_kN < Fhd_max_kN - 1e-6);
    res.Ccd_kN = interacao_parcial ? Fhd_conectores_kN : Fhd_max_kN;
    res.eta_interacao = (Fhd_max_kN > 0.0) ? std::min(1.0, res.Ccd_kN / Fhd_max_kN) : 1.0;

    if (res.lbw <= res.lbw_p) {
        res.secao_compacta = true;
        res.T_aco_kN = Fa_max_kN;

        if (!interacao_parcial && res.Cc_max_kN >= res.T_aco_kN) {
            // Caso (a): LNP na laje de concreto (Interação Completa - NBR 8800:2008 O.2.3.1.1.1-a)
            res.lnp_na_laje = true;
            res.C_aco_kN = 0.0;
            res.Ccd_kN = res.T_aco_kN;
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3) / (0.85 * fcd * laje.bef_mm);
            res.y_lnp_mm = 0.0;

            res.MRd_VM_kNm = res.T_aco_kN * (perfil.d / 2.0 + laje.hF_mm +
                laje.tc_mm - res.a_linha_neutra_mm / 2.0) * 1e-3;
        } else {
            // Caso (b): LNP no perfil com interação completa, ou Caso (c): Interação parcial
            res.lnp_na_laje = false;
            res.a_linha_neutra_mm = interacao_parcial
                ? ((res.Ccd_kN * 1e3) / (0.85 * fcd * laje.bef_mm))
                : laje.tc_mm;

            res.C_aco_kN = 0.5 * (Fa_max_kN - res.Ccd_kN);
            res.T_aco_kN = res.C_aco_kN + res.Ccd_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= perfil.bf * perfil.tf * fyd * 1e-3) {
                // LNP na mesa superior do perfil
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);
                yc = yp / 2.0;

            } else {
                // LNP na alma do perfil
                yp = perfil.tf + (perfil.h
                    * (((res.C_aco_kN * 1e3) - (perfil.bf * perfil.tf * fyd))
                    / (perfil.h * perfil.tw * fyd)));

                yc = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.tw * (yp - perfil.tf) * ((yp + perfil.tf) / 2.0)))
                    / ((perfil.bf * perfil.tf) + ((yp - perfil.tf) * perfil.tw)));

            }

            res.y_lnp_mm = yp;

            // Centro da parte tracionada (a partir da face inferior) pela diferença de momentos
            // estáticos: perfil inteiro (Aa, centroide em d/2) menos a parte comprimida.
            // Usa a mesma área Aa das forças plásticas (consistente com a planilha de referência).
            const double Ac = (yp <= perfil.tf)
                ? perfil.bf * yp
                : perfil.bf * perfil.tf + perfil.tw * (yp - perfil.tf);
            const double At = perfil.Ag - Ac;
            yt = (perfil.Ag * perfil.d / 2.0 - Ac * (perfil.d - yc)) / At;

            double braco_conc = (laje.tc_mm - res.a_linha_neutra_mm / 2.0) + laje.hF_mm + perfil.d - yt;
            res.MRd_VM_kNm = (res.C_aco_kN * (perfil.d - yt - yc) + res.Ccd_kN * braco_conc) * 1e-3;
        }
    } else if (res.lbw <= res.lbw_r) {
        // Alma semicompacta: verificação elástica via seção transformada (reutilizando rotinas_vmp)
        res.secao_compacta = false;
        res.lnp_na_laje = false;

        // Seção homogeneizada de curta duração (n = Ea/Ec), concreto tracionado desprezado
        auto secao_tr = nbr8800::calcularSecaoHomogeneizadaELS(
            perfil, laje, aco.E_MPa / concreto.Ecs_MPa
        );

        double Wef_inf = perfil.Wx + std::sqrt(res.eta_interacao) * (secao_tr.Wtr_inf_mm3 - perfil.Wx);

        res.MRd_VM_kNm = std::min(
            Wef_inf * fyd,
            secao_tr.razao_modular * secao_tr.Wtr_sup_mm3 * fcd
        ) * 1e-6;
    } else {
        res.aplicavel = false;
        res.secao_compacta = false;
        res.MRd_VM_kNm = 0.0;
        return res;
    }

    return res;
}

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(
    const PerfilI& perfil,
    const Aco& aco, const Concreto& concreto, const Laje& laje,
    const ArmacaoLaje& armacaoLaje, double MSd_neg_kNm, double MSd_pos_kNm,
    double Lv_mm, double a_vigas_mm, double alpha_viga, double gamma_a1, double gamma_c
) {
    ResultadoMomentoNegativoVM res;

    double lbf = perfil.lbf;
    double lbf_p = 0.38 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    [[maybe_unused]] double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    double Tds = armacaoLaje.Ads * armacaoLaje.fsd;
    double Tad = 0.0;
    double yp = 0.0;
    double yc = 0.0;
    double yt = 0.0;
    double d3 = 0.0;
    double d4 = 0.0;
    double d5 = 0.0;

    if (lbf <= lbf_p) {
        double lb = 0.0;
        double lb_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);

        if ((Tds + perfil.bf * perfil.tf * fyd) >=
            (perfil.h * perfil.tw + perfil.tf * perfil.bf) * fyd) {
            Tad = (perfil.Ag * fyd - Tds) / 2.0;
            yp = Tad / (perfil.bf * fyd);
            lb = 2.0 * perfil.d1 / perfil.tw;

            if (lb <= lb_p) {
                yc = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + perfil.h * perfil.tw * ((perfil.h / 2.0) + perfil.tf)
                    + perfil.bf / 2.0 * (perfil.tf - yp) * (3.0 * perfil.tf + 2.0 * perfil.h - yp))
                    / ((perfil.bf * perfil.tf) + (perfil.h * perfil.tw) + perfil.bf * (perfil.tf - yp));

                yt = yp / 2.0;
                d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;
                d4 = yt;
                d5 = perfil.d - yp - yc;

                res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad) * d5) * 1e-6;
            } else {
                res.aplicavel = false;
                res.MRd_VM_kNm = 0.0;
            }
        } else {
            Tad = (perfil.Ag * fyd - Tds) / 2.0;
            yp = perfil.tf + perfil.h *
                ((Tad - (perfil.bf * perfil.tf * fyd)) / (perfil.h * perfil.tw * fyd));
            lb = (2.0 * (perfil.h - yp + perfil.tf - perfil.r)) / perfil.tw;

            if (lb <= lb_p) {
                yc = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.d - yp - perfil.tf) * perfil.tw * ((perfil.d - yp + perfil.tf) / 2.0))
                    / ((perfil.bf * perfil.tf) + (perfil.d - yp - perfil.tf) * perfil.tw);

                yt = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (yp - perfil.tf) * perfil.tw * ((yp + perfil.tf) / 2.0))
                    / ((perfil.bf * perfil.tf) + (yp - perfil.tf) * perfil.tw);

                d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;
                d4 = yp - yt;
                d5 = perfil.d - yp - yc;

                res.MRd_neg_kNm = (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad) * d5) * 1e-6;
            } else {
                res.aplicavel = false;
                res.MRd_VM_kNm = 0.0;
            }
        }
    } else {
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

    double Itr = perfil.Ix
        + perfil.Ag * std::pow(ytr - (perfil.d / 2.0), 2)
        + armacaoLaje.Ads * std::pow(perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm - ytr, 2);

    double alpha_g = (h0 * Itr / perfil.Ix)
        / (((std::pow(h0, 2) / 4.0) + ((perfil.Ix + perfil.Iy) / perfil.Ag)) / (e + h0));

    double Iaf_y = std::pow(perfil.bf, 3) * perfil.tf / 12.0;
    double psi = MSd_neg_kNm / MSd_pos_kNm;
    double Cdist = nbr8800::v2008::calcularCdist(psi);

    double Mcr = (alpha_g * Cdist / Lv_mm) *
        std::sqrt(((aco.G_MPa * perfil.It) + (kr * Lv_mm * Lv_mm / (PI * PI))) * aco.E_MPa * Iaf_y);

    double lb_dist = std::sqrt(MSd_neg_kNm * 1e6 / Mcr);
    double X_dist = 0.0;

    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;
    res.MRd_VM_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);

    return res;
}

double calcularGrauInteracaoMinimo(
    [[maybe_unused]] const PerfilI& perfil,
    const Aco& aco,
    double Lv_mm,
    [[maybe_unused]] bool escorada,
    [[maybe_unused]] double MSd_kNm,
    [[maybe_unused]] double MRd_kNm
) {
    // NBR 8800:2008 item O.2.3.1.1.2 (perfis de mesas iguais):
    // eta_min = 1 - (E / (578 * fy)) * (0.75 - 0.03 * Le) >= 0.40 para Le <= 25 m
    double Le_m = Lv_mm / 1000.0;
    if (Le_m > 25.0) return 1.00;
    double eta_calc = 1.0 - (aco.E_MPa / (578.0 * aco.fy_MPa)) * (0.75 - 0.03 * Le_m);
    return std::clamp(std::max(0.40, eta_calc), 0.40, 1.00);
}

ResultadoRotinaE calcularConectoresCisalhamento(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    bool escorada,
    double gamma_a1,
    double gamma_c,
    double gamma_cs
) {
    ResultadoRotinaE res;
    res.beff_mm = laje.bef_mm;
    res.eta_c = v2008::calcularEtaC(concreto.fck_MPa);

    double fyd = aco.fy_MPa / gamma_a1;
    double fcd = concreto.fck_MPa / gamma_c;

    double Fa_kN = (perfil.Ag * fyd) * 1e-3;
    double Fc_kN = (0.85 * res.eta_c * fcd * laje.bef_mm * laje.tc_mm) * 1e-3;
    res.Fhd_kN = std::min(Fa_kN, Fc_kN);

    if (conector.tipo == CONECTOR_PERFIL_U) {
        // NBR 8800:2008 item O.4.2.2.1: Conector em Perfil U laminado
        res.Rg = 1.0;
        res.Rp = 1.0;
        res.QRd1_concreto_kN = (0.3 * (conector.tf + 0.5 * conector.tw) * conector.Lcs *
            std::sqrt(concreto.fck_MPa * concreto.Ecs_MPa)) / (gamma_cs * 1000.0);
        res.QRd2_aco_kN = res.QRd1_concreto_kN;
        res.QRd_kN = res.QRd1_concreto_kN;
        res.s_min_long_mm = std::max(conector.h, conector.Lcs);
        res.s_max_long_mm = 8.0 * (laje.tc_mm + laje.hF_mm); // O.4.3.2
    } else {
        // NBR 8800:2008 item O.4.2.1: Pino com Cabeça (Stud Bolt)
        double d = conector.d;
        res.Acs_mm2 = (PI * d * d) / 4.0;
        int n_linha = std::max(1, conector.nt);

        if (laje.tipo == LAJE_MACICA) {
            res.Rg = 1.00;
            // NBR 8800:2008 item O.4.2.1.3-a: pino soldado diretamente no perfil sem fôrma -> Rp = 1.00!
            res.Rp = 1.00;
        } else {
            // LAJE_STEEL_DECK
            if (!laje.nervura_paralela) { // Nervuras perpendiculares à viga
                if (n_linha == 1) res.Rg = 1.00;
                else if (n_linha == 2) res.Rg = 0.85;
                else res.Rg = 0.70;

                // O.4.2.1.3-b/c e L.4.2.1.3-b/c: depende apenas de emh
                res.Rp = (conector.emh_mm >= 50.0) ? 0.75 : 0.60;
            } else { // Nervuras paralelas à viga
                double relacao = (laje.hF_mm > 0.0) ? (laje.bF_mm / laje.hF_mm) : 2.0;
                res.Rg = (relacao >= 1.5) ? 1.00 : 0.85; // O.4.2.1.2-a3/b2
                res.Rp = 0.75;
            }
        }

        if (conector.Rg > 0.0) res.Rg = conector.Rg;
        if (conector.Rp > 0.0) res.Rp = conector.Rp;

        res.QRd1_concreto_kN = (res.Acs_mm2 * std::sqrt(concreto.fck_MPa * concreto.Ecs_MPa)) / (2.0 * gamma_cs * 1000.0);
        res.QRd2_aco_kN = (res.Rg * res.Rp * res.Acs_mm2 * conector.fu) / (gamma_cs * 1000.0);
        res.QRd_kN = std::min(res.QRd1_concreto_kN, res.QRd2_aco_kN);

        // Limites de espaçamento NBR 8800:2008 (O.4.3.2 e O.4.3.3)
        res.s_min_long_mm = (laje.tipo == LAJE_STEEL_DECK) ? (4.0 * d) : (6.0 * d);
        double h_total_laje = laje.tc_mm + laje.hF_mm;
        res.s_max_long_mm = (laje.tipo == LAJE_STEEL_DECK && !laje.nervura_paralela)
            ? std::min(8.0 * h_total_laje, 915.0)
            : (8.0 * h_total_laje);
    }

    // Conectores necessários para interação completa (100%):
    res.n_semi_vao_calculado = static_cast<int>(std::ceil(res.Fhd_kN / std::max(1e-3, res.QRd_kN)));
    res.nl_conectores = 2 * res.n_semi_vao_calculado;

    // Espaçamento longitudinal adotado:
    bool is_perp_2008 = (laje.tipo == LAJE_STEEL_DECK && !laje.nervura_paralela);
    int n_linha = std::max(1, conector.nt);

    if (is_perp_2008) {
        int k = std::max(1, conector.k_nervuras);
        double passo = (laje.eF_mm > 0.0) ? laje.eF_mm : 200.0;
        res.el_espacamento_mm = k * passo;
        res.alerta_ancoragem_deck = (res.el_espacamento_mm > 450.0);
        int nlin_2008 = static_cast<int>(std::floor((Lv_mm / 2.0) / std::max(1.0, res.el_espacamento_mm)));
        res.n_linhas_meio_vao = nlin_2008;
        res.n_semi_vao_adotado = nlin_2008 * n_linha;
        res.n_total_adotado = 2 * res.n_semi_vao_adotado;
    } else if (conector.espacamento_adotado_mm > 0.0) {
        res.el_espacamento_mm = conector.espacamento_adotado_mm;
        res.alerta_ancoragem_deck = false;
        int nlin_2008 = static_cast<int>(std::floor((Lv_mm / 2.0) / std::max(1.0, res.el_espacamento_mm)));
        res.n_linhas_meio_vao = nlin_2008;
        res.n_semi_vao_adotado = nlin_2008 * n_linha;
        res.n_total_adotado = 2 * res.n_semi_vao_adotado;
    } else {
        // Se espacamento_adotado_mm <= 0.0, dimensiona para interação completa (100%)
        res.alerta_ancoragem_deck = false;
        res.n_semi_vao_adotado = res.n_semi_vao_calculado;
        res.n_total_adotado = res.nl_conectores;
        int nlin_2008 = std::max(1, static_cast<int>(std::ceil(static_cast<double>(res.n_semi_vao_calculado) / n_linha)));
        res.n_linhas_meio_vao = nlin_2008;
        res.el_espacamento_mm = (nlin_2008 > 0) ? ((Lv_mm / 2.0) / nlin_2008) : 200.0;
    }

    // Grau de interação calculado:
    res.grau_interacao_eta = std::min(1.00, (res.n_semi_vao_adotado * res.QRd_kN) / std::max(1e-3, res.Fhd_kN));
    res.grau_interacao_minimo = v2008::calcularGrauInteracaoMinimo(perfil, aco, Lv_mm, escorada);
    res.n_semi_vao_minimo = static_cast<int>(std::ceil(res.grau_interacao_minimo * res.Fhd_kN / std::max(1e-3, res.QRd_kN)));
    res.n_total_minimo = 2 * res.n_semi_vao_minimo;

    res.espacamento_atende = (res.el_espacamento_mm >= res.s_min_long_mm - 1e-4 && res.el_espacamento_mm <= res.s_max_long_mm + 1e-4);
    res.grau_interacao_atende = (res.grau_interacao_eta >= res.grau_interacao_minimo - 1e-5);

    // Verificações geométricas do pino (O.4.1, O.2.6.1, O.4.4.1)
    double h_total_laje = laje.tc_mm + laje.hF_mm;
    double h_min_base = 4.0 * conector.d;
    double h_min_deck = (laje.tipo == LAJE_STEEL_DECK) ? (laje.hF_mm + 40.0) : 0.0;
    res.h_min_normativo_mm = std::max(h_min_base, h_min_deck);
    res.altura_atende = (conector.h >= res.h_min_normativo_mm - 1e-4);

    res.cobrimento_topo_mm = h_total_laje - conector.h;
    res.cobrimento_min_mm = 10.0;
    res.cobrimento_atende = (res.cobrimento_topo_mm >= res.cobrimento_min_mm - 1e-4);

    res.d_max_mesa_mm = 2.5 * perfil.tf;
    bool diam_deck_ok_2008 = (laje.tipo != LAJE_STEEL_DECK || conector.d <= 19.0 + 1e-4);
    bool diam_mesa_ok_2008 = (n_linha == 1 || conector.d <= res.d_max_mesa_mm + 1e-4);
    res.diametro_mesa_atende = diam_deck_ok_2008 && diam_mesa_ok_2008;

    // Verificações geométricas transversais (O.4.1.3 e O.4.4.2)
    if (conector.tipo == CONECTOR_PERFIL_U) {
        res.s_transv_min_mm = 0.0;
        res.dist_borda_min_mesa_mm = 25.0;
        res.bf_min_necessario_mm = conector.Lcs;
        res.nt_max_mesa = 1;
        res.largura_mesa_atende = (perfil.bf >= res.bf_min_necessario_mm - 1e-4);
        res.largura_deck_disp_mm = perfil.bf;
        res.largura_deck_nec_mm = conector.Lcs;
        res.nervura_deck_atende = (laje.tipo != LAJE_STEEL_DECK); // Perfil U não é permitido com steel deck
    } else {
        // Stud Bolt (Pino com cabeça)
        res.s_transv_min_mm = 4.0 * conector.d;
        res.dist_borda_min_mesa_mm = 25.0;
        res.bf_min_necessario_mm = (n_linha - 1) * res.s_transv_min_mm + 2.0 * res.dist_borda_min_mesa_mm;
        res.largura_mesa_atende = (perfil.bf >= res.bf_min_necessario_mm - 1e-4);

        double espaco_livre = perfil.bf - 2.0 * res.dist_borda_min_mesa_mm;
        res.nt_max_mesa = (espaco_livre >= 0.0 && res.s_transv_min_mm > 0.0)
            ? std::max(1, 1 + static_cast<int>(std::floor(espaco_livre / res.s_transv_min_mm)))
            : 1;

        if (laje.tipo == LAJE_STEEL_DECK && laje.nervura_paralela) {
            double hF = std::max(0.0, laje.hF_mm);
            double bF = (laje.bF_mm > 0.0) ? laje.bF_mm : 120.0;
            double w_fundo = std::max(20.0, bF - 0.40 * hF);
            res.largura_deck_disp_mm = w_fundo;
            res.largura_deck_nec_mm = (n_linha - 1) * res.s_transv_min_mm + 2.0 * res.dist_borda_min_mesa_mm;
            res.nervura_deck_atende = (res.largura_deck_disp_mm >= res.largura_deck_nec_mm - 1e-4);
        } else {
            res.largura_deck_disp_mm = perfil.bf;
            res.largura_deck_nec_mm = res.bf_min_necessario_mm;
            res.nervura_deck_atende = true;
        }
    }
    res.geometria_transversal_atende = res.largura_mesa_atende && res.nervura_deck_atende;

    return res;
}

ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    double gamma_a1,
    double gamma_c,
    double gamma_cs
) {
    ConectorCisalhamento conn_u = conector;
    conn_u.tipo = CONECTOR_PERFIL_U;
    return v2008::calcularConectoresCisalhamento(perfil, aco, laje, concreto, conn_u, Lv_mm, false, gamma_a1, gamma_c, gamma_cs);
}

ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c
) {
    ResultadoRotinaF res;
    double Lm_m = (Lv_mm / 2.0) / 1000.0;
    res.nu_Sd_kN_m = Fhd_kN / std::max(0.001, Lm_m);
    res.Hv_Sd_kN_cm = res.nu_Sd_kN_m / 100.0;

    double fcd = concreto.fck_MPa / gamma_c;
    double eta_c = v2008::calcularEtaC(concreto.fck_MPa);
    res.nu_Rd_max_kN_m = 0.20 * eta_c * fcd * laje.tc_mm; // kN/m
    res.Hv_Rd_max_kN_cm = res.nu_Rd_max_kN_m / 100.0;
    res.atende_bielas = (res.nu_Sd_kN_m <= res.nu_Rd_max_kN_m);

    double fyd_arm = 500.0 / 1.15; // CA-50
    double As_mm2_m = (res.nu_Sd_kN_m * 1000.0) / fyd_arm;
    res.As_cos_cm2_m = As_mm2_m / 100.0;
    res.lb_comprimento_cm = (laje.bef_mm / 2.0) / 10.0;

    return res;
}

static double fator_psi_combinacao(CombinacaoServico combo, double psi1, double psi2) {
    switch (combo) {
        case CombinacaoServico::QUASE_PERMANENTE: return psi2;
        case CombinacaoServico::FREQUENTE: return psi1;
        default: return 1.0;
    }
}

ResultadoFlechasVM calcularFlechasVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double Lv_mm,
    const AcoesPiso& acoes,
    double grau_interacao_eta,
    bool escorada
) {
    ResultadoFlechasVM res;
    const double E = aco.E_MPa;
    const double Ia = perfil.Ix;
    const double Wa = perfil.Wx;
    const double g1 = acoes.g1_kN_m;   // kN/m == N/mm
    const double g2 = acoes.g2_kN_m;
    const double q = acoes.q2_kN_m;

    // Seções homogeneizadas (O.1.2 / L.1.2): curta (n = Ea/Ec) e longa (n = 3·Ea/Ec)
    res.alpha_E_curta = E / concreto.Ecs_MPa;
    res.alpha_E_longa = 3.0 * res.alpha_E_curta;
    res.secao_curta = calcularSecaoHomogeneizadaELS(perfil, laje, res.alpha_E_curta);
    res.secao_longa = calcularSecaoHomogeneizadaELS(perfil, laje, res.alpha_E_longa);

    const double Itr_cd = res.secao_curta.Itr_mm4;
    const double Itr_ld = res.secao_longa.Itr_mm4;
    res.Itr_cd_cm4 = Itr_cd / 1e4;
    res.Itr_ld_cm4 = Itr_ld / 1e4;

    // Interação parcial: Ief = Ia + sqrt(eta)·(Itr − Ia) (O.1.2.2.1 / L.1.2.2.1)
    res.fator_interacao = std::sqrt(std::clamp(grau_interacao_eta, 0.0, 1.0));
    const double Ief_cd = Ia + res.fator_interacao * (Itr_cd - Ia);
    const double Ief_ld = Ia + res.fator_interacao * (Itr_ld - Ia);
    res.Ief_cd_cm4 = Ief_cd / 1e4;
    res.Ief_ld_cm4 = Ief_ld / 1e4;

    const double Wef_inf_cd = Wa + res.fator_interacao * (res.secao_curta.Wtr_inf_mm3 - Wa);
    const double Wef_inf_ld = Wa + res.fator_interacao * (res.secao_longa.Wtr_inf_mm3 - Wa);
    res.Wef_inf_cd_cm3 = Wef_inf_cd / 1e3;
    res.Wef_inf_ld_cm3 = Wef_inf_ld / 1e3;

    // Parcelas de deslocamento (k = 5·L⁴ / (384·E); cargas em N/mm, comprimentos em mm)
    const double k = 5.0 * std::pow(Lv_mm, 4.0) / (384.0 * E);
    const double fluencia = (1.0 / Ief_ld) - (1.0 / Ief_cd);

    if (!escorada) {
        res.flecha_g1_mm = k * g1 / Ia;
        res.flecha_perm_curta_mm = res.flecha_g1_mm + k * g2 / Ief_cd;
        res.flecha_fluencia_mm = k * g2 * fluencia;
    } else {
        res.flecha_g1_mm = 0.0;
        res.flecha_perm_curta_mm = k * (g1 + g2) / Ief_cd;
        res.flecha_fluencia_mm = k * (g1 + g2) * fluencia;
    }

    if (acoes.considerar_retracao) {
        // Psh = eps_sh·(Ec/3)·bef·tc ; esh = distância do centro da laje à LNE de longa duração
        const double Psh_N = acoes.eps_sh * (concreto.Ecs_MPa / 3.0) * laje.bef_mm * laje.tc_mm;
        const double esh_mm = perfil.d + laje.hF_mm + laje.tc_mm / 2.0 - res.secao_longa.yLN_mm;
        res.flecha_retracao_mm = Psh_N * esh_mm * Lv_mm * Lv_mm / (8.0 * E * Ief_ld);
    }
    res.flecha_longa_duracao_mm = res.flecha_fluencia_mm + res.flecha_retracao_mm;

    const double psi_max = fator_psi_combinacao(acoes.combinacao_flecha_max, acoes.psi1, acoes.psi2);
    const double psi_dano = fator_psi_combinacao(acoes.combinacao_flecha_dano, acoes.psi1, acoes.psi2);
    res.flecha_var_max_mm = k * psi_max * q / Ief_cd + k * acoes.psi2 * q * fluencia;
    res.flecha_var_dano_mm = k * psi_dano * q / Ief_cd + k * acoes.psi2 * q * fluencia;

    res.flecha_total_bruta_mm = res.flecha_perm_curta_mm + res.flecha_longa_duracao_mm + res.flecha_var_max_mm;
    res.contraflecha_mm = std::clamp(acoes.contraflecha_mm, 0.0, res.flecha_perm_curta_mm); // B.3.2
    res.flecha_total_mm = res.flecha_total_bruta_mm - res.contraflecha_mm;
    res.flecha_dano_mm = res.flecha_longa_duracao_mm + res.flecha_var_dano_mm;
    res.contraflecha_sugerida_mm = std::floor(res.flecha_g1_mm / 5.0) * 5.0;

    const double den = (acoes.denominador_limite_flecha > 0.0) ? acoes.denominador_limite_flecha : 350.0;
    res.limite_total_mm = Lv_mm / den;
    res.limite_dano_mm = acoes.alvenaria_solidarizada ? std::min(res.limite_total_mm, 15.0) : res.limite_total_mm;
    res.taxa_flecha_total = res.flecha_total_mm / res.limite_total_mm;
    res.taxa_flecha_dano = res.flecha_dano_mm / res.limite_dano_mm;
    res.aprovado_flecha = (res.taxa_flecha_total <= 1.00);
    res.aprovado_flecha_dano = (res.taxa_flecha_dano <= 1.00);

    // NBR 8800:2008 L.3.2: permanentes (sem parcela dependente do tempo) + psi1·q, vigas biapoiadas
    res.flecha_vibracao_2008_mm = res.flecha_perm_curta_mm + k * acoes.psi1 * q / Ief_cd;

    // Tensão na mesa inferior em serviço (O.2.3.2-b / L.2.3.2-b: combinação rara de serviço)
    const double L_m = Lv_mm / 1000.0;
    const double M_g1_S = (g1 * L_m * L_m) / 8.0;
    const double M_g2_S = (g2 * L_m * L_m) / 8.0;
    const double M_q2_S = (q * L_m * L_m) / 8.0;

    if (!escorada) {
        res.tensao_mesa_inferior_MPa = (M_g1_S * 1e6 / Wa) + (M_g2_S * 1e6 / Wef_inf_ld) + (M_q2_S * 1e6 / Wef_inf_cd);
    } else {
        res.tensao_mesa_inferior_MPa = ((M_g1_S + M_g2_S) * 1e6 / Wef_inf_ld) + (M_q2_S * 1e6 / Wef_inf_cd);
    }
    res.limite_tensao_MPa = aco.fy_MPa;
    res.tensao_servico_atende = (res.tensao_mesa_inferior_MPa <= res.limite_tensao_MPa);

    return res;
}

} // namespace v2008

// ============================================================================
// NAMESPACE v2024: ABNT NBR 8800:2024
// ============================================================================
namespace v2024 {

double calcularEtaC(double fck_MPa) {
    // NBR 8800:2024 item L.5.3.4 / M.4: fator de fragilidade para concretos fck > 40 MPa
    if (fck_MPa <= 40.0) return 1.00;
    return std::min(1.00, std::cbrt(40.0 / fck_MPa));
}

double calcularEcsConcreto(double fck_MPa, double alpha_ag) {
    // NBR 8800:2024 item 4.6.3.1: Eci = alpha_ag * 5600 * sqrt(fck), Ec = 0.85 * Eci
    return 0.85 * alpha_ag * 5600.0 * std::sqrt(fck_MPa);
}

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double Lv_mm, double kx, double ky, double gamma_a1
) {
    // NBR 8800:2024 Seção 5.3 (págs. 45-48):
    // Extinto o fator Q = Qs * Qa; adotado o Método da Largura Efetiva Unificado (Aef),
    // com lambda_0 calculado sempre com a área bruta Ag e fatores c1, c2 da Tabela 5.
    ResultadoNormalResistentePerfil res;

    res.Nex_kN = ((PI * PI * aco.E_MPa * perfil.Ix) / std::pow(kx * Lv_mm, 2.0)) / 1000.0;
    res.Ney_kN = ((PI * PI * aco.E_MPa * perfil.Iy) / std::pow(ky * Lb_mm, 2.0)) / 1000.0;

    // Flambagem elástica por torção Nez (Item 5.3.5.1-c)
    double r0_sq = (perfil.rx * perfil.rx) + (perfil.ry * perfil.ry);
    double G_MPa = (aco.G_MPa > 0.0) ? aco.G_MPa : 77000.0;
    double Nez_kN = (r0_sq > 0.0)
        ? ((1.0 / r0_sq) * (((PI * PI * aco.E_MPa * perfil.Cw) / std::pow(ky * Lb_mm, 2.0)) + G_MPa * perfil.It)) / 1000.0
        : res.Ney_kN;

    res.Ne_kN = std::min({res.Nex_kN, res.Ney_kN, Nez_kN});

    // Item 5.3.3.2: índice de esbeltez reduzido lambda_0 calculado com Ag (sem Q!)
    res.lb0 = std::sqrt((perfil.Ag * aco.fy_MPa) / (res.Ne_kN * 1000.0));

    // Item 5.3.3.1: fator de redução chi
    res.chi = (res.lb0 <= 1.5) ? std::pow(0.658, res.lb0 * res.lb0) : (0.877 / (res.lb0 * res.lb0));

    double raiz_chi = std::sqrt(res.chi);

    // Item 5.3.4.2 e Tabelas 4 e 5: Largura efetiva das mesas (Elementos AL: c1 = 0.22, c2 = 1.49)
    double lbf = perfil.lbf;
    double lbf_lim = 0.0;
    if (perfil.tipo == TipoFabricacao::LAMINADO) {
        lbf_lim = 0.56 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    } else {
        double kc = std::clamp(4.0 / std::sqrt(perfil.h / perfil.tw), 0.35, 0.76);
        lbf_lim = 0.64 * std::sqrt(aco.E_MPa / (aco.fy_MPa / kc));
    }

    double b_mesa = perfil.bf / 2.0;
    double bef_mesa = b_mesa;
    if (lbf > (lbf_lim / raiz_chi)) {
        double c1_al = 0.22;
        double c2_al = 1.49;
        double sigma_el_f = std::pow(c2_al * (lbf_lim / lbf), 2.0) * aco.fy_MPa;
        double razao_f = std::sqrt(sigma_el_f / (res.chi * aco.fy_MPa));
        bef_mesa = b_mesa * (1.0 - c1_al * razao_f) * razao_f;
        bef_mesa = std::clamp(bef_mesa, 0.0, b_mesa);
    }

    // Item 5.3.4.2 e Tabelas 4 e 5: Largura efetiva da alma (Elemento AA: c1 = 0.18, c2 = 1.31)
    double lbw = perfil.lbw;
    double lbw_lim = 1.49 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double b_alma = (perfil.tipo == TipoFabricacao::LAMINADO && perfil.d1 > 0.0) ? perfil.d1 : perfil.h;
    double bef_alma = b_alma;
    if (lbw > (lbw_lim / raiz_chi)) {
        double c1_aa = 0.18;
        double c2_aa = 1.31;
        double sigma_el_w = std::pow(c2_aa * (lbw_lim / lbw), 2.0) * aco.fy_MPa;
        double razao_w = std::sqrt(sigma_el_w / (res.chi * aco.fy_MPa));
        bef_alma = b_alma * (1.0 - c1_aa * razao_w) * razao_w;
        bef_alma = std::clamp(bef_alma, 0.0, b_alma);
    }

    // Área efetiva total Aef (4 meias-mesas + 1 alma)
    double reducao_mesas = 4.0 * (b_mesa - bef_mesa) * perfil.tf;
    double reducao_alma  = (b_alma - bef_alma) * perfil.tw;
    double Aef = std::max(0.0, perfil.Ag - reducao_mesas - reducao_alma);

    res.Qs = bef_mesa / std::max(1e-6, b_mesa);
    res.Qa = bef_alma / std::max(1e-6, b_alma);
    res.Q  = Aef / std::max(1e-6, perfil.Ag);

    // Item 5.3.2: Nc,Rd = (chi * Aef * fy) / gamma_a1
    res.NRd_kN = ((res.chi * Aef * aco.fy_MPa) / gamma_a1) / 1000.0;
    return res;
}

double calcularMcrFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb) {
    // NBR 8800:2024 Anexo D, item D.2.8-a (pág. 145): Cw = Iy * (d - tf)^2 / 4 para seções I
    if (Lb_mm <= 0.0) return 1e12;
    double Cw_2024 = (perfil.Cw > 0.0) ? perfil.Cw : ((perfil.Iy * std::pow(perfil.d - perfil.tf, 2.0)) / 4.0);
    double Mcr = (((Cb * PI * PI * aco.E_MPa * perfil.Iy) / (Lb_mm * Lb_mm)) *
        std::sqrt((Cw_2024 / perfil.Iy) * (1.0 + 0.039 * (perfil.It * Lb_mm * Lb_mm / Cw_2024)))) / 1e6;
    return Mcr;
}

double calcularMRdFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double gamma_a1, double Cb, double Mpl, double Mr) {
    // NBR 8800:2024 Anexo D, itens D.2.1 (pág. 137) e D.2.8-a (pág. 145):
    // Diferença importante em relação a 2008: Cb agora entra DENTRO da expressão de lambda_r
    // (lambda_r = (1.38 * Cb * sqrt(Iy*J) / (ry*J*beta1)) * sqrt(1 + sqrt(1 + 27*Cw*beta1^2 / (Cb^2 * Iy))))
    // e a equação de interpolação em D.2.1 não multiplica externamente por Cb.
    if (Lb_mm <= 0.0) return Mpl / gamma_a1;

    double lb = Lb_mm / perfil.ry;
    double lb_p = 1.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double beta1 = (0.7 * aco.fy_MPa * perfil.Wx) / (aco.E_MPa * perfil.It);
    double Cw_2024 = (perfil.Cw > 0.0) ? perfil.Cw : ((perfil.Iy * std::pow(perfil.d - perfil.tf, 2.0)) / 4.0);

    double lb_r = ((1.38 * Cb * std::sqrt(perfil.Iy * perfil.It)) / (perfil.ry * perfil.It * beta1))
        * std::sqrt(1.0 + std::sqrt(1.0 + ((27.0 * Cw_2024 * beta1 * beta1) / (Cb * Cb * perfil.Iy))));

    double Mcr = v2024::calcularMcrFLT(perfil, aco, Lb_mm, Cb);
    double MRd_kNm = 0.0;

    if (lb <= lb_p) {
        MRd_kNm = Mpl / gamma_a1;
    } else if (lb <= lb_r) {
        MRd_kNm = (1.0 / gamma_a1) * (Mpl - (Mpl - Mr) * ((lb - lb_p) / (lb_r - lb_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl / gamma_a1);
    } else {
        MRd_kNm = std::min(Mcr / gamma_a1, Mpl / gamma_a1);
    }

    return MRd_kNm;
}

double calcularMRdFLM(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr) {
    // NBR 8800:2024 Anexo D, Tabela D.1 e item D.2.8-f (pág. 146)
    return v2008::calcularMRdFLM(perfil, aco, gamma_a1, Mpl, Mr);
}

double calcularMRdFLA(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, [[maybe_unused]] double Mr) {
    // NBR 8800:2024 Anexo D, Tabela D.1 (FLA): Mr = fy * Wx
    // (mesmo critério da Tabela G.1 de 2008; a tensão residual só entra em FLT e FLM)
    double lbw = perfil.lbw;
    double lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    double lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);

    double Mr_fla_2024 = (aco.fy_MPa * perfil.Wx) / 1e6;
    double MRd_kNm = 0.0;

    if (lbw <= lbw_p) {
        MRd_kNm = Mpl / gamma_a1;
    } else if (lbw <= lbw_r) {
        MRd_kNm = (1.0 / gamma_a1) * (Mpl - (Mpl - Mr_fla_2024) * ((lbw - lbw_p) / (lbw_r - lbw_p)));
        MRd_kNm = std::min(MRd_kNm, Mpl / gamma_a1);
    } else {
        MRd_kNm = 0.0;
    }

    return MRd_kNm;
}

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb, double gamma_a1
) {
    ResultadoMomentoResistentePerfil res;

    res.Mpl_kNm = std::min((perfil.Zx * aco.fy_MPa) / 1e6, (1.5 * perfil.Wx * aco.fy_MPa) / 1e6);
    res.Mr_kNm = (0.7 * aco.fy_MPa * perfil.Wx) / 1e6;
    res.Mcr_kNm = v2024::calcularMcrFLT(perfil, aco, Lb_mm, Cb);

    res.MRd_FLT_kNm = v2024::calcularMRdFLT(perfil, aco, Lb_mm, gamma_a1, Cb, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLM_kNm = v2024::calcularMRdFLM(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);
    res.MRd_FLA_kNm = v2024::calcularMRdFLA(perfil, aco, gamma_a1, res.Mpl_kNm, res.Mr_kNm);

    if (res.MRd_FLT_kNm == 0.0 || res.MRd_FLM_kNm == 0.0 || res.MRd_FLA_kNm == 0.0) {
        res.perfil_esbelto = true;
    }

    res.MRd_kNm = std::min({res.MRd_FLT_kNm, res.MRd_FLM_kNm, res.MRd_FLA_kNm});
    return res;
}

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(
    const PerfilI& perfil, const Aco& aco,
    const Concreto& concreto, const Laje& laje,
    double gamma_a1, double gamma_c, double Fhd_conectores_kN
) {
    // NBR 8800:2024 item L.6.3.1 (págs. 202-207):
    // 1. Inclui coeficiente de fragilidade do concreto eta_c = (40/fck)^(1/3) <= 1.0 (L.5.3.4)
    // 2. Para alma semicompacta (L.6.3.1.2, pág. 207), limita a compressão na laje a 0.85*eta_c*fcd e usa Wef
    ResultadoMomentoPositivoVM res;

    res.lbw = perfil.lbw;
    res.lbw_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 5.70 * std::sqrt(aco.E_MPa / aco.fy_MPa);
    res.eta_c = v2024::calcularEtaC(concreto.fck_MPa);

    double fcd = concreto.fck_MPa / gamma_c;
    double fyd = aco.fy_MPa / gamma_a1;

    res.Cc_max_kN = 0.85 * res.eta_c * fcd * laje.bef_mm * laje.tc_mm * 1e-3;
    double Fa_max_kN = perfil.Ag * fyd * 1e-3;
    double Fhd_max_kN = std::min(res.Cc_max_kN, Fa_max_kN);

    bool interacao_parcial = (Fhd_conectores_kN >= 0.0 && Fhd_conectores_kN < Fhd_max_kN - 1e-6);
    res.Ccd_kN = interacao_parcial ? Fhd_conectores_kN : Fhd_max_kN;
    res.eta_interacao = (Fhd_max_kN > 0.0) ? std::min(1.0, res.Ccd_kN / Fhd_max_kN) : 1.0;

    if (res.lbw <= res.lbw_p) {
        res.secao_compacta = true;
        res.T_aco_kN = Fa_max_kN;

        if (!interacao_parcial && res.Cc_max_kN >= res.T_aco_kN) {
            // Caso (a): LNP na laje de concreto (Interação Completa - NBR 8800:2024 L.6.3.1.1.1-a)
            res.lnp_na_laje = true;
            res.C_aco_kN = 0.0;
            res.Ccd_kN = res.T_aco_kN;
            res.a_linha_neutra_mm = (res.T_aco_kN * 1e3) / (0.85 * res.eta_c * fcd * laje.bef_mm);
            res.y_lnp_mm = 0.0;

            res.MRd_VM_kNm = res.T_aco_kN * (perfil.d / 2.0 + laje.hF_mm +
                laje.tc_mm - res.a_linha_neutra_mm / 2.0) * 1e-3;
        } else {
            // Caso (b): LNP no perfil com interação completa, ou Caso (c): Interação parcial (L.6.3.1.1.1-b/c)
            res.lnp_na_laje = false;
            res.a_linha_neutra_mm = interacao_parcial
                ? ((res.Ccd_kN * 1e3) / (0.85 * res.eta_c * fcd * laje.bef_mm))
                : laje.tc_mm;

            res.C_aco_kN = 0.5 * (Fa_max_kN - res.Ccd_kN);
            res.T_aco_kN = res.C_aco_kN + res.Ccd_kN;

            double yp = 0.0;
            double yc = 0.0;
            double yt = 0.0;

            if (res.C_aco_kN <= perfil.bf * perfil.tf * fyd * 1e-3) {
                // LNP na mesa superior do perfil
                yp = (res.C_aco_kN * 1e3) / (perfil.bf * fyd);
                yc = yp / 2.0;

            } else {
                // LNP na alma do perfil
                yp = perfil.tf + (perfil.h
                    * (((res.C_aco_kN * 1e3) - (perfil.bf * perfil.tf * fyd))
                    / (perfil.h * perfil.tw * fyd)));

                yc = (((perfil.bf * perfil.tf * perfil.tf / 2.0)
                    + (perfil.tw * (yp - perfil.tf) * ((yp + perfil.tf) / 2.0)))
                    / ((perfil.bf * perfil.tf) + ((yp - perfil.tf) * perfil.tw)));

            }

            res.y_lnp_mm = yp;

            // Centro da parte tracionada (a partir da face inferior) pela diferença de momentos
            // estáticos: perfil inteiro (Aa, centroide em d/2) menos a parte comprimida.
            // Usa a mesma área Aa das forças plásticas (consistente com a planilha de referência).
            const double Ac = (yp <= perfil.tf)
                ? perfil.bf * yp
                : perfil.bf * perfil.tf + perfil.tw * (yp - perfil.tf);
            const double At = perfil.Ag - Ac;
            yt = (perfil.Ag * perfil.d / 2.0 - Ac * (perfil.d - yc)) / At;

            double braco_conc = (laje.tc_mm - res.a_linha_neutra_mm / 2.0) + laje.hF_mm + perfil.d - yt;
            res.MRd_VM_kNm = (res.C_aco_kN * (perfil.d - yt - yc) + res.Ccd_kN * braco_conc) * 1e-3;
        }
    } else if (res.lbw <= res.lbw_r) {
        // Alma semicompacta (NBR 8800:2024 L.6.3.1.2, pág. 207):
        // Tensão de tração na face inferior <= fyd (com Wef para interação parcial) e compressão no topo da laje <= 0.85*fcd
        res.secao_compacta = false;
        res.lnp_na_laje = false;

        // Seção homogeneizada de curta duração (n = Ea/Ec), concreto tracionado desprezado
        auto secao_tr = nbr8800::calcularSecaoHomogeneizadaELS(
            perfil, laje, aco.E_MPa / concreto.Ecs_MPa
        );

        double Wef_inf = perfil.Wx + std::sqrt(res.eta_interacao) * (secao_tr.Wtr_inf_mm3 - perfil.Wx);
        double fcd_lim = 0.85 * res.eta_c * fcd;

        res.MRd_VM_kNm = std::min(
            Wef_inf * fyd,
            secao_tr.razao_modular * secao_tr.Wtr_sup_mm3 * fcd_lim
        ) * 1e-6;
    } else {
        res.aplicavel = false;
        res.secao_compacta = false;
        res.MRd_VM_kNm = 0.0;
        return res;
    }

    return res;
}

static double calcularCdist2024(double psi) {
    // NBR 8800:2024 Tabela L.1 (pág. 214) - Coeficiente Cdist para vigas contínuas sob carga distribuída
    static const std::vector<std::pair<double, double>> tabela2024 = {
        {0.50, 2.00}, {0.75, 1.78}, {1.00, 1.65},
        {1.25, 1.56}, {1.50, 1.50}, {1.75, 1.47},
        {2.00, 1.43}, {2.25, 1.36}, {2.50, 1.35}
    };

    if (psi <= 0.50) return 2.00;
    if (psi >= 2.50) return 1.35;

    for (size_t i = 0; i < tabela2024.size() - 1; ++i) {
        if (psi >= tabela2024[i].first && psi <= tabela2024[i + 1].first) {
            double x0 = tabela2024[i].first, y0 = tabela2024[i].second;
            double x1 = tabela2024[i + 1].first, y1 = tabela2024[i + 1].second;
            return y0 + (psi - x0) * (y1 - y0) / (x1 - x0);
        }
    }
    return 2.00;
}

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(
    const PerfilI& perfil,
    const Aco& aco, const Concreto& concreto, const Laje& laje,
    const ArmacaoLaje& armacaoLaje, double MSd_neg_kNm, double MSd_pos_kNm,
    double Lv_mm, double a_vigas_mm, double alpha_viga, double gamma_a1, double gamma_c
) {
    // NBR 8800:2024 itens L.6.4 e L.6.5 (págs. 208-215):
    // 1. Momento plástico negativo da seção transversal (L.6.4.1)
    // 2. Flambagem Lateral com Distorção (L.6.5.2 a L.6.5.6) com kappa_g, eta_b, semiondas n e Tabela L.1 de Cdist.
    ResultadoMomentoNegativoVM res;

    auto calcularMRdSecaoNegativa = [&](double g_a1, double f_sd) -> std::pair<bool, double> {
        double lbf = perfil.lbf;
        double lbf_p = 0.38 * std::sqrt(aco.E_MPa / aco.fy_MPa);
        if (lbf > lbf_p) return {false, 0.0};

        double fyd = aco.fy_MPa / g_a1;
        double Tds = armacaoLaje.Ads * f_sd;
        double lb_p = 3.76 * std::sqrt(aco.E_MPa / aco.fy_MPa);

        if ((Tds + perfil.bf * perfil.tf * fyd) >= (perfil.h * perfil.tw + perfil.tf * perfil.bf) * fyd) {
            double Tad = (perfil.Ag * fyd - Tds) / 2.0;
            double yp = Tad / (perfil.bf * fyd);
            double lb = 2.0 * perfil.d1 / perfil.tw;
            if (lb > lb_p) return {false, 0.0};

            double yc = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                + perfil.h * perfil.tw * ((perfil.h / 2.0) + perfil.tf)
                + (perfil.bf / 2.0) * (perfil.tf - yp) * (3.0 * perfil.tf + 2.0 * perfil.h - yp))
                / ((perfil.bf * perfil.tf) + (perfil.h * perfil.tw) + perfil.bf * (perfil.tf - yp));

            double yt = yp / 2.0;
            double d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;
            double d4 = yt;
            double d5 = perfil.d - yp - yc;
            return {true, (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad) * d5) * 1e-6};
        } else {
            double Tad = (perfil.Ag * fyd - Tds) / 2.0;
            double yp = perfil.tf + perfil.h * ((Tad - (perfil.bf * perfil.tf * fyd)) / (perfil.h * perfil.tw * fyd));
            double lb = (2.0 * (perfil.h - yp + perfil.tf - perfil.r)) / perfil.tw;
            if (lb > lb_p) return {false, 0.0};

            double yc = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                + (perfil.d - yp - perfil.tf) * perfil.tw * ((perfil.d - yp + perfil.tf) / 2.0))
                / ((perfil.bf * perfil.tf) + (perfil.d - yp - perfil.tf) * perfil.tw);

            double yt = ((perfil.bf * perfil.tf * perfil.tf / 2.0)
                + (yp - perfil.tf) * perfil.tw * ((yp + perfil.tf) / 2.0))
                / ((perfil.bf * perfil.tf) + (yp - perfil.tf) * perfil.tw);

            double d3 = laje.tc_mm + laje.hF_mm + yp - armacaoLaje.d_linha_mm;
            double d4 = yp - yt;
            double d5 = perfil.d - yp - yc;
            return {true, (Tds * d3 + Tad * d4 + (perfil.Ag * fyd - Tad) * d5) * 1e-6};
        }
    };

    [[maybe_unused]] double fcd = concreto.fck_MPa / gamma_c;
    auto [ok_d, mrd_neg_kNm] = calcularMRdSecaoNegativa(gamma_a1, armacaoLaje.fsd);
    if (!ok_d) {
        res.aplicavel = false;
        res.MRd_neg_kNm = 0.0;
        res.Mrd_dist_kNm = 0.0;
        res.MRd_VM_kNm = 0.0;
        return res;
    }
    res.MRd_neg_kNm = mrd_neg_kNm;

    // Momento resistente característico MRk^- (L.6.5.2: coeficientes de ponderação iguais a 1.00)
    auto [ok_k, mrk_neg_kNm] = calcularMRdSecaoNegativa(1.00, armacaoLaje.fsd * 1.15);
    double MRk_neg_Nmm = (ok_k ? mrk_neg_kNm : mrd_neg_kNm) * 1e6;

    double h0 = perfil.d - perfil.tf;
    double alpha_e = aco.E_MPa / concreto.Ecs_MPa;
    double I_laje = nbr8800::v2008::calcularInerciaLaje(laje, armacaoLaje, alpha_e);

    double k1 = alpha_viga * aco.E_MPa * I_laje / a_vigas_mm;
    double nu_a = (aco.nu > 0.0) ? aco.nu : 0.30;
    double k2 = (aco.E_MPa * std::pow(perfil.tw, 3.0)) / (4.0 * h0 * (1.0 - nu_a * nu_a));
    double kr = (k1 * k2) / (k1 + k2);

    double ytr = ((perfil.Ag * perfil.d / 2.0)
        + armacaoLaje.Ads * (perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm))
        / (perfil.Ag + armacaoLaje.Ads);

    double y_bar = std::abs(ytr - (perfil.d / 2.0));
    double Ix_mista = perfil.Ix
        + perfil.Ag * std::pow(ytr - (perfil.d / 2.0), 2.0)
        + armacaoLaje.Ads * std::pow(perfil.d + laje.hF_mm + laje.tc_mm - armacaoLaje.d_linha_mm - ytr, 2.0);

    // Item L.6.5.5: fator geométrico kappa_g e fator de rigidez rotacional eta_b
    double kappa_g = (0.31 + 0.69 * std::pow(0.05, y_bar / h0)) * (Ix_mista / perfil.Ix);
    double Iaf_y = std::pow(perfil.bf, 3.0) * perfil.tf / 12.0;
    double eta_b = std::sqrt((kr * std::pow(Lv_mm, 4.0)) / (aco.E_MPa * Iaf_y * h0 * h0));

    // Item L.6.5.6: número de semiondas n
    double n_id = std::sqrt(eta_b) / PI;
    int n1 = std::max(1, static_cast<int>(std::floor(n_id)));
    int n2 = n1 + 1;
    int n_ondas = (n_id <= std::sqrt(static_cast<double>(n1 * n2))) ? n1 : n2;

    double psi = MSd_neg_kNm / std::max(1e-3, MSd_pos_kNm);
    double Cdist = calcularCdist2024(psi);
    double G_MPa = (aco.G_MPa > 0.0) ? aco.G_MPa : 77000.0;

    // Item L.6.5.5: Momento crítico Mcr
    double termo_ondas = std::pow(n_ondas * PI, 2.0) + std::pow(eta_b / (n_ondas * PI), 2.0);
    double Mcr_Nmm = Cdist * (kappa_g / h0) *
        ((G_MPa * perfil.It) + ((aco.E_MPa * Iaf_y * h0 * h0) / (Lv_mm * Lv_mm)) * termo_ondas);

    // Item L.6.5.2: lambda_dist = sqrt(MRk^- / Mcr)
    double lb_dist = std::sqrt(MRk_neg_Nmm / std::max(1.0, Mcr_Nmm));
    double X_dist = 1.0;
    if (lb_dist <= 0.4) X_dist = 1.0;
    else if (lb_dist <= 1.5) X_dist = std::pow(0.658, lb_dist * lb_dist);
    else X_dist = 0.877 / (lb_dist * lb_dist);

    res.Mrd_dist_kNm = X_dist * res.MRd_neg_kNm;
    res.MRd_VM_kNm = std::min(res.MRd_neg_kNm, res.Mrd_dist_kNm);
    return res;
}

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil, const Aco& aco, double a_enrijecedor_mm, double gamma_a1
) {
    ResultadoCortanteResistentePerfil res;

    // NBR 8800:2024 item 5.4.3.1.1: kv = 5.34 para almas sem enrijecedores transversais (a/h > 3)
    bool sem_enrijecedor = (a_enrijecedor_mm <= 0.0)
        || ((a_enrijecedor_mm / perfil.h) > 3.0)
        || ((a_enrijecedor_mm / perfil.h) > std::pow(260.0 / (perfil.h / perfil.tw), 2.0));

    res.kv = sem_enrijecedor ? 5.34 : (5.0 + 5.0 / std::pow(a_enrijecedor_mm / perfil.h, 2.0));
    res.Vpl_kN = (0.6 * perfil.d * perfil.tw * aco.fy_MPa) / 1000.0;

    res.lbw = perfil.lbw;
    res.lbw_p = 1.10 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);
    res.lbw_r = 1.37 * std::sqrt(res.kv * aco.E_MPa / aco.fy_MPa);

    if (res.lbw <= res.lbw_p) {
        res.VRd_kN = res.Vpl_kN / gamma_a1;
    } else if (res.lbw <= res.lbw_r) {
        res.VRd_kN = (res.lbw_p / res.lbw) * (res.Vpl_kN / gamma_a1);
    } else {
        res.VRd_kN = 1.24 * std::pow(res.lbw_p / res.lbw, 2.0) * (res.Vpl_kN / gamma_a1);
    }

    return res;
}

double calcularGrauInteracaoMinimo(
    [[maybe_unused]] const PerfilI& perfil,
    const Aco& aco,
    double Lv_mm,
    bool escorada,
    double MSd_kNm,
    double MRd_kNm
) {
    // NBR 8800:2024 item L.2.3.1.1.2:
    // eta_i = max(cp * cr^2 * eta_0, eta_lim) <= 1.0
    // eta_0 = 1 - (E / (580 * fy)) * [0.75 - 0.45*rho - (0.030 - 0.015*rho)*Le]
    // eta_lim = 0.40 * cr * (1 + 0.8*rho)
    double Le_m = Lv_mm / 1000.0;
    double rho = 0.0; // Perfil I duplamente simétrico (Afb == Aft)
    double cp = escorada ? 1.00 : 0.85;
    double cr = (MSd_kNm > 0.0 && MRd_kNm > 0.0) ? std::clamp(MSd_kNm / MRd_kNm, 0.70, 1.00) : 1.00;

    double colchete = 0.75 - 0.45 * rho - (0.030 - 0.015 * rho) * Le_m;
    double eta_0 = 1.0 - (aco.E_MPa / (580.0 * aco.fy_MPa)) * colchete;
    double eta_lim = 0.40 * cr * (1.0 + 0.8 * rho);

    double eta_i = std::max(cp * cr * cr * eta_0, eta_lim);
    return std::clamp(eta_i, eta_lim, 1.00);
}

ResultadoRotinaE calcularConectoresCisalhamento(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    bool escorada,
    double gamma_a1,
    double gamma_c,
    double gamma_cs
) {
    ResultadoRotinaE res;
    res.beff_mm = laje.bef_mm;
    res.eta_c = v2024::calcularEtaC(concreto.fck_MPa);

    double fyd = aco.fy_MPa / gamma_a1;
    double fcd = concreto.fck_MPa / gamma_c;

    double Fa_kN = (perfil.Ag * fyd) * 1e-3;
    double Fc_kN = (0.85 * res.eta_c * fcd * laje.bef_mm * laje.tc_mm) * 1e-3;
    res.Fhd_kN = std::min(Fa_kN, Fc_kN);

    if (conector.tipo == CONECTOR_PERFIL_U) {
        // NBR 8800:2024 item L.4.2.2.1: Conector em Perfil U laminado
        res.Rg = 1.0;
        res.Rp = 1.0;
        res.QRd1_concreto_kN = (0.3 * (conector.tf + 0.5 * conector.tw) * conector.Lcs *
            std::sqrt(concreto.fck_MPa * concreto.Ecs_MPa)) / (gamma_cs * 1000.0);
        res.QRd2_aco_kN = res.QRd1_concreto_kN;
        res.QRd_kN = res.QRd1_concreto_kN;
        res.s_min_long_mm = std::max(conector.h, conector.Lcs);
        res.s_max_long_mm = 8.0 * (laje.tc_mm + laje.hF_mm);
    } else {
        // NBR 8800:2024 item L.4.2.1: Pino com Cabeça (Stud Bolt)
        double d = conector.d;
        res.Acs_mm2 = (PI * d * d) / 4.0;
        int n_linha = std::max(1, conector.nt);

        if (laje.tipo == LAJE_MACICA) {
            res.Rg = 1.00;
            // NBR 8800:2024 item L.4.2.1.3-a: pino soldado diretamente no perfil sem fôrma -> Rp = 0.75!
            res.Rp = 0.75;
        } else {
            // LAJE_STEEL_DECK (Tabela L.4.2.1.2 e L.4.2.1.3)
            if (!laje.nervura_paralela) { // Nervuras perpendiculares à viga
                if (n_linha == 1) res.Rg = 1.00;
                else if (n_linha == 2) res.Rg = 0.85;
                else res.Rg = 0.70;

                // O.4.2.1.3-b/c e L.4.2.1.3-b/c: depende apenas de emh
                res.Rp = (conector.emh_mm >= 50.0) ? 0.75 : 0.60;
            } else { // Nervuras paralelas à viga
                double relacao = (laje.hF_mm > 0.0) ? (laje.bF_mm / laje.hF_mm) : 2.0;
                res.Rg = (relacao >= 1.5) ? 1.00 : 0.85; // O.4.2.1.2-a3/b2
                res.Rp = 0.75;
            }
        }

        if (conector.Rg > 0.0) res.Rg = conector.Rg;
        if (conector.Rp > 0.0) res.Rp = conector.Rp;

        res.QRd1_concreto_kN = (res.Acs_mm2 * std::sqrt(concreto.fck_MPa * concreto.Ecs_MPa)) / (2.0 * gamma_cs * 1000.0);
        res.QRd2_aco_kN = (res.Rg * res.Rp * res.Acs_mm2 * conector.fu) / (gamma_cs * 1000.0);
        res.QRd_kN = std::min(res.QRd1_concreto_kN, res.QRd2_aco_kN);

        // Limites de espaçamento NBR 8800:2024 (L.4.3.3 e L.4.3.4)
        res.s_min_long_mm = (laje.tipo == LAJE_STEEL_DECK) ? (4.0 * d) : (6.0 * d);
        double h_total_laje = laje.tc_mm + laje.hF_mm;
        res.s_max_long_mm = (laje.tipo == LAJE_STEEL_DECK && !laje.nervura_paralela)
            ? std::min(8.0 * h_total_laje, 915.0)
            : (8.0 * h_total_laje);
    }

    // Conectores necessários para interação completa (100%):
    res.n_semi_vao_calculado = static_cast<int>(std::ceil(res.Fhd_kN / std::max(1e-3, res.QRd_kN)));
    res.nl_conectores = 2 * res.n_semi_vao_calculado;

    // Espaçamento longitudinal adotado:
    bool is_perp_2024 = (laje.tipo == LAJE_STEEL_DECK && !laje.nervura_paralela);
    int n_linha = std::max(1, conector.nt);

    if (is_perp_2024) {
        int k = std::max(1, conector.k_nervuras);
        double passo = (laje.eF_mm > 0.0) ? laje.eF_mm : 200.0;
        res.el_espacamento_mm = k * passo;
        res.alerta_ancoragem_deck = (res.el_espacamento_mm > 450.0);
        int nlin_2024 = static_cast<int>(std::floor((Lv_mm / 2.0) / std::max(1.0, res.el_espacamento_mm)));
        res.n_linhas_meio_vao = nlin_2024;
        res.n_semi_vao_adotado = nlin_2024 * n_linha;
        res.n_total_adotado = 2 * res.n_semi_vao_adotado;
    } else if (conector.espacamento_adotado_mm > 0.0) {
        res.el_espacamento_mm = conector.espacamento_adotado_mm;
        res.alerta_ancoragem_deck = false;
        int nlin_2024 = static_cast<int>(std::floor((Lv_mm / 2.0) / std::max(1.0, res.el_espacamento_mm)));
        res.n_linhas_meio_vao = nlin_2024;
        res.n_semi_vao_adotado = nlin_2024 * n_linha;
        res.n_total_adotado = 2 * res.n_semi_vao_adotado;
    } else {
        // Se espacamento_adotado_mm <= 0.0, dimensiona para interação completa (100%)
        res.alerta_ancoragem_deck = false;
        res.n_semi_vao_adotado = res.n_semi_vao_calculado;
        res.n_total_adotado = res.nl_conectores;
        int nlin_2024 = std::max(1, static_cast<int>(std::ceil(static_cast<double>(res.n_semi_vao_calculado) / n_linha)));
        res.n_linhas_meio_vao = nlin_2024;
        res.el_espacamento_mm = (nlin_2024 > 0) ? ((Lv_mm / 2.0) / nlin_2024) : 200.0;
    }

    // Grau de interação calculado:
    res.grau_interacao_eta = std::min(1.00, (res.n_semi_vao_adotado * res.QRd_kN) / std::max(1e-3, res.Fhd_kN));
    res.grau_interacao_minimo = v2024::calcularGrauInteracaoMinimo(perfil, aco, Lv_mm, escorada);
    res.n_semi_vao_minimo = static_cast<int>(std::ceil(res.grau_interacao_minimo * res.Fhd_kN / std::max(1e-3, res.QRd_kN)));
    res.n_total_minimo = 2 * res.n_semi_vao_minimo;

    res.espacamento_atende = (res.el_espacamento_mm >= res.s_min_long_mm - 1e-4 && res.el_espacamento_mm <= res.s_max_long_mm + 1e-4);
    res.grau_interacao_atende = (res.grau_interacao_eta >= res.grau_interacao_minimo - 1e-5);

    // Verificações geométricas do pino (L.4.1, L.2.6.1, L.4.4.1)
    double h_total_laje = laje.tc_mm + laje.hF_mm;
    double h_min_base = 4.0 * conector.d;
    double h_min_deck = (laje.tipo == LAJE_STEEL_DECK) ? (laje.hF_mm + 40.0) : 0.0;
    res.h_min_normativo_mm = std::max(h_min_base, h_min_deck);
    res.altura_atende = (conector.h >= res.h_min_normativo_mm - 1e-4);

    res.cobrimento_topo_mm = h_total_laje - conector.h;
    res.cobrimento_min_mm = 10.0;
    res.cobrimento_atende = (res.cobrimento_topo_mm >= res.cobrimento_min_mm - 1e-4);

    res.d_max_mesa_mm = 2.5 * perfil.tf;
    bool diam_deck_ok_2024 = (laje.tipo != LAJE_STEEL_DECK || conector.d <= 19.0 + 1e-4);
    bool diam_mesa_ok_2024 = (n_linha == 1 || conector.d <= res.d_max_mesa_mm + 1e-4);
    res.diametro_mesa_atende = diam_deck_ok_2024 && diam_mesa_ok_2024;

    // Verificações geométricas transversais (L.4.3.4 e L.2.6.3-c)
    if (conector.tipo == CONECTOR_PERFIL_U) {
        res.s_transv_min_mm = 0.0;
        res.dist_borda_min_mesa_mm = 25.0;
        res.bf_min_necessario_mm = conector.Lcs;
        res.nt_max_mesa = 1;
        res.largura_mesa_atende = (perfil.bf >= res.bf_min_necessario_mm - 1e-4);
        res.largura_deck_disp_mm = perfil.bf;
        res.largura_deck_nec_mm = conector.Lcs;
        res.nervura_deck_atende = (laje.tipo != LAJE_STEEL_DECK); // Perfil U não é permitido com steel deck
    } else {
        // Stud Bolt (Pino com cabeça)
        res.s_transv_min_mm = 4.0 * conector.d;
        res.dist_borda_min_mesa_mm = 25.0;
        res.bf_min_necessario_mm = (n_linha - 1) * res.s_transv_min_mm + 2.0 * res.dist_borda_min_mesa_mm;
        res.largura_mesa_atende = (perfil.bf >= res.bf_min_necessario_mm - 1e-4);

        double espaco_livre = perfil.bf - 2.0 * res.dist_borda_min_mesa_mm;
        res.nt_max_mesa = (espaco_livre >= 0.0 && res.s_transv_min_mm > 0.0)
            ? std::max(1, 1 + static_cast<int>(std::floor(espaco_livre / res.s_transv_min_mm)))
            : 1;

        if (laje.tipo == LAJE_STEEL_DECK && laje.nervura_paralela) {
            double hF = std::max(0.0, laje.hF_mm);
            double bF = (laje.bF_mm > 0.0) ? laje.bF_mm : 120.0;
            double nec = 50.0 + 4.0 * conector.d * (n_linha - 1);
            res.largura_deck_disp_mm = bF;
            res.largura_deck_nec_mm = nec;
            res.nervura_deck_atende = (bF >= nec - 1e-4);
        } else {
            res.largura_deck_disp_mm = perfil.bf;
            res.largura_deck_nec_mm = res.bf_min_necessario_mm;
            res.nervura_deck_atende = true;
        }
    }
    res.geometria_transversal_atende = res.largura_mesa_atende && res.nervura_deck_atende;

    return res;
}

ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    double gamma_a1,
    double gamma_c,
    double gamma_cs
) {
    ConectorCisalhamento conn_u = conector;
    conn_u.tipo = CONECTOR_PERFIL_U;
    return v2024::calcularConectoresCisalhamento(perfil, aco, laje, concreto, conn_u, Lv_mm, false, gamma_a1, gamma_c, gamma_cs);
}

ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c
) {
    ResultadoRotinaF res = v2008::calcular_rotina_f_armadura_costura(laje, concreto, Fhd_kN, Lv_mm, gamma_c);
    double eta_c = v2024::calcularEtaC(concreto.fck_MPa);
    res.nu_Rd_max_kN_m *= eta_c;
    res.Hv_Rd_max_kN_cm *= eta_c;
    res.atende_bielas = (res.nu_Sd_kN_m <= res.nu_Rd_max_kN_m);
    return res;
}

ResultadoFlechasVM calcularFlechasVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double Lv_mm,
    const AcoesPiso& acoes,
    double grau_interacao_eta,
    bool escorada
) {
    // Mesmo modelo de deslocamentos nas duas edições (Anexo B / L.1.2 x Anexo C / O.1.2)
    return v2008::calcularFlechasVM(perfil, aco, concreto, laje, Lv_mm, acoes, grau_interacao_eta, escorada);
}

} // namespace v2024

} // namespace nbr8800
