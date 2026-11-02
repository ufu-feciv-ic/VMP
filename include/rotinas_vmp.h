#pragma once
#include "modelos.h"

// ============================================================================
// ROTINAS ESPECIALIZADAS DE VIGAS MISTAS PROTENDIDAS (VMP)
// Baseadas em Turini (2021) e Demoner (2018).
// ============================================================================

// ----------------------------------------------------------------------------
// Rotina 1: Seção Transformada / Homogeneizada (ytr, Atr, Itr, Wtr)
// ----------------------------------------------------------------------------
struct ResultadoSecaoTransformada {
    double n_relacao_modular;
    double btr_mm;
    double ytr_mm;       // Distância da LNE à fibra inferior do perfil metálico
    double ay_mm;        // Altura comprimida da laje de concreto
    double Atr_mm2;      // Área homogeneizada
    double Itr_mm4;      // Momento de inércia homogeneizado
    double Wtr_inf_mm3;  // Módulo de resistência na fibra inferior do aço
    double Wtr_sup_mm3;  // Módulo de resistência na fibra superior da laje
    double ep_tr_mm;     // Excentricidade do cabo em relação à LNE transformada
};

ResultadoSecaoTransformada calcular_rotina_1_secao_transformada(
    const PerfilI& perfil,
    const Laje& laje,
    const Concreto& concreto,
    const Aco& aco,
    double multiplicador_fluencia = 1.0, // 1.0 para t=0 (curta); 3.0 para t=infinito (longa)
    double ep_perfil_inferior_mm = -50.0 // Posição do cabo em relação à face inferior
);

// ----------------------------------------------------------------------------
// Rotina 2: Estimativa da Força Máxima de Protensão Inicial (P0)
// ----------------------------------------------------------------------------
struct ResultadoEstimativaP0 {
    double P0_8800_kN; // Limite por flexo-compressão da viga de aço (NBR 8800)
    double P0_6118_kN; // Limite por tensões admissíveis em serviço (NBR 6118)
    double P0_max_kN;  // Menor entre os dois limites normativos
};

ResultadoEstimativaP0 calcular_rotina_2_estimativa_p0(
    const PerfilI& perfil,
    const Aco& aco,
    double NRd_kN,
    double MRd_kNm,
    double ep_I_mm,
    double M_peso_proprio_kNm,
    double P0_6118_estimado_kN = 1431.63
);

// ----------------------------------------------------------------------------
// Rotina 4: Armadura e Quantidade de Cabos (Ap, ncabos)
// ----------------------------------------------------------------------------
struct ResultadoRotina4 {
    int n_cabos;
    double Ap_calculada_mm2;
    double Ap_adotada_mm2;
};

ResultadoRotina4 calcular_rotina_4_armadura_protensao(
    double P0_kN,
    double fpk_MPa = 1900.0,
    double area_unitaria_cordoalha_mm2 = 140.0 // Cordoalha 15.2 mm
);

// ----------------------------------------------------------------------------
// Rotina 5: Perdas de Protensão Imediatas
// ----------------------------------------------------------------------------
struct ResultadoPerdas {
    double delta_P_ancoragem_kN;
    double P_efetiva_kN;
};

ResultadoPerdas calcular_rotina_5_perdas_imediatas(
    double P0_kN,
    double delta_cunha_mm,
    double Ep_MPa,
    double Ap_mm2,
    double Lv_mm
);

// ----------------------------------------------------------------------------
// Rotina 6: Incremento Hiperestático da Força de Protensão (ΔP)
// ----------------------------------------------------------------------------
struct ResultadoIncrementoDP {
    double delta_P_kN;
};

ResultadoIncrementoDP calcular_rotina_6_incremento_dp(
    double q_kN_m,
    double Lv_mm,
    double ep_mm,
    double I_mm4,
    double A_secao_mm2,
    double Ap_mm2
);

// ----------------------------------------------------------------------------
// Rotina H: Tensão de Cálculo no Cabo em ELU (sigma_pd)
// ----------------------------------------------------------------------------
struct ResultadoRotinaH {
    double epsilon_pre;
    double delta_sigma_MPa;
    double sigma_pd_MPa;
};

ResultadoRotinaH calcular_rotina_h_tensao_cabo(
    double P_atuante_kN,
    double Ap_mm2,
    double Ep_MPa,
    double fpyk_MPa,
    double Atr_mm2,
    double Lv_mm,
    double d_total_mm
);

// ----------------------------------------------------------------------------
// Rotina 3: Momento Fletor Resistente de VMP (MRd,VMP+)
// ----------------------------------------------------------------------------
struct ResultadoMomentoVMP {
    double a_linha_neutra_mm; // Altura do bloco comprimido de concreto
    double Cc_max_kN;         // Compressão da laje
    double T_aco_kN;          // Tração do perfil de aço
    double T_cabo_kN;         // Tração do cabo de protensão
    double MRd_VMP_kNm;       // Momento resistente final em kN.m
};

ResultadoMomentoVMP calcular_rotina_3_momento_vmp(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    double Ap_mm2,
    double sigma_pd_MPa,
    double ep_base_perfil_mm = -50.0,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40
);
