#include "verificador.hpp"
#include "geometria.hpp"
#include <cmath>

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga) {
    RelatorioVMP rel;
    PerfilI perfil = viga.perfil;
    calcular_propriedades_secao_i(perfil);

    // ========================================================================
    // 1. ROTINAS BÁSICAS DE DIMENSIONAMENTO INICIAL
    // ========================================================================
    
    // Rotina E: Conectores de Cisalhamento
    rel.conectores = calcular_rotina_e_perfil_u(
        perfil, viga.aco, viga.laje, viga.concreto, viga.conector, viga.L
    );

    // Rotina F: Armadura de Costura
    rel.costura = calcular_rotina_f_armadura_costura(
        viga.laje, viga.concreto, rel.conectores.Fhd_kN, viga.L
    );

    // Rotina A: Compressão Axial do Aço
    rel.compressao_aco = calcular_rotina_a(perfil, viga.aco, viga.Lb, viga.L);

    // Rotina B: Flexão Simples do Aço (FLT)
    rel.flexao_aco = calcular_rotina_b(perfil, viga.aco, viga.Lb, 1.06);

    // Rotina D: Cortante do Perfil de Aço
    rel.cortante_aco = calcular_rotina_d(perfil, viga.aco, 0.0);

    // Rotina 1 (Longa Duração): Seção Transformada t=infinito para estimativa P0
    rel.secao_t_inf = calcular_rotina_1_secao_transformada(
        perfil, viga.laje, viga.concreto, viga.aco, 3.0, viga.cabo.ep
    );

    // Rotina 2: Estimativa da força de protensão máxima P0
    double ep_I = (perfil.d / 2.0) + (-viga.cabo.ep); // 129 mm
    double M_q1 = (viga.cargas.q1_perfil * std::pow(viga.L / 1000.0, 2.0)) / 8.0;
    rel.estimativa_p0 = calcular_rotina_2_estimativa_p0(
        perfil, viga.aco, rel.compressao_aco.NRd_kN, rel.flexao_aco.MRd_kNm, ep_I, M_q1
    );

    // ========================================================================
    // 2. FASE 1 — CONSTRUÇÃO (Perfil de aço + Concreto fresco + Sobrecarga constr.)
    // ========================================================================
    ResultadoPerdas perdas_fase1 = calcular_rotina_5_perdas_imediatas(
        viga.P0_adotado, viga.cabo.delta_cunha, viga.cabo.Ep, viga.cabo.Ap, viga.L
    );

    ResultadoIncrementoDP dp_conc = calcular_rotina_6_incremento_dp(
        viga.cargas.q2_concreto, viga.L, ep_I, perfil.Ix, perfil.Ag, viga.cabo.Ap
    );
    ResultadoIncrementoDP dp_const = calcular_rotina_6_incremento_dp(
        viga.cargas.qs_construcao, viga.L, ep_I, perfil.Ix, perfil.Ag, viga.cabo.Ap
    );

    rel.P1_fase1_kN = perdas_fase1.P_efetiva_kN + dp_conc.delta_P_kN + dp_const.delta_P_kN;

    double L_m = viga.L / 1000.0;
    double q_fase1_sd = 1.3 * (viga.cargas.q1_perfil + viga.cargas.q2_concreto) + 1.2 * viga.cargas.qs_construcao;
    double M_cargas_fase1 = (q_fase1_sd * L_m * L_m) / 8.0;
    double M_prot_fase1 = (0.9 * rel.P1_fase1_kN * (ep_I / 1000.0));

    rel.MSd_fase1_kNm = M_cargas_fase1 - M_prot_fase1;
    rel.VSd_fase1_kN = (q_fase1_sd * L_m) / 2.0;

    bool ok_M_fase1 = (rel.MSd_fase1_kNm <= rel.flexao_aco.MRd_kNm);
    bool ok_V_fase1 = (rel.VSd_fase1_kN <= rel.cortante_aco.VRd_kN);

    rel.verificacoes.push_back({
        "ELU 1 - Flexão", "Momento solicitante na construção",
        rel.MSd_fase1_kNm, rel.flexao_aco.MRd_kNm, "kN.m", ok_M_fase1
    });
    rel.verificacoes.push_back({
        "ELU 1 - Cortante", "Esforço cortante na construção",
        rel.VSd_fase1_kN, rel.cortante_aco.VRd_kN, "kN", ok_V_fase1
    });

    if (!ok_M_fase1 || !ok_V_fase1) rel.aprovado_global = false;

    // ========================================================================
    // 3. FASE 2 — VIGA MISTA EM SERVIÇO (t=0)
    // ========================================================================
    rel.secao_t0 = calcular_rotina_1_secao_transformada(
        perfil, viga.laje, viga.concreto, viga.aco, 1.0, viga.cabo.ep
    );

    ResultadoIncrementoDP dp_cpn = calcular_rotina_6_incremento_dp(
        viga.cargas.q3_revestimento, viga.L, rel.secao_t0.ep_tr_mm, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, viga.cabo.Ap
    );
    ResultadoIncrementoDP dp_cvn = calcular_rotina_6_incremento_dp(
        viga.cargas.q4_uso, viga.L, rel.secao_t0.ep_tr_mm, rel.secao_t0.Itr_mm4, rel.secao_t0.Atr_mm2, viga.cabo.Ap
    );

    rel.P2_fase2_kN = rel.P1_fase1_kN + dp_cpn.delta_P_kN + dp_cvn.delta_P_kN - dp_const.delta_P_kN;

    ResultadoRotinaH rot_h = calcular_rotina_h_tensao_cabo(
        rel.P2_fase2_kN, viga.cabo.Ap, viga.cabo.Ep, viga.cabo.fpyk, rel.secao_t0.Atr_mm2, viga.L, perfil.d + viga.laje.tc
    );

    rel.momento_vmp = calcular_rotina_3_momento_vmp(
        perfil, viga.aco, viga.laje, viga.concreto, viga.cabo.Ap, rot_h.sigma_pd_MPa, viga.cabo.ep
    );

    double q_perm_fase2 = viga.cargas.q1_perfil + viga.cargas.q2_concreto + viga.cargas.q3_revestimento;
    double q_fase2_sd = 1.4 * q_perm_fase2 + 1.4 * viga.cargas.q4_uso;

    rel.MSd_fase2_kNm = (q_fase2_sd * L_m * L_m) / 8.0;
    rel.VSd_fase2_kN = (q_fase2_sd * L_m) / 2.0;

    bool ok_M_fase2 = (rel.MSd_fase2_kNm <= rel.momento_vmp.MRd_VMP_kNm);
    bool ok_V_fase2 = (rel.VSd_fase2_kN <= rel.cortante_aco.VRd_kN);

    rel.verificacoes.push_back({
        "ELU 2 - Flexão VMP", "Momento solicitante positivo VMP",
        rel.MSd_fase2_kNm, rel.momento_vmp.MRd_VMP_kNm, "kN.m", ok_M_fase2
    });
    rel.verificacoes.push_back({
        "ELU 2 - Cortante", "Força cortante resistente",
        rel.VSd_fase2_kN, rel.cortante_aco.VRd_kN, "kN", ok_V_fase2
    });

    if (!ok_M_fase2 || !ok_V_fase2) rel.aprovado_global = false;

    return rel;
}
