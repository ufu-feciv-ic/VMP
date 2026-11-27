// ============================================================================
// Gerador dos dados numéricos do TCC
// Executa o motor de cálculo sobre o estudo de caso V1 e sobre estudos
// paramétricos, escrevendo docs/DADOS_NUMERICOS_E_TABELAS_TCC.md.
// Todo número citado no texto do TCC deve sair deste programa.
// ============================================================================
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "estudo_caso.h"
#include "verificador.h"

namespace {

// Número com vírgula decimal (padrão ABNT)
std::string br(double x, int casas) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*f", casas, x);
    std::string s(buf);
    for (auto& c : s) if (c == '.') c = ',';
    return s;
}

std::string pct(double x, int casas = 1) { return br(100.0 * x, casas) + "%"; }
std::string ok(bool b) { return b ? "Atende" : "**Não atende**"; }

void tabela_caso(std::ostream& out, const RelatorioVigaMista& r) {
    out << "| Grandeza | Símbolo | Valor | Unidade |\n";
    out << "| :--- | :---: | ---: | :---: |\n";
    auto linha = [&](const char* g, const char* s, const std::string& v, const char* u) {
        out << "| " << g << " | " << s << " | " << v << " | " << u << " |\n";
    };
    linha("Largura efetiva", "$b_{ef}$", br(r.fase2.bef_mm, 0), "mm");
    linha("Momento solicitante, construção", "$M_{Sd,c}$", br(r.fase1.MSd1_kNm, 2), "kN·m");
    linha("Momento resistente do perfil isolado", "$M_{Rd,a}$", br(r.fase1.flexao_aco.MRd_kNm, 2), "kN·m");
    linha("Resistência de um conector", "$Q_{Rd}$", br(r.conectores.QRd_kN, 2), "kN");
    linha("Coeficientes de grupo e posição", "$R_g \\cdot R_p$", br(r.conectores.Rg * r.conectores.Rp, 2), "—");
    linha("Força de cisalhamento de cálculo", "$F_{hd}$", br(r.conectores.Fhd_kN, 2), "kN");
    linha("Espaçamento adotado", "$s$", br(r.conectores.el_espacamento_mm, 0), "mm");
    linha("Conectores por meio vão (adotados)", "$n$", std::to_string(r.conectores.n_semi_vao_adotado), "un");
    linha("Conectores por meio vão p/ interação completa", "$n_{compl}$", std::to_string(r.conectores.n_semi_vao_calculado), "un");
    linha("Grau de interação", "$\\eta$", br(r.conectores.grau_interacao_eta, 3), "—");
    linha("Grau de interação mínimo", "$\\eta_i$", br(r.conectores.grau_interacao_minimo, 3), "—");
    linha("Conectores mínimos por meio vão", "$n_{min}$", std::to_string(r.conectores.n_semi_vao_minimo), "un");
    linha("Momento solicitante de cálculo", "$M_{Sd}$", br(r.fase2.MSd2_kNm, 2), "kN·m");
    linha("Compressão no perfil", "$C_{ad}$", br(r.fase2.momento_misto.C_aco_kN, 2), "kN");
    linha("Posição da LNP a partir do topo do perfil", "$y_p$", br(r.fase2.momento_misto.y_lnp_mm, 2), "mm");
    linha("Momento resistente da viga mista", "$M_{Rd}$", br(r.fase2.momento_misto.MRd_VM_kNm, 2), "kN·m");
    linha("Força cortante solicitante", "$V_{Sd}$", br(r.fase2.VSd2_kN, 2), "kN");
    linha("Força cortante resistente", "$V_{Rd}$", br(r.fase2.cortante_misto.VRd_kN, 2), "kN");
    linha("Inércia homogeneizada, curta duração", "$I_{tr}$", br(r.flechas.Itr_cd_cm4, 0), "cm⁴");
    linha("Inércia homogeneizada, longa duração", "$I_{tr,\\infty}$", br(r.flechas.Itr_ld_cm4, 0), "cm⁴");
    linha("Inércia efetiva, curta duração", "$I_{ef}$", br(r.flechas.Ief_cd_cm4, 0), "cm⁴");
    linha("Inércia efetiva, longa duração", "$I_{ef,\\infty}$", br(r.flechas.Ief_ld_cm4, 0), "cm⁴");
    linha("Flecha de $g_1$ no perfil isolado", "$\\delta_{g1}$", br(r.flechas.flecha_g1_mm, 2), "mm");
    linha("Flecha das permanentes (curta duração)", "$\\delta_1$", br(r.flechas.flecha_perm_curta_mm, 2), "mm");
    linha("Acréscimo por fluência", "$\\delta_{fl}$", br(r.flechas.flecha_fluencia_mm, 2), "mm");
    linha("Flecha por retração", "$\\delta_{sh}$", br(r.flechas.flecha_retracao_mm, 2), "mm");
    linha("Flecha das variáveis (quase permanente)", "$\\delta_3$", br(r.flechas.flecha_var_max_mm, 2), "mm");
    linha("Contraflecha considerada", "$\\delta_0$", br(r.flechas.contraflecha_mm, 2), "mm");
    linha("Flecha máxima", "$\\delta_{max}$", br(r.flechas.flecha_total_mm, 2), "mm");
    linha("Limite $L/350$", "$\\delta_{lim}$", br(r.flechas.limite_total_mm, 2), "mm");
    linha("Flecha pós-cura (combinação rara)", "$\\delta_2+\\delta_3$", br(r.flechas.flecha_dano_mm, 2), "mm");
    linha("Tensão na face inferior em serviço", "$\\sigma$", br(r.flechas.tensao_mesa_inferior_MPa, 1), "MPa");
    if (r.vibracao.calculado) {
        const auto& v = r.vibracao;
        linha("Inércia dinâmica da viga mista", "$I_b$", br(v.Ib_mm4 / 1e4, 0), "cm⁴");
        linha("Massa distribuída do piso", "$m$", br(v.m_kg_m2, 1), "kg/m²");
        linha("Deslocamento do piso (modo A)", "$\\delta$", br(v.delta_mm, 2), "mm");
        linha("Frequência fundamental", "$f_0$", br(v.f0_Hz, 2), "Hz");
        linha("Largura colaborante do piso", "$B_b$", br(v.Bb_m, 2), "m");
        linha("Massa modal", "$M$", br(v.M_kg, 0), "kg");
        linha("Razão de amortecimento", "$\\beta$", br(v.beta, 3), "—");
        linha("Aceleração de pico", "$a_p$", br(v.ap_m_s2, 4), "m/s²");
        linha("Valor eficaz ponderado", "$a_{w,rms}$", br(v.aw_rms_m_s2, 4), "m/s²");
        linha("Fator de resposta", "$R$", br(v.R, 2), "—");
    }
}

