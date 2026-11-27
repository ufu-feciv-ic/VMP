#include "verificador.h"
#include "geometria.h"
#include <cmath>
#include <algorithm>

nbr8800::ResultadoRotinaE calcularConectoresVigaMista(const VigaMistaConfig& cfg, double bef_mm) {
    Laje laje_calc = cfg.laje;
    laje_calc.bef_mm = bef_mm;

    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        return nbr8800::v2024::calcularConectoresCisalhamento(
            cfg.perfil, cfg.aco, laje_calc, cfg.concreto, cfg.conector,
            cfg.geometria.L_vao_mm, cfg.geometria.escorada
        );
    } else {
        return nbr8800::v2008::calcularConectoresCisalhamento(
            cfg.perfil, cfg.aco, laje_calc, cfg.concreto, cfg.conector,
            cfg.geometria.L_vao_mm, cfg.geometria.escorada
        );
    }
}

double calcularLarguraInfluencia(const GeometriaVigaMista& geo) {
    if (geo.eh_viga_borda) {
        double d_prox = (geo.dist_esq_mm > 0.0) ? geo.dist_esq_mm : geo.dist_dir_mm;
        return d_prox / 2.0 + geo.dist_borda_mm;
    }
    return (geo.dist_esq_mm + geo.dist_dir_mm) / 2.0;
}

