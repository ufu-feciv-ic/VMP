#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "verificador.h"
#include "catalogo.h"

int main() {
    std::ofstream out("docs/DADOS_NUMERICOS_E_TABELAS_TCC.md");
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir docs/DADOS_NUMERICOS_E_TABELAS_TCC.md\n";
        return 1;
    }

    out << "# DADOS NUMÉRICOS, TABELAS E ESTUDOS PARAMÉTRICOS PARA O TCC\n";
    out << "## Dimensionamento de Vigas Mistas de Aço e Concreto segundo a ABNT NBR 8800:2024\n\n";

    VigaMistaConfig cfg_ref;
    cfg_ref.norma = NormaEdicao::NBR8800_2024;
    cfg_ref.perfil = perfil_w360x51();
    cfg_ref.aco = aco_a572_gr50();
    cfg_ref.concreto = concreto_c30();
    cfg_ref.laje.tipo = LAJE_MACICA;
    cfg_ref.laje.tc_mm = 120.0;
    cfg_ref.conector.tipo = CONECTOR_STUD_BOLT;
    cfg_ref.conector.d = 19.0;
    cfg_ref.conector.h = 100.0;
    cfg_ref.conector.fu = 420.0;
    cfg_ref.conector.nt = 1;
    cfg_ref.geometria.L_vao_mm = 7000.0;
    cfg_ref.geometria.Lb_mm = 3500.0;
    cfg_ref.geometria.dist_esq_mm = 1750.0;
    cfg_ref.geometria.dist_dir_mm = 1750.0;

    RelatorioVigaMista rel = verificar_viga_mista(cfg_ref);
    out << "- Momento Resistente Fase 1 (`MRd_kNm`): " << rel.fase1.flexao_aco.MRd_kNm << " kNm\n";
    out << "- Momento Resistente Fase 2 (`MRd_VM_kNm`): " << rel.fase2.momento_misto.MRd_VM_kNm << " kNm\n";
    out << "- Capacidade Conector Stud (`QRd_kN`): " << rel.conectores.QRd_kN << " kN\n";
    out << "- Flecha Total (`flecha_total_mm`): " << rel.flechas.flecha_total_mm << " mm\n";

    out.close();
    std::cout << "Arquivo docs/DADOS_NUMERICOS_E_TABELAS_TCC.md gerado com sucesso!\n";
    return 0;
}
