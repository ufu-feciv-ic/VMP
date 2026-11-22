#pragma once
#include "modelos.h"

inline PerfilI perfil_w360x51() {
    PerfilI p;
    p.nome = "W 360 x 51,0";
    p.tipo = LAMINADO;
    p.d = 355.0;
    p.bf = 171.0;
    p.tw = 7.2;
    p.tf = 11.6;
    p.r = 0.0;
    p.h = 331.8;
    p.d1 = 308.0;
    p.Ag = 6356.0;          // 63.56 cm²
    p.Ix = 14222e4;         // 14222 cm⁴
    p.Wx = 801.2e3;         // 801.2 cm³
    p.rx = 149.6;           // 14.96 cm
    p.Zx = 897.4e3;         // 897.4 cm³
    p.Iy = 967e4;           // 967 cm⁴
    p.Wy = 113.1e3;         // 113.1 cm³
    p.ry = 39.0;            // 3.90 cm
    p.Zy = 174.5e3;         // 174.5 cm³
    p.rt = 44.5;            // cm
    p.It = 22.1e4;          // 22.1 cm⁴
    p.lbf = 7.37;           // esbeltez mesa
    p.lbw = 46.08;          // esbeltez alma
    p.Cw = 2.85e11;         // 285.000 cm⁶
    return p;
}

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
    a.G_MPa = 7700.0;     // MPa
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

inline Laje laje_mista_130() {
    Laje l;
    l.tipo = TipoLaje::LAJE_STEEL_DECK;
    l.tc_mm = 130;
    l.bef_mm = 3000;
    l.eF_mm = 305;
    l.bF_mm = 130;
    l.hF_mm = 50;
    l.nervura_paralela = true;
    return l;
}

inline ArmacaoLaje laje_5cm_m()
{
    ArmacaoLaje a;
    a.As = 5e2;
    a.Ads = (5e2) * 3000/1000;
    a.fsd = 500 / 1.15;
    a.d_linha_mm = 20;
    return a;
}

inline CaboProtensao cabo_190RB() {
    CaboProtensao c;
    c.tipo = TipoProtensao::PRE_TRACAO;
    c.tracado = TipoTracado::CABO_RETO;
    c.ancoragem = TipoAncoragem::ATIVA_PASSIVA;
    c.elemento = TipoElementoProtensao::CORDOALHA;
    c.relaxacao = TipoRelaxacao::RELAXACAO_BAIXA;
    c.cabo = TipoCabo::CP_190_RB;
    c.cordoalha = TipoCordoalha::CORDOALHA_15_2;
    c.fpk = 1864.0; // MPa
    c.fpyk = 0.90*c.fpk; // MPa
    c.Ep = 200000; // MPa
    c.diametro = 15.2; // mm
    c.area_unit = 140; // mm²
    c.n_cordoalhas = 0.0;
    c.Ap = 0.0;
    c.ep = -50; //mm
    c.delta_cunha = 6; //mm
    return c;
}

// ============================================================================
// DADOS DO EXEMPLO 2 (Thiago Tononi Turini, 2021 - Apêndice C)
// ============================================================================

inline PerfilI perfil_w460x89() {
    PerfilI p;
    p.nome = "W 460 x 89";
    p.tipo = LAMINADO;
    p.d = 463.0;
    p.bf = 192.0;
    p.tw = 10.5;
    p.tf = 17.7;
    p.r = 0.0;
    p.h = 428.0;
    p.d1 = 404.0;
    p.Ag = 11410.0;          // 114.1 cm²
    p.Ix = 41105e4;          // 41105 cm⁴
    p.Wx = 1775.6e3;         // 1775.6 cm³
    p.rx = 189.8;            // 18.98 cm
    p.Zx = 2019.4e3;         // 2019.4 cm³
    p.Iy = 2093e4;           // 2093 cm⁴
    p.Wy = 218.0e3;          // 218.0 cm³
    p.ry = 42.8;             // 4.28 cm
    p.Zy = 339.0e3;          // 339.0 cm³
    p.rt = 50.1;             // 5.01 cm
    p.It = 92.49e4;          // 92.49 cm⁴
    p.lbf = 5.42;            // esbeltez da mesa: 192 / (2 * 17.7) = 5.42
    p.lbw = 44.10;           // esbeltez da alma: 463 / 10.5 = 44.10
    p.Cw = 1035073e6;        // 1035073 cm⁶
    return p;
}

inline Laje laje_mista_exemplo2() {
    Laje l;
    l.tipo = TipoLaje::LAJE_STEEL_DECK;
    l.tc_mm = 130.0;
    l.bef_mm = 2000.0;
    l.eF_mm = 305.0;
    l.bF_mm = 130.0;
    l.hF_mm = 50.0;
    l.nervura_paralela = false; // Nervuras perpendiculares ao perfil I
    return l;
}

inline ArmacaoLaje laje_exemplo2_armacao() {
    ArmacaoLaje a;
    a.As = 5e2;                      // 5 cm²/m = 500 mm²/m
    a.Ads = (5e2) * 2000.0 / 1000.0; // 10 cm² = 1000 mm²
    a.fsd = 500.0 / 1.15;            // Aço CA-50
    a.d_linha_mm = 20.0;             // 20 mm
    return a;
}

inline CaboProtensao cabo_exemplo2() {
    CaboProtensao c;
    c.tipo = TipoProtensao::POS_TRACAO;
    c.tracado = TipoTracado::CABO_POLIGONAL;
    c.ancoragem = TipoAncoragem::ATIVA_PASSIVA;
    c.elemento = TipoElementoProtensao::CORDOALHA;
    c.relaxacao = TipoRelaxacao::RELAXACAO_BAIXA;
    c.cabo = TipoCabo::CP_190_RB;
    c.cordoalha = TipoCordoalha::CORDOALHA_15_2;
    c.fpk = 1864.0;          // MPa
    c.fpyk = 0.90 * c.fpk;   // 1677.6 MPa
    c.Ep = 200000.0;         // MPa
    c.diametro = 15.2;       // mm
    c.area_unit = 140.0;     // mm²
    c.n_cordoalhas = 6.0;    // 2 cabos de 3 cordoalhas = 6
    c.Ap = 840.0;            // 8.4 cm² = 840 mm²
    c.ep = -50.0;            // mm
    c.delta_cunha = 6.0;     // mm
    return c;
}
