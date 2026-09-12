#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
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
            std::cout << "                 SUITE DE TESTES TDD                  \n";
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

#define ASSERT_FALSE(expr) \
    do { \
        if (expr) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_FALSE(" #expr ") em " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_EQ(actual, expected) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        if (!(_act == _exp)) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_EQ: " #actual " == " #expected \
                << " (Obtido: " << _act << ", Esperado: " << _exp << ") em " \
                << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_NE(actual, expected) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        if (_act == _exp) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_NE: " #actual " != " #expected \
                << " (Valores sao iguais a " << _act << ") em " \
                << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)

#define ASSERT_THROWS(expr, ExceptionType) \
    do { \
        bool _threw = false; \
        try { \
            expr; \
        } catch (const ExceptionType&) { \
            _threw = true; \
        } catch (...) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_THROWS: Excecao lancada mas nao do tipo " #ExceptionType \
                << " em " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
        if (!_threw) { \
            std::ostringstream _ss; \
            _ss << "Falha em ASSERT_THROWS: Nenhuma excecao do tipo " #ExceptionType \
                << " foi lancada em " << __FILE__ << ":" << __LINE__; \
            throw ::TestFramework::TestFailureException(_ss.str()); \
        } \
    } while (0)
