#pragma once
#include <string>

enum class NormaEdicao {
    NBR8800_2008,
    NBR8800_2024
};

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

enum TipoElementoProtensao {
    CORDOALHA,
    FIO,
    BARRA
};

enum TipoRelaxacao {
    RELAXACAO_BAIXA, // RB
    RELAXACAO_NORMA  // RN
};

enum TipoAncoragem {
    ATIVA_PASSIVA, 
    ATIVA_ATIVA
};

enum TipoCabo {
    CP_190_RB, 
    CP_210_RB,
    CP_175_RB
};

enum TipoCordoalha {
    CORDOALHA_15_2, // área unitária = 140 mm²
    CORDOALHA_12_7  // área unitária = 98,7 mm²
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
    double fy_MPa = 0.0;  // Tensão de escoamento (MPa)
    double fu_MPa = 0.0;  // Tensão de ruptura (MPa)
    double E_MPa = 0.0;   // Módulo de elasticidade (MPa)
    double G_MPa = 0.0;
    double nu = 0.3;      // Coeficiente de Poisson
};

// ----------------------------------------------------------------------------
// Concreto da Laje (NBR 6118 / NBR 8800)
// ----------------------------------------------------------------------------
struct Concreto {
    std::string classe = "Zerado";
    double fck_MPa = 0.0;    // Resistência característica à compressão (MPa)
    double Ecs_MPa = 0.0;    // Módulo secante do concreto (MPa)
    double fctm_MPa = 0.0;   // Resistência média à tração (MPa)
    double alpha_ag = 1.0;   // Fator de agregado graúdo (NBR 8800:2024 item 4.6.3.1: 1.0 granito/gnaisse)
};

// ----------------------------------------------------------------------------
// Laje de Concreto (Maciça ou com Fôrma de Aço Incorporada - Steel Deck)
// ----------------------------------------------------------------------------
struct Laje {
    TipoLaje tipo = LAJE_MACICA;
    double tc_mm = 120.0;            // Espessura efetiva de concreto (total se maciça, ou capa acima da fôrma se steel deck) (mm)
    double bef_mm = 0.0;             // Largura colaborante beff (mm)
    double hF_mm = 0.0;              // Altura da fôrma de aço (se steel deck) (mm)
    double bF_mm = 0.0;              // Largura média da nervura de concreto preenchida no vale da fôrma (mm)
    double eF_mm = 0.0;              // Passo entre nervuras da chapa (mm)
    double tf_deck_mm = 0.80;        // Espessura da chapa da fôrma (mm)
    bool nervura_paralela = false;   // Orientação das nervuras (false = transversal/perpendicular à viga; true = longitudinal/paralela)
};

// ----------------------------------------------------------------------------
// Cabo de Protensão Externa (VMP)
// ----------------------------------------------------------------------------
struct CaboProtensao {
    TipoProtensao tipo = PRE_TRACAO;
    TipoTracado tracado = CABO_RETO;
    TipoAncoragem ancoragem = ATIVA_PASSIVA;
    TipoElementoProtensao elemento = CORDOALHA;
    TipoRelaxacao relaxacao = RELAXACAO_BAIXA;
    TipoCabo cabo = CP_190_RB;
    TipoCordoalha cordoalha = CORDOALHA_15_2;
    double fpk = 0.0;         // Resistência característica à ruptura (MPa)
    double fpyk = 0.0;        // Tensão de escoamento (MPa)
    double sigma_Pd = 0.0;
    double Ep = 0.0;          // Módulo de elasticidade do cabo (MPa)
    double diametro = 0.0;    // Diâmetro nominal da cordoalha (mm)
    double area_unit = 0.0;
    int n_cordoalhas = 0;     // Número de cordoalhas
    double Ap = 0.0;          // Área total de aço de protensão (mm²)
    double ep = 0.0;          // Posição do cabo em relação à face inferior do perfil (mm)
    double delta_cunha = 0.0; // Acomodação da cunha na ancoragem (mm)
};

struct ArmacaoLaje {
    double As = 0.0;
    double Ads = 0.0;
    double fsd = 0.0;
    double d_linha_mm = 0.0;
};

// ----------------------------------------------------------------------------
// Conector de Cisalhamento (Perfil U ou Pino com Cabeça - Stud Bolt)
// ----------------------------------------------------------------------------
struct ConectorCisalhamento {
    TipoConector tipo = CONECTOR_STUD_BOLT;

    double fu = 420.0; // Tensão de ruptura do conector (MPa)
    double h = 100.0;  // Altura do conector (mm)

