#include <iostream>
#include "calculator.hpp"

int main() {
    Calculator calc;
    std::cout << "==========================================\n";
    std::cout << "  VMP - Aplicacao Inicializada com Sucesso \n";
    std::cout << "==========================================\n";
    std::cout << "Exemplo Calculator: 15 + 25 = " << calc.add(15, 25) << "\n";
    std::cout << "Tudo pronto para o fluxo de TDD!\n";
    return 0;
}
