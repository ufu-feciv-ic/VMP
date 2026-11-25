#pragma once
#include <string>
#include <vector>

namespace nbr6120 {

// ----------------------------------------------------------------------------
// 1. Uso e Sobrecarga Variável (NBR 6120:2019 Tabela 10 & NBR 8800:2024 Tabela 2)
// ----------------------------------------------------------------------------
struct ItemUsoOcupacao {
    const char* descricao;
    double q_kN_m2;
    char categoria_psi; // 'R', 'C' ou 'A'
};

inline const std::vector<ItemUsoOcupacao>& obter_tabela_ocupacoes() {
    static const std::vector<ItemUsoOcupacao> tabela = {
        { "Edif. comerciais/escritórios – Salas de uso geral e sanitários", 2.50, 'C' },
        { "Edif. comerciais/escritórios – Corredores de uso comum", 3.00, 'C' },
        { "Edif. comerciais/escritórios – Call center", 3.00, 'C' },
        { "Edif. comerciais/escritórios – Regiões de arquivos deslizantes", 5.00, 'A' },
        { "Edif. residenciais – Dormitórios, sala, copa, cozinha", 1.50, 'R' },
        { "Edif. residenciais – Corredores de uso comum / Salão de festas", 3.00, 'R' },
        { "Edif. residenciais – Despensa, área de serviço e lavanderia", 2.00, 'R' },
        { "Edif. residenciais – Academia", 3.00, 'R' },
        { "Edif. residenciais – Depósitos", 3.00, 'A' },
        { "Escolas – Sala de aula / Corredores", 3.00, 'C' },
        { "Escolas – Auditório / Laboratórios", 4.00, 'C' },
        { "Hospitais – Dormitórios, enfermaria, recuperação", 2.00, 'C' },
        { "Hospitais – Salas de cirurgia / Raios X", 3.00, 'C' },
        { "Hotéis – Dormitórios", 1.50, 'C' },
        { "Hotéis – Restaurante / Corredores", 3.00, 'C' },
        { "Lojas e shopping centers – Circulações e lojas em geral", 4.00, 'C' },
        { "Lojas e shopping centers – Praça de alimentação (público)", 5.00, 'C' },
        { "Bibliotecas – Sala de leitura (sem estantes)", 3.00, 'C' },
        { "Bibliotecas – Sala com estantes de livros (até 2,2 m)", 6.00, 'A' },
        { "Garagens e estacionamentos – Veículos de passeio", 3.00, 'A' },
        { "Depósitos de uso geral (estoque até 2,5 m)", 7.50, 'A' },
        { "Coberturas – Com acesso apenas para manutenção", 1.00, 'A' },
        { "Outro (Personalizado)", 0.00, 'C' }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// 2. Divisórias sem posição definida (NBR 6120:2019 Tabela 11)
// ----------------------------------------------------------------------------
struct ItemDivisoria {
    const char* descricao;
    double q_adicional_kN_m2;
};

inline const std::vector<ItemDivisoria>& obter_tabela_divisorias() {
    static const std::vector<ItemDivisoria> tabela = {
        { "Não há divisórias sem posição definida", 0.00 },
        { "Divisórias leves com p.p. <= 1,0 kN/m", 0.50 },
        { "Divisórias com 1,0 < p.p. <= 2,0 kN/m", 0.75 },
        { "Divisórias com 2,0 < p.p. <= 3,0 kN/m", 1.00 }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// 3. Revestimentos de Piso (NBR 6120:2019 Tabela 4)
// ----------------------------------------------------------------------------
struct ItemRevestimento {
    const char* descricao;
    double peso_kN_m2;
};

inline const std::vector<ItemRevestimento>& obter_tabela_revestimentos() {
    static const std::vector<ItemRevestimento> tabela = {
        { "Nenhum", 0.00 },
        { "Revestimento residencial/comercial – 5 cm (argamassa/cerâmica)", 1.00 },
        { "Revestimento residencial/comercial – 7 cm", 1.40 },
        { "Revestimento de piso industrial – 5 cm", 1.70 },
        { "Piso elevado com placas de aço (até 30 cm)", 0.50 },
        { "Piso elevado com placas de polipropileno", 0.15 },
        { "Impermeabilização c/ manta e proteção – 10 cm", 1.80 }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// 4. Forros (NBR 6120:2019 Tabela 8)
// ----------------------------------------------------------------------------
struct ItemForro {
    const char* descricao;
    double peso_kN_m2;
};

inline const std::vector<ItemForro>& obter_tabela_forros() {
    static const std::vector<ItemForro> tabela = {
        { "Nenhum", 0.00 },
        { "Forro de gesso acartonado (drywall c/ suporte)", 0.25 },
        { "Forro de fibra mineral (c/ suporte)", 0.10 },
        { "Forro de gesso em placas", 0.15 },
        { "Forro de PVC ou placas de alumínio", 0.10 }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// 5. Instalações e Dutos Suspensos (NBR 6120:2019 Tabela 8)
// ----------------------------------------------------------------------------
struct ItemInstalacao {
    const char* descricao;
    double peso_kN_m2;
};

inline const std::vector<ItemInstalacao>& obter_tabela_instalacoes() {
    static const std::vector<ItemInstalacao> tabela = {
        { "Nenhum", 0.00 },
        { "Dutos de ar-condicionado c/ isolamento térmico", 0.30 },
        { "Dutos de ventilação s/ isolamento térmico", 0.20 },
        { "Sprinkler – rede DN até 65 mm", 0.10 },
        { "Sprinkler – rede DN até 80 mm", 0.15 }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// 6. Alvenarias e Paredes sobre a Viga (NBR 6120:2019 Tabelas 2 e 3)
// ----------------------------------------------------------------------------
struct ItemParede {
    const char* descricao;
    double peso_kN_m2_parede;
};

inline const std::vector<ItemParede>& obter_tabela_paredes() {
    static const std::vector<ItemParede> tabela = {
        { "Nenhuma parede sobre a viga", 0.00 },
        { "Bloco cerâmico vazado 9 cm – revest. 1 cm/face", 1.10 },
        { "Bloco cerâmico vazado 9 cm – revest. 2 cm/face", 1.60 },
        { "Bloco cerâmico vazado 11,5 cm – revest. 2 cm/face", 1.70 },
        { "Bloco cerâmico vazado 14 cm – revest. 1 cm/face", 1.50 },
        { "Bloco cerâmico vazado 14 cm – revest. 2 cm/face", 1.90 },
        { "Bloco de concreto vazado 14 cm – revest. 2 cm/face", 2.20 },
        { "Bloco de concreto vazado 19 cm – revest. 2 cm/face", 2.60 },
        { "Concreto celular autoclavado 10 cm", 1.00 },
        { "Drywall leve (chapas duplas + lã mineral)", 0.50 }
    };
    return tabela;
}

// ----------------------------------------------------------------------------
// Estrutura de Estado para o Assistente de Cargas
// ----------------------------------------------------------------------------
struct AssistenteCargasConfig {
    int idx_ocupacao = 0;
    double q_custom_kN_m2 = 2.50;
    
    int idx_divisoria = 2; // 0.75 kN/m² padrão escritório
    int idx_revestimento = 1; // 1.00 kN/m²
    int idx_forro = 1; // 0.25 kN/m²
    int idx_instalacao = 1; // 0.30 kN/m²
    double contrapiso_espessura_cm = 0.0; // 0.20 kN/m² por cm
    double outras_permanentes_area_kN_m2 = 0.0;

    int idx_parede = 0;
    double altura_parede_m = 2.80;
    double carga_linear_adicional_kN_m = 0.0;

    // Sobrecarga de construção
    double qc_construcao_kN_m2 = 1.00; // 1.0 kN/m² construtivo
};

struct ResultadoCargasAssistente {
    // Totais em kN/m²
    double g_concreto_kN_m2 = 0.0;
    double g_deck_kN_m2 = 0.0;
    double g_revestimento_kN_m2 = 0.0;
    double g_forro_kN_m2 = 0.0;
    double g_instalacoes_kN_m2 = 0.0;
    double g_contrapiso_kN_m2 = 0.0;
    double g2_area_total_kN_m2 = 0.0;

    double q_uso_kN_m2 = 0.0;
    double q_div_kN_m2 = 0.0;
    double q_total_area_kN_m2 = 0.0;

    // Totais lineares na viga (kN/m)
    double binf_m = 0.0;
    double g_perfil_linear_kN_m = 0.0;
    double g_parede_linear_kN_m = 0.0;

    double g1_linear_kN_m = 0.0; // Fase 1: (concreto + deck)*binf + g_perfil
    double q1_linear_kN_m = 0.0; // Fase 1: qc * binf
    double g2_linear_kN_m = 0.0; // Fase 2: g2_area * binf + g_parede + adicional
    double q2_linear_kN_m = 0.0; // Fase 2: q_total_area * binf

    // Fatores psi da ocupação
    char categoria_psi = 'C';
    double psi0 = 0.7;
    double psi1 = 0.6;
    double psi2 = 0.4;
};

inline ResultadoCargasAssistente calcular_cargas_assistente(
    const AssistenteCargasConfig& cfg,
    double binf_m,
    double peso_perfil_kg_m,
    bool eh_steel_deck,
    double peso_proprio_deck_laje_kN_m2,
    double consumo_concreto_m3_m2,
    double espessura_laje_macica_mm = 120.0,
    double gamma_concreto_kN_m3 = 25.0
) {
    ResultadoCargasAssistente res;
    res.binf_m = (binf_m > 0.0) ? binf_m : 1.0;

    // 1. Fase 1: Laje fresca + fôrma + perfil
    if (eh_steel_deck) {
        if (consumo_concreto_m3_m2 > 0.0) {
            res.g_concreto_kN_m2 = consumo_concreto_m3_m2 * gamma_concreto_kN_m3;
        } else {
            res.g_concreto_kN_m2 = (peso_proprio_deck_laje_kN_m2 > 0.0) ? (peso_proprio_deck_laje_kN_m2 - 0.10) : 2.50;
        }
        res.g_deck_kN_m2 = (peso_proprio_deck_laje_kN_m2 > res.g_concreto_kN_m2) ? (peso_proprio_deck_laje_kN_m2 - res.g_concreto_kN_m2) : 0.10;
    } else {
        res.g_concreto_kN_m2 = (espessura_laje_macica_mm / 1000.0) * gamma_concreto_kN_m3;
        res.g_deck_kN_m2 = 0.0;
    }

    res.g_perfil_linear_kN_m = (peso_perfil_kg_m * 9.80665) / 1000.0;
    res.g1_linear_kN_m = (res.g_concreto_kN_m2 + res.g_deck_kN_m2) * res.binf_m + res.g_perfil_linear_kN_m;
    res.q1_linear_kN_m = cfg.qc_construcao_kN_m2 * res.binf_m;

    // 2. Fase 2: Permanentes aplicadas pós-cura
    const auto& revs = obter_tabela_revestimentos();
    if (cfg.idx_revestimento >= 0 && cfg.idx_revestimento < (int)revs.size()) {
        res.g_revestimento_kN_m2 = revs[cfg.idx_revestimento].peso_kN_m2;
    }

    const auto& forros = obter_tabela_forros();
    if (cfg.idx_forro >= 0 && cfg.idx_forro < (int)forros.size()) {
        res.g_forro_kN_m2 = forros[cfg.idx_forro].peso_kN_m2;
    }

    const auto& insts = obter_tabela_instalacoes();
    if (cfg.idx_instalacao >= 0 && cfg.idx_instalacao < (int)insts.size()) {
        res.g_instalacoes_kN_m2 = insts[cfg.idx_instalacao].peso_kN_m2;
    }

    res.g_contrapiso_kN_m2 = cfg.contrapiso_espessura_cm * 0.20; // argamassa ~20 kN/m³ = 0.20 kN/m² por cm
    res.g2_area_total_kN_m2 = res.g_revestimento_kN_m2 + res.g_forro_kN_m2 + res.g_instalacoes_kN_m2 + res.g_contrapiso_kN_m2 + cfg.outras_permanentes_area_kN_m2;

    const auto& paredes = obter_tabela_paredes();
    if (cfg.idx_parede >= 0 && cfg.idx_parede < (int)paredes.size()) {
        res.g_parede_linear_kN_m = paredes[cfg.idx_parede].peso_kN_m2_parede * cfg.altura_parede_m;
    }

    res.g2_linear_kN_m = res.g2_area_total_kN_m2 * res.binf_m + res.g_parede_linear_kN_m + cfg.carga_linear_adicional_kN_m;

    // 3. Fase 2: Ações variáveis
    const auto& ocups = obter_tabela_ocupacoes();
    if (cfg.idx_ocupacao >= 0 && cfg.idx_ocupacao < (int)ocups.size()) {
        if (cfg.idx_ocupacao == (int)ocups.size() - 1) { // personalizado
            res.q_uso_kN_m2 = cfg.q_custom_kN_m2;
            res.categoria_psi = 'C';
        } else {
            res.q_uso_kN_m2 = ocups[cfg.idx_ocupacao].q_kN_m2;
            res.categoria_psi = ocups[cfg.idx_ocupacao].categoria_psi;
        }
    }

    const auto& divs = obter_tabela_divisorias();
    if (cfg.idx_divisoria >= 0 && cfg.idx_divisoria < (int)divs.size()) {
        res.q_div_kN_m2 = divs[cfg.idx_divisoria].q_adicional_kN_m2;
    }

    res.q_total_area_kN_m2 = res.q_uso_kN_m2 + res.q_div_kN_m2;
    res.q2_linear_kN_m = res.q_total_area_kN_m2 * res.binf_m;

    // Fatores psi da Tabela 2 NBR 8800:2024
    if (res.categoria_psi == 'R') {
        res.psi0 = 0.5; res.psi1 = 0.4; res.psi2 = 0.3;
    } else if (res.categoria_psi == 'A') {
        res.psi0 = 0.8; res.psi1 = 0.7; res.psi2 = 0.6;
    } else { // 'C'
        res.psi0 = 0.7; res.psi1 = 0.6; res.psi2 = 0.4;
    }

    return res;
}

} // namespace nbr6120
