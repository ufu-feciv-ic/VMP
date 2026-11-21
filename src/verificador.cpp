#include "verificador.h"
#include "geometria.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace {

// Calcula o índice de interação à flexo-compressão (NBR 8800:2008 item 5.5.1.2)
double calcularIndiceFlexoCompressao(double NSd_kN, double NRd_kN, double MSd_kNm, double MRd_kNm) {
    if (NRd_kN <= 0.0 || MRd_kNm <= 0.0) return 0.0;
    double razao_N = std::abs(NSd_kN) / NRd_kN;
    double razao_M = std::abs(MSd_kNm) / MRd_kNm;
    if (razao_N >= 0.2) {
        return razao_N + (8.0 / 9.0) * razao_M;
    } else {
        return (razao_N / 2.0) + razao_M;
    }
}

// Seleciona a força atuante no trecho correspondente à seção
double obterForcaNoTrecho(const ForcasProtensaoTrechos& forcas, TrechoCabo trecho) {
    switch (trecho) {
        case TRECHO_ATIVA:    return forcas.P_ativa_kN;
        case TRECHO_MEIO_VAO: return forcas.P_meio_kN;
        case TRECHO_PASSIVA:  return forcas.P_passiva_kN;
    }
    return forcas.P_meio_kN;
}

} // namespace

const ResultadoSecaoELU* ResultadoFaseELU::secaoApoioEsquerdo() const {
    if (secoes.empty()) return nullptr;
    return &secoes.front();
}

const ResultadoSecaoELU* ResultadoFaseELU::secaoMeioVao() const {
    if (secoes.empty()) return nullptr;
    const ResultadoSecaoELU* melhor = &secoes.front();
    double x_alvo = secoes.back().x_mm / 2.0;
    for (const auto& s : secoes) {
        if (std::abs(s.x_mm - x_alvo) < std::abs(melhor->x_mm - x_alvo)) {
            melhor = &s;
        }
    }
    return melhor;
}

const ResultadoSecaoELU* ResultadoFaseELU::secaoApoioDireito() const {
    if (secoes.empty()) return nullptr;
    return &secoes.back();
}

