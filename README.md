# VMP — Vigas Mistas e Protendidas

Software em C++ focado na verificação e dimensionamento de **estruturas de aço, estruturas mistas e vigas mistas protendidas**, desenvolvido com **Clean Architecture** (Arquitetura Limpa) e **TDD** (*Test-Driven Development*), sem dependências de bibliotecas externas pesadas e com suporte integrado ao VS Code e CMake.

Para uma explicação detalhada dos conceitos arquiteturais e dos *design patterns* aplicados, consulte o arquivo [ARQUITETURA.md](ARQUITETURA.md). Para acompanhar a sequência de implementação das rotinas de cálculo baseadas na dissertação de referência, consulte o [ROADMAP.md](ROADMAP.md).

---

## 📁 Estrutura do Projeto

```text
VMP/
├── include/
│   ├── nucleo/
│   │   ├── secao_i.hpp              # Classe SecaoI (dimensões, inércia, raio de concordância r)
│   │   ├── material.hpp             # Classe Aco (ASTM A36, A572 Gr 50)
│   │   └── esforcos_internos.hpp    # Struct EsforcosInternos (M, V, N)
│   └── normas/
│       ├── i_rotina_verificacao.hpp # Interface base abstrata (Strategy Pattern)
│       └── nbr8800_2008/
│           └── rotina_a.hpp         # Rotina A - Compressão axial (NBR 8800:2008)
├── src/
│   ├── main.cpp                     # Demonstração executável do dimensionamento
│   ├── nucleo/
│   │   ├── secao_i.cpp              # Implementação geométrica e raio de concordância
│   │   └── material.cpp             # Implementação de materiais de catálogo
│   └── normas/
│       └── nbr8800_2008/
│           └── rotina_a.cpp         # Equações analíticas da Rotina A
├── tests/
│   ├── test_framework.hpp           # Micro-framework de testes zero-dependency
│   ├── test_main.cpp                # Ponto de entrada da suíte de testes TDD
│   ├── nucleo/
│   │   └── test_secao_i.cpp         # Testes de geometria, esbeltez e raio r
│   └── normas/
│       └── nbr8800_2008/
│           └── test_rotina_a.cpp     # Testes da Rotina A aferidos contra dissertação
├── CMakeLists.txt                   # Build system com detecção automática de arquivos
├── ARQUITETURA.md                   # Documentação detalhada da arquitetura e design patterns
├── ROADMAP.md                       # Sequência de desenvolvimento e aferição das rotinas
└── README.md                        # Este arquivo
```

---

## 🚀 Como Compilar e Executar

### 1. Pelo Terminal (PowerShell / Prompt / Bash)

```powershell
# 1. Compilar o projeto (gera a biblioteca, o app e os testes)
cmake --build build

# 2. Executar a suíte de testes TDD
.\build\vmp_tests.exe

# 3. Executar o programa demonstrativo
.\build\vmp_app.exe
```

### 2. Pelo VS Code
* **Executar Testes (Build + Run):** Pressione `Ctrl + Shift + P` e selecione **Tasks: Run Test Task** (ou atalho de tarefa `"Executar Testes"`).
* **Compilar Tudo:** Atalho `Ctrl + Shift + B` (executa `"Compilar Tudo"`).
* **Depuração (F5):** Na aba *Run and Debug* (`Ctrl + Shift + D`), selecione **"Debugar Testes (vmp_tests)"** ou **"Debugar App (vmp_app)"** e tecle `F5`.

---

## 🖥️ Exemplo de Execução do Programa (`vmp_app.exe`)

O ponto de entrada [`src/main.cpp`](src/main.cpp) executa a verificação completa do perfil laminado **W 360 x 57.8** em aço **ASTM A572 Grau 50** sob a **NBR 8800:2008 (Rotina A)**:

```text
==========================================
   VMP - Vigas Mistas e Protendidas       
   Verificacao de Estruturas de Aco       
==========================================

Perfil: W 360 x 57.8
Aco:    ASTM A572 Grau 50 (fy = 345 MPa)
Largura da mesa (bf):    172 mm
Espessura da mesa (tf):  13.1 mm
Espessura da alma (tw):  7.9 mm
Raio concordancia (r):   10.2 mm
Altura entre mesas (h):  331.8 mm
Cota de dispersao (k):   23.3 mm
Esbeltez da mesa (AL):   6.56489
Esbeltez da alma (AA):   42

--- Resultados da Rotina A (NBR 8800:2008) ---
Forca de Euler Nex:      2202.43 kN
Forca de Euler Ney:      2452.05 kN
Forca critica Ne:        2202.43 kN
Fator de mesa Qs:        1
Fator de alma Qa:        1
Fator total Q:           1
Esbeltez global lambda0: 1.07081
Fator de reducao chi:    0.618828
Resistencia NRd:         1420.72 kN
==========================================
```

---

## 🔄 Detecção Automática de Novos Arquivos

O [`CMakeLists.txt`](CMakeLists.txt) utiliza a instrução `file(GLOB_RECURSE ... CONFIGURE_DEPENDS)`.
* Ao adicionar ou remover novos arquivos `.cpp` ou `.hpp` dentro de `src/`, `include/` ou `tests/`, **não é necessário editar o CMakeLists.txt nem reconfigurar o CMake manualmente**.
* O gerador de compilação detecta as mudanças na próxima vez que `cmake --build build` for executado.

---

## 🧪 Suíte de Testes TDD (`tests/test_framework.hpp`)

O projeto inclui um micro-framework de testes unitários em cabeçalho único, **sem dependências externas** (sem necessidade de instalar Catch2, GoogleTest ou Boost):

### Macros Disponíveis:
* `TEST_CASE("Descricao do teste") { ... }`
* `ASSERT_EQ(obtido, esperado)`
* `ASSERT_NE(obtido, esperado)`
* `ASSERT_TRUE(expressao_booleana)`
* `ASSERT_FALSE(expressao_booleana)`
* `ASSERT_THROWS(expressao, TipoExcecao)`

### Exemplo de Teste no VMP:
```cpp
#include "test_framework.hpp"
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"
#include "normas/nbr8800_2008/rotina_a.hpp"

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina A - Flambagem Global de Euler") {
    SecaoI perfil = SecaoI::w360x57_8();
    Aco aco = Aco::a572_gr50();

    ResultadoRotinaA res = RotinaA::calcular(perfil, aco, 12000.0, 3000.0);
    ASSERT_EQ(res.Q, 1.0);
    ASSERT_TRUE(res.NRd_kN > 1400.0);
}
```

---

## 🔁 Fluxo de Desenvolvimento TDD Recomendado
1. **Red:** Crie o teste unitário em `tests/` com base nos valores teóricos esperados da norma/dissertação e veja o teste falhar.
2. **Green:** Escreva a implementação mínima necessária em `src/` e `include/` até que o teste passe.
3. **Refactor:** Melhore a clareza e a modularidade do código mantendo a suíte de testes 100% verde com `.\build\vmp_tests.exe`.
