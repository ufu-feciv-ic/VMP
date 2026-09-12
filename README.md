# VMP - C++ TDD MVP Template

Estrutura MVP configurada para desenvolvimento C++ moderno focado em **TDD** (Test-Driven Development), sem dependência de bibliotecas externas, com detecção automática de novos arquivos e suporte integrado ao VS Code.

---

## 📁 Estrutura de Diretórios

```text
VMP/
├── .vscode/
│   ├── c_cpp_properties.json   # Configuração do IntelliSense (C++20, headers, GCC)
│   ├── tasks.json              # Atalhos de compilação e execução de testes
│   ├── launch.json             # Depuração via GDB (Testes e App)
│   └── settings.json           # Associações de extensões de arquivos
├── include/
│   └── calculator.hpp          # Header files (.hpp) públicos do seu projeto
├── src/
│   ├── calculator.cpp          # Implementação das regras de negócio (.cpp)
│   └── main.cpp                # Ponto de entrada do executável da aplicação
├── tests/
│   ├── test_framework.hpp      # Micro-framework de testes zero-dependency
│   ├── test_main.cpp           # Ponto de entrada do executável de testes
│   └── test_calculator.cpp     # Casos de teste TDD
├── CMakeLists.txt              # Configuração do CMake com CONFIGURE_DEPENDS
├── .gitignore                  # Arquivos ignorados pelo Git (builds, binários)
└── README.md
```

---

## 🚀 Como Executar

### 1. Pelo VS Code
- **Executar Testes (Build + Run):**
  - Atalho de tarefa de teste: menu **Terminal -> Run Test Task...** (ou executar a task `"Executar Testes"`).
- **Compilar Tudo:**
  - Atalho: `Ctrl + Shift + B` (executa `"Compilar Tudo"`).
- **Depuração (F5):**
  - Na aba "Run and Debug" (`Ctrl + Shift + D`), selecione **"Debugar Testes (vmp_tests)"** ou **"Debugar App (vmp_app)"** e pressione `F5`.

### 2. Pelo Terminal (PowerShell / Bash)
```powershell
# Compilar projeto
cmake --build build

# Executar testes
.\build\vmp_tests.exe

# Executar aplicacao
.\build\vmp_app.exe
```

---

## 🔄 Detecção Automática de Novos Arquivos

O `CMakeLists.txt` utiliza a diretiva `CONFIGURE_DEPENDS`:
- Ao criar um novo arquivo em `src/`, `include/` ou `tests/`, **não é necessário editar o CMakeLists.txt nem rodar cmake novamente manualmente**.
- O sistema de build (`ninja`) detecta adições ou remoções de arquivos automaticamente na próxima execução de compilação.

---

## 🧪 Micro-Framework de Testes (`tests/test_framework.hpp`)

Framework zero-dependency incluído no projeto:
- Não requer instalação ou download de bibliotecas (Catch2, GTest, etc.).
- Auto-registro de testes: basta declarar `TEST_CASE("Nome do Teste") { ... }` em qualquer arquivo `.cpp` na pasta `tests/`.

### Macros disponíveis:
- `TEST_CASE("Descricao do teste") { ... }`
- `ASSERT_EQ(obtido, esperado)`
- `ASSERT_NE(obtido, esperado)`
- `ASSERT_TRUE(expressao_booleana)`
- `ASSERT_FALSE(expressao_booleana)`
- `ASSERT_THROWS(expressao, TipoExcecao)`

### Exemplo de uso em TDD:
```cpp
#include "test_framework.hpp"
#include "meu_modulo.hpp"

TEST_CASE("Modulo - Deve processar valor corretamente") {
    MeuModulo modulo;
    ASSERT_EQ(modulo.calcular(10), 20);
    ASSERT_TRUE(modulo.esta_valido());
}
```

---

## 🔁 Fluxo TDD Recomendado
1. **Red**: Crie o arquivo de teste em `tests/` e escreva o caso de teste que falha.
2. **Green**: Crie a assinatura no `include/` e a implementação mínima em `src/` até passar.
3. **Refactor**: Melhore a estrutura mantendo os testes executando com `.\build\vmp_tests.exe` ou pelo VS Code.
