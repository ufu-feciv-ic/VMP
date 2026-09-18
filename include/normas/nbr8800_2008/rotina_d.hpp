#pragma once

#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"

namespace vmp::normas::nbr8800_2008
{
    struct ResultadoRotinaD
    {
        double lambda_alma; // h / tw
        double lambda_p_alma;
        double lambda_r_alma;

        double kv;

        double Aw; // d * tw para seções I
        
        // Força cortante correspondende à plastificação da alma por cisalhamento
        double Vpl; // 0,6 * Aw * fy

        // Força cortante resistente de cálculo
        double VRd_N;
        double VRd_kN;
    };

    class RotinaD
    {
        public:

        static ResultadoRotinaD calcular
        (
            const nucleo::SecaoI& secao,
            const nucleo::Aco& material,
            double gamma_a1 = 1.1,
            double dist_a = 0.0// distancia dos enrijecedores 
        );

        static double calcular_kv(double dist_a, double h, double tw);
        static double calcular_VRd(double Vpl, double lambda, double lambda_p, double lambda_r, double gamma_a1);
    };
}