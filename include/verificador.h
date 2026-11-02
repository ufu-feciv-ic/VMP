#pragma once
#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include <vector>
#include <string>

// Item individual de verificação com diagnóstico simples
struct ItemVerificacao {
    std::string rotina;
    std::string descricao;
    double solicitante;
    double resistente;
    std::string unidade;
    bool aprovado;
};

// Relatório completo da verificação de todas as fases da VMP
struct RelatorioVMP {
    bool aprovado_global = true;
    std::vector<ItemVerificacao> verificacoes;

    // Resultados intermediários ricos
    ResultadoRotinaE conectores;
    ResultadoRotinaF costura;
    ResultadoRotinaA compressao_aco;
    ResultadoRotinaB flexao_aco;
    ResultadoRotinaD cortante_aco;
    ResultadoEstimativaP0 estimativa_p0;
    ResultadoSecaoTransformada secao_t0;
    ResultadoSecaoTransformada secao_t_inf;
    ResultadoMomentoVMP momento_vmp;

    // Forças nas fases
    double P1_fase1_kN;
    double P2_fase2_kN;
    double MSd_fase1_kNm;
    double MSd_fase2_kNm;
    double VSd_fase1_kN;
    double VSd_fase2_kN;
};

// Função executora sequencial da verificação de uma VMP
RelatorioVMP verificar_viga_vmp(const VigaVMP& viga);