    // Especificações do Perfil U
    double tf = 0.0;   // Espessura da mesa do conector U (mm)
    double tw = 0.0;   // Espessura da alma do conector U (mm)
    double Lcs = 0.0;  // Comprimento do perfil transversal (mm)

    // Especificações do Stud Bolt (pino com cabeça)
    double d = 19.0;   // Diâmetro do pino (mm)
    int nt = 1;        // Número de conectores por linha transversal / por nervura
    double Rg = 0.0;   // Coeficiente de grupo (se <= 0, determinado pela norma)
    double Rp = 0.0;   // Coeficiente de posição (se <= 0, determinado pela norma)
    double emh_mm = 60.0; // Distância da borda do fuste do pino à alma da nervura a meia altura (mm)
    double espacamento_adotado_mm = 0.0; // Espaçamento longitudinal adotado (mm). Se <= 0, calcula para interação completa.
};

// ----------------------------------------------------------------------------
// Geometria Estrutural e Larguras de Influência da Viga Mista
// ----------------------------------------------------------------------------
struct GeometriaVigaMista {
    double L_vao_mm = 7000.0;     // Vão teórico da viga (mm)
    double Lb_mm = 3500.0;        // Comprimento destravado na Fase 1 (mm)
    double dist_esq_mm = 1750.0;  // Distância até viga adjacente à esquerda (mm)
    double dist_dir_mm = 1750.0;  // Distância até viga adjacente à direita (mm)
    bool eh_viga_borda = false;   // Viga central ou de borda
    double dist_borda_mm = 0.0;   // Balanço da laje até a borda livre (mm)
    bool escorada = false;        // Construção escorada ou não escorada
    double Cb = 1.00;             // Fator de modificação para momento não uniforme na Fase 1
};

// ----------------------------------------------------------------------------
// Ações Lineares e Combinações por Fases (Fase 1 Construção / Fase 2 Mista)
// ----------------------------------------------------------------------------
struct AcoesPiso {
    // Fase 1: Perfil de Aço Isolado (Construção) - Cargas lineares diretas (kN/m)
    double g1_kN_m = 5.749;                // Carga permanente antes da cura (peso próprio perfil + laje fresca) (kN/m)
    double q1_kN_m = 1.750;                // Sobrecarga variável transitória de construção (kN/m)

    // Fase 2: Seção Mista (Pós-cura) - Cargas lineares diretas (kN/m)
    double g2_kN_m = 7.000;                // Carga permanente aplicada após a cura (revestimentos, alvenaria, etc.) (kN/m)
    double q2_kN_m = 5.250;                // Sobrecarga variável de uso (kN/m)

    // Coeficientes de ponderação e combinação
    double gamma_g1 = 1.25;                // Ponderação permanente Fase 1
    double gamma_q1 = 1.30;                // Ponderação variável Fase 1
    double gamma_g2 = 1.40;                // Ponderação permanente Fase 2
    double gamma_q2 = 1.50;                // Ponderação variável Fase 2
    double psi1 = 0.70;                    // Fator combinação frequente ELS
    double psi2 = 0.40;                    // Fator combinação quase permanente ELS
    double contraflecha_mm = 0.0;          // Contraflecha de fabricação (mm)
};

// ----------------------------------------------------------------------------
// Ações e Carregamentos Distribuídos (Modelo VMP)
// ----------------------------------------------------------------------------
struct CargasVMP {
    double q1_perfil = 0.0;         // Peso próprio do perfil metálico (kN/m)
    double q2_concreto = 0.0;       // Peso próprio do concreto fresco (kN/m)
    double qs_construcao = 0.0;     // Sobrecarga transitória de construção (kN/m)
    double q3_revestimento = 0.0;   // Carga permanente de acabamento (kN/m)
    double q4_uso = 0.0;            // Sobrecarga acidental de uso (kN/m)
    double psi1 = 0.6;              // Fator combinação frequente
    double psi2 = 0.4;              // Fator combinação quase permanente
    bool escorada = false;          // Viga escorada ou não escorada
};

// ----------------------------------------------------------------------------
// Modelo Agregado da Viga Mista (NBR 8800:2008 / NBR 8800:2024)
// ----------------------------------------------------------------------------
struct VigaMistaConfig {
    NormaEdicao norma = NormaEdicao::NBR8800_2024;
    PerfilI perfil;
    Aco aco;
    Concreto concreto;
    Laje laje;
    ConectorCisalhamento conector;
    GeometriaVigaMista geometria;
    AcoesPiso acoes;
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

    double L = 0.0;          // Vão teórico da viga (mm)
    double Lb = 0.0;         // Comprimento destravado à flexotorção (mm)
    double P0_adotado = 0.0; // Força inicial de protensão adotada (kN)
};
