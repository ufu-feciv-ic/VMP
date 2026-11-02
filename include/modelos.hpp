#pragma once
#include <string>

// ============================================================================
// MODELOS DE DADOS - ARQUITETURA SIMPLIFICADA (ZERO NAMESPACES)
// Structs diretas com valores padrão e inicialização limpa.
// ============================================================================

enum TipoFabricacao {
    LAMINADO,
    SOLDADO
};

enum TipoLaje {
    LAJE_MACICA,
    LAJE_STEEL_DECK
};

enum TipoProtensao {
    PRE_TRACAO,
    POS_TRACAO
};

enum TipoTracado {
    CABO_RETO,
    CABO_POLIGONAL
};

// ----------------------------------------------------------------------------
// Geometria e Propriedades do Perfil I de Aço
// ----------------------------------------------------------------------------
struct PerfilI {
    std::string nome = "Perfil Customizado";
    TipoFabricacao tipo = LAMINADO;

    // Dimensões nominais (mm)
    double d = 0.0;   // Altura total
    double bf = 0.0;  // Largura da mesa
    double tf = 0.0;  // Espessura da mesa
    double tw = 0.0;  // Espessura da alma
    double r = 0.0;   // Raio de adoçamento (laminados)

    // Propriedades geométricas (calculáveis automaticamente ou do catálogo)
    double d1 = 0.0;  // Altura livre da alma (mm)
    double Ag = 0.0;  // Área bruta da seção (mm²)
    double Ix = 0.0;  // Inércia no eixo x-x (mm⁴)
    double Iy = 0.0;  // Inércia no eixo y-y (mm⁴)
    double Wx = 0.0;  // Módulo de resistência elástico x-x (mm³)
    double Wy = 0.0;  // Módulo de resistência elástico y-y (mm³)
    double Zx = 0.0;  // Módulo de resistência plástico x-x (mm³)
    double Zy = 0.0;  // Módulo de resistência plástico y-y (mm³)
    double rx = 0.0;  // Raio de giração x-x (mm)
    double ry = 0.0;  // Raio de giração y-y (mm)
    double It = 0.0;  // Inércia à torção de St. Venant (mm⁴)
    double Cw = 0.0;  // Constante de empenamento (mm⁶)
};

// ----------------------------------------------------------------------------
// Material de Aço Estrutural
// ----------------------------------------------------------------------------
struct Aco {
    std::string nome = "ASTM A572 Gr. 50";
    double fy = 345.0;    // Tensão de escoamento (MPa)
    double fu = 450.0;    // Tensão de ruptura (MPa)
    double E = 200000.0;  // Módulo de elasticidade (MPa)
    double nu = 0.3;      // Coeficiente de Poisson
};

// ----------------------------------------------------------------------------
// Concreto da Laje (NBR 6118)
// ----------------------------------------------------------------------------
struct Concreto {
    std::string classe = "C30";
    double fck = 30.0;     // Resistência característica à compressão (MPa)
    double Ecs = 26838.0;  // Módulo secante do concreto (MPa)
    double fctm = 2.9;     // Resistência média à tração (MPa)
};

// ----------------------------------------------------------------------------
// Laje de Concreto
// ----------------------------------------------------------------------------
struct Laje {
    TipoLaje tipo = LAJE_MACICA;
    double tc = 130.0;               // Espessura total da laje (mm)
    double largura_efetiva = 3000.0; // Largura colaborante beff (mm)
    double hF = 0.0;                 // Altura da fôrma de aço (se steel deck) (mm)
    bool nervura_paralela = false;   // Orientação das nervuras (se steel deck)
};

// ----------------------------------------------------------------------------
// Cabo de Protensão Externa
// ----------------------------------------------------------------------------
struct CaboProtensao {
    TipoProtensao tipo = PRE_TRACAO;
    TipoTracado tracado = CABO_RETO;
    std::string tipo_aco = "CP-190 RB";
    double fpk = 1900.0;      // Resistência característica à ruptura (MPa)
    double fpyk = 1710.0;     // Tensão de escoamento (MPa)
    double Ep = 200000.0;     // Módulo de elasticidade do cabo (MPa)
    double diametro = 12.7;   // Diâmetro nominal da cordoalha (mm)
    int n_cordoalhas = 3;     // Número de cordoalhas
    double Ap = 420.0;        // Área total de aço de protensão (mm²) (ex: 3 * 140 mm²)
    double ep = -50.0;        // Posição do cabo em relação à face inferior do perfil (mm)
    double delta_cunha = 6.0; // Acomodação da cunha na ancoragem (mm)
};

// ----------------------------------------------------------------------------
// Conector de Cisalhamento Perfil U
// ----------------------------------------------------------------------------
struct ConectorU {
    double tf = 4.75;   // Espessura da mesa do conector U (mm)
    double tw = 4.75;   // Espessura da alma do conector U (mm)
    double Lcs = 50.0;  // Comprimento do conector (mm)
    double h = 75.0;    // Altura do conector (mm)
    double fu = 400.0;  // Tensão de ruptura do conector (MPa)
};

// ----------------------------------------------------------------------------
// Ações e Carregamentos Distribuídos
// ----------------------------------------------------------------------------
struct CargasVMP {
    double q1_perfil = 0.56;        // Peso próprio do perfil metálico (kN/m)
    double q2_concreto = 9.75;      // Peso próprio do concreto fresco (kN/m)
    double qs_construcao = 3.0;     // Sobrecarga transitória de construção (kN/m)
    double q3_revestimento = 6.0;   // Carga permanente de acabamento (kN/m)
    double q4_uso = 15.0;           // Sobrecarga acidental de uso (kN/m)
    double psi1 = 0.6;              // Fator combinação frequente
    double psi2 = 0.4;              // Fator combinação quase permanente
    bool escorada = false;          // Viga escorada ou não escorada
};

// ----------------------------------------------------------------------------
// Modelo Agregado da Viga Mista Protendida (VMP)
// ----------------------------------------------------------------------------
struct VigaVMP {
    PerfilI perfil;
    Aco aco;
    Concreto concreto;
    Laje laje;
    CaboProtensao cabo;
    ConectorU conector;
    CargasVMP cargas;

    double L = 12000.0;        // Vão teórico da viga (mm)
    double Lb = 3000.0;        // Comprimento destravado à flexotorção (mm)
    double P0_adotado = 400.0; // Força inicial de protensão adotada (kN)
};