std::vector<SecaoTransversalViga> gerarSecoesViga(
    const VigaVMP& viga,
    [[maybe_unused]] const PerfilI& perfil,
    [[maybe_unused]] double ytr_t0_mm
) {
    int n_div = std::max(3, viga.num_secoes);
    std::vector<double> coords_x;
    coords_x.reserve(static_cast<size_t>(n_div + 1));

    for (int i = 0; i < n_div; ++i) {
        double x = viga.L * static_cast<double>(i) / static_cast<double>(n_div - 1);
        coords_x.push_back(x);
    }

    // Garante que o meio do vão (x = L/2) esteja sempre presente na malha de seções
    double x_meio = viga.L / 2.0;
    bool contem_meio = false;
    for (double x : coords_x) {
        if (std::abs(x - x_meio) < 1e-3) {
            contem_meio = true;
            break;
        }
    }
    if (!contem_meio && viga.L > 0.0) {
        coords_x.push_back(x_meio);
        std::sort(coords_x.begin(), coords_x.end());
    }

    std::vector<SecaoTransversalViga> secoes;
    secoes.reserve(coords_x.size());

    for (size_t i = 0; i < coords_x.size(); ++i) {
        double x = coords_x[i];
        SecaoTransversalViga sec;
        sec.indice = static_cast<int>(i);
        sec.x_mm = x;

        // Nome amigável da seção
        std::ostringstream oss;
        oss << "Seção " << i << " (x = " << std::fixed << std::setprecision(0) << x << " mm";
        if (std::abs(x) < 1e-3) {
            oss << " - Apoio Esq.";
        } else if (std::abs(x - x_meio) < 1e-3) {
            oss << " - Meio do Vão";
        } else if (std::abs(x - viga.L) < 1e-3) {
            oss << " - Apoio Dir.";
        }
        oss << ")";
        sec.nome = oss.str();

        // Classificação do trecho do cabo e posição vertical do cabo na seção x
        if (viga.cabo.tracado == CABO_RETO) {
            sec.ep_perfil_mm = viga.cabo.ep;
            if (x < x_meio - 1e-3) {
                sec.trecho = TRECHO_ATIVA;
            } else if (x > x_meio + 1e-3) {
                sec.trecho = (viga.cabo.ancoragem == ATIVA_PASSIVA) ? TRECHO_PASSIVA : TRECHO_ATIVA;
            } else {
                sec.trecho = TRECHO_MEIO_VAO;
            }
        } else if (viga.cabo.tracado == CABO_POLIGONAL) {
            // [STAND-BY]: Ponto de extensão para traçado poligonal.
            // Classifica os 3 trechos com base na posição do desviador X_ep_mm:
            double X_ep = (viga.cabo.X_ep_mm > 0.0) ? viga.cabo.X_ep_mm : (viga.L / 3.0);
            if (x <= X_ep + 1e-3) {
                sec.trecho = TRECHO_ATIVA;
            } else if (x >= (viga.L - X_ep) - 1e-3) {
                sec.trecho = (viga.cabo.ancoragem == ATIVA_PASSIVA) ? TRECHO_PASSIVA : TRECHO_ATIVA;
            } else {
                sec.trecho = TRECHO_MEIO_VAO;
            }
            // Enquanto o traçado poligonal está em stand-by, mantém ep constante:
            sec.ep_perfil_mm = viga.cabo.ep;
        } else {
            // [STAND-BY]: Ponto de extensão para traçado parabólico (CABO_PARABOLICO)
            sec.trecho = TRECHO_MEIO_VAO;
            sec.ep_perfil_mm = viga.cabo.ep;
        }

        secoes.push_back(sec);
    }

    return secoes;
}

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga) {
    RelatorioVMP rel;
    PerfilI perfil = viga.perfil;
    if (perfil.Ag <= 0.0 || perfil.Ix <= 0.0) {
        calcular_propriedades_secao_i(perfil);
    }

    CaboProtensao cabo = viga.cabo;
    double L_m = viga.L / 1000.0;

    // ========================================================================
    // 1. RESISTÊNCIAS BASE DO PERFIL DE AÇO (NBR 8800:2008 ou NBR 8800:2024)
    // ========================================================================
    const bool usar_2024 = (viga.norma == NBR8800_2024);

    // Rotina A: Compressão Axial do Perfil de Aço (NRd,I)
    rel.compressao_aco = usar_2024
        ? nbr8800::v2024::calcularNormalResistentePerfil(perfil, viga.aco, viga.Lb, viga.L, 1.0, 1.0)
        : nbr8800::v2008::calcularNormalResistentePerfil(perfil, viga.aco, viga.Lb, viga.L, 1.0, 1.0);

    // Rotina B: Momento Fletor Resistente do Perfil de Aço (MRd,I)
    rel.flexao_aco = usar_2024
        ? nbr8800::v2024::calcularMomentoResistentePerfil(perfil, viga.aco, viga.Lb, viga.Cb_fase1)
        : nbr8800::v2008::calcularMomentoResistentePerfil(perfil, viga.aco, viga.Lb, viga.Cb_fase1);

    // Rotina D: Força Cortante Resistente do Perfil de Aço (VRd)
    rel.cortante_aco = usar_2024
        ? nbr8800::v2024::calcularCortanteResistentePerfil(perfil, viga.aco, 0.0)
        : nbr8800::v2008::calcularCortanteResistentePerfil(perfil, viga.aco, 0.0);

    // ========================================================================
    // 2. SEÇÃO TRANSFORMADA (Rotina 1) & ESTIMATIVA DE P0 E ARMADURA (Rotinas 2 e 4)
    // ========================================================================
    // Seção transformada para curta duração (t = 0)
    rel.secao_t0 = nbr8800::calcularSecaoTransformada(
        perfil, viga.laje, viga.concreto, viga.aco, 1.0, cabo.ep, true
    );

    // Seção transformada para longa duração (t = infinito)
    rel.secao_t_inf = nbr8800::calcularSecaoTransformada(
        perfil, viga.laje, viga.concreto, viga.aco, 3.0, cabo.ep, false
    );

    // Momentos solicitantes característicos no meio do vão para estimativa de P0,max
    double q_perm_total = viga.cargas.q1_perfil + viga.cargas.q2_concreto + viga.cargas.q3_revestimento;
    double MSd_g_kNm = (q_perm_total * L_m * L_m) / 8.0;
    double MSd_q_kNm = (viga.cargas.q4_uso * L_m * L_m) / 8.0;

    // Rotina 2: Estimativa da força máxima de protensão inicial (P0,max)
    // (Internamente já diferencia PRE_TRACAO e POS_TRACAO)
    rel.estimativa_p0 = nbr8800::calcularEstimativaP0max(
        perfil, viga.aco, viga.concreto, cabo,
        rel.compressao_aco.NRd_kN, rel.flexao_aco.MRd_kNm,
        MSd_g_kNm, MSd_q_kNm,
        0.9, 1.1, viga.cargas.psi1,
        rel.secao_t_inf.ytr_mm, rel.secao_t_inf.Atr_mm2,
        rel.secao_t_inf.Wtr_inf_mm3, rel.secao_t_inf.Wtr_sup_mm3
    );

    // Força inicial efetiva adotada (P0)
    rel.P0_efetivo_kN = (viga.P0_adotado > 0.0) ? viga.P0_adotado : rel.estimativa_p0.P0_max_kN;

    // Rotina 4: Dimensionamento da armadura de protensão (Ap, n_cordoalhas)
    TipoProtensao tipo_prot = cabo.tipo;
    rel.armadura_protensao = nbr8800::calcularArmaduraProtensao(cabo, tipo_prot, rel.P0_efetivo_kN);
    if (cabo.Ap <= 0.0) {
        cabo.Ap = rel.armadura_protensao.Ap_real;
        cabo.n_cordoalhas = static_cast<int>(rel.armadura_protensao.n_cordoalhas);
    }

    // Gera a malha de X seções ao longo da viga [0, L]
    rel.malha_secoes = gerarSecoesViga(viga, perfil, rel.secao_t0.ytr_mm);

    // ========================================================================
    // 3. FASE 1 (ELU 1) — CONSTRUÇÃO
    //    - Pré-tração: Perfil I de aço protendido (q1 + q2 + qs + P1)
    //    - Pós-tração: Viga Mista sem protensão (q1 + q2 + qs, Rotina C)
    // ========================================================================
    rel.fase1_elu.numero_fase = 1;
    double q_sd_fase1 = 1.3 * (viga.cargas.q1_perfil + viga.cargas.q2_concreto)
                      + 1.2 * viga.cargas.qs_construcao;
    double VSd_max_fase1 = (q_sd_fase1 * L_m) / 2.0;
    rel.fase1_elu.VSd_max_kN = VSd_max_fase1;
    rel.fase1_elu.VRd_kN = rel.cortante_aco.VRd_kN;
    rel.VSd_fase1_kN = VSd_max_fase1;

    double dp_qs_fase1_kN = 0.0; // Guardado para ser removido na transição p/ Fase 2 na pré-tração

    if (cabo.tipo == PRE_TRACAO) {
        rel.fase1_elu.descricao = "ELU 1 - Construção (Pré-Tração: Perfil I Protendido)";
        double ep_I_meio = (perfil.d / 2.0) + cabo.ep;

        // Perdas imediatas na Fase 1
        ForcasProtensaoTrechos P1_base{};
        if (cabo.tracado == CABO_RETO) {
            auto perda_imed = nbr8800::calcularPerdaAncoragemTracadoReto(cabo, rel.P0_efetivo_kN, viga.L);
            P1_base = { perda_imed.P1_kN, perda_imed.P1_kN, perda_imed.P1_kN };
        } else {
            // [STAND-BY]: Traçado poligonal
            P1_base = nbr8800::calcularPerdaImediataTracadoPoligonal(cabo, rel.P0_efetivo_kN, viga.L, 0.0, ep_I_meio);
        }

        // Incrementos hiperestáticos no perfil de aço (q2 e qs)
        double dp_q2_f1 = 0.0;
        if (cabo.tracado == CABO_RETO) {
            dp_q2_f1 = nbr8800::calcularIncrementoDpTracadoReto(
                viga.cargas.q2_concreto, viga.aco.E_MPa, cabo.Ep,
                viga.L, ep_I_meio, perfil.Ix, perfil.Ag, cabo.Ap
            );
            dp_qs_fase1_kN = nbr8800::calcularIncrementoDpTracadoReto(
                viga.cargas.qs_construcao, viga.aco.E_MPa, cabo.Ep,
                viga.L, ep_I_meio, perfil.Ix, perfil.Ag, cabo.Ap
            );
        } else {
            // [STAND-BY]: Traçado poligonal
            dp_q2_f1 = nbr8800::calcularIncrementoDpTracadoPoligonal(
                viga.cargas.q2_concreto, viga.aco.E_MPa, cabo.Ep,
                viga.L, cabo.X_ep_mm, 0.0, ep_I_meio, perfil.Ix, perfil.Ag, cabo.Ap
            );
            dp_qs_fase1_kN = nbr8800::calcularIncrementoDpTracadoPoligonal(
                viga.cargas.qs_construcao, viga.aco.E_MPa, cabo.Ep,
                viga.L, cabo.X_ep_mm, 0.0, ep_I_meio, perfil.Ix, perfil.Ag, cabo.Ap
            );
        }

        double acressimo_f1 = dp_q2_f1 + dp_qs_fase1_kN;
        rel.fase1_elu.forcas_P = {
            P1_base.P_ativa_kN + acressimo_f1,
            P1_base.P_meio_kN + acressimo_f1,
            P1_base.P_passiva_kN + acressimo_f1
        };
        rel.P1_fase1_kN = rel.fase1_elu.forcas_P.P_meio_kN;

        // Verificação seção a seção na Fase 1 (Pré-tração)
        for (const auto& sec : rel.malha_secoes) {
            ResultadoSecaoELU res_sec;
            res_sec.indice_secao = sec.indice;
            res_sec.nome_secao = sec.nome;
            res_sec.x_mm = sec.x_mm;
            res_sec.ep_perfil_mm = sec.ep_perfil_mm;
            res_sec.ep_LNE_mm = (perfil.d / 2.0) + sec.ep_perfil_mm;
            res_sec.P_atuante_kN = obterForcaNoTrecho(rel.fase1_elu.forcas_P, sec.trecho);

            double x_m = sec.x_mm / 1000.0;
            double M_cargas_kNm = 0.5 * q_sd_fase1 * (L_m * x_m - x_m * x_m);
            double M_prot_kNm = 0.9 * res_sec.P_atuante_kN * (res_sec.ep_LNE_mm / 1000.0);

            res_sec.MSd_pos_kNm = std::max(0.0, M_cargas_kNm - M_prot_kNm);
            res_sec.MSd_neg_kNm = M_prot_kNm;
            res_sec.VSd_kN = q_sd_fase1 * std::abs((L_m / 2.0) - x_m);
            res_sec.VSd_max_kN = VSd_max_fase1;

            res_sec.MRd_pos_kNm = rel.flexao_aco.MRd_kNm;
            res_sec.MRd_neg_kNm = rel.flexao_aco.MRd_kNm;
            res_sec.VRd_kN = rel.cortante_aco.VRd_kN;
            res_sec.NRd_kN = rel.compressao_aco.NRd_kN;

            res_sec.indice_momento_pos = (res_sec.MRd_pos_kNm > 0.0) ? (res_sec.MSd_pos_kNm / res_sec.MRd_pos_kNm) : 0.0;
            res_sec.indice_momento_neg = (res_sec.MRd_neg_kNm > 0.0) ? (std::abs(res_sec.MSd_neg_kNm) / res_sec.MRd_neg_kNm) : 0.0;
            res_sec.indice_cortante = (res_sec.VRd_kN > 0.0) ? (res_sec.VSd_max_kN / res_sec.VRd_kN) : 0.0;

            res_sec.indice_flexo_compressao_pos = calcularIndiceFlexoCompressao(
                0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_pos_kNm, res_sec.MRd_pos_kNm
            );
            res_sec.indice_flexo_compressao_neg = calcularIndiceFlexoCompressao(
                0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_neg_kNm, res_sec.MRd_neg_kNm
            );

            bool ok_pos = (res_sec.MSd_pos_kNm <= res_sec.MRd_pos_kNm);
            bool ok_neg = (std::abs(res_sec.MSd_neg_kNm) <= res_sec.MRd_neg_kNm);
            bool ok_cort = (res_sec.VSd_max_kN <= res_sec.VRd_kN);
            res_sec.aprovado = ok_pos && ok_neg && ok_cort;

            if (!res_sec.aprovado) {
                rel.fase1_elu.aprovado_fase = false;
                rel.aprovado_global = false;
            }
            rel.fase1_elu.secoes.push_back(res_sec);
        }
    } else {
        // POS_TRACAO na Fase 1: Viga mista já consolidada, porém ainda sem protensão (P1 = 0)
        rel.fase1_elu.descricao = "ELU 1 - Construção (Pós-Tração: Viga Mista Sem Protensão)";
        rel.fase1_elu.forcas_P = { 0.0, 0.0, 0.0 };
        rel.P1_fase1_kN = 0.0;

        // Rotina C.1: Momento Positivo Resistente da Viga Mista sem protensão
        rel.momento_vm_sem_protensao = usar_2024
            ? nbr8800::v2024::calcularMomentoPositivoVM(perfil, viga.aco, viga.concreto, viga.laje)
            : nbr8800::v2008::calcularMomentoPositivoVM(perfil, viga.aco, viga.concreto, viga.laje);

        for (const auto& sec : rel.malha_secoes) {
            ResultadoSecaoELU res_sec;
            res_sec.indice_secao = sec.indice;
            res_sec.nome_secao = sec.nome;
            res_sec.x_mm = sec.x_mm;
            res_sec.ep_perfil_mm = sec.ep_perfil_mm;
            res_sec.ep_LNE_mm = 0.0;
            res_sec.P_atuante_kN = 0.0;

            double x_m = sec.x_mm / 1000.0;
            res_sec.MSd_pos_kNm = 0.5 * q_sd_fase1 * (L_m * x_m - x_m * x_m);
            res_sec.MSd_neg_kNm = 0.0;
            res_sec.VSd_kN = q_sd_fase1 * std::abs((L_m / 2.0) - x_m);
            res_sec.VSd_max_kN = VSd_max_fase1;

            res_sec.MRd_pos_kNm = rel.momento_vm_sem_protensao.MRd_VM_kNm;
            res_sec.MRd_neg_kNm = 0.0;
            res_sec.VRd_kN = rel.cortante_aco.VRd_kN;
            res_sec.NRd_kN = rel.compressao_aco.NRd_kN;

            res_sec.indice_momento_pos = (res_sec.MRd_pos_kNm > 0.0) ? (res_sec.MSd_pos_kNm / res_sec.MRd_pos_kNm) : 0.0;
            res_sec.indice_cortante = (res_sec.VRd_kN > 0.0) ? (res_sec.VSd_max_kN / res_sec.VRd_kN) : 0.0;

            bool ok_pos = rel.momento_vm_sem_protensao.aplicavel && (res_sec.MSd_pos_kNm <= res_sec.MRd_pos_kNm);
            bool ok_cort = (res_sec.VSd_max_kN <= res_sec.VRd_kN);
            res_sec.aprovado = ok_pos && ok_cort;

            if (!res_sec.aprovado) {
                rel.fase1_elu.aprovado_fase = false;
                rel.aprovado_global = false;
            }
            rel.fase1_elu.secoes.push_back(res_sec);
        }
    }

    if (const auto* sec_meio_f1 = rel.fase1_elu.secaoMeioVao()) {
        rel.MSd_fase1_kNm = sec_meio_f1->MSd_pos_kNm;
        rel.MSd_neg_fase1_kNm = sec_meio_f1->MSd_neg_kNm;
        rel.verificacoes.push_back({
            "ELU 1 - Flexão Positiva", "Momento solicitante positivo no meio do vão (Fase 1)",
            sec_meio_f1->MSd_pos_kNm, sec_meio_f1->MRd_pos_kNm, "kN.m",
            sec_meio_f1->MSd_pos_kNm <= sec_meio_f1->MRd_pos_kNm
        });
        if (cabo.tipo == PRE_TRACAO) {
            rel.verificacoes.push_back({
                "ELU 1 - Flexão Negativa", "Momento solicitante negativo da protensão (Fase 1)",
                std::abs(sec_meio_f1->MSd_neg_kNm), sec_meio_f1->MRd_neg_kNm, "kN.m",
                std::abs(sec_meio_f1->MSd_neg_kNm) <= sec_meio_f1->MRd_neg_kNm
            });
        }
        rel.verificacoes.push_back({
            "ELU 1 - Cortante", "Esforço cortante máximo na construção (Fase 1)",
            VSd_max_fase1, rel.cortante_aco.VRd_kN, "kN",
            VSd_max_fase1 <= rel.cortante_aco.VRd_kN
        });
    }

    // ========================================================================
    // 4. FASE 2 (ELU 2) — VIGA MISTA PROTENDIDA EM SERVIÇO (t = 0)
    // ========================================================================
    rel.fase2_elu.numero_fase = 2;
    rel.fase2_elu.descricao = "ELU 2 - Viga Mista Protendida em Curta Duração (t = 0)";

    double ep_tr0_meio = rel.secao_t0.ytr_mm + cabo.ep;

    // Incrementos hiperestáticos na seção mista (t = 0) devido a q3 (CPN) e q4 (CVN)
    double dp_cpn_f2 = 0.0;
    double dp_cvn_f2 = 0.0;
    if (cabo.tracado == CABO_RETO) {
        dp_cpn_f2 = nbr8800::calcularIncrementoDpTracadoReto(
            viga.cargas.q3_revestimento, viga.aco.E_MPa, cabo.Ep,
            viga.L, ep_tr0_meio, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, cabo.Ap
        );
        dp_cvn_f2 = nbr8800::calcularIncrementoDpTracadoReto(
            viga.cargas.q4_uso, viga.aco.E_MPa, cabo.Ep,
            viga.L, ep_tr0_meio, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, cabo.Ap
        );
    } else {
        // [STAND-BY]: Traçado poligonal
        dp_cpn_f2 = nbr8800::calcularIncrementoDpTracadoPoligonal(
            viga.cargas.q3_revestimento, viga.aco.E_MPa, cabo.Ep,
            viga.L, cabo.X_ep_mm, 0.0, ep_tr0_meio, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, cabo.Ap
        );
        dp_cvn_f2 = nbr8800::calcularIncrementoDpTracadoPoligonal(
            viga.cargas.q4_uso, viga.aco.E_MPa, cabo.Ep,
            viga.L, cabo.X_ep_mm, 0.0, ep_tr0_meio, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, cabo.Ap
        );
    }

    if (cabo.tipo == PRE_TRACAO) {
        // Na pré-tração, parte de P1, retira o incremento transitório de construção (qs) e soma CPN e CVN
        double delta_f2 = dp_cpn_f2 + dp_cvn_f2 - dp_qs_fase1_kN;
        rel.fase2_elu.forcas_P = {
            rel.fase1_elu.forcas_P.P_ativa_kN + delta_f2,
            rel.fase1_elu.forcas_P.P_meio_kN + delta_f2,
            rel.fase1_elu.forcas_P.P_passiva_kN + delta_f2
        };
    } else {
        // Na pós-tração, a protensão P0 é aplicada agora na seção mista (t = 0)
        ForcasProtensaoTrechos P2_base{};
        if (cabo.tracado == CABO_RETO) {
            auto perda_imed = nbr8800::calcularPerdaAncoragemTracadoReto(cabo, rel.P0_efetivo_kN, viga.L);
            P2_base = { perda_imed.P1_kN, perda_imed.P1_kN, perda_imed.P1_kN };
        } else {
            // [STAND-BY]: Traçado poligonal
            P2_base = nbr8800::calcularPerdaImediataTracadoPoligonal(cabo, rel.P0_efetivo_kN, viga.L, 0.0, ep_tr0_meio);
        }
        double delta_f2 = dp_cpn_f2 + dp_cvn_f2;
        rel.fase2_elu.forcas_P = {
            P2_base.P_ativa_kN + delta_f2,
            P2_base.P_meio_kN + delta_f2,
            P2_base.P_passiva_kN + delta_f2
        };
    }
    rel.P2_fase2_kN = rel.fase2_elu.forcas_P.P_meio_kN;

    double q_sd_fase2 = 1.4 * q_perm_total + 1.4 * viga.cargas.q4_uso;
    double MSd_pos_max_fase2 = (q_sd_fase2 * L_m * L_m) / 8.0;
    double VSd_max_fase2 = (q_sd_fase2 * L_m) / 2.0;
    rel.fase2_elu.VSd_max_kN = VSd_max_fase2;
    rel.fase2_elu.VRd_kN = rel.cortante_aco.VRd_kN;
    rel.VSd_fase2_kN = VSd_max_fase2;

    // Verificação seção a seção na Fase 2 (t = 0)
    for (const auto& sec : rel.malha_secoes) {
        ResultadoSecaoELU res_sec;
        res_sec.indice_secao = sec.indice;
        res_sec.nome_secao = sec.nome;
        res_sec.x_mm = sec.x_mm;
        res_sec.ep_perfil_mm = sec.ep_perfil_mm;
        res_sec.ep_LNE_mm = rel.secao_t0.ytr_mm + sec.ep_perfil_mm;
        res_sec.P_atuante_kN = obterForcaNoTrecho(rel.fase2_elu.forcas_P, sec.trecho);

        double x_m = sec.x_mm / 1000.0;
        res_sec.MSd_pos_kNm = 0.5 * q_sd_fase2 * (L_m * x_m - x_m * x_m);
        res_sec.MSd_neg_kNm = 0.9 * res_sec.P_atuante_kN * (res_sec.ep_LNE_mm / 1000.0);
        res_sec.VSd_kN = q_sd_fase2 * std::abs((L_m / 2.0) - x_m);
        res_sec.VSd_max_kN = VSd_max_fase2;

        CaboProtensao cabo_secao = cabo;
        cabo_secao.ep = sec.ep_perfil_mm;

        // Rotina H: Tensão de cálculo no cabo em ELU na seção (t = 0)
        res_sec.tensao_cabo = nbr8800::calcularTensaoCaboELU(
            perfil, viga.laje, cabo_secao, viga.concreto,
            res_sec.P_atuante_kN,
            rel.secao_t0.ytr_mm, rel.secao_t0.Atr_mm2, rel.secao_t0.Itr_mm4,
            viga.L
        );
        cabo_secao.sigma_Pd = res_sec.tensao_cabo.sigma_pd_MPa;

        // Rotina 3.1/3.2: Momento Resistente Positivo VMP na seção (interação completa pressuposta)
        // (Nota: calcularMomentoPostivoVMP recebe sigma_pd em kN/cm² = MPa * 0.1)
        res_sec.res_momento_pos = usar_2024
            ? nbr8800::v2024::calcularMomentoPostivoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                cabo_secao.Ap, res_sec.tensao_cabo.sigma_pd_MPa * 0.1
            )
            : nbr8800::v2008::calcularMomentoPostivoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                cabo_secao.Ap, res_sec.tensao_cabo.sigma_pd_MPa * 0.1
            );

        // Rotina 3.3: Momento Resistente Negativo VMP na seção
        res_sec.res_momento_neg = usar_2024
            ? nbr8800::v2024::calcularMomentoNegativoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                viga.armacao_laje,
                res_sec.MSd_neg_kNm, MSd_pos_max_fase2,
                viga.Lb, viga.a_vigas_mm, viga.alpha_viga
            )
            : nbr8800::v2008::calcularMomentoNegativoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                viga.armacao_laje,
                res_sec.MSd_neg_kNm, MSd_pos_max_fase2,
                viga.Lb, viga.a_vigas_mm, viga.alpha_viga
            );

        res_sec.MRd_pos_kNm = res_sec.res_momento_pos.MRd_VMP_kNm;
        res_sec.MRd_neg_kNm = res_sec.res_momento_neg.MRd_VMP_kNm;
        res_sec.VRd_kN = rel.cortante_aco.VRd_kN;
        res_sec.NRd_kN = rel.compressao_aco.NRd_kN;

        res_sec.indice_momento_pos = (res_sec.MRd_pos_kNm > 0.0) ? (res_sec.MSd_pos_kNm / res_sec.MRd_pos_kNm) : 0.0;
        res_sec.indice_momento_neg = (res_sec.MRd_neg_kNm > 0.0) ? (std::abs(res_sec.MSd_neg_kNm) / res_sec.MRd_neg_kNm) : 0.0;
        res_sec.indice_cortante = (res_sec.VRd_kN > 0.0) ? (res_sec.VSd_max_kN / res_sec.VRd_kN) : 0.0;

        res_sec.indice_flexo_compressao_pos = calcularIndiceFlexoCompressao(
            0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_pos_kNm, res_sec.MRd_pos_kNm
        );
        res_sec.indice_flexo_compressao_neg = calcularIndiceFlexoCompressao(
            0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_neg_kNm, res_sec.MRd_neg_kNm
        );

        bool ok_pos = res_sec.res_momento_pos.aplicavel && (res_sec.MSd_pos_kNm <= res_sec.MRd_pos_kNm);
        bool ok_neg = res_sec.res_momento_neg.aplicavel && (std::abs(res_sec.MSd_neg_kNm) <= res_sec.MRd_neg_kNm);
        bool ok_cort = (res_sec.VSd_max_kN <= res_sec.VRd_kN);
        res_sec.aprovado = ok_pos && ok_neg && ok_cort;

        if (!res_sec.aprovado) {
            rel.fase2_elu.aprovado_fase = false;
            rel.aprovado_global = false;
        }
        rel.fase2_elu.secoes.push_back(res_sec);
    }

    if (const auto* sec_meio_f2 = rel.fase2_elu.secaoMeioVao()) {
        rel.momento_vmp = sec_meio_f2->res_momento_pos;
        rel.MSd_fase2_kNm = sec_meio_f2->MSd_pos_kNm;
        rel.MSd_neg_fase2_kNm = sec_meio_f2->MSd_neg_kNm;
        rel.verificacoes.push_back({
            "ELU 2 - Flexão Positiva VMP", "Momento solicitante positivo no meio do vão (t = 0)",
            sec_meio_f2->MSd_pos_kNm, sec_meio_f2->MRd_pos_kNm, "kN.m",
            sec_meio_f2->MSd_pos_kNm <= sec_meio_f2->MRd_pos_kNm
        });
        rel.verificacoes.push_back({
            "ELU 2 - Flexão Negativa VMP", "Momento solicitante negativo da protensão (t = 0)",
            std::abs(sec_meio_f2->MSd_neg_kNm), sec_meio_f2->MRd_neg_kNm, "kN.m",
            std::abs(sec_meio_f2->MSd_neg_kNm) <= sec_meio_f2->MRd_neg_kNm
        });
        rel.verificacoes.push_back({
            "ELU 2 - Cortante", "Esforço cortante máximo em serviço (t = 0)",
            VSd_max_fase2, rel.cortante_aco.VRd_kN, "kN",
            VSd_max_fase2 <= rel.cortante_aco.VRd_kN
        });
    }

    // ========================================================================
    // 5. FASE 3 (ELU 3) — VIGA MISTA PROTENDIDA EM LONGA DURAÇÃO (t = infinito)
    // ========================================================================
    rel.fase3_elu.numero_fase = 3;
    rel.fase3_elu.descricao = "ELU 3 - Viga Mista Protendida em Longa Duração (t = infinito)";
    rel.fase3_elu.VSd_max_kN = VSd_max_fase2;
    rel.fase3_elu.VRd_kN = rel.cortante_aco.VRd_kN;
    rel.VSd_fase3_kN = VSd_max_fase2;

    // Rotina 5 (t = infinito): Perda progressiva por relaxação do aço em cada trecho
    auto relax_ativa   = nbr8800::calcularPerdaRelaxacaoTracadoReto(cabo, rel.fase2_elu.forcas_P.P_ativa_kN);
    auto relax_meio    = nbr8800::calcularPerdaRelaxacaoTracadoReto(cabo, rel.fase2_elu.forcas_P.P_meio_kN);
    auto relax_passiva = nbr8800::calcularPerdaRelaxacaoTracadoReto(cabo, rel.fase2_elu.forcas_P.P_passiva_kN);

    rel.fase3_elu.forcas_P = {
        relax_ativa.P3_kN,
        relax_meio.P3_kN,
        relax_passiva.P3_kN
    };
    rel.P3_fase3_kN = rel.fase3_elu.forcas_P.P_meio_kN;

    // Verificação seção a seção na Fase 3 (t = infinito)
    for (const auto& sec : rel.malha_secoes) {
        ResultadoSecaoELU res_sec;
        res_sec.indice_secao = sec.indice;
        res_sec.nome_secao = sec.nome;
        res_sec.x_mm = sec.x_mm;
        res_sec.ep_perfil_mm = sec.ep_perfil_mm;
        res_sec.ep_LNE_mm = rel.secao_t_inf.ytr_mm + sec.ep_perfil_mm;
        res_sec.P_atuante_kN = obterForcaNoTrecho(rel.fase3_elu.forcas_P, sec.trecho);

        double x_m = sec.x_mm / 1000.0;
        res_sec.MSd_pos_kNm = 0.5 * q_sd_fase2 * (L_m * x_m - x_m * x_m);
        res_sec.MSd_neg_kNm = 0.9 * res_sec.P_atuante_kN * (res_sec.ep_LNE_mm / 1000.0);
        res_sec.VSd_kN = q_sd_fase2 * std::abs((L_m / 2.0) - x_m);
        res_sec.VSd_max_kN = VSd_max_fase2;

        CaboProtensao cabo_secao = cabo;
        cabo_secao.ep = sec.ep_perfil_mm;

        // Rotina H: Tensão de cálculo no cabo em ELU na seção (t = infinito)
        res_sec.tensao_cabo = nbr8800::calcularTensaoCaboELU(
            perfil, viga.laje, cabo_secao, viga.concreto,
            res_sec.P_atuante_kN,
            rel.secao_t_inf.ytr_mm, rel.secao_t_inf.Atr_mm2, rel.secao_t_inf.Itr_mm4,
            viga.L
        );
        cabo_secao.sigma_Pd = res_sec.tensao_cabo.sigma_pd_MPa;

        // Rotina 3.1/3.2: Momento Resistente Positivo VMP na seção (t = infinito, interação completa pressuposta)
        res_sec.res_momento_pos = usar_2024
            ? nbr8800::v2024::calcularMomentoPostivoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                cabo_secao.Ap, res_sec.tensao_cabo.sigma_pd_MPa * 0.1
            )
            : nbr8800::v2008::calcularMomentoPostivoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                cabo_secao.Ap, res_sec.tensao_cabo.sigma_pd_MPa * 0.1
            );

        // Rotina 3.3: Momento Resistente Negativo VMP na seção (t = infinito)
        res_sec.res_momento_neg = usar_2024
            ? nbr8800::v2024::calcularMomentoNegativoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                viga.armacao_laje,
                res_sec.MSd_neg_kNm, MSd_pos_max_fase2,
                viga.Lb, viga.a_vigas_mm, viga.alpha_viga
            )
            : nbr8800::v2008::calcularMomentoNegativoVMP(
                perfil, viga.aco, viga.concreto, viga.laje, cabo_secao,
                viga.armacao_laje,
                res_sec.MSd_neg_kNm, MSd_pos_max_fase2,
                viga.Lb, viga.a_vigas_mm, viga.alpha_viga
            );

        res_sec.MRd_pos_kNm = res_sec.res_momento_pos.MRd_VMP_kNm;
        res_sec.MRd_neg_kNm = res_sec.res_momento_neg.MRd_VMP_kNm;
        res_sec.VRd_kN = rel.cortante_aco.VRd_kN;
        res_sec.NRd_kN = rel.compressao_aco.NRd_kN;

        res_sec.indice_momento_pos = (res_sec.MRd_pos_kNm > 0.0) ? (res_sec.MSd_pos_kNm / res_sec.MRd_pos_kNm) : 0.0;
        res_sec.indice_momento_neg = (res_sec.MRd_neg_kNm > 0.0) ? (std::abs(res_sec.MSd_neg_kNm) / res_sec.MRd_neg_kNm) : 0.0;
        res_sec.indice_cortante = (res_sec.VRd_kN > 0.0) ? (res_sec.VSd_max_kN / res_sec.VRd_kN) : 0.0;

        res_sec.indice_flexo_compressao_pos = calcularIndiceFlexoCompressao(
            0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_pos_kNm, res_sec.MRd_pos_kNm
        );
        res_sec.indice_flexo_compressao_neg = calcularIndiceFlexoCompressao(
            0.9 * res_sec.P_atuante_kN, res_sec.NRd_kN, res_sec.MSd_neg_kNm, res_sec.MRd_neg_kNm
        );

        bool ok_pos = res_sec.res_momento_pos.aplicavel && (res_sec.MSd_pos_kNm <= res_sec.MRd_pos_kNm);
        bool ok_neg = res_sec.res_momento_neg.aplicavel && (std::abs(res_sec.MSd_neg_kNm) <= res_sec.MRd_neg_kNm);
        bool ok_cort = (res_sec.VSd_max_kN <= res_sec.VRd_kN);
        res_sec.aprovado = ok_pos && ok_neg && ok_cort;

        if (!res_sec.aprovado) {
            rel.fase3_elu.aprovado_fase = false;
            rel.aprovado_global = false;
        }
        rel.fase3_elu.secoes.push_back(res_sec);
    }

    if (const auto* sec_meio_f3 = rel.fase3_elu.secaoMeioVao()) {
        rel.MSd_fase3_kNm = sec_meio_f3->MSd_pos_kNm;
        rel.MSd_neg_fase3_kNm = sec_meio_f3->MSd_neg_kNm;
        rel.verificacoes.push_back({
            "ELU 3 - Flexão Positiva VMP", "Momento solicitante positivo no meio do vão (t = inf)",
            sec_meio_f3->MSd_pos_kNm, sec_meio_f3->MRd_pos_kNm, "kN.m",
            sec_meio_f3->MSd_pos_kNm <= sec_meio_f3->MRd_pos_kNm
        });
        rel.verificacoes.push_back({
            "ELU 3 - Flexão Negativa VMP", "Momento solicitante negativo da protensão (t = inf)",
            std::abs(sec_meio_f3->MSd_neg_kNm), sec_meio_f3->MRd_neg_kNm, "kN.m",
            std::abs(sec_meio_f3->MSd_neg_kNm) <= sec_meio_f3->MRd_neg_kNm
        });
        rel.verificacoes.push_back({
            "ELU 3 - Cortante", "Esforço cortante máximo em longa duração (t = inf)",
            VSd_max_fase2, rel.cortante_aco.VRd_kN, "kN",
            VSd_max_fase2 <= rel.cortante_aco.VRd_kN
        });
    }

    return rel;
}
