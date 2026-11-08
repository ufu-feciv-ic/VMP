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
    p.It = 34.45e4;       // 34.45 cm⁴
    p.lbf = 6.56;         // esbeltez mesa
    p.lbw = 38.98;        // esbeltez alma
    p.Cw = 330395.1e6;     // 333,394 cm⁶
    return p;
}

inline Aco aco_a572_gr50() {
    Aco a;
    a.nome = "ASTM A572 Grau 50";
    a.fy_MPa = 345.0;     // MPa
    a.fu_MPa = 450.0;     // MPa
    a.E_MPa = 200000.0;   // MPa
    return a;
}

inline Concreto concreto_c30() {
    Concreto c;
    c.classe = "C30";
    c.fck_MPa = 30.0;     // MPa
    c.Ecs_MPa = 26838.0;  // MPa
    c.fctm_MPa = 2.9;     // MPa
    return c;
}

inline Laje laje_macica_130() {
    Laje l;
    l.tipo = TipoLaje::LAJE_MACICA;
    l.tc_mm = 130;
    l.bef_mm = 3000;
    l.hF_mm = 0.0;
    l.nervura_paralela = false;
    return l;
}

inline CaboProtensao cabo_190RB() {
    CaboProtensao c;
    c.tipo = TipoProtensao::PRE_TRACAO;
    c.tipo_aco = TipoTracado::CABO_RETO;
    c.tipo_aco = "CP-190RB";
    c.fpk = 1900; // MPa
    c.fpyk = 0.85*c.fpk; // MPa
    c.Ep = 200000; // MPa
    c.diametro = 0.0;
    c.n_cordoalhas = 3;
    c.Ap = 0.0;
    c.ep = -50; //mm
    c.delta_cunha = 6; //mm
    return c;
}
