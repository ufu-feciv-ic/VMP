#pragma once
#include "modelos.h"
#include "rotinas_vmp.h"

namespace nbr8800 {

// ============================================================================
// ESTRUTURAS DE RESULTADO COMPARTILHADAS ENTRE AS EDIÇÕES 2008 E 2024
// ============================================================================

// ----------------------------------------------------------------------------
// Rotina A: Compressão Axial do Perfil de Aço (NRd)
// ----------------------------------------------------------------------------
struct ResultadoNormalResistentePerfil {
    double Nex_kN = 0.0;
    double Ney_kN = 0.0;
    double Ne_kN = 0.0;
    double Qs = 1.0;
    double Qa = 1.0;
    double Q = 1.0;
    double lb0 = 0.0;
    double chi = 1.0;
    double NRd_kN = 0.0;
};

// ----------------------------------------------------------------------------
// Rotina B: Momento Fletor Resistente do Perfil de Aço Isolado (MRd)
// ----------------------------------------------------------------------------
struct ResultadoMomentoResistentePerfil {
    double Mpl_kNm = 0.0;
    double Mr_kNm = 0.0;
    double Mcr_kNm = 0.0;
    double MRd_FLT_kNm = 0.0;
    double MRd_FLM_kNm = 0.0;
    double MRd_FLA_kNm = 0.0;
    double MRd_kNm = 0.0;
    bool perfil_esbelto = false;
};

// ----------------------------------------------------------------------------
// Rotina C: Cálculo do Momento Resistente de Cálculo da Viga Mista
// ----------------------------------------------------------------------------
struct ResultadoMomentoPositivoVM {
    bool aplicavel = true;
    bool secao_compacta = true;
    bool lnp_na_laje = true;
    double lbw = 0.0;
    double lbw_p = 0.0;
    double lbw_r = 0.0;
    double eta_c = 1.0;
    double eta_interacao = 1.0;
    double a_linha_neutra_mm = 0.0;
    double y_lnp_mm = 0.0;
    double Cc_max_kN = 0.0;
    double Ccd_kN = 0.0;
    double C_aco_kN = 0.0;
    double T_aco_kN = 0.0;
    double MRd_VM_kNm = 0.0;
};

struct ResultadoMomentoNegativoVM {
    bool aplicavel = true;
    double MRd_neg_kNm = 0.0;
    double Mrd_dist_kNm = 0.0;
    double MRd_VM_kNm = 0.0;
};

// ----------------------------------------------------------------------------
// Rotina D: Força Cortante Resistente do Perfil de Aço (VRd)
// ----------------------------------------------------------------------------
struct ResultadoCortanteResistentePerfil {
    double kv = 5.0;
    double lbw = 0.0;   // esbeltez da alma
    double lbw_p = 0.0; // limite de esbeltez plástica da alma
    double lbw_r = 0.0; // limite de esbeltez elástica da alma
    double Vpl_kN = 0.0;
    double VRd_kN = 0.0;
};

// ----------------------------------------------------------------------------
// Rotina E: Conectores de Cisalhamento (Perfil U e Pino com Cabeça - Stud Bolt)
// ----------------------------------------------------------------------------
struct ResultadoRotinaE {
    double beff_mm = 0.0;
    double Acs_mm2 = 0.0;
    double Rg = 1.0;
    double Rp = 1.0;
    double eta_c = 1.0;
    double QRd1_concreto_kN = 0.0;
    double QRd2_aco_kN = 0.0;
    double QRd_kN = 0.0;                // Resistência individual de cálculo de 1 conector
    double Fhd_kN = 0.0;                // Força horizontal rasante para interação completa (100%)

