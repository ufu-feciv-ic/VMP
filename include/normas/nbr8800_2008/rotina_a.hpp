#pragma once

#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"

namespace vmp::normas::nbr8800_2008 {

    struct ResultadoRotinaA {
        // Flambagem global elastica (Euler)
        double Nex;             // N
        double Ney;             // N
        double Ne;              // N

        // Flambagem local da mesa (Elemento AL)
        double lambda_mesa;     // bf / (2*tf)
        double lambda_p_mesa;
        double lambda_r_mesa;
        double Qs;

        // Flambagem local da alma (Elemento AA)
        double lambda_alma;     // d1 / tw
        double lambda_p_alma;
        double befa;            // mm
        double sigma;           // MPa
        double Qa;

        // Fator total de reducao por esbeltez local
        double Q;

        // Indice de esbeltez global e fator de reducao
        double lambda_0;
        double chi;

        // Resistencia de calculo a compressao axial
        double NRd_N;           // N
        double NRd_kN;          // kN
    };

    class RotinaA {
    public:
        // Executa o calculo completo conforme a Rotina A da NBR 8800:2008
        static ResultadoRotinaA calcular(
            const nucleo::SecaoI& secao,
            const nucleo::Aco& material,
            double Lv,
            double Lb,
            double Kx = 1.0,
            double Ky = 1.0,
            double gamma_a1 = 1.10
        );

        // Sub-rotinas isoladas
        static double calcular_euler_nex(double E, double Ix, double Kx, double Lv);
        static double calcular_euler_ney(double E, double Iy, double Ky, double Lb);
        static double calcular_qs_laminado(double bf, double tf, double E, double fy);
        static double calcular_qs_soldado(double bf, double tf, double d1, double tw, double E, double fy);
        static double calcular_kc(double d1, double tw);
        static double calcular_qa(double d1, double tw, double Ag, double E, double fy, double Ne, double& out_bef, double& out_sigma);
        static double calcular_chi(double lambda_0);
    };

} // namespace vmp::normas::nbr8800_2008
