#include "normas/nbr8800_2008/rotina_a.hpp"
#include <cmath>
#include <algorithm>

namespace vmp::normas::nbr8800_2008 {

    const double PI = 3.14159265358979323846;

    double RotinaA::calcular_euler_nex(double E, double Ix, double Kx, double Lv) {
        double kl = Kx * Lv;
        return (PI * PI * E * Ix) / (kl * kl);
    }

    double RotinaA::calcular_euler_ney(double E, double Iy, double Ky, double Lb) {
        double kl = Ky * Lb;
        return (PI * PI * E * Iy) / (kl * kl);
    }

    double RotinaA::calcular_qs_laminado(double bf, double tf, double E, double fy) {
        double lambda = bf / (2.0 * tf);
        double lambda_p = 0.56 * std::sqrt(E / fy);
        double lambda_r = 1.03 * std::sqrt(E / fy);

        if (lambda <= lambda_p) {
            return 1.0;
        }
        if (lambda <= lambda_r) {
            return 1.415 - 0.74 * lambda * std::sqrt(fy / E);
        }
        return (0.69 * E) / (fy * lambda * lambda);
    }

    double RotinaA::calcular_kc(double d1, double tw) {
        double kc = 4.0 / std::sqrt(d1 / tw);
        if (kc < 0.35) kc = 0.35;
        if (kc > 0.76) kc = 0.76;
        return kc;
    }

    double RotinaA::calcular_qs_soldado(double bf, double tf, double d1, double tw, double E, double fy) {
        double kc = calcular_kc(d1, tw);
        double lambda = bf / (2.0 * tf);
        double lambda_p = 0.64 * std::sqrt(E / (fy / kc));
        double lambda_r = 1.17 * std::sqrt(E / (fy / kc));

        if (lambda <= lambda_p) {
            return 1.0;
        }
        if (lambda <= lambda_r) {
            return 1.415 - 0.65 * lambda * std::sqrt(fy / (E * kc));
        }
        return (0.90 * E * kc) / (fy * lambda * lambda);
    }

    double RotinaA::calcular_qa(double d1, double tw, double Ag, double E, double fy, double Ne, double& out_bef, double& out_sigma) {
        double lambda = d1 / tw;
        double lambda_p = 1.49 * std::sqrt(E / fy);

        if (lambda <= lambda_p) {
            out_bef = d1;
            out_sigma = 0.0;
            return 1.0;
        }

        double lambda_0_1 = std::sqrt((Ag * fy) / Ne);
        double chi_0_1 = calcular_chi(lambda_0_1);
        out_sigma = chi_0_1 * fy;

        double raiz_e_sigma = std::sqrt(E / out_sigma);
        out_bef = 1.92 * tw * raiz_e_sigma * (1.0 - (0.34 / lambda) * raiz_e_sigma);

        if (out_bef < 0.0) out_bef = 0.0;
        if (out_bef > d1) out_bef = d1;

        return out_bef / d1;
    }

    double RotinaA::calcular_chi(double lambda_0) {
        if (lambda_0 <= 1.5) {
            return std::pow(0.658, lambda_0 * lambda_0);
        }
        return 0.877 / (lambda_0 * lambda_0);
    }

    ResultadoRotinaA RotinaA::calcular(
        const nucleo::SecaoI& secao,
        const nucleo::Aco& material,
        double Lv,
        double Lb,
        double Kx,
        double Ky,
        double gamma_a1
    ) {
        ResultadoRotinaA res;

        double E = material.E;
        double fy = material.fy;
        double Ag = secao.Ag;
        double bf = secao.bf;
        double tf = secao.tf;
        double tw = secao.tw;
        double d1 = secao.d1;

        // 1. Flambagem elastica global de Euler
        res.Nex = calcular_euler_nex(E, secao.Ix, Kx, Lv);
        res.Ney = calcular_euler_ney(E, secao.Iy, Ky, Lb);
        res.Ne = std::min(res.Nex, res.Ney);

        // 2. Flambagem local da mesa (Qs)
        res.lambda_mesa = secao.esbeltez_mesa();
        if (secao.eh_laminado()) {
            res.lambda_p_mesa = 0.56 * std::sqrt(E / fy);
            res.lambda_r_mesa = 1.03 * std::sqrt(E / fy);
            res.Qs = calcular_qs_laminado(bf, tf, E, fy);
        } else {
            double kc = calcular_kc(d1, tw);
            res.lambda_p_mesa = 0.64 * std::sqrt(E / (fy / kc));
            res.lambda_r_mesa = 1.17 * std::sqrt(E / (fy / kc));
            res.Qs = calcular_qs_soldado(bf, tf, d1, tw, E, fy);
        }

        // 3. Flambagem local da alma (Qa)
        res.lambda_alma = secao.esbeltez_alma();
        res.lambda_p_alma = 1.49 * std::sqrt(E / fy);
        res.Qa = calcular_qa(d1, tw, Ag, E, fy, res.Ne, res.befa, res.sigma);

        // 4. Fator total de esbeltez local
        res.Q = res.Qs * res.Qa;

        // 5. Indice de esbeltez global e fator de reducao chi
        res.lambda_0 = std::sqrt((res.Q * Ag * fy) / res.Ne);
        res.chi = calcular_chi(res.lambda_0);

        // 6. Resistencia de calculo a compressao
        res.NRd_N = (res.chi * res.Q * Ag * fy) / gamma_a1;
        res.NRd_kN = res.NRd_N / 1000.0;

        return res;
    }

} // namespace vmp::normas::nbr8800_2008
