#include "nucleo/material.hpp"

namespace vmp::nucleo {

    Aco::Aco(std::string nome, double fy, double fu, double E)
        : nome(nome), fy(fy), fu(fu), E(E) {}

    Aco Aco::a36() {
        return Aco("ASTM A36", 250.0, 400.0, 200000.0);
    }

    Aco Aco::a572_gr50() {
        return Aco("ASTM A572 Grau 50", 345.0, 450.0, 200000.0);
    }

} // namespace vmp::nucleo
