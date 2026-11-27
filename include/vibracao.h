#pragma once
#include "modelos.h"
#include <vector>

// ============================================================================
// VIBRAÇÃO DE PISOS DEVIDA AO CAMINHAR DE PESSOAS
// ABNT NBR 8800:2024 Anexo I (avaliação simplificada, modo A - vigas secundárias)
// ABNT NBR 8800:2008 L.3.2 (critério indireto: deslocamento frequente <= 20 mm)
// ============================================================================

namespace nbr8800 {

// ----------------------------------------------------------------------------
// Ocupações (Tabela I.3 - limites; Tabela I.2 - amortecimento; I.1.5 - psi_vib)
// ----------------------------------------------------------------------------
struct OcupacaoVibracao {
    const char* descricao;
    double psi_vib;        // I.1.5
    double beta_ocupacao;  // Tabela I.2
    double ap_lim_m_s2;    // Tabela I.3
    double R_max;          // Tabela I.3
};

const std::vector<OcupacaoVibracao>& obter_tabela_ocupacoes_vibracao();

// Preenche psi_vib, beta_ocupacao, ap_lim e R_max a partir de uma linha da tabela
void aplicar_ocupacao_vibracao(ParametrosVibracao& params, const OcupacaoVibracao& ocup);

// ----------------------------------------------------------------------------
// Resultado da verificação
// ----------------------------------------------------------------------------
struct ResultadoVibracaoPiso {
    bool calculado = false;

    // Rigidez dinâmica (I.1.7) - concreto não fissurado, viga considerada mista
    double Ec_din_MPa = 0.0;
    double alpha_din = 0.0;        // Ea / Ec,din
    double Ib_mm4 = 0.0;           // inércia da viga mista secundária
    double Is_mm4_por_mm = 0.0;    // inércia da laje por unidade de largura (homogeneizada)

    // Carregamento e massa (I.1.5 / I.2.3)
    double b_mm = 0.0;             // distância entre vigas secundárias (largura de influência)
    double w_vib_kN_m = 0.0;       // g1 + g2 + psi_vib·q2
    double m_kg_m2 = 0.0;          // massa distribuída do piso

    // Frequência (I.2.2 e Tabela I.1, modo A)
    double delta_viga_mm = 0.0;
    double delta_laje_mm = 0.0;
    double delta_mm = 0.0;
    double f0_Hz = 0.0;

    // Massa modal (I.2.3)
    double kb = 2.0;
    double Db_mm4_por_mm = 0.0;    // Ib / b
    double Bb_rigidez_m = 0.0;
    double Bb_limite_m = 0.0;
    double Bb_m = 0.0;
    double M_kg = 0.0;

    // Resposta (I.2.4 / I.2.5)
    double beta = 0.0;
    double ap_m_s2 = 0.0;
    double W = 1.0;
    double zeta = 0.10;
    double aw_rms_m_s2 = 0.0;
    double R = 0.0;

    double ap_lim_m_s2 = 0.0;
    double R_max = 0.0;
    double utilizacao = 0.0;       // ap/ap,lim ou R/Rmax, conforme o critério

    bool atende_frequencia_minima = true;  // f0 >= 3 Hz (I.1.6)
    bool faixa_metodo_valida = true;       // 3 Hz <= f0 <= 9 Hz (I.2.1)
    bool aprovado = true;
};

// Avaliação simplificada do Anexo I (modo A). Cargas lineares em kN/m.
ResultadoVibracaoPiso calcularVibracaoPisoAnexoI(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    double Lv_mm,
    double b_influencia_mm,
    double g1_kN_m,
    double g2_kN_m,
    double q2_kN_m,
    const ParametrosVibracao& params
);

} // namespace nbr8800
