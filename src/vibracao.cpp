#include "vibracao.h"
#include "rotinas_nbr8800.h"
#include <algorithm>
#include <cmath>

namespace nbr8800 {

const std::vector<OcupacaoVibracao>& obter_tabela_ocupacoes_vibracao() {
    // psi_vib (I.1.5), beta da ocupação (Tabela I.2), a_p,lim e R_max (Tabela I.3)
    static const std::vector<OcupacaoVibracao> tabela = {
        { "Escritórios convencionais",                          0.10, 0.010, 0.050,  8.0 },
        { "Escritórios corporativos (andar corrido)",           0.10, 0.005, 0.050,  8.0 },
        { "Escolas (salas de aula e áreas comuns)",             0.10, 0.010, 0.050,  8.0 },
        { "Circulação de público (mall, aeroporto, rodoviária)",0.00, 0.000, 0.150, 24.0 },
        { "Centros comerciais e lojas em geral",                0.10, 0.010, 0.050,  8.0 },
        { "Garagens",                                           0.00, 0.005, 0.150, 24.0 },
        { "Passarelas e escadas internas",                      0.00, 0.005, 0.150, 24.0 },
        { "Passarelas e escadas externas",                      0.00, 0.005, 0.200, 32.0 },
        { "Residências e hotéis",                               0.10, 0.010, 0.050,  8.0 },
        { "Hospitais (atendimento e internação)",               0.10, 0.010, 0.050,  8.0 },
        { "Hospitais (pacientes sensíveis)",                    0.10, 0.010, 0.025,  4.0 },
        { "Hospitais (cirurgia e laboratórios convencionais)",  0.10, 0.000, 0.012,  2.0 },
    };
    return tabela;
}

void aplicar_ocupacao_vibracao(ParametrosVibracao& params, const OcupacaoVibracao& ocup) {
    params.psi_vib = ocup.psi_vib;
    params.beta_ocupacao = ocup.beta_ocupacao;
    params.ap_lim_m_s2 = ocup.ap_lim_m_s2;
    params.R_max = ocup.R_max;
}

ResultadoVibracaoPiso calcularVibracaoPisoAnexoI(
    const PerfilI& perfil,
    const Aco& aco,
    const Laje& laje,
    double Lv_mm,
    double b_influencia_mm,
    double g1_kN_m,
    double g2_kN_m,
    double q2_kN_m,
    const ParametrosVibracao& params
) {
    ResultadoVibracaoPiso res;
    if (Lv_mm <= 0.0 || b_influencia_mm <= 0.0 || aco.E_MPa <= 0.0) return res;
    res.calculado = true;

    const double E = aco.E_MPa;
    const double g = 9.81;          // m/s²
    const double P0_N = 750.0;      // I.2.4: peso de uma pessoa

    // I.1.7: viga considerada mista, concreto não fissurado, módulo dinâmico
    res.Ec_din_MPa = (params.Ec_din_MPa > 0.0) ? params.Ec_din_MPa : 38000.0;
    res.alpha_din = E / res.Ec_din_MPa;
    auto secao = calcularSecaoHomogeneizadaELS(perfil, laje, res.alpha_din, false);
    res.Ib_mm4 = secao.Itr_mm4;

    // Inércia da laje por unidade de largura (Tabela I.1, legenda): tc³ / (12·n)
    res.Is_mm4_por_mm = (params.Is_laje_mm4_por_mm > 0.0)
        ? params.Is_laje_mm4_por_mm
        : std::pow(laje.tc_mm, 3.0) / (12.0 * res.alpha_din);

    // I.1.5: combinação frequente com psi_vib (0,1 ou 0)
    res.b_mm = b_influencia_mm;
    res.w_vib_kN_m = g1_kN_m + g2_kN_m + params.psi_vib * q2_kN_m;
    const double b_m = res.b_mm / 1000.0;
    res.m_kg_m2 = (res.w_vib_kN_m * 1000.0) / (g * b_m);

    // Tabela I.1, modo A: viga secundária biapoiada + flexibilidade da laje (b³/Is)
    // w em N/mm (= kN/m), comprimentos em mm
    res.delta_viga_mm = 5.0 * res.w_vib_kN_m * std::pow(Lv_mm, 4.0) / (384.0 * E * res.Ib_mm4);
    res.delta_laje_mm = res.w_vib_kN_m * std::pow(res.b_mm, 3.0) / (384.0 * E * res.Is_mm4_por_mm);
    res.delta_mm = res.delta_viga_mm + res.delta_laje_mm;

    // I.2.2
    res.f0_Hz = 18.0 / std::sqrt(res.delta_mm);

    // I.2.3: largura colaborante e massa modal do modo A
    res.kb = params.viga_interna ? 2.0 : 1.0;
    res.Db_mm4_por_mm = res.Ib_mm4 / res.b_mm;
    const double L_m = Lv_mm / 1000.0;
    res.Bb_rigidez_m = res.kb * L_m * std::pow(res.Is_mm4_por_mm / res.Db_mm4_por_mm, 0.25);
    res.Bb_limite_m = (params.largura_piso_m > 0.0) ? (2.0 / 3.0) * params.largura_piso_m : 1e9;
    res.Bb_m = std::min(res.Bb_rigidez_m, res.Bb_limite_m);
    res.M_kg = 0.5 * res.m_kg_m2 * L_m * res.Bb_m;

    // Tabela I.2: razão de amortecimento crítico
    res.beta = params.beta_estrutura
        + (params.forro_instalacoes_suspensos ? 0.010 : 0.0)
        + params.beta_ocupacao
        + params.beta_divisorias;
    const double beta_M = std::max(1e-9, res.beta * res.M_kg);

    // I.2.4: aceleração de pico e valor eficaz ponderado
    res.ap_m_s2 = 0.55 * P0_N * std::exp(-0.35 * res.f0_Hz) / (2.0 * beta_M);
    if (res.f0_Hz < 4.0) res.W = 0.5 * std::sqrt(res.f0_Hz);
    else if (res.f0_Hz <= 8.0) res.W = 1.0;
    else res.W = 8.0 / res.f0_Hz;
    res.zeta = (res.f0_Hz <= 5.0) ? 0.15 : 0.10;
    res.aw_rms_m_s2 = res.W * res.zeta * P0_N / (2.0 * std::sqrt(2.0) * beta_M);

    // I.2.5
    res.R = res.aw_rms_m_s2 / 0.005;
    res.ap_lim_m_s2 = params.ap_lim_m_s2;
    res.R_max = params.R_max;
    res.utilizacao = (params.criterio == CriterioVibracao::ACELERACAO_PICO)
        ? res.ap_m_s2 / std::max(1e-9, res.ap_lim_m_s2)
        : res.R / std::max(1e-9, res.R_max);

    res.atende_frequencia_minima = (res.f0_Hz >= 3.0);
    res.faixa_metodo_valida = (res.f0_Hz >= 3.0 && res.f0_Hz <= 9.0);
    // Acima de 9 Hz (piso de alta frequência) o método simplificado não se aplica e a
    // resposta ao caminhar deixa de ser governante: considera-se atendido.
    res.aprovado = res.atende_frequencia_minima && (res.f0_Hz > 9.0 || res.utilizacao <= 1.0);
    return res;
}

} // namespace nbr8800
