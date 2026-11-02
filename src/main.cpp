#include <iostream>
#include <iomanip>
#include "catalogo.hpp"
#include "verificador.hpp"

int main() {
    std::cout << "===================================================================\n";
    std::cout << "  VMP - SISTEMA DE CALCULO DE VIGAS MISTAS E PROTENDIDAS\n";
    std::cout << "  Afericao contra Turini (2021) - Apendice B / Demoner (2018)\n";
    std::cout << "===================================================================\n\n";

    // 1. DADOS DE ENTRADA (W360x57.8, Aço A572 Gr.50, Laje maciça C30)
    VigaVMP viga;
    viga.perfil = perfil_w360x57_8();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    
    viga.laje.tipo = LAJE_MACICA;
    viga.laje.tc = 130.0;               // 130 mm
    viga.laje.largura_efetiva = 3000.0; // 3000 mm

    viga.cabo.tipo = PRE_TRACAO;
    viga.cabo.tracado = CABO_RETO;
    viga.cabo.Ap = 420.0;               // 3 cordoalhas = 4.2 cm²
    viga.cabo.Ep = 200000.0;            // 200 GPa
    viga.cabo.ep = -50.0;               // 50 mm abaixo da mesa inferior
    viga.cabo.delta_cunha = 6.0;        // 6 mm

    viga.conector.tf = 4.75;
    viga.conector.tw = 4.75;
    viga.conector.Lcs = 50.0;

    viga.cargas.q1_perfil = 0.56;        // kN/m
    viga.cargas.q2_concreto = 9.75;      // kN/m
    viga.cargas.qs_construcao = 3.0;     // kN/m
    viga.cargas.q3_revestimento = 6.0;   // kN/m
    viga.cargas.q4_uso = 15.0;           // kN/m

    viga.L = 12000.0;                   // 12 m
    viga.Lb = 3000.0;                   // 3 m
    viga.P0_adotado = 400.0;            // 400 kN

    // 2. VERIFICAÇÃO ESTRUTURAL
    RelatorioVMP rel = verificar_viga_vmp(viga);

    // 3. APRESENTAÇÃO DOS RESULTADOS
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "--- 1. CONECTORES E ARMADURA DE COSTURA ---\n";
    std::cout << "  QRd (Perfil U)       : " << rel.conectores.QRd_kN << " kN (Esperado: 76.72 kN)\n";
    std::cout << "  Fhd total            : " << rel.conectores.Fhd_kN << " kN (Esperado: 2273.86 kN)\n";
    std::cout << "  nl (conectores)      : " << rel.conectores.nl_conectores << " un (Esperado: 60 un)\n";
    std::cout << "  el (espacamento)     : " << rel.conectores.el_espacamento_mm << " mm (Esperado: 200.0 mm)\n";
    std::cout << "  Hv,Sd (cisalhamento) : " << rel.costura.Hv_Sd_kN_cm << " kN/cm (Esperado: 1.89 kN/cm)\n";
    std::cout << "  As,cos minima        : " << rel.costura.As_cos_cm2_m << " cm2/m (Esperado: 2.60 cm2/m)\n\n";

    std::cout << "--- 2. PERFIL DE ACO ISOLADO (NBR 8800) ---\n";
    std::cout << "  NRd (compressao)     : " << rel.compressao_aco.NRd_kN << " kN (Esperado: 1416.61 kN)\n";
    std::cout << "  MRd (flexao FLT)     : " << rel.flexao_aco.MRd_kNm << " kN.m (Esperado: 285.98 kN.m)\n";
    std::cout << "  VRd (cortante)       : " << rel.cortante_aco.VRd_kN << " kN (Esperado: 532.21 kN)\n";
    std::cout << "  P0 max (estimativa)  : " << rel.estimativa_p0.P0_max_kN << " kN (Esperado: 912.57 kN)\n\n";

    std::cout << "--- 3. FASE 1 - CONSTRUCAO ---\n";
    std::cout << "  P1 (com perdas e dP) : " << rel.P1_fase1_kN << " kN (Esperado: 404.62 kN)\n";
    std::cout << "  MSd positivo         : " << rel.MSd_fase1_kNm << " kN.m (Esperado: 259.09 kN.m)\n";
    std::cout << "  VSd cortante         : " << rel.VSd_fase1_kN << " kN (Esperado: 102.00 kN)\n\n";

    std::cout << "--- 4. FASE 2 - VIGA MISTA PROTENDIDA (t=0) ---\n";
    std::cout << "  ytr (LNE transformada: " << rel.secao_t0.ytr_mm / 10.0 << " cm (Esperado: 39.33 cm)\n";
    std::cout << "  Itr (inercia homog.) : " << rel.secao_t0.Itr_mm4 / 1e4 << " cm4 (Esperado: 60834 cm4)\n";
    std::cout << "  P2 (com incrementos) : " << rel.P2_fase2_kN << " kN (Esperado: 448.41 kN)\n";
    std::cout << "  MSd positivo         : " << rel.MSd_fase2_kNm << " kN.m (Esperado: 789.01 kN.m)\n";
    std::cout << "  MRd,VMP resistente   : " << rel.momento_vmp.MRd_VMP_kNm << " kN.m (Esperado: 802.53 kN.m)\n";
    std::cout << "  VSd cortante         : " << rel.VSd_fase2_kN << " kN (Esperado: 263.00 kN)\n\n";

    std::cout << "--- QUADRO DE VERIFICACOES NORMATIVAS ---\n";
    for (const auto& item : rel.verificacoes) {
        std::cout << "  [" << (item.aprovado ? "OK" : "REPROVADO") << "] "
                  << item.rotina << ": " << item.descricao << " -> "
                  << item.solicitante << " / " << item.resistente << " " << item.unidade << "\n";
    }

    std::cout << "\nSTATUS GLOBAL DA VIGA: " 
              << (rel.aprovado_global ? "APROVADA COM SUCESSO!" : "REPROVADA") 
              << "\n===================================================================\n";

    return rel.aprovado_global ? 0 : 1;
}
