#include "normas/nbr8800_2008/rotina_d.hpp"
#include <cmath>

namespace vmp::normas::nbr8800_2008
{
    double RotinaD::calcular_kv(double dist_a, double h, double tw)
    {
        if(dist_a == 0 || (dist_a / h) > 3 || (dist_a / h) > (pow((260 / (h / tw)), 2))) return 5;
        else return 5 + 5 / (pow((dist_a/h),2));
    }

    double RotinaD::calcular_VRd(double Vpl, double lambda, double lambda_p, double lambda_r, double gamma_a1)
    {
        if (lambda <= lambda_p) return Vpl/gamma_a1;
        else if (lambda > lambda_p && lambda <= lambda_r) return ((lambda_p/lambda) / (Vpl/gamma_a1));
        else if (lambda > lambda_r) return 1.24 * (pow((lambda_p/lambda), 2) / (Vpl/gamma_a1));
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
        double d1 = secao.d1;
        double tw = secao.tw;

        res.kv = calcular_kv(dist_a, d1, tw);

        res.Aw = d * tw;
        res.Vpl = 0.6 * res.Aw * fy;

        res.lambda_alma = d / tw;
        res.lambda_p_alma = 1.1 * sqrt((res.kv * E) / fy);
        res.lambda_r_alma = 1.37 * sqrt((res.kv * E) / fy);

        res.VRd_N = calcular_VRd(res.Vpl, res.lambda_alma, res.lambda_p_alma, res.lambda_r_alma, gamma_a1);
    }
}