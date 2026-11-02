#include "verificador.h"
#include "geometria.h"
#include <cmath>

RelatorioVMP verificar_viga_vmp(const VigaVMP& viga) {
    RelatorioVMP rel;
    PerfilI perfil = viga.perfil;
    calcular_propriedades_secao_i(perfil);

    
}
