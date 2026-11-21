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
// Rotina 5: Perdas de Protensão Imediatas e Progressivas
// ----------------------------------------------------------------------------
struct ForcasProtensaoTrechos {
    double P_ativa_kN = 0.0;   // Trecho [0, X_ep] (P_n,1)
    double P_meio_kN = 0.0;    // Trecho central [X_ep, L - X_ep] (P_n,2)
    double P_passiva_kN = 0.0; // Trecho [L - X_ep, L] (P_n,3)
};

struct ResultadoPerdaImediataTracadoReto
{
    double delta_P_ancoragem_kN; // Perda da força por encunhamento
    double P1_kN; // Força resultante pós perdas imediatas
};

ResultadoPerdaImediataTracadoReto calcularPerdaAncoragemTracadoReto(
    const CaboProtensao& cabo,
    double P0_kN,
    double Lv_mm
);

// [Em Stand-by] Rotina 5.1: Perda imediata (atrito + ancoragem) para traçado poligonal
ForcasProtensaoTrechos calcularPerdaImediataTracadoPoligonal(
    const CaboProtensao& cabo,
    double P0_kN,
    double Lv_mm,
    double ep_i_mm,
    double ep_f_mm
);

double calcularPsi1000 (double sigma_P0_MPa, double fptk_MPa, 
    TipoElementoProtensao elemento, TipoRelaxacao relaxacao);

struct ResultadoPerdaRelaxacaoTracadoReto {
    double psi_1000; // Coeficiente de relaxação a 1000h (em valor decimal)
    double delta_P_relaxacao_kN; // Perda da força diferida no tempo
    double P3_kN; // Força final de protensão
    double sigma_P0_MPa;
};

ResultadoPerdaRelaxacaoTracadoReto calcularPerdaRelaxacaoTracadoReto (
    const CaboProtensao& cabo, 
    double P_atuante_kN
);

// ----------------------------------------------------------------------------
// Rotina 6: Incremento Hiperestático da Força de Protensão (ΔP)
// ----------------------------------------------------------------------------
double calcularIncrementoDpTracadoReto(
    double q_kN_m,
    double Eg_MPa,
    double Ep_MPa,
    double Lv_mm,
    double ep_mm,
    double Ix_mm4,
    double Ag_mm2,
    double Ap_mm2
);

// [Em Stand-by] Rotina 6.2: Incremento hiperestático ΔP para traçado poligonal
double calcularIncrementoDpTracadoPoligonal(
    double q_kN_m,
    double Eg_MPa,
    double Ep_MPa,
    double Lv_mm,
    double X_ep_mm,
    double ep_i_mm,
    double ep_f_mm,
    double Ix_mm4,
    double Ag_mm2,
    double Ap_mm2
);

// ----------------------------------------------------------------------------
// Rotina H: Tensão de Cálculo no Cabo em ELU (sigma_pd)
// ----------------------------------------------------------------------------
struct ResultadoTensaoCabo {
    double epsilon_pre;
    double delta_sigma_MPa;
    double sigma_pd_MPa;
    double ro_p;
};

ResultadoTensaoCabo calcularTensaoCaboELU(
    const PerfilI& perfil,
    const Laje& laje, 
    const CaboProtensao& cabo, 
    const Concreto& concreto,
    double P_atuante_kN,
    double ytr_mm, 
    double Atr_mm2,
    double Itr_mm4,
    double Lv_mm
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
    const CaboProtensao& cabo,
    const ArmacaoLaje& armacaoLaje,
    double MSd_neg_kNm,
    double MSd_pos_kNm,
    double Lv_mm, // comprimento vao
    double a_vigas_mm, // espaçamento entre as vigas
    double alpha_viga = 3.0,
    double gamma_a1 = 1.10, 
    double gamma_c = 1.40
);

double calcularCdist(double psi);

double calcularInerciaLaje(
    const Laje& laje,
    const ArmacaoLaje& armacaoLaje,
    double alpha_e
);

// ----------------------------------------------------------------------------
// Fase 4: ELS Deformação Excessiva (Flechas) - Turini (2021)
// ----------------------------------------------------------------------------
struct ResultadoELSDeformacao {
    // Verificação de regime elástico
    double M_Ga_Sd_kNm = 0.0;
    double M_L_Sd_kNm = 0.0;
    double sigma_inf_MPa = 0.0;
    double fy_MPa = 0.0;
    bool regime_elastico = true;

    // Parcelas de flecha no meio do vão (cm)
    double delta_PP_cm = 0.0;
    double delta_conc_cm = 0.0;
    double delta_CPN_cm = 0.0;
    double delta_CVN_curta_cm = 0.0;
    double delta_CVN_longa_cm = 0.0;
    double delta_protensao_cm = 0.0;
    double delta_contraflecha_cm = 0.0;
    double delta_total_cm = 0.0;
    double delta_lim_cm = 0.0;

    bool aprovado = true;
};

ResultadoELSDeformacao calcularELSDeformacao(
    const VigaVMP& viga,
    const PerfilI& perfil,
    const ResultadoSecaoTransformada& secao_t0,
    const ResultadoSecaoTransformada& secao_t_inf,
    double P1_kN,
    double P3_kN
);

// ----------------------------------------------------------------------------
// Fase 5: ELS Vibração no Piso - Turini (2021) / NBR 8800 Anexo L
// ----------------------------------------------------------------------------
struct ResultadoELSVibracao {
    // Propriedades da seção transformada para vibração
    double bef_vib_mm = 0.0;
    double Ecs_vib_MPa = 0.0;
    ResultadoSecaoTransformada secao_vib{};

    // Parcelas de flecha na combinação frequente dinâmica (cm)
    double delta_PP_cm = 0.0;
    double delta_conc_cm = 0.0;
    double delta_CPN_cm = 0.0;
    double delta_CVN_cm = 0.0;
    double delta_protensao_cm = 0.0;
    double delta_total_cm = 0.0;
    double delta_lim_cm = 2.0; // 20 mm para residencial/comercial

    bool aprovado = true;
};

ResultadoELSVibracao calcularELSVibracao(
    const VigaVMP& viga,
    const PerfilI& perfil,
    double P_atuante_kN
);

} // inline namespace v2008

namespace v2024 {

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

ResultadoMomentoNegativoVMP calcularMomentoNegativoVMP(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const CaboProtensao& cabo,
    const ArmacaoLaje& armacaoLaje,
    double MSd_neg_kNm,
    double MSd_pos_kNm,
    double Lv_mm,
    double a_vigas_mm,
    double alpha_viga = 3.0,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40
);

double calcularCdist(double psi);

} // namespace v2024
} // namespace nbr8800