RelatorioVigaMista verificar_viga_mista(const VigaMistaConfig& cfg) {
    RelatorioVigaMista rel;
    rel.aprovado_global = true;

    PerfilI perfil = cfg.perfil;
    calcular_propriedades_secao_i(perfil);

    double bef_mm = (cfg.laje.bef_mm > 0.0)
        ? cfg.laje.bef_mm
        : nbr8800::calcularLarguraEfetiva(cfg.geometria);
    double L_m = cfg.geometria.L_vao_mm / 1000.0;

    Laje laje_calc = cfg.laje;
    laje_calc.bef_mm = bef_mm;

    // ========================================================================
    // 1. FASE 1: PERFIL DE AÇO ISOLADO (Reutiliza Rotinas B e D de rotinas_nbr8800)
    // ========================================================================
    rel.fase1.g1_kN_m = cfg.acoes.g1_kN_m;
    rel.fase1.q1_kN_m = cfg.acoes.q1_kN_m;
    rel.fase1.qSd1_kN_m = cfg.acoes.gamma_g1 * rel.fase1.g1_kN_m + cfg.acoes.gamma_q1 * rel.fase1.q1_kN_m;
    rel.fase1.MSd1_kNm = (rel.fase1.qSd1_kN_m * L_m * L_m) / 8.0;
    rel.fase1.VSd1_kN = (rel.fase1.qSd1_kN_m * L_m) / 2.0;

    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        rel.fase1.flexao_aco = nbr8800::v2024::calcularMomentoResistentePerfil(
            perfil, cfg.aco, cfg.geometria.Lb_mm, cfg.geometria.Cb
        );
        rel.fase1.cortante_aco = nbr8800::v2024::calcularCortanteResistentePerfil(perfil, cfg.aco);
    } else {
        rel.fase1.flexao_aco = nbr8800::v2008::calcularMomentoResistentePerfil(
            perfil, cfg.aco, cfg.geometria.Lb_mm, cfg.geometria.Cb
        );
        rel.fase1.cortante_aco = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, cfg.aco);
    }

    rel.fase1.taxa_flexao = rel.fase1.MSd1_kNm / std::max(1e-3, rel.fase1.flexao_aco.MRd_kNm);
    rel.fase1.taxa_cortante = rel.fase1.VSd1_kN / std::max(1e-3, rel.fase1.cortante_aco.VRd_kN);
    rel.fase1.aprovado_flexao = (rel.fase1.taxa_flexao <= 1.00) && !rel.fase1.flexao_aco.perfil_esbelto;
    rel.fase1.aprovado_cortante = (rel.fase1.taxa_cortante <= 1.00);

    // ========================================================================
    // 2. CONECTORES DE CISALHAMENTO (Reutiliza Rotina E de rotinas_nbr8800)
    // ========================================================================
    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        rel.conectores = nbr8800::v2024::calcularConectoresCisalhamento(
            perfil, cfg.aco, laje_calc, cfg.concreto, cfg.conector,
            cfg.geometria.L_vao_mm, cfg.geometria.escorada
        );
    } else {
        rel.conectores = nbr8800::v2008::calcularConectoresCisalhamento(
            perfil, cfg.aco, laje_calc, cfg.concreto, cfg.conector,
            cfg.geometria.L_vao_mm, cfg.geometria.escorada
        );
    }

    // ========================================================================
    // 3. FASE 2: MOMENTO E CORTANTE DA VIGA MISTA (Reutiliza Rotinas C e D)
    // ========================================================================
    rel.fase2.bef_mm = bef_mm;
    rel.fase2.g2_kN_m = cfg.acoes.g2_kN_m;
    rel.fase2.q2_kN_m = cfg.acoes.q2_kN_m;
    rel.fase2.qSd2_kN_m = cfg.acoes.gamma_g1_fase2 * rel.fase1.g1_kN_m
        + cfg.acoes.gamma_g2 * rel.fase2.g2_kN_m
        + cfg.acoes.gamma_q2 * rel.fase2.q2_kN_m;
    rel.fase2.MSd2_kNm = (rel.fase2.qSd2_kN_m * L_m * L_m) / 8.0;
    rel.fase2.VSd2_kN = (rel.fase2.qSd2_kN_m * L_m) / 2.0;

    double Fhd_conectores_kN = rel.conectores.grau_interacao_eta * rel.conectores.Fhd_kN;

    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        rel.fase2.momento_misto = nbr8800::v2024::calcularMomentoPositivoVM(
            perfil, cfg.aco, cfg.concreto, laje_calc, 1.10, 1.40, Fhd_conectores_kN
        );
        rel.fase2.cortante_misto = rel.fase1.cortante_aco;

        // Refina o grau de interação mínimo da NBR 8800:2024 (L.2.3.1.1.2) com cr = max(0.7, MSd2 / MRd)
        rel.conectores.grau_interacao_minimo = nbr8800::v2024::calcularGrauInteracaoMinimo(
            perfil, cfg.aco, cfg.geometria.L_vao_mm, cfg.geometria.escorada,
            rel.fase2.MSd2_kNm, rel.fase2.momento_misto.MRd_VM_kNm
        );
        rel.conectores.n_semi_vao_minimo = static_cast<int>(std::ceil(rel.conectores.grau_interacao_minimo * rel.conectores.Fhd_kN / std::max(1e-3, rel.conectores.QRd_kN)));
        rel.conectores.n_total_minimo = 2 * rel.conectores.n_semi_vao_minimo;
        rel.conectores.grau_interacao_atende = (rel.conectores.grau_interacao_eta >= rel.conectores.grau_interacao_minimo - 1e-5);
    } else {
        rel.fase2.momento_misto = nbr8800::v2008::calcularMomentoPositivoVM(
            perfil, cfg.aco, cfg.concreto, laje_calc, 1.10, 1.40, Fhd_conectores_kN
        );
        rel.fase2.cortante_misto = rel.fase1.cortante_aco;
        rel.conectores.n_semi_vao_minimo = static_cast<int>(std::ceil(rel.conectores.grau_interacao_minimo * rel.conectores.Fhd_kN / std::max(1e-3, rel.conectores.QRd_kN)));
        rel.conectores.n_total_minimo = 2 * rel.conectores.n_semi_vao_minimo;
        rel.conectores.grau_interacao_atende = (rel.conectores.grau_interacao_eta >= rel.conectores.grau_interacao_minimo - 1e-5);
    }

    rel.fase2.taxa_flexao = rel.fase2.MSd2_kNm / std::max(1e-3, rel.fase2.momento_misto.MRd_VM_kNm);
    rel.fase2.taxa_cortante = rel.fase2.VSd2_kN / std::max(1e-3, rel.fase2.cortante_misto.VRd_kN);
    rel.fase2.aprovado_flexao = (rel.fase2.taxa_flexao <= 1.00) && rel.fase2.momento_misto.aplicavel;
    rel.fase2.aprovado_cortante = (rel.fase2.taxa_cortante <= 1.00);

    // ========================================================================
    // 4. ARMADURA DE COSTURA (Reutiliza Rotina F de rotinas_nbr8800)
    // ========================================================================
    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        rel.costura = nbr8800::v2024::calcular_rotina_f_armadura_costura(
            laje_calc, cfg.concreto, Fhd_conectores_kN, cfg.geometria.L_vao_mm
        );
    } else {
        rel.costura = nbr8800::v2008::calcular_rotina_f_armadura_costura(
            laje_calc, cfg.concreto, Fhd_conectores_kN, cfg.geometria.L_vao_mm
        );
    }

    // ========================================================================
    // 5. ELS: FLECHAS E TENSÃO EM SERVIÇO (Reutiliza calcularSecaoTransformada)
    // ========================================================================
    if (cfg.norma == NormaEdicao::NBR8800_2024) {
        rel.flechas = nbr8800::v2024::calcularFlechasVM(
            perfil, cfg.aco, cfg.concreto, laje_calc, cfg.geometria.L_vao_mm,
            cfg.acoes, rel.conectores.grau_interacao_eta, cfg.geometria.escorada
        );
    } else {
        rel.flechas = nbr8800::v2008::calcularFlechasVM(
            perfil, cfg.aco, cfg.concreto, laje_calc, cfg.geometria.L_vao_mm,
            cfg.acoes, rel.conectores.grau_interacao_eta, cfg.geometria.escorada
        );
    }

    // ========================================================================
    // 5b. ELS: VIBRAÇÃO DO PISO (2024: Anexo I, modo A | 2008: L.3.2)
    // ========================================================================
    rel.b_influencia_mm = calcularLarguraInfluencia(cfg.geometria);
    if (cfg.vibracao.verificar) {
        rel.vibracao = nbr8800::calcularVibracaoPisoAnexoI(
            perfil, cfg.aco, laje_calc, cfg.geometria.L_vao_mm, rel.b_influencia_mm,
            rel.fase1.g1_kN_m, rel.fase2.g2_kN_m, rel.fase2.q2_kN_m, cfg.vibracao
        );
    }

    // ========================================================================
    // 6. MONTAGEM DO QUADRO DE DIAGNÓSTICO CONSOLIDADO
    // ========================================================================
    rel.itens.push_back({
        "Flexão Fase 1 (Perfil Isolado)",
        "Flambagem Lateral com Torção e Flambagens Locais (Rotina B)",
        rel.fase1.MSd1_kNm,
        rel.fase1.flexao_aco.MRd_kNm,
        "kN.m",
        rel.fase1.taxa_flexao,
        rel.fase1.aprovado_flexao
    });
    rel.itens.push_back({
        "Cortante Fase 1 (Perfil Isolado)",
        "Resistência da Alma ao Cortante (Rotina D)",
        rel.fase1.VSd1_kN,
        rel.fase1.cortante_aco.VRd_kN,
        "kN",
        rel.fase1.taxa_cortante,
        rel.fase1.aprovado_cortante
    });

    bool conn_geom_vert_ok = rel.conectores.altura_atende && rel.conectores.cobrimento_atende && rel.conectores.diametro_mesa_atende;
    bool conn_geom_transv_ok = rel.conectores.geometria_transversal_atende;

    rel.itens.push_back({
        "Conectores - Grau de Interação",
        "Grau de interação eta adotado vs mínimo normativo exigido",
        rel.conectores.grau_interacao_minimo,
        rel.conectores.grau_interacao_eta,
        "-",
        rel.conectores.grau_interacao_minimo / std::max(0.001, rel.conectores.grau_interacao_eta),
        rel.conectores.grau_interacao_atende
    });
    rel.itens.push_back({
        "Conectores - Espaçamento Longitudinal",
        "Espaçamento longitudinal adotado vs limites normativos (s_min a s_max)",
        rel.conectores.el_espacamento_mm,
        rel.conectores.s_max_long_mm,
        "mm",
        rel.conectores.el_espacamento_mm / std::max(1.0, rel.conectores.s_max_long_mm),
        rel.conectores.espacamento_atende
    });
    rel.itens.push_back({
        "Conectores - Altura e Cobrimento",
        "Altura mínima exigida e cobrimento superior da cabeça",
        rel.conectores.h_min_normativo_mm,
        cfg.conector.h,
        "mm",
        rel.conectores.h_min_normativo_mm / std::max(1.0, cfg.conector.h),
        conn_geom_vert_ok
    });
    rel.itens.push_back({
        "Conectores - Geometria Transversal",
        (cfg.conector.tipo == CONECTOR_PERFIL_U)
            ? "Largura da mesa do perfil vs comprimento do conector U"
            : (cfg.laje.tipo == LAJE_STEEL_DECK && cfg.laje.nervura_paralela)
                ? "Largura da mesa e calha da fôrma vs espaçamento transversal (st >= 4d)"
                : "Largura da mesa do perfil vs espaçamento transversal de pinos (st >= 4d)",
        rel.conectores.bf_min_necessario_mm,
        cfg.perfil.bf,
        "mm",
        rel.conectores.bf_min_necessario_mm / std::max(1.0, cfg.perfil.bf),
        conn_geom_transv_ok
    });

    rel.itens.push_back({
        "Flexão Fase 2 (Viga Mista)",
        "Momento Resistente da Seção Mista (Rotina C)",
        rel.fase2.MSd2_kNm,
        rel.fase2.momento_misto.MRd_VM_kNm,
        "kN.m",
        rel.fase2.taxa_flexao,
        rel.fase2.aprovado_flexao
    });
    rel.itens.push_back({
        "Cortante Fase 2 (Viga Mista)",
        "Força cortante resistente do perfil de aço (ELU, Rotina D)",
        rel.fase2.VSd2_kN,
        rel.fase2.cortante_misto.VRd_kN,
        "kN",
        rel.fase2.taxa_cortante,
        rel.fase2.aprovado_cortante
    });

    rel.itens.push_back({
        "ELS Tensão Mesa Inferior",
        "Superposição elástica de tensões em serviço (Rotina 1)",
        rel.flechas.tensao_mesa_inferior_MPa,
        rel.flechas.limite_tensao_MPa,
        "MPa",
        rel.flechas.tensao_mesa_inferior_MPa / std::max(1.0, rel.flechas.limite_tensao_MPa),
        rel.flechas.tensao_servico_atende
    });

    rel.itens.push_back({
        "ELS Flecha Máxima",
        "δmax = δ1 + δ2 + δ3 − δ0 (aspecto visual, Tabela B.1)",
        rel.flechas.flecha_total_mm,
        rel.flechas.limite_total_mm,
        "mm",
        rel.flechas.taxa_flecha_total,
        rel.flechas.aprovado_flecha
    });
    rel.itens.push_back({
        "ELS Flecha Pós-Cura",
        "δ2 + δ3 (danos a elementos não estruturais, Tabela B.1)",
        rel.flechas.flecha_dano_mm,
        rel.flechas.limite_dano_mm,
        "mm",
        rel.flechas.taxa_flecha_dano,
        rel.flechas.aprovado_flecha_dano
    });

    if (cfg.vibracao.verificar) {
        if (cfg.norma == NormaEdicao::NBR8800_2024) {
            const bool por_ap = (cfg.vibracao.criterio == CriterioVibracao::ACELERACAO_PICO);
            rel.itens.push_back({
                "ELS Vibração do Piso",
                por_ap ? "Aceleração de pico a_p (Anexo I, modo A)" : "Fator de resposta R (Anexo I, modo A)",
                por_ap ? rel.vibracao.ap_m_s2 : rel.vibracao.R,
                por_ap ? rel.vibracao.ap_lim_m_s2 : rel.vibracao.R_max,
                por_ap ? "m/s²" : "-",
                rel.vibracao.utilizacao,
                rel.vibracao.aprovado
            });
        } else {
            const double lim = 20.0;
            rel.itens.push_back({
                "ELS Vibração do Piso",
                "Deslocamento frequente sem parcela diferida <= 20 mm (L.3.2)",
                rel.flechas.flecha_vibracao_2008_mm,
                lim,
                "mm",
                rel.flechas.flecha_vibracao_2008_mm / lim,
                rel.flechas.flecha_vibracao_2008_mm <= lim
            });
        }
    }

    for (const auto& item : rel.itens) {
        if (!item.aprovado) {
            rel.aprovado_global = false;
            break;
        }
    }

    return rel;
}

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga) {
    RelatorioVMP rel;
    PerfilI perfil = viga.perfil;
    calcular_propriedades_secao_i(perfil);

    rel.compressao_aco = nbr8800::v2008::calcularNormalResistentePerfil(perfil, viga.aco, viga.Lb, viga.L, 1.0, 1.0);
    rel.flexao_aco = nbr8800::v2008::calcularMomentoResistentePerfil(perfil, viga.aco, viga.Lb);
    rel.cortante_aco = nbr8800::v2008::calcularCortanteResistentePerfil(perfil, viga.aco);

    return rel;
}
