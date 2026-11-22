#pragma once
#include "modelos_viga_mista.h"
#include <vector>
#include <string>

namespace viga_mista {

// ----------------------------------------------------------------------------
// Resultados da Fase 1 - Perfil de Aço Isolado
// ----------------------------------------------------------------------------
struct ResultadoFase1Perfil {
    double g1_kN_m;
    double q1_kN_m;
    double qSd1_kN_m;
    double MSd1_kNm;
    double VSd1_kN;

    // Resistência à flexão
    double Mcr_kNm;
    double lambda_LT;
    double chi_LT;
    double Mpl_kNm;
    double MRd_FLT_kNm;
    double MRd_FLM_kNm;
    double MRd_FLA_kNm;
    double MRd_aco_kNm;

    // Resistência ao cortante
    double kv;
    double Vpl_kN;
    double VRd_kN;

    // Diagnósticos
    double taxa_flexao;
    double taxa_cortante;
    bool aprovado_flexao;
    bool aprovado_cortante;
};

// ----------------------------------------------------------------------------
// Resultados da Conexão de Cisalhamento (Stud Bolt)
// ----------------------------------------------------------------------------
struct ResultadoConectoresStud {
    double Acs_mm2;
    double Rg;
    double Rp;
    double QRd1_concreto_kN;
    double QRd2_aco_kN;
    double QRd_adotado_kN;
    double Fhd_interface_kN;

    int n_semi_vao_calculado;
    int n_total_calculado;
    int n_total_adotado;
    double espacamento_adotado_mm;
    double s_min_long_mm;
    double s_max_long_mm;

    double grau_interacao_eta;
    double grau_interacao_minimo;
    bool espacamento_atende;
    bool grau_interacao_atende;

    // Verificações geométricas do pino Stud Bolt (NBR 8800 L.8.1, L.6.6 e L.8.4)
    double h_min_normativo_mm;    // Comprimento mínimo exigido (max(4*d, hF + 40mm))
    double cobrimento_topo_mm;    // Cobrimento de concreto acima da cabeça do conector
    double cobrimento_min_mm;     // Cobrimento superior mínimo da norma (10 mm)
    double d_max_mesa_mm;         // Diâmetro máximo admissível (2.5 * tf)
    bool altura_atende;           // h_cs >= h_min_normativo_mm
    bool cobrimento_atende;       // cobrimento_topo_mm >= cobrimento_min_mm
    bool diametro_mesa_atende;    // d_cs <= d_max_mesa_mm
};

// ----------------------------------------------------------------------------
// Resultados da Fase 2 - Momento Resistente da Viga Mista (ELU)
// ----------------------------------------------------------------------------
struct ResultadoMomentoMisto {
    double bef_mm;
    double binf_mm;
    double g2_kN_m;
    double q2_kN_m;
    double qSd2_kN_m;
    double MSd2_kNm;
    double VSd2_kN;

    // Equilíbrio plástico
    double fcd_MPa;
    double fyd_MPa;
    double Fa_kN;
    double Fc_max_kN;
    double Ccd_kN;
    double Cad_kN;
    double a_bloco_concreto_mm;
    double yp_lnp_aco_mm;
    bool lnp_na_laje;
    bool compacta;

    double MRd_VM_kNm;
    double VRd_VM_kN;

    // Verificação de tensões em construção não escorada (L.6.3.2)
    double tensao_mesa_inferior_MPa;
    double limite_tensao_MPa;
    bool tensao_servico_atende;

    double taxa_flexao;
    double taxa_cortante;
    bool aprovado_flexao;
    bool aprovado_cortante;
};

// ----------------------------------------------------------------------------
// Resultados da Armadura de Costura contra Fendilhamento (L.5.3)
// ----------------------------------------------------------------------------
struct ResultadoArmaduraCostura {
    double nu_Sd_kN_m;
    double nu_Rd_max_kN_m;
    double As_costura_cm2_m;
    bool atende_bielas;
};

// ----------------------------------------------------------------------------
// Resultados de ELS - Deformações / Flechas (L.5.2)
// ----------------------------------------------------------------------------
struct ResultadoFlechasVigaMista {
    double n_curta;
    double n_longa;
    double Itr_cd_cm4;
    double Itr_ld_cm4;
    double Ief_cd_cm4;
    double Ief_ld_cm4;

