#pragma once
#include "modelos.h"

inline PerfilI perfil_w360x57_8() {
    PerfilI p;
    p.nome = "W 360 x 57,8";
    p.tipo = LAMINADO;
    p.d = 358.0;
    p.bf = 172.0;
    p.tw = 7.9;
    p.tf = 13.1;
    p.r = 0.0;
    p.h = 332;
    p.d1 = 308;
    p.Ag = 7250;           // 72.5 cm²
    p.Ix = 16143e4;        // 16143 cm⁴ 
    p.Wx = 901.8e3;        // 901,8 cm³
    p.rx = 142.0;          // 14.2 cm
    p.Zx = 1014.8e3;      // 1014,8 cm³
    p.Iy = 1113e4;         // 1113 cm⁴ 
    p.Wy = 129.4e3;       // 129,4 cm³
    p.ry = 39.2;           // 3.92 cm    
    p.Zy = 199.8e3;       // 199,8 cm³
    p.rt = 45.3;          // 4,53 cm  
    p.It = 34.45e3;       // 34.45 cm⁴
    p.lbf = 6.56;         // esbeltez mesa
    p.lbw = 38.98;        // esbeltez alma
    p.Cw = 330.394e6;     // 333,394 cm⁶
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
