#pragma once
#include "modelos.h"
#include <string>

// ============================================================================
// MODELOS DE DADOS PARA VIGA MISTA DE AÇO E CONCRETO (NBR 8800)
// 100% Agnóstico de Protensão
// ============================================================================

namespace viga_mista {

enum class NormaEdicao {
    NBR8800_2008,
    NBR8800_2024
};

enum class TipoLajeMista {
    MACICA,
    STEEL_DECK
};

enum class OrientacaoDeck {
    PERPENDICULAR_A_VIGA,
    PARALELA_A_VIGA
};

// ----------------------------------------------------------------------------
// Dados da Laje de Concreto e Fôrma de Aço
// ----------------------------------------------------------------------------
struct DadosLaje {
    TipoLajeMista tipo = TipoLajeMista::MACICA;
    double tc_mm = 120.0;               // Espessura da laje sobre a chapa (ou total se maciça)
    double hF_deck_mm = 0.0;            // Altura da nervura da fôrma (mm)
    double bF_deck_mm = 0.0;            // Largura média da nervura de concreto (mm)
    double passo_deck_mm = 0.0;         // Espaçamento/passo entre nervuras (mm)
    double tf_deck_mm = 0.80;           // Espessura da chapa de aço da fôrma (mm)
    OrientacaoDeck orientacao = OrientacaoDeck::PERPENDICULAR_A_VIGA;
};

// ----------------------------------------------------------------------------
// Conectores Stud Bolt (Pinos com Cabeça)
// ----------------------------------------------------------------------------
struct DadosStudBolt {
    double d_cs_mm = 19.0;              // Diâmetro nominal do pino (mm)
    double h_cs_mm = 100.0;             // Altura total do pino após solda (mm)
    double fu_MPa = 420.0;              // Resistência à tração do aço do pino (MPa)
    int n_por_linha = 1;                // Número de conectores por linha transversal / por nervura
    double espacamento_adotado_mm = 0.0; // Espaçamento uniforme adotado (mm). Se 0, calcula uniformemente.
    double Rg = 1.0;                    // Fator de grupo (definido ou calculado pela norma)
    double Rp = 1.0;                    // Fator de posição (definido ou calculado pela norma)
};

// ----------------------------------------------------------------------------
// Geometria Estrutural e de Influência
// ----------------------------------------------------------------------------
struct GeometriaVigaMista {
    double L_vao_mm = 7000.0;           // Vão teórico da viga (mm)
    double Lb_mm = 3500.0;              // Comprimento destravado na Fase 1 (mm)
    double dist_esq_mm = 1750.0;        // Distância até viga adjacente à esquerda (mm)
    double dist_dir_mm = 1750.0;        // Distância até viga adjacente à direita (mm)
    bool eh_viga_borda = false;         // Viga central ou de borda
    double dist_borda_mm = 0.0;         // Balanço da laje até a borda livre (mm)
    bool escorada = false;              // Construção escorada ou não escorada
};

// ----------------------------------------------------------------------------
// Ações e Combinações de Serviço/Último por Fases
// ----------------------------------------------------------------------------
struct AcoesPiso {
    // Fase 1: Perfil de Aço Isolado (Construção)
    double peso_proprio_perfil_kN_m = 0.0; // Se 0, calcula automaticamente do PerfilI (Ag * 78.5 kN/m³)
    double q_laje_fresco_kN_m2 = 3.0;      // Peso próprio da laje fresca (ex: tc * 25 kN/m³)
    double q_sobrecarga_const_kN_m2 = 1.0; // Ação variável transitória de construção (kN/m²)

    // Fase 2: Seção Mista (Pós-cura)
    double g_revestimento_kN_m2 = 4.0;     // Cargas permanentes após a cura (revestimento, alvenaria)
    double q_uso_kN_m2 = 3.0;              // Ação variável de uso (NBR 6120) (kN/m²)
    double g_linear_adicional_kN_m = 0.0;  // Carga linear permanente adicional (ex: alvenaria direta)

    // Coeficientes de combinação e limites
    double gamma_g1 = 1.25;                // Ponderação permanente Fase 1
    double gamma_q1 = 1.30;                // Ponderação variável Fase 1
    double gamma_g2 = 1.40;                // Ponderação permanente Fase 2
    double gamma_q2 = 1.50;                // Ponderação variável Fase 2
    double psi1 = 0.70;                    // Fator combinação frequente ELS
    double psi2 = 0.40;                    // Fator combinação quase permanente ELS
    double psi_vib = 0.00;                 // Fator para massa de vibração do piso

    // Parâmetros de vibração (NBR 8800:2024 Anexo I)
    std::string ocupacao = "Garagens";     // Categoria de ocupação
    double beta_amortecimento = 0.025;     // Razão de amortecimento total
    double kb_vibracao = 2.0;              // Fator kb do piso (2.0 demais casos)
    int np_vaos = 3;                       // Número de vãos contíguos
    double Lp_vao_principal_mm = 7000.0;   // Vão principal do piso
};

// ----------------------------------------------------------------------------
// Configuração Agregada da Viga Mista
// ----------------------------------------------------------------------------
struct VigaMistaConfig {
    NormaEdicao norma = NormaEdicao::NBR8800_2024;
    PerfilI perfil;
    Aco aco;
    Concreto concreto;
    DadosLaje laje;
    DadosStudBolt conector;
    GeometriaVigaMista geometria;
    AcoesPiso acoes;
};

} // namespace viga_mista
