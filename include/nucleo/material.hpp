#pragma once

#include <string>

namespace vmp::nucleo {

    class Aco {
    public:
        std::string nome;
        double fy;  // Tensao de escoamento em MPa (N/mm²)
        double fu;  // Tensao de ruptura em MPa (N/mm²)
        double E;   // Modulo de elasticidade em MPa (N/mm²)

        Aco(std::string nome, double fy, double fu, double E = 200000.0);

        // Acos estruturais usuais
        static Aco a36();
        static Aco a572_gr50();
    };

} // namespace vmp::nucleo
