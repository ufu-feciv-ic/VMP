# Guia Prático de Padrões de Projeto (Design Patterns) — VMP

Este guia é um **manual prático de desenvolvimento** para que você tenha total autonomia na escrita do código das próximas sprints do [ROADMAP.md](ROADMAP.md). 

Aqui você aprenderá **quando usar**, **como estruturar** e **como codificar** cada *design pattern* utilizando o estilo de C++ simples, direto e em português adotado no projeto.

---

## Índice dos Padrões

1. [Static Factory Method (Método Fabril Estático)](#1-static-factory-method) — Criação de entidades e catálogos
2. [Value Object / DTO (Data Transfer Object)](#2-value-object--dto) — Retorno rico de dados de cálculo
3. [Funções Matemáticas Puras vs. Strategy Pattern](#3-funções-matemáticas-puras-vs-strategy-pattern) — Rotinas estáticas e o papel real da IRotinaVerificacao
4. [Notification Pattern (Padrão Notificação)](#4-notification-pattern) — Tratamento de erros e reprovações normativas
5. [Template Method (Método Esqueleto)](#5-template-method) — Algoritmo padronizado de verificação
6. [Builder Pattern (Padrão Construtor Fluente)](#6-builder-pattern) — Montagem de seções compostas complexas
7. [Stage Accumulator / Pipeline](#7-stage-accumulator--pipeline) — Histórico de tensões por fases construtivas
8. [Checklist do Desenvolvedor: Como criar uma nova rotina do zero](#8-checklist-do-desenvolvedor)

---

## 1. Static Factory Method

### Quando usar?
Sempre que uma classe tiver **muitos parâmetros no construtor** e você quiser fornecer opções pré-configuradas de catálogos comerciais ou normas técnicas (ex.: aços ASTM, perfis Gerdau, classes de concreto C25/C30, cabos de protensão padrão).

### Como estruturar no código?

#### No arquivo de cabeçalho (`.hpp`):
```cpp
// include/nucleo/concreto.hpp
#pragma once
#include <string>

namespace vmp::nucleo {

    class Concreto {
    public:
        std::string classe; // Ex: "C30"
        double fck;         // MPa (ex: 30.0)
        double Ecs;         // MPa (ex: 26838.0)
        double fctm;        // MPa (ex: 2.9)

        // Construtor geral
        Concreto(std::string classe, double fck, double Ecs);

        // --- STATIC FACTORY METHODS ---
        static Concreto c25();
        static Concreto c30();
        static Concreto c40();
    };

}
```

#### No arquivo de implementação (`.cpp`):
```cpp
// src/nucleo/concreto.cpp
#include "nucleo/concreto.hpp"
#include <cmath>

namespace vmp::nucleo {

    Concreto::Concreto(std::string classe, double fck, double Ecs)
        : classe(classe), fck(fck), Ecs(Ecs) {
        // fctm = 0.3 * fck^(2/3) segundo a NBR 6118
        this->fctm = 0.3 * std::pow(fck, 2.0 / 3.0);
    }

    Concreto Concreto::c25() {
        return Concreto("C25", 25.0, 24000.0);
    }

    Concreto Concreto::c30() {
        return Concreto("C30", 30.0, 26838.0);
    }

    Concreto Concreto::c40() {
        return Concreto("C40", 40.0, 31000.0);
    }

}
```

### Como consumir no restante do código ou no teste:
```cpp
// Muito legivel e sem "parametros magicos":
Concreto concreto = Concreto::c30();
```

---

## 2. Value Object / DTO (Data Transfer Object)

### Quando usar?
Sempre que uma rotina de cálculo produzir **múltiplos resultados parciais** que precisem ser inspecionados em testes ou apresentados em telas e relatórios (como $V_{pl}, k_v, \lambda_p, V_{Rd}$).

### Regras do DTO no VMP:
* Use uma `struct` simples;
* Mantenha os campos públicos com nomes em português claros e com a unidade anotada em comentários;
* Não coloque lógica complexa dentro do DTO; ele é apenas um "recipiente de dados".

### Exemplo Prático (Sprint 2 — Rotina D de Força Cortante):

```cpp
// include/normas/nbr8800_2008/rotina_d.hpp
#pragma once
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"

namespace vmp::normas::nbr8800_2008 {

    struct ResultadoRotinaD {
        double kv;       // Coeficiente de flambagem por cisalhamento
        double lambda;   // Esbeltez da alma (d1 / tw)
        double lambda_p; // Limite de escoamento plástico
        double lambda_r; // Limite de flambagem inelástica
        double Vpl_kN;   // Força cortante plástica (0.6 * d * tw * fy) em kN
        double VRd_kN;   // Força cortante resistente final em kN
    };

    class RotinaD {
    public:
        static ResultadoRotinaD calcular(
            const nucleo::SecaoI& secao,
            const nucleo::Aco& material,
            double espacamento_enrijecedores_a = 0.0 // 0.0 = sem enrijecedores
        );
    };

}
```

---

## 3. Funções Matemáticas Puras vs. Strategy Pattern

### A Regra de Ouro: Evite o "Inferno de Interfaces"
> [!WARNING]
> **NÃO crie uma interface para cada rotina (Rotina A, B, C, D, 1, 2...)!**
> Criar `IRotinaCortante`, `IRotinaCompressao`, etc., geraria dezenas de interfaces vazias e classes adaptadoras desnecessárias. Sub-rotinas normativas são **funções matemáticas analíticas puras**.

### Como implementar as Rotinas (A até H e 1 até 6):
Todas as rotinas devem ser implementadas como **classes simples com métodos estáticos puros**, exatamente como a [`RotinaA`](include/normas/nbr8800_2008/rotina_a.hpp):

```cpp
// include/normas/nbr8800_2008/rotina_d.hpp
namespace vmp::normas::nbr8800_2008 {

    class RotinaD {
    public:
        // Metodo estatico direto: recebe dados, retorna struct DTO
        static ResultadoRotinaD calcular(const nucleo::SecaoI& secao, const nucleo::Aco& aco, double a = 0.0);
    };

}
```

* **Vantagem:** Chamada limpa e direta em qualquer lugar:
  ```cpp
  ResultadoRotinaD res = RotinaD::calcular(perfil, aco);
  ```
* **Composição fácil:** Se a Rotina 2 precisa dos resultados das Rotinas A e B, ela simplesmente as chama diretamente, sem precisar de injeção de 5 ponteiros.

---

### Onde REALMENTE aplicar o Strategy Pattern?
O Strategy Pattern deve ser usado **apenas no nível macro** do elemento estrutural completo, através da interface [`IRotinaVerificacao`](include/normas/i_rotina_verificacao.hpp).

A aplicação (ou interface gráfica) só precisa de um ponto de entrada para rodar a verificação global da viga inteira:

```cpp
// include/normas/i_rotina_verificacao.hpp
#pragma once
#include "nucleo/relatorio_verificacao.hpp"

namespace vmp::normas {

    class IRotinaVerificacao {
    public:
        virtual ~IRotinaVerificacao() = default;

        // Metodo macro que roda todas as fases (ELU 1, 2, 3 e ELS)
        virtual nucleo::RelatorioVerificacao verificar_viga(...) = 0;
    };

}
```

E as implementações concretas de alto nível apenas orquestram suas respectivas rotinas estáticas:
* `VerificadorVMP_NBR8800_2008` chama internamente `nbr8800_2008::RotinaA::calcular`, `RotinaD::calcular`, etc.
* `VerificadorVMP_NBR8800_2024` chamaria internamente as rotinas da versão 2024.

---

## 4. Notification Pattern (Padrão Notificação)

### Quando usar?
Sempre que você precisar **validar se uma peça atende à norma**, sem lançar exceções (`throw`). 

> [!IMPORTANT]
> **Por que NUNCA usar `throw` para limites normativos?**
> Se um perfil falhar em esbeltez ($\frac{KL}{r} > 200$) ou em momento ($M_{Sd} > M_{Rd}$), isso **não é um erro de software**, mas sim o resultado da verificação física da estrutura. Usar `throw` impediria algoritmos de otimização de testarem vários perfis e destruiria a geração de memórias de cálculo.

### Como estruturar no código?

#### 1. As Classes de Notificação:
```cpp
// include/nucleo/relatorio_verificacao.hpp
#pragma once
#include <string>
#include <vector>

namespace vmp::nucleo {

    enum class Severidade {
        Passou,  // Aprovado dentro dos limites
        Aviso,   // Ex: Seção esbelta com redução Q < 1.0 ou taxa > 95%
        Falha    // Reprovado por norma (ex: KL/r > 200 ou Sd > Rd)
    };

    struct ItemVerificacao {
        std::string item_norma; // Ex: "NBR 8800 - Item 5.3.4"
        std::string descricao;  // Ex: "Esbeltez global maxima de compressao"
        double solicitante;     // Valor calculado / atuante
        double limite;          // Valor resistente / admissivel
        Severidade severidade;
        std::string mensagem;   // Ex: "A esbeltez excede o limite de 200."
    };

    class RelatorioVerificacao {
    public:
        void registrar(ItemVerificacao item) {
            if (item.severidade == Severidade::Falha) {
                aprovado_ = false;
            }
            itens_.push_back(item);
        }

        bool esta_aprovado() const { return aprovado_; }
        const std::vector<ItemVerificacao>& itens() const { return itens_; }

    private:
        bool aprovado_ = true;
        std::vector<ItemVerificacao> itens_;
    };

}
```

#### 2. Como usar dentro de uma rotina:
```cpp
RelatorioVerificacao relatorio;

double esbeltez_global = (K * L) / r;
if (esbeltez_global > 200.0) {
    relatorio.registrar({
        "NBR 8800:5.3.4",
        "Esbeltez de barra comprimida",
        esbeltez_global,
        200.0,
        Severidade::Falha,
        "A barra nao atende ao limite maximo de esbeltez de 200."
    });
} else {
    relatorio.registrar({
        "NBR 8800:5.3.4",
        "Esbeltez de barra comprimida",
        esbeltez_global,
        200.0,
        Severidade::Passou,
        "Atende ao limite de esbeltez."
    });
}
```

---

## 5. Template Method (Método Esqueleto)

### Quando usar?
Quando a **sequência de passos** de um procedimento de verificação for sempre idêntica, mas as equações matemáticas de cada etapa possam variar.

### Exemplo: O Ciclo de Verificação de uma Viga
Em todas as normas, o fluxo de verificação de uma barra segue o mesmo esqueleto:
1. Validar geometria de entrada;
2. Classificar seção quanto à flambagem local;
3. Calcular esbeltez e redução por flambagem global;
4. Calcular o esforço resistente final;
5. Comparar solicitante com resistente.

```cpp
class VerificadorBarraBase {
public:
    virtual ~VerificadorBarraBase() = default;

    // METODO ESQUELETO (Nao e virtual - a ordem e imutavel!)
    ResultadoVerificacao verificar(const SecaoI& secao, const Aco& aco, double carga_sd) {
        validar_geometria(secao);
        double q = calcular_fator_local(secao, aco);
        double chi = calcular_fator_global(secao, aco, q);
        double rd = calcular_resistencia(secao, aco, q, chi);

        ResultadoVerificacao res;
        res.resistencia_rd = rd;
        res.aprovado = (carga_sd <= rd);
        return res;
    }

protected:
    // Passos customizaveis pelas subclasses de cada norma
    virtual void validar_geometria(const SecaoI& secao) = 0;
    virtual double calcular_fator_local(const SecaoI& secao, const Aco& aco) = 0;
    virtual double calcular_fator_global(const SecaoI& secao, const Aco& aco, double q) = 0;
    virtual double calcular_resistencia(const SecaoI& secao, const Aco& aco, double q, double chi) = 0;
};
```

---

## 6. Builder Pattern (Padrão Construtor Fluente)

### Quando usar?
Para montar objetos compostos e com muitas partes opcionais, como a **Viga Mista Protendida (VMP)**, que exige perfil metálico, laje, armaduras passivas, conectores e cabos de protensão.

### Como codificar em C++ simples (Method Chaining com referências):

```cpp
// include/nucleo/vmp_builder.hpp
#pragma once
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"
#include "nucleo/concreto.hpp"
#include "nucleo/laje.hpp"

namespace vmp::nucleo {

    class VigaMistaProtendida {
    public:
        SecaoI perfil;
        Aco material_aco;
        Concreto concreto;
        Laje laje;
        double forca_protensao_p0 = 0.0;
        bool escorada = false;

        VigaMistaProtendida(SecaoI perfil, Aco aco, Concreto conc, Laje laje)
            : perfil(perfil), material_aco(aco), concreto(conc), laje(laje) {}
    };

    class VigaMistaProtendidaBuilder {
    private:
        SecaoI perfil_ = SecaoI::w360x57_8();
        Aco aco_ = Aco::a572_gr50();
        Concreto concreto_ = Concreto::c30();
        Laje laje_ = Laje::macica(130.0);
        double p0_ = 0.0;
        bool escorada_ = false;

    public:
        VigaMistaProtendidaBuilder& com_perfil(SecaoI perfil) {
            this->perfil_ = perfil;
            return *this; // Retorna referencia para encadeamento
        }

        VigaMistaProtendidaBuilder& com_concreto(Concreto conc) {
            this->concreto_ = conc;
            return *this;
        }

        VigaMistaProtendidaBuilder& com_laje(Laje laje) {
            this->laje_ = laje;
            return *this;
        }

        VigaMistaProtendidaBuilder& com_protensao(double p0) {
            this->p0_ = p0;
            return *this;
        }

        VigaMistaProtendidaBuilder& eh_escorada(bool escorada) {
            this->escorada_ = escorada;
            return *this;
        }

        VigaMistaProtendida construir() {
            VigaMistaProtendida viga(perfil_, aco_, concreto_, laje_);
            viga.forca_protensao_p0 = p0_;
            viga.escorada = escorada_;
            return viga;
        }
    };

}
```

### Como consumir:
```cpp
auto viga = VigaMistaProtendidaBuilder()
    .com_perfil(SecaoI::w360x57_8())
    .com_concreto(Concreto::c30())
    .com_protensao(400.0) // 400 kN
    .eh_escorada(false)
    .construir();
```

---

## 7. Stage Accumulator / Pipeline (Fases Construtivas)

### Quando usar?
Exclusivo para **Vigas Mistas Protendidas (VMP)**. Na viga mista protendida, as tensões finais em cada fibra da seção transversal são o somatório acumulado dos acréscimos de cada fase da vida útil:
$$\sigma_{total} = \Delta \sigma_{Fase 1} + \Delta \sigma_{Fase 2} + \Delta \sigma_{Fase 3}$$

### Como estruturar no código:

```cpp
// Representa as tensoes normais nas fibras criticas da secao
struct EstadoTensoes {
    double fibra_sup_concreto = 0.0; // MPa
    double mesa_sup_aco = 0.0;       // MPa
    double mesa_inf_aco = 0.0;       // MPa
    double cabo_protensao = 0.0;     // MPa

    // Metodo acumulador
    void somar_incremento(const EstadoTensoes& delta) {
        fibra_sup_concreto += delta.fibra_sup_concreto;
        mesa_sup_aco += delta.mesa_sup_aco;
        mesa_inf_aco += delta.mesa_inf_aco;
        cabo_protensao += delta.cabo_protensao;
    }
};

// Interface de uma fase construtiva
class IFaseConstrutiva {
public:
    virtual ~IFaseConstrutiva() = default;
    virtual EstadoTensoes calcular_incremento_tensoes(...) = 0;
};
```

E no orquestrador:
```cpp
EstadoTensoes estado_atual;

// Fase 1: Perfil de aco isolado sob peso proprio e protensao inicial
EstadoTensoes delta1 = fase1.calcular_incremento_tensoes(...);
estado_atual.somar_incremento(delta1);

// Fase 2: Concreto curado recebendo cargas de servico (secao mista t=0)
EstadoTensoes delta2 = fase2.calcular_incremento_tensoes(...);
estado_atual.somar_incremento(delta2);

// Fase 3: Efeitos diferidos de fluencia e relaxacao (t=inf)
EstadoTensoes delta3 = fase3.calcular_incremento_tensoes(...);
estado_atual.somar_incremento(delta3);
```

---

## 8. Checklist do Desenvolvedor

Sempre que você for iniciar uma nova tarefa do [ROADMAP.md](ROADMAP.md) (por exemplo, a **Sprint 2: Rotina D - Força Cortante**), siga rigorosamente estes 5 passos:

### Passo 1: Escreva o Teste Unitário Primeiro (Red)
Crie o arquivo de teste `tests/normas/nbr8800_2008/test_rotina_d.cpp`:
```cpp
#include "test_framework.hpp"
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"
#include "normas/nbr8800_2008/rotina_d.hpp"

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina D - Forca Cortante do W 360x57.8 sem enrijecedores") {
    SecaoI perfil = SecaoI::w360x57_8();
    Aco aco = Aco::a572_gr50();

    // Na Tabela 13 da dissertacao: VRd = 532.22 kN
    ResultadoRotinaD res = RotinaD::calcular(perfil, aco, 0.0);
    
    double diff = std::abs(res.VRd_kN - 532.22) / 532.22 * 100.0;
    ASSERT_TRUE(diff < 0.5); // Diferenca menor que 0.5%
}
```

### Passo 2: Crie a Assinatura no Cabeçalho
Crie `include/normas/nbr8800_2008/rotina_d.hpp` com a `struct` de resultado (DTO) e a classe com método estático.

### Passo 3: Implemente as Fórmulas
Crie `src/normas/nbr8800_2008/rotina_d.cpp` com as fórmulas do fluxograma do Apêndice A do documento (página 188 do PDF: $k_v, \lambda_p, \lambda_r, V_{pl}, V_{Rd}$).

### Passo 4: Compile o Projeto
No terminal:
```powershell
cmake --build build
```
*(Lembre-se: o CMake detecta os arquivos novos automaticamente sem precisar mexer no `CMakeLists.txt`!)*

### Passo 5: Rode a Suíte de Testes (Green)
```powershell
.\build\vmp_tests.exe
```
Se o teste passar, você concluiu com sucesso o ciclo TDD com código modular, limpo e testado!
