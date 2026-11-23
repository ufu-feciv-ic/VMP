#pragma once
#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include <vector>
#include <string>

// ============================================================================
// VERIFICADOR DE VIGA MISTA (REUTILIZANDO ROTINAS_NBR8800 E ROTINAS_VMP)
// Diferenciação explícita entre nbr8800::v2008 e nbr8800::v2024
// ============================================================================

struct ItemDiagnostico {
    std::string item;
    std::string descricao;
    double solicitante = 0.0;
    double resistente = 0.0;
    std::string unidade;
    double taxa_utilizacao = 0.0;
    bool aprovado = true;
};

// Resultados consolidados da Fase 1 (Perfil Isolado) usando ResultadoMomentoResistentePerfil e ResultadoCortanteResistentePerfil
struct ResultadoFase1VM {
    double g1_kN_m = 0.0;
    double q1_kN_m = 0.0;
    double qSd1_kN_m = 0.0;
    double MSd1_kNm = 0.0;
    double VSd1_kN = 0.0;

    nbr8800::ResultadoMomentoResistentePerfil flexao_aco;
    nbr8800::ResultadoCortanteResistentePerfil cortante_aco;

    double taxa_flexao = 0.0;
    double taxa_cortante = 0.0;
    bool aprovado_flexao = true;
    bool aprovado_cortante = true;
};

// Resultados consolidados da Fase 2 (Seção Mista) usando ResultadoMomentoPositivoVM e ResultadoCortanteResistentePerfil
struct ResultadoFase2VM {
    double bef_mm = 0.0;
    double g2_kN_m = 0.0;
    double q2_kN_m = 0.0;
    double qSd2_kN_m = 0.0;
    double MSd2_kNm = 0.0;
    double VSd2_kN = 0.0;

    nbr8800::ResultadoMomentoPositivoVM momento_misto;
    nbr8800::ResultadoCortanteResistentePerfil cortante_misto;

    double taxa_flexao = 0.0;
    double taxa_cortante = 0.0;
    bool aprovado_flexao = true;
    bool aprovado_cortante = true;
};

// Relatório Consolidado de Viga Mista (NBR 8800:2008 / NBR 8800:2024)
struct RelatorioVigaMista {
    bool aprovado_global = true;
    std::vector<ItemDiagnostico> itens;

    ResultadoFase1VM fase1;
    nbr8800::ResultadoRotinaE conectores;
    ResultadoFase2VM fase2;
    nbr8800::ResultadoRotinaF costura;
    nbr8800::ResultadoFlechasVM flechas;
};

// Funções do Verificador de Viga Mista (despacham para nbr8800::v2008 ou nbr8800::v2024)
nbr8800::ResultadoRotinaE calcularConectoresVigaMista(const VigaMistaConfig& cfg, double bef_mm);
RelatorioVigaMista verificar_viga_mista(const VigaMistaConfig& cfg);

// ----------------------------------------------------------------------------
// Compatibilidade com o modelo VMP (Protendida)
// ----------------------------------------------------------------------------
struct ItemVerificacao {
    std::string rotina;
    std::string descricao;
    double solicitante;
    double resistente;
    std::string unidade;
    bool aprovado;
};

struct RelatorioVMP {
    bool aprovado_global = true;
    std::vector<ItemVerificacao> verificacoes;

    nbr8800::ResultadoRotinaE conectores;
    nbr8800::ResultadoRotinaF costura;
    nbr8800::ResultadoNormalResistentePerfil compressao_aco;
    nbr8800::ResultadoMomentoResistentePerfil flexao_aco;
    nbr8800::ResultadoCortanteResistentePerfil cortante_aco;
    nbr8800::ResultadoEstimativaP0 estimativa_p0;
    nbr8800::ResultadoSecaoTransformada secao_t0;
    nbr8800::ResultadoSecaoTransformada secao_t_inf;
    nbr8800::ResultadoMomentoPositivoVMP momento_vmp;

    double P1_fase1_kN = 0.0;
    double P2_fase2_kN = 0.0;
    double MSd_fase1_kNm = 0.0;
    double MSd_fase2_kNm = 0.0;
    double VSd_fase1_kN = 0.0;
    double VSd_fase2_kN = 0.0;
};

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga);
