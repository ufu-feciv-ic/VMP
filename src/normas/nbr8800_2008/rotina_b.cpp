#include "normas/nbr8800_2008/rotina_b.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace vmp::normas::nbr8800_2008
{
    ResultadoFLT RotinaB::calcular_FLT(
        const nucleo::SecaoI& secao,
        const nucleo::Aco& material,
        double Lb,
        double Cb,
        double gamma_a1
    )
    {
        ResultadoFLT res;
        
        res.lambda = Lb / secao.ry;
        res.lambda_p = 1.76 * std::sqrt(material.E / material.fy);

        res.Mpl_Nmm = std::min(secao.Zx * material.fy, 1.5 * secao.Wx * material.fy);

        res.Mr_Nmm = 0.7 * material.fy * secao.Wx;

        if (res.lambda <= res.lambda_p)
        {
            res.regime = RegimeEsbeltez::Plastico;
            res.MRd_Nmm = res.Mpl_Nmm / gamma_a1;
            res.MRd_kNm = res.MRd_Nmm / 1e6;
        }

        return res;
    }
}