    int n_linhas_meio_vao = 0;          // Número de linhas transversais de conectores por meio vão: floor((L/2) / s)
    int n_semi_vao_adotado = 0;         // Conectores por meio vão adotados: n_linhas_meio_vao * nt
    int n_total_adotado = 0;            // Total de conectores efetivamente adotados no vão: 2 * n_semi_vao_adotado
    int n_semi_vao_calculado = 0;       // Conectores por meio vão necessários p/ 100% (interação completa): ceil(Fhd / QRd)
    int nl_conectores = 0;              // Total de conectores no vão p/ 100%: 2 * n_semi_vao_calculado
    int n_semi_vao_minimo = 0;          // Conectores por meio vão necessários p/ atender eta_i: ceil(eta_i * Fhd / QRd)
    int n_total_minimo = 0;             // Total de conectores necessários p/ atender eta_i: 2 * n_semi_vao_minimo
    int nt_max_mesa = 1;                // Número máximo de pinos por linha suportados pela largura da mesa do perfil
    bool alerta_ancoragem_deck = false; // Se deck perp e s > 450 mm, requer fixação adicional da fôrma (solda tampão)
    double el_espacamento_mm = 0.0;     // Espaçamento longitudinal adotado (mm)
    double s_min_long_mm = 0.0;
    double s_max_long_mm = 0.0;

    double grau_interacao_eta = 1.0;    // Grau de interação efetivo (Sigma_QRd / Fhd)
    double grau_interacao_minimo = 0.4; // Grau mínimo exigido pela norma (O.2.3.1.1.2 ou L.2.3.1.1.2)
    bool espacamento_atende = true;
    bool grau_interacao_atende = true;

    // Verificações geométricas normativas do conector
    double h_min_normativo_mm = 0.0;
    double cobrimento_topo_mm = 0.0;
    double cobrimento_min_mm = 10.0;
    double d_max_mesa_mm = 0.0;
    bool altura_atende = true;
    bool cobrimento_atende = true;
    bool diametro_mesa_atende = true;

    // Verificações geométricas transversais (Stud Bolt / Mesa / Nervura)
    double s_transv_min_mm = 0.0;          // Espaçamento transversal mínimo entre eixos (4 * d)
    double dist_borda_min_mesa_mm = 25.0;  // Distância mínima ao bordo livre da mesa (25 mm conforme NBR 8800 L.4.4.2)
    double bf_min_necessario_mm = 0.0;     // Largura mínima da mesa necessária: (nt - 1) * s_transv_min + 2 * dist_borda_min
    bool largura_mesa_atende = true;       // perfil.bf >= bf_min_necessario_mm

    // Verificação de interferência em Steel Deck com nervuras paralelas
    double largura_deck_disp_mm = 0.0;     // Largura disponível na calha/fundo da fôrma
    double largura_deck_nec_mm = 0.0;      // Largura necessária na calha da fôrma para nt pinos
    bool nervura_deck_atende = true;       // Se atende na fôrma
    bool geometria_transversal_atende = true; // Consolidado transversal: largura_mesa_atende && nervura_deck_atende
};

// ----------------------------------------------------------------------------
// Rotina F: Armadura de Costura contra Fendilhamento (As,cos)
// ----------------------------------------------------------------------------
struct ResultadoRotinaF {
    double Hv_Sd_kN_cm = 0.0;
    double Hv_Rd_max_kN_cm = 0.0;
    double nu_Sd_kN_m = 0.0;
    double nu_Rd_max_kN_m = 0.0;
    double As_cos_cm2_m = 0.0;
    double lb_comprimento_cm = 0.0;
    bool atende_bielas = true;
};

// ----------------------------------------------------------------------------
// Seção homogeneizada para os estados-limites de serviço (O.1.2 / L.1.2)
// Concreto tracionado desprezado: se a linha neutra elástica cair dentro da
// laje, recalcula-se a profundidade comprimida x (equilíbrio de momentos estáticos).
// ----------------------------------------------------------------------------
struct ResultadoSecaoHomogeneizadaELS {
    double razao_modular = 0.0;   // n = Ea / Ec (já multiplicada por 3 para longa duração)
    double btr_mm = 0.0;          // bef / n
    bool lne_na_laje = false;     // linha neutra elástica dentro da laje de concreto
    double x_mm = 0.0;            // profundidade comprimida da laje (se lne_na_laje)
    double yLN_mm = 0.0;          // posição da LNE medida a partir da face inferior do perfil
    double Itr_mm4 = 0.0;
    double Wtr_inf_mm3 = 0.0;     // módulo na face inferior do perfil
    double Wtr_sup_mm3 = 0.0;     // módulo na face superior da laje (em unidades de aço)
};

ResultadoSecaoHomogeneizadaELS calcularSecaoHomogeneizadaELS(
    const PerfilI& perfil,
    const Laje& laje,
    double razao_modular,
    bool desprezar_concreto_tracionado = true
);

// ----------------------------------------------------------------------------
// Rotina ELS: Flechas por fases (Anexo B / L.1.2 / L.2.3.2) e tensão em serviço
//
// Parcelas (modelo da planilha de referência e do Anexo B da NBR 8800:2024):
//   delta_g1 = k·g1/Ia                         (não escorada; 0 se escorada)
//   delta1   = delta_g1 + k·g2/Ief,s            (permanentes, sem efeitos de longa duração)
//   delta_fl = k·g_mista·(1/Ief,l − 1/Ief,s)     (acréscimo por fluência)
//   delta_sh = Psh·esh·L² / (8·E·Ief,l)          (retração, opcional – AISC 360-16 C-I3)
//   delta2   = delta_fl + delta_sh
//   delta3   = k·psi·q/Ief,s + k·psi2·q·(1/Ief,l − 1/Ief,s)
//   delta_max = delta1 + delta2 + delta3 − min(contraflecha; delta1) <= L/350
//   delta2 + delta3(dano) <= L/350 (e <= 15 mm com alvenaria solidarizada)
// com k = 5·L⁴ / (384·E).
// ----------------------------------------------------------------------------
struct ResultadoFlechasVM {
    ResultadoSecaoHomogeneizadaELS secao_curta; // n = Ea / Ec
    ResultadoSecaoHomogeneizadaELS secao_longa; // n = 3·Ea / Ec

