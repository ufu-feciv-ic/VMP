#include "normas/nbr8800_2008/rotina_d.hpp"
#include <cmath>

namespace vmp::normas::nbr8800_2008
{
    double RotinaD::calcular_kv(double dist_a, double h, double tw)
    {
        if (dist_a <= 0.0 || (dist_a / h) > 3.0 || (dist_a / h) > std::pow(260.0 / (h / tw), 2.0)) {
            return 5.0;
        }
        return 5.0 + 5.0 / std::pow(dist_a / h, 2.0);
    }

    double RotinaD::calcular_VRd(double Vpl, double lambda, double lambda_p, double lambda_r, double gamma_a1)
    {
        double VRd_plastico = Vpl / gamma_a1;

        if (lambda <= lambda_p) {
            return VRd_plastico;
        } else if (lambda <= lambda_r) {
            return (lambda_p / lambda) * VRd_plastico;
        } else {
            return 1.24 * std::pow(lambda_p / lambda, 2.0) * VRd_plastico;
        }
    }

    ResultadoRotinaD RotinaD::calcular(
        const nucleo::SecaoI& secao,
        const nucleo::Aco& material,
        double gamma_a1,
        double dist_a
    ) {
        ResultadoRotinaD res;

        double E = material.E;
        double fy = material.fy;
        double d = secao.d;
        double h = secao.d1; // Altura livre da alma (NBR 8800 item 5.4.3.1.1)
        double tw = secao.tw;

        res.kv = calcular_kv(dist_a, h, tw);

        res.Aw = d * tw;
        res.Vpl = 0.6 * res.Aw * fy;

        res.lambda_alma = h / tw;
        res.lambda_p_alma = 1.10 * std::sqrt((res.kv * E) / fy);
        res.lambda_r_alma = 1.37 * std::sqrt((res.kv * E) / fy);

        res.VRd_N = calcular_VRd(res.Vpl, res.lambda_alma, res.lambda_p_alma, res.lambda_r_alma, gamma_a1);
        res.VRd_kN = res.VRd_N / 1000.0;

        return res;
    }
}