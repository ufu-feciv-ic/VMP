#include "calculator.hpp"
#include <stdexcept>

int Calculator::add(int a, int b) const {
    return a + b;
}

int Calculator::subtract(int a, int b) const {
    return a - b;
}

int Calculator::multiply(int a, int b) const {
    return a * b;
}

int Calculator::divide(int a, int b) const {
    if (b == 0) {
        throw std::invalid_argument("Divisao por zero nao permitida!");
    }
    return a / b;
}
