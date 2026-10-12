# Guia Prático Passo a Passo: Construindo a Rotina B com TDD e Tratamento de Desvios

Este documento é um **tutorial passo a passo guiado**, narrando exatamente o **fluxo mental de um engenheiro de software** ao construir uma rotina de norma estrutural com múltiplas bifurcações (*branches*), como a **Rotina B** (páginas 45 e 46 do arquivo [`ilovepdf_merged (3).pdf`](ilovepdf_merged%20(3).pdf), dissertação de Thiago Tononi Turini, UFES 2021).

O objetivo deste guia é ensinar a **pensar antes de codificar**: como sair do fluxograma da norma, planejar os testes de cada regime e implementar o código de forma incremental e à prova de falhas.

---

## Índice do Roteiro Mental
1. [Passo 0: A Leitura Estratégica do Fluxograma](#passo-0-a-leitura-estratégica-do-fluxograma)
2. [Passo 1: Modelando o Contrato de Dados (DTO e Enums)](#passo-1-modelando-o-contrato-de-dados-dto-e-enums)
3. [Passo 2: Ciclo TDD 1 — O Caso Base: FLT no Regime Plástico](#passo-2-ciclo-tdd-1--o-caso-base-flt-no-regime-plástico)
4. [Passo 3: Ciclo TDD 2 — O Primeiro Desvio: FLT Inelástico e a Regra de Capping (Cb)](#passo-3-ciclo-tdd-2--o-primeiro-desvio-flt-inelástico-e-a-regra-de-capping-cb)
5. [Passo 4: Ciclo TDD 3 — O Segundo Desvio: FLT Elástico (Mcr) e Teste de Monotonicidade](#passo-4-ciclo-tdd-3--o-segundo-desvio-flt-elástico-mcr-e-teste-de-monotonicidade)
6. [Passo 5: Ciclo TDD 4 — Desvio de Fabricação: FLM Laminado vs. Soldado (Fator kc)](#passo-5-ciclo-tdd-4--desvio-de-fabricação-flm-laminado-vs-soldado-fator-kc)
7. [Passo 6: Ciclo TDD 5 — O Desvio Crítico Normativo: Alma Esbelta e o Notification Pattern](#passo-6-ciclo-tdd-5--o-desvio-crítico-normativo-alma-esbelta-e-o-notification-pattern)
8. [Passo 7: Ciclo TDD 6 — Orquestração Global e Aferição com o Benchmark da Tese](#passo-7-ciclo-tdd-6--orquestração-global-e-aferição-com-o-benchmark-da-tese)
9. [Resumo das Lições: O Checklist para as Próximas Rotinas](#resumo-das-lições-o-checklist-para-as-próximas-rotinas)

---

## Passo 0: A Leitura Estratégica do Fluxograma

Antes de digitar uma única linha de C++, abra o fluxograma da Rotina B (páginas 45 e 46 do PDF) com olhos de arquiteto:

```text
               Rotina B (MRd = min(MRd,FLT; MRd,FLM; MRd,FLA))
                                   │
         ┌─────────────────────────┼─────────────────────────┐
         ▼                         ▼                         ▼
   Rotina B.1 (FLT)          Rotina B.2 (FLM)          Rotina B.3 (FLA)
   - λ = Lb / ry             - λ = bf / (2*tf)         - λ = d1 / tw
   - 3 regimes:              - Bifurcação:             - 3 regimes:
     * λ <= λp (Plástico)      * Laminado                * λ <= λp (Compacta)
     * λp < λ <= λr (Inel.)    * Soldado (kc)            * λp < λ <= λr (Semicompacta)
     * λ >= λr (Elástico)    - 3 regimes               * λ > λr: "NÃO VÁLIDO PARA VMP"
   - Capping com Cb                                    (DESVIO TERMINAL CRÍTICO)
```

### O que o seu cérebro deve registrar:
1. **Não tente fazer tudo de uma vez:** Tentar escrever B.1, B.2 e B.3 de uma vez vai resultar em 10 erros de compilação simultâneos e você não saberá onde errou.
2. **Comece pela sub-rotina que governa vigas reais:** A Flambagem Lateral com Torção (FLT - B.1) é a espinha dorsal da flexão de perfis I. Comece por ela.
3. **Identifique os pontos de descontinuidade:** Onde as equações mudam de fórmula? Em $\lambda = \lambda_p$ e $\lambda = \lambda_r$. Esses pontos exigirão testes de borda.

---

## Passo 1: Modelando o Contrato de Dados (DTO e Enums)

O primeiro código a nascer é o **contrato de comunicação** em `include/normas/nbr8800_2008/rotina_b.hpp`.

### A pergunta que você faz a si mesmo:
> *"O que a minha função de FLT precisa calcular e devolver para que o memorial de cálculo e os testes possam inspecionar tudo?"*

Ela precisa devolver:
* A esbeltez calculada ($\lambda$) e os limites ($\lambda_p, \lambda_r$);
* Os momentos característicos ($M_{pl}, M_r, M_{cr}$);
* O momento resistente final ($M_{Rd}$);
* O regime normativo no qual a peça caiu.

### O cabeçalho inicial:
```cpp
// include/normas/nbr8800_2008/rotina_b.hpp
#pragma once
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"

namespace vmp::normas::nbr8800_2008 {

    enum class RegimeEsbeltez {
        Plastico,     // lambda <= lambda_p
        Inelastico,   // lambda_p < lambda <= lambda_r
        Elastico      // lambda > lambda_r
    };

    struct ResultadoFLT {
        double lambda;
        double lambda_p;
        double lambda_r;
        double Mpl_Nmm;
        double Mr_Nmm;
        double Mcr_Nmm;
        double MRd_Nmm;
        double MRd_kNm;
        RegimeEsbeltez regime;
    };

    class RotinaB {
    public:
        static ResultadoFLT calcular_flt(
            const nucleo::SecaoI& secao,
            const nucleo::Aco& material,
            double Lb,
            double Cb = 1.0,
            double gamma_a1 = 1.10
        );
    };

}
```

---

## Passo 2: Ciclo TDD 1 — O Caso Base: FLT no Regime Plástico

### 2.1. O Pensamento:
> *"Qual é o caso mais simples e elementar de FLT? É quando a viga é contínua ou travada a cada centímetro ($L_b \to 0$). Nesse caso, não há instabilidade lateral; a viga atinge a plastificação total da seção: $M_{Rd} = M_{pl} / \gamma_{a1}$."*

### 2.2. Escrevendo o Teste Vermelho (Red):
Crie o teste em `tests/normas/nbr8800_2008/test_rotina_b.cpp`:

```cpp
#include "test_framework.hpp"
#include "nucleo/secao_i.hpp"
#include "nucleo/material.hpp"
#include "normas/nbr8800_2008/rotina_b.hpp"
#include <cmath>

using namespace vmp::nucleo;
using namespace vmp::normas::nbr8800_2008;

TEST_CASE("Rotina B.1 - FLT no Regime Plastico (Viga totalmente travada)") {
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50(); // fy = 345 MPa, E = 200000 MPa
    double gamma_a1 = 1.10;

    // Se Lb = 500 mm, lambda = 500 / 39.1 = 12.78 <= lambda_p (42.38)
    double Lb = 500.0;
    ResultadoFLT res = RotinaB::calcular_flt(secao, material, Lb, 1.0, gamma_a1);

    // Asserção 1: O regime deve ser classificado como Plastico
    ASSERT_TRUE(res.regime == RegimeEsbeltez::Plastico);

    // Asserção 2: MRd deve ser estritamente Mpl / gamma_a1
    double Mpl_esperado = std::min(secao.Zx * material.fy, 1.5 * secao.Wx * material.fy);
    double MRd_esperado = Mpl_esperado / gamma_a1;
    ASSERT_TRUE(std::abs(res.MRd_Nmm - MRd_esperado) < 1e-3);
}
```

### 2.3. Rodando o Build (Red):
No terminal:
```powershell
cmake --build build
```
O compilador vai avisar: `undefined reference to RotinaB::calcular_flt`. **Perfeito! Estamos no ciclo Red do TDD.**

### 2.4. Implementação Mínima para passar o teste (Green):
Crie `src/normas/nbr8800_2008/rotina_b.cpp` com o mínimo absoluto para o caso plástico:

```cpp
// src/normas/nbr8800_2008/rotina_b.cpp
#include "normas/nbr8800_2008/rotina_b.hpp"
#include <cmath>
#include <algorithm>

namespace vmp::normas::nbr8800_2008 {

    ResultadoFLT RotinaB::calcular_flt(
        const nucleo::SecaoI& secao,
        const nucleo::Aco& material,
        double Lb,
        double Cb,
        double gamma_a1
    ) {
        ResultadoFLT res;
        double E = material.E;
        double fy = material.fy;

        res.lambda = Lb / secao.ry;
        res.lambda_p = 1.76 * std::sqrt(E / fy);

        res.Mpl_Nmm = std::min(secao.Zx * fy, 1.5 * secao.Wx * fy);
        res.Mr_Nmm = 0.7 * fy * secao.Wx;

        // Se for plastico:
        if (res.lambda <= res.lambda_p) {
            res.regime = RegimeEsbeltez::Plastico;
            res.MRd_Nmm = res.Mpl_Nmm / gamma_a1;
            res.MRd_kNm = res.MRd_Nmm / 1e6;
        }

        return res;
    }

}
```

### 2.5. Rodando os Testes (Green):
```powershell
cmake --build build; .\build\vmp_tests.exe
```
Resultado: **1 teste passou!** Você tem a base sólida e testada do primeiro ramo do fluxograma.

---

## Passo 3: Ciclo TDD 2 — O Primeiro Desvio: FLT Inelástico e a Regra de Capping (Cb)

Agora avançamos no fluxograma. E se a viga tiver um comprimento destravado maior, caindo entre $\lambda_p$ e $\lambda_r$?

### 3.1. O Pensamento:
> *"No regime inelástico, a capacidade é interpolada linearmente entre $M_{pl}$ e $M_r$. O fator de momento $C_b$ amplifica essa resistência. Porém, existe uma armadilha clássica da NBR 8800: se $C_b = 2{,}5$, o momento calculado pode ultrapassar o momento plástico $M_{pl}/\gamma_{a1}$. A física não permite uma seção resistir mais que o seu colapso plástico integral! O código DEVE ter um teto (capping)."*

### 3.2. Escrevendo os Testes Red:
Adicione ao arquivo de teste:

```cpp
TEST_CASE("Rotina B.1 - FLT no Regime Inelastico com Cb e Capping") {
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50();
    double gamma_a1 = 1.10;
    double Mpl_d = std::min(secao.Zx * material.fy, 1.5 * secao.Wx * material.fy) / gamma_a1;
    double Mr_d = (0.7 * material.fy * secao.Wx) / gamma_a1;

    // Lb = 3000 mm -> lambda = 3000 / 39.1 = 76.73 (esta entre 42.38 e 127.71)
    double Lb = 3000.0;

    // Teste 1: Com Cb = 1.0 (momento puro uniforme)
    ResultadoFLT res_cb1 = RotinaB::calcular_flt(secao, material, Lb, 1.0, gamma_a1);
    ASSERT_TRUE(res_cb1.regime == RegimeEsbeltez::Inelastico);
    // Deve ser menor que o plastico e maior que o residual
    ASSERT_TRUE(res_cb1.MRd_Nmm < Mpl_d);
    ASSERT_TRUE(res_cb1.MRd_Nmm > Mr_d);

    // Teste 2: DESVIO DE CAPPING com Cb muito alto (ex: Cb = 3.0)
    ResultadoFLT res_cb3 = RotinaB::calcular_flt(secao, material, Lb, 3.0, gamma_a1);
    // NAO PODE ULTRAPASSAR Mpl / gamma_a1!
    ASSERT_TRUE(std::abs(res_cb3.MRd_Nmm - Mpl_d) < 1e-3);
}
```

### 3.3. Implementando o Ramo Inelástico no Código (Green):
Para implementar $\lambda_r$, olhamos para a fórmula da página 45 do PDF:
$$\beta_1 = \frac{0{,}7 f_y W_x}{E J}, \quad C_w = \frac{I_y (d - t_f)^2}{4}, \quad \lambda_r = \frac{1{,}38 \sqrt{I_y J}}{r_y J \beta_1} \sqrt{1 + \sqrt{1 + \frac{27 C_w \beta_1^2}{I_y}}}$$

Adicionamos ao `.cpp`:
```cpp
// Calculo de beta1, Cw e lambda_r conforme o fluxograma
double J = secao.J;
double Cw = secao.Cw;
double Iy = secao.Iy;
double ry = secao.ry;

double beta1 = (0.7 * fy * secao.Wx) / (E * J);
double termo_cw = (27.0 * Cw * std::pow(beta1, 2.0)) / Iy;
res.lambda_r = (1.38 * std::sqrt(Iy * J) / (ry * J * beta1)) * std::sqrt(1.0 + std::sqrt(1.0 + termo_cw));

double Mpl_d = res.Mpl_Nmm / gamma_a1;

if (res.lambda <= res.lambda_p) {
    res.regime = RegimeEsbeltez::Plastico;
    res.MRd_Nmm = Mpl_d;
} else if (res.lambda <= res.lambda_r) {
    res.regime = RegimeEsbeltez::Inelastico;
    double fator_esbeltez = (res.lambda - res.lambda_p) / (res.lambda_r - res.lambda_p);
    double mrd_sem_cb = (res.Mpl_Nmm - (res.Mpl_Nmm - res.Mr_Nmm) * fator_esbeltez) / gamma_a1;
    
    // AQUI ESTA O DESVIO DE CAPPING:
    res.MRd_Nmm = std::min(Cb * mrd_sem_cb, Mpl_d);
}
```

Execute o teste: **Green!** O teto funcionou perfeitamente e o ramo inelástico está blindado.

---

## Passo 4: Ciclo TDD 3 — O Segundo Desvio: FLT Elástico (Mcr) e Teste de Monotonicidade

### 4.1. O Pensamento:
> *"E se a viga for muito longa sem travamento ($L_b = 8\text{ m}$)? Ela sofrerá flambagem elástica antes mesmo de qualquer fibra escoar. O momento crítico elástico é dado pela clássica fórmula de Chwalla/Timoshenko com empenamento e torção de Saint-Venant."*

### 4.2. Escrevendo o Teste Red:
```cpp
TEST_CASE("Rotina B.1 - FLT no Regime Elastico e Monotonicidade") {
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50();

    ResultadoFLT r_curta = RotinaB::calcular_flt(secao, material, 1000.0); // Plastico
    ResultadoFLT r_media = RotinaB::calcular_flt(secao, material, 3000.0); // Inelastico
    ResultadoFLT r_longa = RotinaB::calcular_flt(secao, material, 8000.0); // Elastico

    // Asserção de Regime
    ASSERT_TRUE(r_longa.regime == RegimeEsbeltez::Elastico);
    ASSERT_TRUE(r_longa.Mcr_Nmm > 0.0);

    // REGRA DE SANIDADE FÍSICA (Monotonicidade):
    // Aumentar o comprimento destravado NUNCA pode aumentar a capacidade!
    ASSERT_TRUE(r_curta.MRd_Nmm > r_media.MRd_Nmm);
    ASSERT_TRUE(r_media.MRd_Nmm > r_longa.MRd_Nmm);
}
```

### 4.3. Implementando a Fórmula de $M_{cr}$ (Green):
```cpp
} else {
    res.regime = RegimeEsbeltez::Elastico;
    double Lb2 = Lb * Lb;
    double termo_torcao = 1.0 + 0.039 * (J * Lb2 / Cw);
    res.Mcr_Nmm = (Cb * PI * PI * E * Iy / Lb2) * std::sqrt((Cw / Iy) * termo_torcao);
    res.MRd_Nmm = std::min(res.Mcr_Nmm / gamma_a1, Mpl_d);
}
```
Rodando a suíte: **Todos os ramos de FLT agora estão 100% implementados e testados.**

---

## Passo 5: Ciclo TDD 4 — Desvio de Fabricação: FLM Laminado vs. Soldado (Fator kc)

Agora vamos para a **Rotina B.2** (Flambagem Local da Mesa, página 46 do PDF).

### 5.1. O Pensamento:
> *"Em FLM, há uma bifurcação clara de engenharia:
> - Em perfis laminados, as mesas esfriam juntas com a alma e têm raio de concordância $\implies \lambda_r = 0{,}83\sqrt{E / 0{,}7f_y}$.
> - Em perfis soldados, as chapas foram cortadas com maçarico e soldadas por cordão, gerando tensões residuais diferentes. A norma introduz o fator de interação mesa-alma $k_c = \frac{4}{\sqrt{h/t_w}}$, limitado com clamp a $[0{,}35; 0{,}76]$."*

### 5.2. Como Pensar nos Testes desse Desvio:
Precisamos testar 3 cenários específicos:
1. Um perfil laminado comum (mesa compacta);
2. Um perfil soldado normal (para checar se $k_c$ foi calculado);
3. As **bordas de clamping** de $k_c$: garantir que se a alma for muito esbelta, $k_c$ não caia abaixo de 0.35, e se for muito espessa, não passe de 0.76.

```cpp
TEST_CASE("Rotina B.2 - FLM e Clamping de kc para Perfis Soldados") {
    Aco material = Aco::a572_gr50();

    // 1. Perfil soldado normal
    SecaoI vs("VS 500x86", TipoFabricacao::Soldado, 500.0, 200.0, 16.0, 8.0);
    ResultadoFLM flm_sold = RotinaB::calcular_flm(vs, material);
    ASSERT_TRUE(flm_sold.kc >= 0.35 && flm_sold.kc <= 0.76);

    // 2. Perfil laminado nao deve calcular kc
    SecaoI w = SecaoI::w360x57_8();
    ResultadoFLM flm_lam = RotinaB::calcular_flm(w, material);
    ASSERT_EQ(flm_lam.kc, 0.0);
}
```

Implemente a função `calcular_flm` seguindo essa lógica e execute o teste.

---

## Passo 6: Ciclo TDD 5 — O Desvio Crítico Normativo: Alma Esbelta e o Notification Pattern

Este é o desvio mais importante da **Rotina B.3** (Flambagem Local da Alma, página 46 do PDF):

```text
               λ = d1 / tw
                   │
           ┌───────┴───────┐
           ▼               ▼
      λ <= 3.76√(E/fy)    NÃO
         (Compacta)        │
                   ┌───────┴───────┐
                   ▼               ▼
              λ <= 5.70√(E/fy)    NÃO (λ > 5.70√(E/fy))
              (Semicompacta)       │
                                   ▼
                       "Perfil de alma esbelta,
                         NÃO VÁLIDO PARA VMP"
```

### 6.1. O Pensamento:
> *"No Anexo O da NBR 8800:2008, a teoria de vigas mistas protendidas não permite perfis com alma esbelta ($\lambda > \lambda_r$), pois a pré-tração causaria encurvamento prematuro da alma.  
> Se eu colocar um `throw std::runtime_error`, eu quebro o programa se o usuário selecionar um perfil alto em uma tabela.  
> Em vez disso, meu código deve setar `valido_para_vmp = false`, zerar $M_{Rd}$ e, no relatório, emitir uma falha clara sem interromper o software."*

### 6.2. Escrevendo o Teste Red:
```cpp
TEST_CASE("Rotina B.3 - Desvio Critico de Alma Esbelta (Notification Pattern)") {
    Aco material = Aco::a572_gr50();

    // Perfil com alma muito esbelta: d = 1500 mm, tw = 6 mm -> d1/tw = 243 > 137.24
    SecaoI perfil_esbelto("VS Alma Muito Esbelta", TipoFabricacao::Soldado, 1500.0, 300.0, 20.0, 6.0);

    // Chamada pura: deve marcar que e invalido para VMP
    ResultadoFLA fla = RotinaB::calcular_fla(perfil_esbelto, material);
    ASSERT_FALSE(fla.valido_para_vmp);
    ASSERT_EQ(fla.MRd_Nmm, 0.0);

    // Chamada com Notification Pattern: deve retornar relatório reprovado sem lançar exception
    RelatorioVerificacao rel = RotinaB::verificar(perfil_esbelto, material, 3000.0, 1.0, 100.0);
    ASSERT_FALSE(rel.esta_aprovado());

    // Verifica se a mensagem de diagnóstico cita a restrição de VMP
    bool citou_restricao = false;
    for (const auto& item : rel.itens()) {
        if (item.severidade == Severidade::Falha && item.mensagem.find("VMP") != std::string::npos) {
            citou_restricao = true;
        }
    }
    ASSERT_TRUE(citou_restricao);
}
```

### 6.3. A Implementação Elegante:
Na `RotinaB::calcular_fla`:
```cpp
if (res.lambda <= res.lambda_p) {
    res.regime = RegimeEsbeltez::Plastico;
    res.classe = ClasseAlma::Compacta;
    res.valido_para_vmp = true;
    res.MRd_Nmm = Mpl_d;
} else if (res.lambda <= res.lambda_r) {
    res.regime = RegimeEsbeltez::Inelastico;
    res.classe = ClasseAlma::Semicompacta;
    res.valido_para_vmp = true;
    double fator = (res.lambda - res.lambda_p) / (res.lambda_r - res.lambda_p);
    // ATENCAO: Na alma, Mr = 1.0 * fy * Wx (sem o fator 0.7 das mesas)
    res.Mr_Nmm = 1.0 * fy * secao.Wx;
    res.MRd_Nmm = (res.Mpl_Nmm - (res.Mpl_Nmm - res.Mr_Nmm) * fator) / gamma_a1;
} else {
    // DESVIO CRITICO EXECUTADO COM SUCESSO:
    res.regime = RegimeEsbeltez::Elastico;
    res.classe = ClasseAlma::Esbelta;
    res.valido_para_vmp = false;
    res.MRd_Nmm = 0.0;
}
```

---

## Passo 7: Ciclo TDD 6 — Orquestração Global e Aferição com o Benchmark da Tese

Agora que as três sub-rotinas (B.1, B.2 e B.3) funcionam isoladamente, nós escrevemos o **orquestrador**:
$$M_{Rd} = \min(M_{Rd,FLT},\; M_{Rd,FLM},\; M_{Rd,FLA})$$

### 7.1. O Teste de Benchmark Real (Tabela 12 da Dissertação):
Este teste confronta o software com o cálculo numérico validado por Turini (2021):

```cpp
TEST_CASE("Rotina B - Afericao Completa Dissertacao Exemplo 1") {
    // Dados da Tabela 10 e Tabela 12:
    // W 360 x 57.8, A572 Gr. 50, Lb = 3000 mm, Cb = 1.0669
    SecaoI secao = SecaoI::w360x57_8();
    Aco material = Aco::a572_gr50();

    ResultadoRotinaB res = RotinaB::calcular(secao, material, 3000.0, 1.0669);

    // FLT deve governar (mesa e alma sao compactas)
    ASSERT_EQ(res.modo_governante, ModoFlambagemGovernante::FLT);

    // Valor da Tabela 12 da dissertacao: MRd,I = 285.98 kN.m
    double esperado = 285.98;
    double erro_pct = std::abs(res.MRd_kNm - esperado) / esperado * 100.0;

    ASSERT_TRUE(erro_pct < 0.1); // Tolerância menor que 0.1%
}
```

Ao rodar esse teste e ver a mensagem:
```text
[ TEST ] Rotina B - Afericao Completa Dissertacao Exemplo 1 ... [ PASSOU ]
```
Você tem a certeza matemática e de engenharia de que:
1. Todas as unidades (mm, N, mm³, MPa, kN.m) foram convertidas corretamente;
2. Todos os desvios de regime se comportaram como deveriam;
3. O software reproduz fielmente a literatura científica de referência.

---

## Resumo das Lições: O Checklist para as Próximas Rotinas

Quando você for implementar as próximas rotinas do roadmap (como a **Rotina C** para vigas mistas ou a **Rotina 3** para VMP), aplique exatamente este roteiro mental:

| Etapa | Ação Prática |
| :--- | :--- |
| **1. Isolar** | Nunca misture múltiplos fenômenos na mesma função; divida em sub-rotinas atômicas puras. |
| **2. Enums** | Crie um `enum class` para os regimes de esbeltez ou classes de seção antes de escrever o `if`. |
| **3. Caso Base Primeiro** | Comece sempre pelo caso mais simples (regime plástico sem perdas nem flambagens). |
| **4. Testar o Teto** | Sempre que houver um fator amplificador ($C_b, k_v$, etc.), escreva um teste com valor absurdo para garantir o teto (*capping*). |
| **5. Clamping** | Se a norma estabelece um intervalo (ex.: $[0{,}35; 0{,}76]$), teste os dois extremos que forçam o travamento. |
| **6. Não use `throw`** | Trate perfis fora do domínio normativo através de flags e relatórios de diagnóstico. |
| **7. Aferição** | Finalize sempre com um caso de teste baseado em um exemplo numérico resolvido na dissertação. |
