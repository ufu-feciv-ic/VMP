#pragma once
#include "modelos.h"
#include "catalogo.h"
#include "rotinas_nbr8800.h"

// ============================================================================
// ESTUDO DE CASO DO TCC - VIGA V1
// Fonte: planilha referencias/Viga_Mista_NBR8800-2024.xlsx (abas Dados, Cargas e Memorial)
// Escritório, vão de 9 m, vigas a cada 2,8 m, W460x52,0 (A572 Gr. 50), steel deck
// Metform MF-75 (h = 140 mm, t = 0,80 mm) com nervuras perpendiculares, C30,
// pinos Ø19 x 125 mm (fu = 415 MPa), construção não escorada, contraflecha de 15 mm.
// ============================================================================

namespace estudo_caso_v1 {

// Ações por componente (Cargas!D6:D47), em kN/m
constexpr double binf_m = 2.80;
constexpr double g_concreto_kN_m = 2.5625 * binf_m;     // consumo 0,1025 m³/m² x 25 kN/m³
constexpr double g_forma_kN_m = 0.0919197 * binf_m;     // chapa MF-75 t = 0,80 mm
constexpr double g_perfil_kN_m = 0.51012;               // 52 kg/m x 9,81
constexpr double g1_kN_m = g_concreto_kN_m + g_forma_kN_m + g_perfil_kN_m;
constexpr double q1_kN_m = 1.00 * binf_m;               // sobrecarga de construção
constexpr double g2_kN_m = 1.55 * binf_m;               // revestimento + forro + instalações
constexpr double q2_kN_m = 3.25 * binf_m;               // escritório 2,5 + divisórias 0,75

// Coeficientes da Tabela 1 aplicados por componente, reduzidos a um gama equivalente por parcela
constexpr double gamma_g1_construcao =
    (1.25 * g_concreto_kN_m + 1.15 * (g_forma_kN_m + g_perfil_kN_m)) / g1_kN_m;
constexpr double gamma_g1_normal =
    (1.35 * g_concreto_kN_m + 1.25 * (g_forma_kN_m + g_perfil_kN_m)) / g1_kN_m;

inline VigaMistaConfig configuracao(NormaEdicao norma = NormaEdicao::NBR8800_2024) {
    VigaMistaConfig cfg;
    cfg.norma = norma;
    cfg.perfil = perfil_w460x52();
    cfg.aco = aco_a572_gr50();

    cfg.concreto.classe = "C30";
    cfg.concreto.fck_MPa = 30.0;
    cfg.concreto.alpha_ag = 1.0;
    cfg.concreto.Ecs_MPa = (norma == NormaEdicao::NBR8800_2024)
        ? nbr8800::v2024::calcularEcsConcreto(30.0, 1.0)
        : nbr8800::v2008::calcularEcsConcreto(30.0, 1.0);

    cfg.laje.tipo = LAJE_STEEL_DECK;
    cfg.laje.tc_mm = 65.0;
    cfg.laje.hF_mm = 75.0;
    cfg.laje.bF_mm = 137.0;
    cfg.laje.eF_mm = 274.0;
    cfg.laje.tf_deck_mm = 0.80;
    cfg.laje.nervura_paralela = false;

    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 125.0;
    cfg.conector.fu = 415.0;
    cfg.conector.nt = 1;
    cfg.conector.k_nervuras = 1;
    cfg.conector.emh_mm = 59.0;

    cfg.geometria.L_vao_mm = 9000.0;
    cfg.geometria.Lb_mm = 0.0;     // steel deck ligado ao perfil: mesa comprimida contida na concretagem
    cfg.geometria.dist_esq_mm = 2800.0;
    cfg.geometria.dist_dir_mm = 2800.0;
    cfg.geometria.escorada = false;
    cfg.geometria.Cb = 1.0;

    cfg.acoes.g1_kN_m = g1_kN_m;
    cfg.acoes.q1_kN_m = q1_kN_m;
    cfg.acoes.g2_kN_m = g2_kN_m;
    cfg.acoes.q2_kN_m = q2_kN_m;
    cfg.acoes.gamma_g1 = gamma_g1_construcao;
    cfg.acoes.gamma_q1 = 1.30;
    cfg.acoes.gamma_g1_fase2 = gamma_g1_normal;
    cfg.acoes.gamma_g2 = 1.50;
    cfg.acoes.gamma_q2 = 1.50;
    cfg.acoes.psi1 = 0.60;
    cfg.acoes.psi2 = 0.40;
    cfg.acoes.contraflecha_mm = 15.0;
    cfg.acoes.combinacao_flecha_max = CombinacaoServico::QUASE_PERMANENTE;
    cfg.acoes.combinacao_flecha_dano = CombinacaoServico::RARA;
    cfg.acoes.alvenaria_solidarizada = false;
    cfg.acoes.considerar_retracao = true;
    cfg.acoes.eps_sh = 0.0002;

    cfg.vibracao.verificar = true;
    cfg.vibracao.Ec_din_MPa = 38000.0;
    cfg.vibracao.psi_vib = 0.10;
    cfg.vibracao.forro_instalacoes_suspensos = true;
    cfg.vibracao.beta_ocupacao = 0.010;     // escritórios convencionais
    cfg.vibracao.beta_divisorias = 0.0;
    cfg.vibracao.viga_interna = true;
    cfg.vibracao.largura_piso_m = 20.0;
    cfg.vibracao.Is_laje_mm4_por_mm = 13.17e6 / 1000.0; // catálogo Metform MF-75|140|80
    cfg.vibracao.criterio = CriterioVibracao::ACELERACAO_PICO;
    cfg.vibracao.ap_lim_m_s2 = 0.050;
    cfg.vibracao.R_max = 8.0;
    return cfg;
}

} // namespace estudo_caso_v1
