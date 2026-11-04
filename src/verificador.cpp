#include "verificador.h"
#include "geometria.h"
#include <cmath>

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga) {
    RelatorioVMP rel;
    PerfilI perfil = viga.perfil;
    calcular_propriedades_secao_i(perfil);

    // Rotina A: Compressão Axial (NBR 8800:2008)
    rel.compressao_aco = nbr8800::calcular_normal_resistente(perfil, viga.aco, viga.Lb, viga.L, 1.0, 1.0);

    return rel;
}
