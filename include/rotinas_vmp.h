#pragma once
#include "modelos.h"

// ============================================================================
// ROTINAS ESPECIALIZADAS DE VIGAS MISTAS PROTENDIDAS (VMP)
// Baseadas em Turini (2021) e Demoner (2018).
// ============================================================================

namespace nbr8800 {
inline namespace v2008 {


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

ResultadoSecaoTransformada calcularSecaoTransformada(
    const PerfilI& perfil,
    const Laje& laje,
    const Concreto& concreto,
    const Aco& aco,
    double multiplicador_fluencia = 1.0, // 1.0 para t=0 (curta); 3.0 para t=infinito (longa)
    double ep_perfil_inferior_mm = 0.0, // Posição do cabo em relação à face inferior
    bool tempo_imediato = true
);

// ----------------------------------------------------------------------------
// Rotina 2: Estimativa da Força Máxima de Protensão Inicial (P0)
// ----------------------------------------------------------------------------
struct ResultadoEstimativaP0 {
    double P0_8800_kN; // Limite por flexo-compressão da viga de aço (NBR 8800)
    double P0_6118_kN; // Limite por tensões admissíveis em serviço (NBR 6118)
    double P0_max_1;
    double P0_max_2;
    double P0_max_kN;  // Menor entre os dois limites normativos
};

ResultadoEstimativaP0 calcularEstimativaP0max(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const CaboProtensao& cabo,
    double NRd_kN,
    double MRd_kNm,
    double MSd_g_kNm,
    double MSd_q_kNm,
    double gamma_P,
    double beta,
    double psi_1,
    double ytr_mm,
    double Atr_mm,
    double Wtr_inf_mm,
    double Wtr_sup_mm
);

// ----------------------------------------------------------------------------
// Rotina 4: Armadura e Quantidade de Cabos (Ap, ncabos)
// ----------------------------------------------------------------------------
struct ResultadoArmaduraProtensao {
    double sigma_adm_MPa;
    double Ap_calc;
    double n_cordoalhas;
    double Ap_real;
};

ResultadoArmaduraProtensao calcularArmaduraProtensao(
    const CaboProtensao& cabo,
    TipoProtensao& protensao,
    double P0_kN
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

ResultadoIncrementoDP calcularIncrementoDpTraçadoReto(
    double q_kN_m,
    double Eg_MPa,
    double Ep_MPa,
    double Lv_mm,
    double ep_mm,
    double Ix_mm4,
    double Ag_mm2,
    double Ap_mm2
);

ResultadoIncrementoDP calcularIncrementoDpTraçadoPoligonal(
    double q_kN_m,
    double Eg_MPa,
    double Ep_MPa,
    double Lv_mm,
    double ep_mm,
    double Ix_mm4,
    double Ag_mm2,
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
struct ResultadoMomentoPositivoVMP {
    bool aplicavel = true;
    double lbw; // Esbeltez da alma
    double lbw_p; // Esbeltez da alma no limite plastico
    double lbw_r; // Esbeltez da alma no limite elástico
    double a_linha_neutra_mm; // Altura do bloco comprimido de concreto
    double y_lnp_mm;
    double Cc_max_kN;         // Compressão da laje
    double C_aco_kN;          // Compressão do perfil de aço
    double T_aco_kN;          // Tração do perfil de aço
    double T_cabo_kN;         // Tração do cabo de protensão
    double MRd_VMP_kNm;       // Momento resistente final em kN.m
};

ResultadoMomentoPositivoVMP calcularMomentoPostivoVMP(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const CaboProtensao& cabo, 
    double Ap_mm2,
    double sigma_pd_MPa,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40
);

struct ResultadoMomentoNegativoVMP {
    bool aplicavel = true;
    double MRd_neg_kNm;
    double Mrd_dist_kNm;
    double MRd_VMP_kNm;
};

ResultadoMomentoNegativoVMP calcularMomentoNegativoVMP(
    const PerfilI& perfi,
    const Aco& aco,
    const Concreto& concreto, 
    const Laje& laje,
    const CaboProtensao& cabo
);

}
}