    double alpha_E_curta = 0.0; // Ea / Ec
    double alpha_E_longa = 0.0; // 3·Ea / Ec
    double fator_interacao = 1.0; // sqrt(eta)
    double Itr_cd_cm4 = 0.0;
    double Itr_ld_cm4 = 0.0;
    double Ief_cd_cm4 = 0.0;
    double Ief_ld_cm4 = 0.0;
    double Wef_inf_cd_cm3 = 0.0;
    double Wef_inf_ld_cm3 = 0.0;

    double flecha_g1_mm = 0.0;            // delta_g1: perfil isolado na concretagem
    double flecha_perm_curta_mm = 0.0;    // delta1
    double flecha_fluencia_mm = 0.0;      // delta_fl
    double flecha_retracao_mm = 0.0;      // delta_sh
    double flecha_longa_duracao_mm = 0.0; // delta2 = delta_fl + delta_sh
    double flecha_var_max_mm = 0.0;       // delta3 na combinação de delta_max
    double flecha_var_dano_mm = 0.0;      // delta3 na combinação de dano
    double flecha_total_bruta_mm = 0.0;   // delta_tot = delta1 + delta2 + delta3
    double contraflecha_mm = 0.0;         // delta0 = min(contraflecha; delta1)
    double flecha_total_mm = 0.0;         // delta_max = delta_tot − delta0
    double flecha_dano_mm = 0.0;          // delta2 + delta3(dano)
    double contraflecha_sugerida_mm = 0.0; // múltiplo de 5 mm, <= delta_g1

    double limite_total_mm = 0.0;         // L / 350
    double limite_dano_mm = 0.0;          // L / 350 (e 15 mm com alvenaria)
    double taxa_flecha_total = 0.0;
    double taxa_flecha_dano = 0.0;
    bool aprovado_flecha = true;
    bool aprovado_flecha_dano = true;

    // NBR 8800:2008 L.3.2: deslocamento na combinação frequente, sem parcela dependente do tempo
    double flecha_vibracao_2008_mm = 0.0;

    // Verificação de tensão em serviço na mesa inferior (O.2.3.2-b / L.2.3.2-b)
    double tensao_mesa_inferior_MPa = 0.0;
    double limite_tensao_MPa = 0.0;
    bool tensao_servico_atende = true;
};

// Função geométrica comum de largura colaborante efetiva (bef)
double calcularLarguraEfetiva(const GeometriaVigaMista& geo);

// ============================================================================
// EDIÇÃO 1: ABNT NBR 8800:2008 (Anexo O / Item 5)
// ============================================================================
inline namespace v2008 {

double calcularEtaC(double fck_MPa);
double calcularEcsConcreto(double fck_MPa, double alpha_ag = 1.0);

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Lv_mm,
    double kx,
    double ky,
    double gamma_a1 = 1.10
);

double calcularMcrFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb = 1.0);
double calcularMRdFLT(const PerfilI& perfil, const Aco& aco, double lb_mm, double gamma_a1, double Cb, double Mpl, double Mr);
double calcularMRdFLM(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr);
double calcularMRdFLA(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr);

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Cb = 1.0,
    double gamma_a1 = 1.10
);

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double Fhd_conectores_kN = -1.0
);

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const ArmacaoLaje& armacaoLaje,
    double MSd_neg_kNm,
    double Msd_pos_kNm,
    double Lv_mm,
    double a_vigas_mm,
    double alpha_viga = 3.0,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40
);

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double a_enrijecedor_mm = 0.0,
    double gamma_a1 = 1.10
);

