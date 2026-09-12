#include "test_framework.hpp"
#include "calculator.hpp"
#include <stdexcept>

TEST_CASE("Calculator - Adicao de numeros positivos") {
    Calculator calc;
    ASSERT_EQ(calc.add(2, 3), 5);
}

TEST_CASE("Calculator - Subtracao basica") {
    Calculator calc;
    ASSERT_EQ(calc.subtract(10, 4), 6);
}

TEST_CASE("Calculator - Multiplicacao com zero e negativos") {
    Calculator calc;
    ASSERT_EQ(calc.multiply(5, 0), 0);
    ASSERT_EQ(calc.multiply(-2, 3), -6);
}

TEST_CASE("Calculator - Divisao valida") {
    Calculator calc;
    ASSERT_EQ(calc.divide(20, 4), 5);
}

TEST_CASE("Calculator - Divisao por zero deve lancar std::invalid_argument") {
    Calculator calc;
    ASSERT_THROWS(calc.divide(10, 0), std::invalid_argument);
}
