#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <cmath>
#include <exception>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace TestFramework {

    inline void init_console() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    class TestRegistry {
    public:
        static TestRegistry& instance() {
            static TestRegistry reg;
            return reg;
        }

        void register_test(const std::string& name, std::function<void()> func) {
            tests_.push_back({name, func});
        }

        int run_all() {
            init_console();
            int passed = 0;
            int failed = 0;

            std::cout << "\n======================================================\n";
            std::cout << "        VMP SIMPLES - SUITE DE TESTES UNITARIOS       \n";
            std::cout << "======================================================\n\n";

            for (const auto& test : tests_) {
                std::cout << "[ TEST ] " << test.name << " ... ";
                try {
                    test.func();
                    std::cout << "\033[32m[ PASSOU ]\033[0m\n";
                    passed++;
                } catch (const std::exception& e) {
                    std::cout << "\033[31m[ FALHOU ]\033[0m\n";
                    std::cout << "         \033[31m-> " << e.what() << "\033[0m\n";
                    failed++;
                } catch (...) {
                    std::cout << "\033[31m[ FALHOU ]\033[0m\n";
                    std::cout << "         \033[31m-> Excecao desconhecida!\033[0m\n";
                    failed++;
                }
            }

            std::cout << "\n------------------------------------------------------\n";
            std::cout << "Total: " << tests_.size() << " | "
                      << "\033[32mPassou: " << passed << "\033[0m | "
                      << (failed > 0 ? "\033[31m" : "\033[32m")
                      << "Falhou: " << failed << "\033[0m\n";
            std::cout << "======================================================\n\n";

            return (failed == 0) ? 0 : 1;
        }

    private:
        std::vector<TestCase> tests_;
    };

    struct AutoRegister {
        AutoRegister(const std::string& name, std::function<void()> func) {
            TestRegistry::instance().register_test(name, func);
        }
    };

    class TestFailureException : public std::exception {
    public:
        explicit TestFailureException(std::string msg) : msg_(std::move(msg)) {}
        const char* what() const noexcept override { return msg_.c_str(); }
    private:
        std::string msg_;
    };

} // namespace TestFramework

#define TF_CONCAT_INTERNAL(a, b) a##b
#define TF_CONCAT(a, b) TF_CONCAT_INTERNAL(a, b)

#define TEST_CASE(name) \
    static void TF_CONCAT(test_func_, __LINE__)(); \
    static ::TestFramework::AutoRegister TF_CONCAT(auto_reg_, __LINE__)(name, TF_CONCAT(test_func_, __LINE__)); \
    static void TF_CONCAT(test_func_, __LINE__)()

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_TRUE(" #expr ") em " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_NEAR(actual, expected, tol) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        auto _diff = std::abs(_act - _exp); \
        if (_diff > (tol)) { \
            double _denom = std::abs(_exp); \
            double _rel_pct = (_denom > 1e-12) ? ((_diff / _denom) * 100.0) : 0.0; \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_NEAR: |" #actual " - " #expected "| <= " #tol << "\n" \
                << "         -> Obtido:   " << _act << "\n" \
                << "         -> Esperado: " << _exp << "\n" \
                << "         -> Dif. Abs: " << _diff << " > Tol: " << (tol) << "\n" \
                << "         -> Dif. Rel: " << _rel_pct << "%\n" \
                << "         -> Local:    " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_PERCENT(actual, expected, max_percent) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        auto _diff = std::abs(_act - _exp); \
        double _denom = std::abs(_exp); \
        double _pct_tol = (max_percent); \
        double _rel_pct = (_denom > 1e-12) ? ((_diff / _denom) * 100.0) : (_diff * 100.0); \
        if (_rel_pct > _pct_tol) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_PERCENT: erro relativo de " #actual " excede " << _pct_tol << "%\n" \
                << "         -> Obtido:   " << _act << "\n" \
                << "         -> Esperado: " << _exp << "\n" \
                << "         -> Dif. Rel: " << _rel_pct << "% > Tol: " << _pct_tol << "%\n" \
                << "         -> Dif. Abs: " << _diff << "\n" \
                << "         -> Local:    " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_APPROX(actual, expected, rel_tol, abs_tol) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        auto _diff = std::abs(_act - _exp); \
        auto _r_tol = (rel_tol); \
        auto _a_tol = (abs_tol); \
        auto _allowed = std::max(_a_tol, _r_tol * std::max(std::abs(_act), std::abs(_exp))); \
        if (_diff > _allowed) { \
            double _denom = std::abs(_exp); \
            double _rel_pct = (_denom > 1e-12) ? ((_diff / _denom) * 100.0) : 0.0; \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_APPROX: |" #actual " - " #expected "| excede tolerancia mista\n" \
                << "         -> Obtido:   " << _act << "\n" \
                << "         -> Esperado: " << _exp << "\n" \
                << "         -> Dif. Abs: " << _diff << " (permitido: " << _allowed << ")\n" \
                << "         -> Dif. Rel: " << _rel_pct << "%\n" \
                << "         -> Local:    " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)