double calcularGrauInteracaoMinimo(
    const PerfilI& perfil,
    const Aco& aco,
    double Lv_mm,
    bool escorada = false,
    double MSd_kNm = 0.0,
    double MRd_kNm = 0.0
);

ResultadoRotinaE calcularConectoresCisalhamento(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    bool escorada = false,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double gamma_cs = 1.25
);

ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double gamma_cs = 1.25
);

ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c = 1.40
);

ResultadoFlechasVM calcularFlechasVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double Lv_mm,
    const AcoesPiso& acoes,
    double grau_interacao_eta,
    bool escorada = false
);

} // namespace v2008

// ============================================================================
// EDIÇÃO 2: ABNT NBR 8800:2024 (Anexo L / Anexo D / Item 5)
// ============================================================================
namespace v2024 {

double calcularEtaC(double fck_MPa);
double calcularEcsConcreto(double fck_MPa, double alpha_ag = 1.0);

ResultadoNormalResistentePerfil calcularNormalResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Lv_mm,
    double kx,
    double ky,
    double gamma_a1 = 1.10
);

double calcularMcrFLT(const PerfilI& perfil, const Aco& aco, double Lb_mm, double Cb = 1.0);
double calcularMRdFLT(const PerfilI& perfil, const Aco& aco, double lb_mm, double gamma_a1, double Cb, double Mpl, double Mr);
double calcularMRdFLM(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr);
double calcularMRdFLA(const PerfilI& perfil, const Aco& aco, double gamma_a1, double Mpl, double Mr);

ResultadoMomentoResistentePerfil calcularMomentoResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double Lb_mm,
    double Cb = 1.0,
    double gamma_a1 = 1.10
);

ResultadoMomentoPositivoVM calcularMomentoPositivoVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double Fhd_conectores_kN = -1.0
);

ResultadoMomentoNegativoVM calcularMomentoNegativoVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    const ArmacaoLaje& armacaoLaje,
    double MSd_neg_kNm,
    double Msd_pos_kNm,
    double Lv_mm,
    double a_vigas_mm,
    double alpha_viga = 3.0,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40
);

ResultadoCortanteResistentePerfil calcularCortanteResistentePerfil(
    const PerfilI& perfil,
    const Aco& aco,
    double a_enrijecedor_mm = 0.0,
    double gamma_a1 = 1.10
);

double calcularGrauInteracaoMinimo(
    const PerfilI& perfil,
    const Aco& aco,
    double Lv_mm,
    bool escorada = false,
    double MSd_kNm = 0.0,
    double MRd_kNm = 0.0
);

ResultadoRotinaE calcularConectoresCisalhamento(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    bool escorada = false,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double gamma_cs = 1.25
);

ResultadoRotinaE calcular_rotina_e_perfil_u(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    const Concreto& concreto,
    const ConectorCisalhamento& conector,
    double Lv_mm,
    double gamma_a1 = 1.10,
    double gamma_c = 1.40,
    double gamma_cs = 1.25
);

ResultadoRotinaF calcular_rotina_f_armadura_costura(
    const Laje& laje,
    const Concreto& concreto,
    double Fhd_kN,
    double Lv_mm,
    double gamma_c = 1.40
);

ResultadoFlechasVM calcularFlechasVM(
    const PerfilI& perfil,
    const Aco& aco,
    const Concreto& concreto,
    const Laje& laje,
    double Lv_mm,
    const AcoesPiso& acoes,
    double grau_interacao_eta,
    bool escorada = false
);

} // namespace v2024

} // namespace nbr8800