    double flecha_fase1_mm;       // Perfil isolado sob g1
    double flecha_perm_ld_mm;     // Permanentes pós-cura (g2) com fluência
    double flecha_var_cd_mm;      // Parcela instantânea de q2 (1 - psi2)
    double flecha_var_ld_mm;      // Parcela quase permanente de q2 com fluência (psi2)
    double flecha_total_mm;       // Soma total das parcelas
    double flecha_ativa_mm;       // Flecha pós-cura (g2 + q2)

    double limite_total_mm;       // Ex: L / 250 ou L / 350
    double limite_ativa_mm;       // Ex: L / 350 ou L / 500
    double taxa_flecha_total;
    bool aprovado_flecha;
};

// ----------------------------------------------------------------------------
// Resultados de ELS - Vibrações no Piso (NBR 8800:2024 Anexo I)
// ----------------------------------------------------------------------------
struct ResultadoVibracaoPiso {
    double Ec_din_MPa;
    double ndin;
    double Itr_din_cm4;
    double Ief_din_cm4;
    double Is_laje_mm3;

    double wvib_kN_m;
    double delta_viga_mm;
    double delta_laje_mm;
    double delta_din_total_mm;

    double f0_Hz;                 // Frequência natural fundamental
    double Bb_m;                  // Largura colaborante do piso
    double massa_modal_kg;        // Massa modal Modo A
    double beta_amort;            // Razão de amortecimento adotada
    double ap_m_s2;               // Aceleração de pico
    double ap_percent_g;          // Aceleração de pico em % de g
    double ap_lim_m_s2;           // Limite normativo conforme ocupação
    double R_fator_resposta;
    double R_max_limite;

    double taxa_aceleracao;
    bool aprovado_vibracao;
};

// ----------------------------------------------------------------------------
// Relatório Consolidado de Viga Mista
// ----------------------------------------------------------------------------
struct ItemDiagnostico {
    std::string item;
    std::string descricao;
    double solicitante;
    double resistente;
    std::string unidade;
    double taxa_utilizacao;
    bool aprovado;
};

struct RelatorioVigaMista {
    bool aprovado_global = true;
    std::vector<ItemDiagnostico> itens;

    ResultadoFase1Perfil fase1;
    ResultadoConectoresStud conectores;
    ResultadoMomentoMisto fase2;
    ResultadoArmaduraCostura costura;
    ResultadoFlechasVigaMista flechas;
    ResultadoVibracaoPiso vibracao;
};

// ----------------------------------------------------------------------------
// Funções do Motor de Viga Mista
// ----------------------------------------------------------------------------

double calcularLarguraInfluencia(const GeometriaVigaMista& geo);
double calcularLarguraEfetiva(const GeometriaVigaMista& geo);

ResultadoFase1Perfil calcularFase1Perfil(const VigaMistaConfig& cfg);

ResultadoConectoresStud calcularConectoresStud(const VigaMistaConfig& cfg, double bef_mm);

ResultadoMomentoMisto calcularMomentoMistoELU(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    const ResultadoConectoresStud& conn
);

ResultadoArmaduraCostura calcularArmaduraCostura(
    const VigaMistaConfig& cfg,
    double bef_mm,
    const ResultadoConectoresStud& conn
);

ResultadoFlechasVigaMista calcularFlechasVigaMista(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    double grau_interacao_eta
);

ResultadoVibracaoPiso calcularVibracaoPiso(
    const VigaMistaConfig& cfg,
    double bef_mm,
    double binf_mm,
    double grau_interacao_eta
);

RelatorioVigaMista verificar_viga_mista(const VigaMistaConfig& cfg);

} // namespace viga_mista
