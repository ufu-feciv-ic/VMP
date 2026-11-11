#pragma once
#include <string>

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

enum TipoCabo {
    CP_190_RB, 
    CP_210_RB,
    CP_175_RB
};

enum TipoCordoalha {
    CORDOALHA_15_2, // área unitária = 140 mm²
    CORDOALHA_12_7 // área unitária = 98,7 mm²
};

enum TipoConector {
    CONECTOR_PERFIL_U = 0,
    CONECTOR_STUD_BOLT = 1
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
    double r = 0.0;   // Raio (laminados)

    // Propriedades geométricas (calculáveis automaticamente ou do catálogo)
    double h = 0.0;   // Altura alma (mm)
    double d1 = 0.0;  // Altura livre da alma (mm)
    double Ag = 0.0;  // Área bruta da seção (mm²)
    double Ix = 0.0;  // Inércia no eixo x-x (mm⁴)
    double Wx = 0.0;  // Módulo de resistência elástico x-x (mm³)
    double rx = 0.0;  // Raio de giração x-x (mm)
    double Zx = 0.0;  // Módulo de resistência plástico x-x (mm³)
    double Iy = 0.0;  // Inércia no eixo y-y (mm⁴)
    double Wy = 0.0;  // Módulo de resistência elástico y-y (mm³)
    double ry = 0.0;  // Raio de giração y-y (mm)
    double Zy = 0.0;  // Módulo de resistência plástico y-y (mm³)
    double rt = 0.0;  // Raio de giração à flambagem lateral (mm)
    double It = 0.0;  // Inércia à torção de St. Venant (mm⁴)
    double lbf = 0.0; // Esbeltez da mesa 
    double lbw = 0.0; // Esbeltez da alma
    double Cw = 0.0;  // Constante de empenamento (mm⁶)
};

// ----------------------------------------------------------------------------
// Material de Aço Estrutural
// ----------------------------------------------------------------------------
struct Aco {
    std::string nome = "Zerado";
    double fy_MPa = 0.0;    // Tensão de escoamento (MPa)
    double fu_MPa = 0.0;    // Tensão de ruptura (MPa)
    double E_MPa = 0.0;  // Módulo de elasticidade (MPa)
    double nu = 0.0;      // Coeficiente de Poisson
};

// ----------------------------------------------------------------------------
// Concreto da Laje (NBR 6118)
// ----------------------------------------------------------------------------
struct Concreto {
    std::string classe = "Zerado";
    double fck_MPa = 0.0;     // Resistência característica à compressão (MPa)
    double Ecs_MPa = 0.0;  // Módulo secante do concreto (MPa)
    double fctm_MPa = 0.0;     // Resistência média à tração (MPa)
};

// ----------------------------------------------------------------------------
// Laje de Concreto
// ----------------------------------------------------------------------------
struct Laje {
    TipoLaje tipo = LAJE_MACICA;
    double tc_mm = 0.0;               // Espessura total da laje (mm)
    double bef_mm = 0.0; // Largura colaborante beff (mm)
    double hF_mm = 0.0;                 // Altura da fôrma de aço (se steel deck) (mm)
    bool nervura_paralela = false;   // Orientação das nervuras (se steel deck)
};

// ----------------------------------------------------------------------------
// Cabo de Protensão Externa
// ----------------------------------------------------------------------------
struct CaboProtensao {
    TipoProtensao tipo = PRE_TRACAO;
    TipoTracado tracado = CABO_RETO;
    TipoCabo cabo = CP_190_RB;
    TipoCordoalha cordoalha = CORDOALHA_15_2;
    double fpk = 0.0;      // Resistência característica à ruptura (MPa)
    double fpyk = 0.0;     // Tensão de escoamento (MPa)
    double sigma_Pd = 0.0;
    double Ep = 0.0;     // Módulo de elasticidade do cabo (MPa)
    double diametro = 0.0;   // Diâmetro nominal da cordoalha (mm)
    double area_unit = 0.0;
    int n_cordoalhas = 0;     // Número de cordoalhas
    double Ap = 0.0;        // Área total de aço de protensão (mm²) (ex: 3 * 140 mm²)
    double ep = 0.0;        // Posição do cabo em relação à face inferior do perfil (mm)
    double delta_cunha = 0.0; // Acomodação da cunha na ancoragem (mm)
};

struct ArmacaoLaje {
    double Ads;
    double fsd;
};

// ----------------------------------------------------------------------------
// Conector de Cisalhamento
// ----------------------------------------------------------------------------
struct ConectorCisalhamento {
    TipoConector tipo = CONECTOR_PERFIL_U;

    double fu = 0.0;  // Tensão de ruptura do conector (MPa)
    double h = 0.0;   // Altura do conector (mm)

    // Especificações do Perfil U
    double tf = 0.0;  // Espessura da mesa do conector U (mm)
    double tw = 0.0;  // Espessura da alma do conector U (mm)
    double Lcs = 0.0; // Comprimento do pergil transversal (mm)

    // Especificações do Stud Bolt (pino com cabeça)
    double d = 0.0;   // Diâmetro (mm)
    int nt = 0.0;     // Número de conectores por linha transversal
    double Rg = 0.0;  // Coeficiente de grupo (1.0 para lajes maciças)
    double Rp = 0.0;  // Coeficiente de posição (0.75 para lajes maciças)
};

// ----------------------------------------------------------------------------
// Ações e Carregamentos Distribuídos
// ----------------------------------------------------------------------------
struct CargasVMP {
    double q1_perfil = 0.0;        // Peso próprio do perfil metálico (kN/m)
    double q2_concreto = 0.0;      // Peso próprio do concreto fresco (kN/m)
    double qs_construcao = 0.0;     // Sobrecarga transitória de construção (kN/m)
    double q3_revestimento = 0.0;   // Carga permanente de acabamento (kN/m)
    double q4_uso = 0.0;           // Sobrecarga acidental de uso (kN/m)
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
    ConectorCisalhamento conector;
    CargasVMP cargas;

    double L = 0.0;        // Vão teórico da viga (mm)
    double Lb = 0.0;        // Comprimento destravado à flexotorção (mm)
    double P0_adotado = 0.0; // Força inicial de protensão adotada (kN)
};
