#include <iostream>
#include <iomanip>
#include "catalogo.h"
#include "verificador.h"

namespace {

VigaVMP criar_exemplo1_turini() {
    VigaVMP viga;
    viga.perfil = perfil_w360x57_8();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_macica_130();
    viga.armacao_laje = laje_5cm_m();
    viga.cabo = cabo_190RB();

    viga.L = 12000.0;
    viga.Lb = 3000.0;
    viga.a_vigas_mm = 3000.0;
    viga.alpha_viga = 3.0;
    viga.Cb_fase1 = 1.06;
    viga.P0_adotado = 400.0;
    viga.num_secoes = 5;

    viga.cargas.q1_perfil = 0.56;
    viga.cargas.q2_concreto = 9.75;
    viga.cargas.qs_construcao = 3.00;
    viga.cargas.q3_revestimento = 6.00;
    viga.cargas.q4_uso = 15.00;
    viga.cargas.psi1 = 0.6;
    viga.cargas.psi2 = 0.4;
    return viga;
}

VigaVMP criar_exemplo2_turini() {
    VigaVMP viga;
    viga.perfil = perfil_w460x89();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_mista_exemplo2();
    viga.armacao_laje = laje_exemplo2_armacao();
    viga.cabo = cabo_exemplo2();
    viga.cabo.tracado = CABO_RETO;

    viga.L = 15000.0;
    viga.Lb = 3000.0;
    viga.a_vigas_mm = 2000.0;
    viga.alpha_viga = 3.0;
    viga.Cb_fase1 = 1.06;
    viga.P0_adotado = 0.0; // Usa P0_max
    viga.num_secoes = 5;

    viga.cargas.q1_perfil = 0.88;
    viga.cargas.q2_concreto = 6.81;
    viga.cargas.qs_construcao = 2.00;
    viga.cargas.q3_revestimento = 4.00;
    viga.cargas.q4_uso = 10.00;
    viga.cargas.psi1 = 0.6;
    viga.cargas.psi2 = 0.4;
    return viga;
}

void imprimir_resumo_comparativo(const std::string& titulo, VigaVMP viga) {
    viga.norma = NBR8800_2008;
    RelatorioVMP r08 = verificar_viga_vmp(viga);

    viga.norma = NBR8800_2024;
    RelatorioVMP r24 = verificar_viga_vmp(viga);

    std::cout << "-------------------------------------------------------------------\n";
    std::cout << "  " << titulo << "\n";
    std::cout << "-------------------------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Parametro                   | NBR 8800:2008 | NBR 8800:2024 | Unid.\n";
    std::cout << "----------------------------+---------------+---------------+------\n";
    std::cout << "NRd,I (Rotina A)            | " << std::setw(13) << r08.compressao_aco.NRd_kN
              << " | " << std::setw(13) << r24.compressao_aco.NRd_kN << " | kN\n";
    std::cout << "MRd,I (Rotina B)            | " << std::setw(13) << r08.flexao_aco.MRd_kNm
              << " | " << std::setw(13) << r24.flexao_aco.MRd_kNm << " | kN.m\n";
    std::cout << "VRd   (Rotina D)            | " << std::setw(13) << r08.cortante_aco.VRd_kN
              << " | " << std::setw(13) << r24.cortante_aco.VRd_kN << " | kN\n";
    std::cout << "P0,max (Rotina 2)           | " << std::setw(13) << r08.estimativa_p0.P0_max_kN
              << " | " << std::setw(13) << r24.estimativa_p0.P0_max_kN << " | kN\n";
    std::cout << "P1 (Fase 1 - Construcao)    | " << std::setw(13) << r08.P1_fase1_kN
              << " | " << std::setw(13) << r24.P1_fase1_kN << " | kN\n";
    std::cout << "P2 (Fase 2 - Servico t=0)   | " << std::setw(13) << r08.P2_fase2_kN
              << " | " << std::setw(13) << r24.P2_fase2_kN << " | kN\n";
    std::cout << "P3 (Fase 3 - Servico t=inf) | " << std::setw(13) << r08.P3_fase3_kN
              << " | " << std::setw(13) << r24.P3_fase3_kN << " | kN\n";

    if (const auto* s2_08 = r08.fase2_elu.secaoMeioVao()) {
        const auto* s2_24 = r24.fase2_elu.secaoMeioVao();
        std::cout << "MSd+ (Fase 2 - Meio Vao)    | " << std::setw(13) << s2_08->MSd_pos_kNm
                  << " | " << std::setw(13) << s2_24->MSd_pos_kNm << " | kN.m\n";
        std::cout << "MRd,VMP+ (Fase 2 - t=0)     | " << std::setw(13) << s2_08->MRd_pos_kNm
                  << " | " << std::setw(13) << s2_24->MRd_pos_kNm << " | kN.m\n";
        std::cout << "MSd- (Fase 2 - Protensao)   | " << std::setw(13) << s2_08->MSd_neg_kNm
                  << " | " << std::setw(13) << s2_24->MSd_neg_kNm << " | kN.m\n";
        std::cout << "MRd,VMP- (Fase 2 - FLD)     | " << std::setw(13) << s2_08->MRd_neg_kNm
                  << " | " << std::setw(13) << s2_24->MRd_neg_kNm << " | kN.m\n";
    }
    std::cout << "sigma,inf (ELS Servico)     | " << std::setw(13) << r08.els_deformacao.sigma_inf_MPa
              << " | " << std::setw(13) << r24.els_deformacao.sigma_inf_MPa << " | MPa\n";
    std::cout << "delta,total (ELS Flecha)    | " << std::setw(13) << r08.els_deformacao.delta_total_cm
              << " | " << std::setw(13) << r24.els_deformacao.delta_total_cm << " | cm\n";
    std::cout << "delta,lim (L/350)           | " << std::setw(13) << r08.els_deformacao.delta_lim_cm
              << " | " << std::setw(13) << r24.els_deformacao.delta_lim_cm << " | cm\n";
    std::cout << "delta,vib (ELS Vibracao)    | " << std::setw(13) << r08.els_vibracao.delta_total_cm
              << " | " << std::setw(13) << r24.els_vibracao.delta_total_cm << " | cm\n";
    std::cout << "delta,lim,vib (NBR 8800)    | " << std::setw(13) << r08.els_vibracao.delta_lim_cm
              << " | " << std::setw(13) << r24.els_vibracao.delta_lim_cm << " | cm\n";
    std::cout << "Status ELU                  | " << std::setw(13) << (r08.aprovado_elu ? "APROVADO" : "REPROVADO")
              << " | " << std::setw(13) << (r24.aprovado_elu ? "APROVADO" : "REPROVADO") << " |\n";
    std::cout << "Status ELS                  | " << std::setw(13) << (r08.aprovado_els ? "APROVADO" : "REPROVADO")
              << " | " << std::setw(13) << (r24.aprovado_els ? "APROVADO" : "REPROVADO") << " |\n";
    std::cout << "Status Global Final         | " << std::setw(13) << (r08.aprovado_global ? "APROVADO" : "REPROVADO")
              << " | " << std::setw(13) << (r24.aprovado_global ? "APROVADO" : "REPROVADO") << " |\n\n";
}

} // namespace

int main() {
    std::cout << "===================================================================\n";
    std::cout << "  VMP - SISTEMA DE CALCULO DE VIGAS MISTAS E PROTENDIDAS\n";
    std::cout << "  Afericao Turini (2021) & Comparativo ABNT NBR 8800 (2008 vs 2024)\n";
    std::cout << "===================================================================\n\n";

    imprimir_resumo_comparativo("ESTUDO DE CASO 1: Pre-Tracao, Cabo Reto (W360x57.8, L=12m)", criar_exemplo1_turini());
    imprimir_resumo_comparativo("ESTUDO DE CASO 2: Pos-Tracao, Steel Deck (W460x89, L=15m)", criar_exemplo2_turini());

    return 0;
}
