#pragma once
#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include <vector>
#include <string>

using namespace nbr8800;

// Trecho do cabo ao qual uma seção transversal pertence
enum TrechoCabo {
    TRECHO_ATIVA,
    TRECHO_MEIO_VAO,
    TRECHO_PASSIVA
};

// Definição geométrica de uma seção transversal discretizada ao longo da viga [0, L]
struct SecaoTransversalViga {
    int indice = 0;
    std::string nome;          // Ex: "Seção 0 (Apoio Esq.)", "Seção 1", "Seção 2 (Meio do Vão)"...
    TrechoCabo trecho = TRECHO_MEIO_VAO;
    double x_mm = 0.0;         // Coordenada longitudinal medida a partir do apoio esquerdo (0 a L)
    double ep_perfil_mm = 0.0; // Posição do cabo relativa à face inferior do perfil I na seção (negativa se acima da face inferior)
};

// Resultado completo da verificação ELU em uma seção específica (x) para uma Fase
struct ResultadoSecaoELU {
    int indice_secao = 0;
    std::string nome_secao;
    double x_mm = 0.0;
    double P_atuante_kN = 0.0;
    double ep_LNE_mm = 0.0;       // Excentricidade do cabo em relação à LNE da seção na fase (ep,I ou ep,tr,X)
    double ep_perfil_mm = 0.0;    // Posição do cabo em relação à face inferior do perfil I

    // Esforços solicitantes de cálculo na seção
    double MSd_pos_kNm = 0.0;     // Momento solicitante positivo na seção
    double MSd_neg_kNm = 0.0;     // Momento solicitante negativo na seção (protensão)
    double VSd_kN = 0.0;          // Esforço cortante solicitante na seção
    double VSd_max_kN = 0.0;      // Esforço cortante solicitante máximo do vão na fase

    // Resultados das rotinas de resistência na seção
    ResultadoTensaoCabo tensao_cabo{};
    ResultadoMomentoPositivoVMP res_momento_pos{};
    ResultadoMomentoNegativoVMP res_momento_neg{};
    double MRd_pos_kNm = 0.0;
    double MRd_neg_kNm = 0.0;
    double VRd_kN = 0.0;
    double NRd_kN = 0.0;

    // Índices de aproveitamento (Solicitante / Resistente)
    double indice_momento_pos = 0.0;
    double indice_momento_neg = 0.0;
    double indice_cortante = 0.0;
    double indice_flexo_compressao_pos = 0.0;
    double indice_flexo_compressao_neg = 0.0;
    bool aprovado = true;
};

// Resultado consolidado de uma Fase ELU (contendo todas as X seções analisadas)
struct ResultadoFaseELU {
    int numero_fase = 1; // 1 = Construção, 2 = VMP (t=0), 3 = VMP (t=inf)
    std::string descricao;
    ForcasProtensaoTrechos forcas_P{};
    double VSd_max_kN = 0.0;
    double VRd_kN = 0.0;
    std::vector<ResultadoSecaoELU> secoes;
    bool aprovado_fase = true;

    // Helpers para buscar seções notáveis rapidamente (apoio x=0 e meio do vão x=L/2)
    const ResultadoSecaoELU* secaoApoioEsquerdo() const;
    const ResultadoSecaoELU* secaoMeioVao() const;
    const ResultadoSecaoELU* secaoApoioDireito() const;
};

// Item individual de verificação com diagnóstico simples
struct ItemVerificacao {
    std::string rotina;
    std::string descricao;
    double solicitante;
    double resistente;
    std::string unidade;
    bool aprovado;
};

// Relatório completo da verificação de todas as fases e seções da VMP
struct RelatorioVMP {
    bool aprovado_elu = true;
    bool aprovado_els = true;
    bool aprovado_global = true;
    std::vector<ItemVerificacao> verificacoes;

    // Malha de seções geradas ao longo da viga
    std::vector<SecaoTransversalViga> malha_secoes;

    // Resultados intermediários das rotinas base
    ResultadoRotinaE conectores{};
    ResultadoRotinaF costura{};
    ResultadoNormalResistentePerfil compressao_aco{};
    ResultadoMomentoResistentePerfil flexao_aco{};
    ResultadoCortanteResistentePerfil cortante_aco{};
    ResultadoMomentoPositivoVM momento_vm_sem_protensao{};
    ResultadoSecaoTransformada secao_t0{};
    ResultadoSecaoTransformada secao_t_inf{};
    ResultadoEstimativaP0 estimativa_p0{};
    ResultadoArmaduraProtensao armadura_protensao{};
    ResultadoMomentoPositivoVMP momento_vmp{}; // No meio do vão (Fase 2, t=0)

    // Força inicial efetiva e forças nas fases (no meio do vão)
    double P0_efetivo_kN = 0.0;
    double P1_fase1_kN = 0.0;
    double P2_fase2_kN = 0.0;
    double P3_fase3_kN = 0.0;

    // Esforços solicitantes de referência no meio do vão / apoio
    double MSd_fase1_kNm = 0.0;
    double MSd_neg_fase1_kNm = 0.0;
    double MSd_fase2_kNm = 0.0;
    double MSd_neg_fase2_kNm = 0.0;
    double MSd_fase3_kNm = 0.0;
    double MSd_neg_fase3_kNm = 0.0;
    double VSd_fase1_kN = 0.0;
    double VSd_fase2_kN = 0.0;
    double VSd_fase3_kN = 0.0;

    // Relatórios detalhados por Fase (com todas as X seções da viga)
    ResultadoFaseELU fase1_elu{};
    ResultadoFaseELU fase2_elu{};
    ResultadoFaseELU fase3_elu{};

    // Verificações em Estado Limite de Serviço (ELS)
    ResultadoELSDeformacao els_deformacao{};
    ResultadoELSVibracao els_vibracao{};
};

// Gera as X seções transversais ao longo da viga [0, L] com o perfil geométrico do cabo
std::vector<SecaoTransversalViga> gerarSecoesViga(
    const VigaVMP& viga,
    const PerfilI& perfil,
    double ytr_t0_mm
);

// Função executora sequencial da verificação completa de uma VMP
RelatorioVMP verificar_viga_vmp(const VigaVMP& viga);
