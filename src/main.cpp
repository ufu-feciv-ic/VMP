#include <iostream>
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"
#include "normas/nbr8800_2008/rotina_a.hpp"

int main() {
    using namespace vmp::nucleo;
    using namespace vmp::normas::nbr8800_2008;

    std::cout << "==========================================\n";
    std::cout << "   VMP - Vigas Mistas e Protendidas       \n";
    std::cout << "   Verificacao de Estruturas de Aco       \n";
    std::cout << "==========================================\n\n";

    // Criacao do perfil W 360 x 57.8 e material A572 Gr 50
    SecaoI perfil = SecaoI::w360x57_8();
    Aco aco = Aco::a572_gr50();

    std::cout << "Perfil: " << perfil.nome << "\n";
    std::cout << "Aco:    " << aco.nome << " (fy = " << aco.fy << " MPa)\n";
    std::cout << "Largura da mesa (bf):    " << perfil.bf << " mm\n";
    std::cout << "Espessura da mesa (tf):  " << perfil.tf << " mm\n";
    std::cout << "Espessura da alma (tw):  " << perfil.tw << " mm\n";
    std::cout << "Raio concordancia (r):   " << perfil.r << " mm\n";
    std::cout << "Altura entre mesas (h):  " << perfil.altura_entre_mesas() << " mm\n";
    std::cout << "Cota de dispersao (k):   " << perfil.cota_k() << " mm\n";
    std::cout << "Esbeltez da mesa (AL):   " << perfil.esbeltez_mesa() << "\n";
    std::cout << "Esbeltez da alma (AA):   " << perfil.esbeltez_alma() << "\n\n";

    // Calculo da compressao axial pela NBR 8800:2008 (Rotina A)
    double Lv = 12000.0;
    double Lb = 3000.0;
    ResultadoRotinaA res = RotinaA::calcular(perfil, aco, Lv, Lb);

    std::cout << "--- Resultados da Rotina A (NBR 8800:2008) ---\n";
    std::cout << "Forca de Euler Nex:      " << res.Nex / 1000.0 << " kN\n";
    std::cout << "Forca de Euler Ney:      " << res.Ney / 1000.0 << " kN\n";
    std::cout << "Forca critica Ne:        " << res.Ne / 1000.0 << " kN\n";
    std::cout << "Fator de mesa Qs:        " << res.Qs << "\n";
    std::cout << "Fator de alma Qa:        " << res.Qa << "\n";
    std::cout << "Fator total Q:           " << res.Q << "\n";
    std::cout << "Esbeltez global lambda0: " << res.lambda_0 << "\n";
    std::cout << "Fator de reducao chi:    " << res.chi << "\n";
    std::cout << "Resistencia NRd:         " << res.NRd_kN << " kN\n";
    std::cout << "==========================================\n";

    return 0;
}
