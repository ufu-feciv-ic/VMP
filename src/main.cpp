#include <iostream>
#include "calculator.hpp"
#include "SecaoTransversal.hpp"

int main() {
    Calculator calc;

    SecaoTransversal SecaoI ("W360x57,8", 57.8, 358, 172, 7.9, 13.1, 332, 308, 72.5, 0,
           16143, 902, 14.9, 1015, 1113, 129, 3.92, 199.8, 4.53, 34.5, 330394);
    

    std::cout << SecaoI.bf << std::endl;
    std::cout << SecaoI.tf << std::endl;
    std::cout << SecaoI.bf / (2*SecaoI.tf) << std::endl;
    std::cout << SecaoI.LambdaMesa << std::endl;
    std::cout << SecaoI.LambdaAlma << std::endl;

    std::cout << "==========================================\n";
    std::cout << "  VMP - Aplicacao Inicializada com Sucesso \n";
    std::cout << "==========================================\n";
    std::cout << "Exemplo Calculator: 15 + 25 = " << calc.add(15, 25) << "\n";
    std::cout << "Tudo pronto para o fluxo de TDD!\n";
    return 0;
}