void quadro_verificacoes(std::ostream& out, const RelatorioVigaMista& r) {
    out << "| Verificação | Solicitante | Resistente / limite | Utilização | Situação |\n";
    out << "| :--- | ---: | ---: | ---: | :---: |\n";
    for (const auto& it : r.itens) {
        out << "| " << it.item << " | " << br(it.solicitante, 3) << " " << it.unidade
            << " | " << br(it.resistente, 3) << " " << it.unidade
            << " | " << pct(it.taxa_utilizacao) << " | " << ok(it.aprovado) << " |\n";
    }
    out << "\nResultado global: **" << (r.aprovado_global ? "aprovado" : "não aprovado") << "**.\n";
}

} // namespace

int main(int argc, char** argv) {
    const std::string caminho = (argc > 1) ? argv[1] : "docs/DADOS_NUMERICOS_E_TABELAS_TCC.md";
    std::ofstream out(caminho);
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir " << caminho << "\n";
        return 1;
    }

    out << "# Dados numéricos e tabelas do TCC\n\n";
    out << "> Arquivo gerado automaticamente por `vmp_tabelas` (src/gerar_tabelas_tcc.cpp). "
           "Não edite à mão: altere o código ou o estudo de caso e gere novamente.\n\n";

    // ------------------------------------------------------------------------
    // 1. Estudo de caso V1 nas duas edições
    // ------------------------------------------------------------------------
    auto r24 = verificar_viga_mista(estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2024));
    auto r08 = verificar_viga_mista(estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008));

    out << "## 1. Estudo de caso V1 — ABNT NBR 8800:2024\n\n";
    out << "Dados: W460×52,0 (A572 Gr. 50), steel deck MF-75 (h = 140 mm, t = 0,80 mm) com nervuras "
           "perpendiculares, C30, vão de 9,0 m, vigas a cada 2,8 m, pinos Ø19 × 125 mm (fu = 415 MPa) "
           "em todas as nervuras, construção não escorada, contraflecha de 15 mm.\n\n";
    out << "Ações lineares: g1 = " << br(estudo_caso_v1::g1_kN_m, 3) << " kN/m; q1 = "
        << br(estudo_caso_v1::q1_kN_m, 2) << " kN/m; g2 = " << br(estudo_caso_v1::g2_kN_m, 2)
        << " kN/m; q2 = " << br(estudo_caso_v1::q2_kN_m, 2) << " kN/m.\n\n";
    out << "### 1.1 Grandezas intermediárias\n\n";
    tabela_caso(out, r24);
    out << "\n### 1.2 Quadro de verificações\n\n";
    quadro_verificacoes(out, r24);

    // ------------------------------------------------------------------------
    // 2. Comparação 2008 x 2024 no mesmo caso
    // ------------------------------------------------------------------------
    out << "\n## 2. Mesmo caso nas duas edições da norma\n\n";
    out << "| Grandeza | NBR 8800:2008 | NBR 8800:2024 | Variação |\n";
    out << "| :--- | ---: | ---: | ---: |\n";
    auto cmp = [&](const char* g, double a, double b, int c) {
        double var = (std::abs(a) > 1e-12) ? (b - a) / a : 0.0;
        out << "| " << g << " | " << br(a, c) << " | " << br(b, c) << " | " << pct(var) << " |\n";
    };
    auto cfg08 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008);
    auto cfg24 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2024);
    cmp("Módulo do concreto $E_c$ (MPa)", cfg08.concreto.Ecs_MPa, cfg24.concreto.Ecs_MPa, 0);
    cmp("$Q_{Rd}$ (kN)", r08.conectores.QRd_kN, r24.conectores.QRd_kN, 2);
    cmp("Grau de interação $\\eta$", r08.conectores.grau_interacao_eta, r24.conectores.grau_interacao_eta, 3);
    cmp("Grau mínimo $\\eta_i$", r08.conectores.grau_interacao_minimo, r24.conectores.grau_interacao_minimo, 3);
    cmp("Conectores mínimos por meio vão", r08.conectores.n_semi_vao_minimo, r24.conectores.n_semi_vao_minimo, 0);
    cmp("$M_{Rd}$ da viga mista (kN·m)", r08.fase2.momento_misto.MRd_VM_kNm, r24.fase2.momento_misto.MRd_VM_kNm, 2);
    cmp("$V_{Rd}$ (kN)", r08.fase2.cortante_misto.VRd_kN, r24.fase2.cortante_misto.VRd_kN, 2);
    cmp("$I_{ef}$ curta duração (cm⁴)", r08.flechas.Ief_cd_cm4, r24.flechas.Ief_cd_cm4, 0);
    cmp("$\\delta_{max}$ (mm)", r08.flechas.flecha_total_mm, r24.flechas.flecha_total_mm, 2);
    out << "\nVibração: na edição 2008 (L.3.2) o critério é indireto — deslocamento frequente sem a parcela "
           "diferida = " << br(r08.flechas.flecha_vibracao_2008_mm, 2) << " mm ≤ 20 mm ("
        << ok(r08.flechas.flecha_vibracao_2008_mm <= 20.0) << "). Na edição 2024 (Anexo I): f0 = "
        << br(r24.vibracao.f0_Hz, 2) << " Hz, a_p = " << br(r24.vibracao.ap_m_s2, 4) << " m/s² ≤ "
        << br(r24.vibracao.ap_lim_m_s2, 3) << " m/s² (" << ok(r24.vibracao.aprovado) << ").\n";

    // ------------------------------------------------------------------------
    // 3. Estudo paramétrico: vão
    // ------------------------------------------------------------------------
    out << "\n## 3. Estudo paramétrico — variação do vão (V1, conectores em todas as nervuras)\n\n";
    out << "| L (m) | $\\eta$ | $\\eta_i$ 2008 | $\\eta_i$ 2024 | $M_{Sd}/M_{Rd}$ 2024 | $\\delta_{max}/\\delta_{lim}$ | $f_0$ (Hz) | $a_p$ (m/s²) | Global 2024 |\n";
    out << "| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | :---: |\n";
    for (double L : {6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0}) {
        auto c24 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2024);
        auto c08 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008);
        c24.geometria.L_vao_mm = c08.geometria.L_vao_mm = L * 1000.0;
        auto a = verificar_viga_mista(c24);
        auto b = verificar_viga_mista(c08);
        out << "| " << br(L, 1) << " | " << br(a.conectores.grau_interacao_eta, 3)
            << " | " << br(b.conectores.grau_interacao_minimo, 3)
            << " | " << br(a.conectores.grau_interacao_minimo, 3)
            << " | " << pct(a.fase2.taxa_flexao)
            << " | " << pct(a.flechas.taxa_flecha_total)
            << " | " << br(a.vibracao.f0_Hz, 2)
            << " | " << br(a.vibracao.ap_m_s2, 4)
            << " | " << ok(a.aprovado_global) << " |\n";
    }

    // ------------------------------------------------------------------------
    // 4. Estudo paramétrico: densidade de conectores
    // ------------------------------------------------------------------------
    out << "\n## 4. Estudo paramétrico — conector a cada k nervuras (V1, L = 9 m)\n\n";
    out << "| k | s (mm) | n por meio vão | $\\eta$ | $\\eta_i$ 2008 | $\\eta_i$ 2024 | Atende 2008 | Atende 2024 | $M_{Rd}$ 2024 (kN·m) | $\\delta_{max}$ (mm) |\n";
    out << "| ---: | ---: | ---: | ---: | ---: | ---: | :---: | :---: | ---: | ---: |\n";
    for (int k : {1, 2, 3, 4}) {
        auto c24 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2024);
        auto c08 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008);
        c24.conector.k_nervuras = c08.conector.k_nervuras = k;
        auto a = verificar_viga_mista(c24);
        auto b = verificar_viga_mista(c08);
        out << "| " << k << " | " << br(a.conectores.el_espacamento_mm, 0)
            << " | " << a.conectores.n_semi_vao_adotado
            << " | " << br(a.conectores.grau_interacao_eta, 3)
            << " | " << br(b.conectores.grau_interacao_minimo, 3)
            << " | " << br(a.conectores.grau_interacao_minimo, 3)
            << " | " << ok(b.conectores.grau_interacao_atende)
            << " | " << ok(a.conectores.grau_interacao_atende)
            << " | " << br(a.fase2.momento_misto.MRd_VM_kNm, 1)
            << " | " << br(a.flechas.flecha_total_mm, 2) << " |\n";
    }

    // ------------------------------------------------------------------------
    // 5. Estudo paramétrico: resistência do concreto (eta_c)
    // ------------------------------------------------------------------------
    out << "\n## 5. Estudo paramétrico — resistência do concreto (V1, interação completa)\n\n";
    out << "| $f_{ck}$ (MPa) | $\\eta_c$ 2024 | $Q_{Rd}$ 2008 (kN) | $Q_{Rd}$ 2024 (kN) | $n_{compl}$ 2008 | $n_{compl}$ 2024 | $M_{Rd}$ 2008 (kN·m) | $M_{Rd}$ 2024 (kN·m) |\n";
    out << "| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |\n";
    for (double fck : {25.0, 30.0, 35.0, 40.0, 45.0, 50.0}) {
        auto c24 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2024);
        auto c08 = estudo_caso_v1::configuracao(NormaEdicao::NBR8800_2008);
        c24.concreto.fck_MPa = c08.concreto.fck_MPa = fck;
        c24.concreto.Ecs_MPa = nbr8800::v2024::calcularEcsConcreto(fck, 1.0);
        c08.concreto.Ecs_MPa = nbr8800::v2008::calcularEcsConcreto(fck, 1.0);
        // Interação completa: conectores dimensionados para Fhd (espaçamento livre)
        c24.laje.tipo = c08.laje.tipo = LAJE_MACICA;
        c24.laje.tc_mm = c08.laje.tc_mm = 120.0;
        c24.laje.hF_mm = c08.laje.hF_mm = 0.0;
        c24.conector.espacamento_adotado_mm = c08.conector.espacamento_adotado_mm = 0.0;
        auto a = verificar_viga_mista(c24);
        auto b = verificar_viga_mista(c08);
        out << "| " << br(fck, 0) << " | " << br(a.fase2.momento_misto.eta_c, 3)
            << " | " << br(b.conectores.QRd_kN, 2) << " | " << br(a.conectores.QRd_kN, 2)
            << " | " << b.conectores.n_semi_vao_calculado << " | " << a.conectores.n_semi_vao_calculado
            << " | " << br(b.fase2.momento_misto.MRd_VM_kNm, 1)
            << " | " << br(a.fase2.momento_misto.MRd_VM_kNm, 1) << " |\n";
    }
    out << "\nNos casos da seção 5 a laje é maciça (tc = 120 mm), o que torna visível o efeito de "
           "$R_p$ (1,00 em 2008 e 0,75 em 2024 para pinos soldados diretamente no perfil).\n";

    out.close();
    std::cout << "Arquivo " << caminho << " gerado com sucesso.\n";
    return 0;
}
