#pragma once
#include "modelos.hpp"

inline PerfilI perfil_w360x57_8() {
    PerfilI p;
    p.nome = "W 360 x 57,8";
    p.tipo = LAMINADO;
    p.d = 358.0;
    p.bf = 172.0;
    p.tw = 7.9;
    p.tf = 13.1;
    p.r = 14.0;
    p.d1 = 303.8;
    p.Ag = 7340.0;         // 73.4 cm²
    p.Ix = 160400000.0;     // 16040 cm⁴ (ou 16143 cm⁴ oficial)
    p.Iy = 11100000.0;      // 1110 cm⁴ (ou 1113 cm⁴ oficial)
    p.Wx = 896000.0;        // 896 cm³
    p.Wy = 129000.0;        // 129 cm³
    p.Zx = 1010500.0;       // 1010.5 cm³
    p.Zy = 203200.0;        // 203.2 cm³
    p.rx = 148.0;           // 14.8 cm
    p.ry = 39.1;            // 3.91 cm
    p.It = 346000.0;        // 34.6 cm⁴
    p.Cw = 333333000000.0;  // 333333 cm⁶
    return p;
}

inline Aco aco_a572_gr50() {
    Aco a;
    a.nome = "ASTM A572 Grau 50";
    a.fy = 345.0;     // MPa
    a.fu = 450.0;     // MPa
    a.E = 200000.0;   // MPa
    return a;
}

inline Concreto concreto_c30() {
    Concreto c;
    c.classe = "C30";
    c.fck = 30.0;     // MPa
    c.Ecs = 26838.0;  // MPa
    c.fctm = 2.9;     // MPa
    return c;
}
