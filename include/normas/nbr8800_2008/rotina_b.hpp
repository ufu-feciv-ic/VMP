#pragma once

#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"

namespace vmp::normas::nbr8800_2008 {

    enum class RegimeEsbeltez 
    {
        Plastico, // lambda <= lambda_p 
        Inelastico, // lampda_p < lambda <= lambda_r
        Elastico // lambda > lambda_r
    };

    struct ResultadoFLT 
    {
        double lambda;
        double lambda_p;
        double lambda_r;
        double Mpl_Nmm;
        double Mr_Nmm;
        double Mcr_Nmm;
        double MRd_Nmm;
        double MRd_kNm;
        RegimeEsbeltez regime;
    };

    class RotinaB 
    {
        public:
            static ResultadoFLT calcular_FLT(
                const nucleo::SecaoI& secao,
                const nucleo::Aco& material,
                double Lb,
                double Cb = 1.0,
                double gamma_a1 = 1.10
            );
    };

}