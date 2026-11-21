# Plano Diretor: Finalização do Software VMP e Escrita do TCC
**Tema:** Desenvolvimento de Programa para Verificação de Vigas Mistas de Aço e Concreto Protendidas (VMP): Aferição com Turini (2021) e Atualização para a ABNT NBR 8800:2024.

---

## 1. Visão Geral e Diagnóstico do Projeto

O projeto consiste na reprodução moderna em **C++20** (com interface gráfica em **Dear ImGui + Raylib** e testes unitários automatizados) da metodologia de verificação de Vigas Mistas Protendidas (VMP) proposta por **Thiago Tononi Turini (2021)** e **Karla Demoner Ribeiro (2018)**, originalmente formulada sobre a **ABNT NBR 8800:2008** e **ABNT NBR 6118:2014**, atualizando-a para a nova edição da **ABNT NBR 8800:2024**.

### 1.1. O que já está pronto e validado (`nbr8800::v2008`)
- [x] **Modelagem de Dados e Catálogo (`include/modelos.h`, `include/catalogo.h`, `src/geometria.cpp`)**:
  - Estruturas completas para `PerfilI` (laminado e soldado), `Aco`, `Concreto`, `Laje` (maciça e *steel deck*), `ArmacaoLaje`, `CaboProtensao`, `ConectorCisalhamento`, `CargasVMP` e `VigaVMP`.
  - Cálculo automático das propriedades geométricas do perfil I ($A_g, I_x, W_x, Z_x, r_x, I_y, W_y, Z_y, r_y, I_t, C_w, r_t, \lambda_f, \lambda_w$).
  - Presets prontos para os **Exemplos 1 e 2 de Turini (2021)**.
- [x] **Rotinas Base do Perfil e Viga Mista (`src/rotinas_nbr8800.cpp`)**:
  - **Rotina A**: Força normal de compressão resistente do perfil de aço ($N_{Rd,I}$);
  - **Rotina B**: Momento fletor resistente do perfil de aço ($M_{Rd,I}$ — FLT, FLM e FLA);
  - **Rotina C**: Momento resistente positivo ($M_{Rd,VM}^+$ compacta e semicompacta) e negativo ($M_{Rd,VM}^-$) de viga mista convencional (sem protensão);
  - **Rotina D**: Força cortante resistente do perfil de aço ($V_{Rd}$).
- [x] **Rotinas Especializadas de VMP (`src/rotinas_vmp.cpp`)**:
  - **Rotina 1**: Propriedades da seção mista transformada/homogeneizada em $t = 0$ ($n$) e $t = \infty$ ($3n$);
  - **Rotina 2**: Estimativa da força máxima de protensão inicial $P_{0,\text{máx}}$ (critérios NBR 8800 e NBR 6118 para pré-tração e pós-tração);
  - **Rotina 3.1 e 3.2**: Momento fletor resistente positivo da VMP ($M_{Rd,VMP}^+$ para seção compacta e semicompacta);
  - **Rotina 3.3**: Momento fletor resistente negativo da VMP ($M_{Rd,VMP}^-$ considerando plastificação e Flambagem Lateral com Distorção — FLD);
  - **Rotina 4**: Dimensionamento da armadura de protensão ($A_p$ e número de cordoalhas);
  - **Rotina 5**: Perdas imediatas por acomodação da ancoragem (cabo reto) e perdas progressivas por relaxação do aço ($\psi_{1000}$);
  - **Rotina 6**: Incremento hiperestático da força de protensão ($\Delta P$) para cabo reto;
  - **Rotina G**: Momento de inércia fissurado da laje por unidade de largura ($I_{\text{laje}}$ para laje maciça e laje mista com *steel deck*);
  - **Rotina H**: Tensão de cálculo no cabo de protensão no ELU ($\sigma_{pd}$).
- [x] **Verificador Multifásico Discretizado em $X$ Seções (`src/verificador.cpp`)**:
  - Geração da malha de $X$ seções ao longo de $[0, L]$;
  - Verificação completa do **ELU 1 (Construção)**, **ELU 2 (Serviço em $t=0$)** e **ELU 3 (Longa Duração em $t=\infty$)** para pré-tração e pós-tração.

---

## 2. Roteiro de Término do Código (Backlog de Desenvolvimento)

### Etapa 1 — Fechamento das Rotinas Complementares de Turini (2021)
1. **Rotina E — Conectores de Cisalhamento (`src/rotinas_nbr8800.cpp`)**:
   - Implementar `calcular_rotina_e_perfil_u` e adicionar suporte a pino com cabeça (*Stud Bolt*): cálculo de $Q_{Rd}$, força horizontal total $F_{hd}$ (incluindo o efeito da protensão quando aplicável), quantidade de conectores $n_l$ e espaçamento $e_l$.
2. **Rotina F — Armadura de Costura (`src/rotinas_nbr8800.cpp`)**:
   - Implementar `calcular_rotina_f_armadura_costura`: tensão de cisalhamento longitudinal na laje ($H_{v,Sd} \le H_{v,Rd,\text{máx}}$) e taxa de armadura transversal $A_{s,\text{cos}}$.
3. **Traçado Poligonal do Cabo (`src/rotinas_vmp.cpp` e `src/verificador.cpp`)**:
   - Retirar de `[STAND-BY]` a **Rotina 5.1** (`calcularPerdaImediataTracadoPoligonal`): perda por atrito nos desviadores + acomodação da cunha nos 3 trechos ($[0, X_{ep}]$, $[X_{ep}, L-X_{ep}]$, $[L-X_{ep}, L]$);
   - Retirar de `[STAND-BY]` a **Rotina 6.2** (`calcularIncrementoDpTracadoPoligonal`) e a variação linear da excentricidade $e_p(x)$ nos trechos inclinados em `gerarSecoesViga`.
4. **Verificações em Estado Limite de Serviço — ELS (Opcional / Recomendado)**:
   - Verificação das tensões elásticas nas fibras superior e inferior do aço e do concreto nas Fases 1, 2 e 3 (combinações rara, frequente e quase permanente);
   - Verificação de flechas (contraflecha de protensão e flecha final em serviço).

### Etapa 2 — Módulo Normativo ABNT NBR 8800:2024 (`nbr8800::v2024`)
Aproveitando que o código já está encapsulado em `namespace nbr8800 { inline namespace v2008 { ... } }`, criar o módulo `namespace nbr8800::v2024` permitindo execução e comparação direta entre as duas edições:
1. **Seletor de Norma em `VigaVMP` (`include/modelos.h`)**:
   - `enum EdicaoNBR8800 { NBR8800_2008, NBR8800_2024 };`
2. **Mapeamento e Implementação das Alterações da NBR 8800:2024**:
   - **Compressão Axial (Rotina A)**: Atualização do método de consideração da flambagem local (larguras/áreas efetivas em substituição ao fator $Q = Q_s Q_a$ tradicional) e limites de esbeltez $\lambda_{p}, \lambda_{r}$;
   - **Flexão Simples e Instabilidade Lateral/Local (Rotina B)**: Revisão das formulações de FLT, FLM e FLA conforme a edição 2024;
   - **Força Cortante (Rotina D)**: Revisão dos coeficientes e limites de plastificação/flambagem por cisalhamento da alma;
   - **Vigas Mistas e Conectores (Rotinas C, E e 3)**: Atualização dos critérios do Anexo de Vigas Mistas da NBR 8800:2024 (fatores de redução de conectores em fôrma de aço incorporada, limites de esbeltez da alma para plastificação total/parcial e flambagem lateral com distorção — FLD).
3. **Comparador Automático (`comparar_normas_vmp`)**:
   - Função no verificador que roda a mesma `VigaVMP` em `v2008` e `v2024` e calcula as diferenças absolutas e percentuais ($\Delta\%$) em cada rotina e seção.

### Etapa 3 — Interface Gráfica (`gui/app_gui.cpp`) e Gerador de Tabelas (`src/main.cpp`)
1. **Atualização Completa do `gui/app_gui.cpp`**:
   - Botões de carregamento instantâneo: *"Exemplo 1 - Turini (2021)"* e *"Exemplo 2 - Turini (2021)"*;
   - Chave seletora da norma: `ABNT NBR 8800:2008`, `ABNT NBR 8800:2024` e `Comparativo 2008 vs. 2024`;
   - Abas de entrada para todos os componentes de `VigaVMP` (Perfil/Aço, Laje/Concreto/Armação, Cabo de Protensão, Vão/Carregamentos);
   - Painel de resultados com abas para **Resumo Geral**, **Fase 1 (ELU 1)**, **Fase 2 (ELU 2 - $t=0$)**, **Fase 3 (ELU 3 - $t=\infty$)** e tabela seção a seção ($x \in [0, L]$);
   - Desenho 2D no Raylib mostrando a seção mista (Perfil I + Laje + Cabo + Linha Neutra) e o esquema longitudinal da viga com o traçado do cabo e diagramas de momento.
2. **Gerador de Tabelas para o TCC (`src/main.cpp`)**:
   - Configurar o `vmp_demo.exe` para imprimir no console (e exportar em `.md` / `.csv` / `.tex`) todas as tabelas de aferição e comparação 2008 vs. 2024 dos estudos de caso.

---

## 3. Plano de Escrita do TCC (Estrutura Sugerida da Monografia)

### Capítulo 1 — Introdução
- **1.1. Contextualização**: O uso de estruturas mistas de aço e concreto e o ganho estrutural proporcionado pela protensão externa (aumento de vão livre, redução de altura útil, controle de flechas e tensões).
- **1.2. Justificativa**: A publicação da nova **ABNT NBR 8800:2024** após 16 anos da edição de 2008 e a necessidade de atualizar as ferramentas computacionais e metodologias de projeto desenvolvidas em pesquisas anteriores (Ribeiro, 2018; Turini, 2021).
- **1.3. Objetivos**:
  - *Objetivo Geral*: Desenvolver e validar um programa computacional em C++ para verificação de vigas mistas protendidas, atualizado para a ABNT NBR 8800:2024.
  - *Objetivos Específicos*:
    1. Reproduzir e aferir as rotinas de cálculo de Turini (2021) segundo a NBR 8800:2008;
    2. Mapear e implementar as alterações introduzidas pela NBR 8800:2024 nas verificações do perfil metálico e da seção mista;
    3. Desenvolver uma interface gráfica interativa para entrada de dados, visualização geométrica e inspeção da memória de cálculo seção a seção;
    4. Realizar um estudo comparativo entre as edições de 2008 e 2024 nos exemplos de referência (pré-tração e pós-tração).
- **1.4. Organização do Trabalho**.

### Capítulo 2 — Fundamentação Teórica e Normativa
- **2.1. Vigas Mistas de Aço e Concreto**: Largura efetiva ($b_{\text{ef}}$), homogeneização de seções ($t=0$ e fluência em $t=\infty$), interação completa e parcial, conectores de cisalhamento.
- **2.2. Protensão Externa em Vigas Mistas**:
  - Métodos construtivos: Pré-tração (no perfil metálico isolado) vs. Pós-tração (na seção mista);
  - Geometria dos cabos (traçado reto e poligonal com desviadores);
  - Limites da força de protensão inicial ($P_{0,\text{máx}}$ pela NBR 8800 e NBR 6118);
  - Perdas imediatas (atrito e acomodação de cunha) e perdas progressivas (relaxação do aço de protensão);
  - Efeito hiperestático de autoequilíbrio: incremento de força no cabo ($\Delta P$) devido ao carregamento externo.
- **2.3. Verificações no Estado Limite Último (ELU) por Fases**:
  - Fase 1 (Construção), Fase 2 (Operação imediata $t=0$) e Fase 3 (Operação em longa duração $t=\infty$);
  - Momento fletor resistente positivo ($M_{Rd,VMP}^+$) e negativo ($M_{Rd,VMP}^-$ e Flambagem Lateral com Distorção — FLD).
- **2.4. Principais Alterações da ABNT NBR 8800:2024 em relação à NBR 8800:2008**:
  - Quadro síntese comparativo das equações e critérios alterados na edição 2024.

### Capítulo 3 — Metodologia e Desenvolvimento do Software
- **3.1. Arquitetura do Sistema**: Linguagem C++20, organização modular (`modelos`, `geometria`, `rotinas_nbr8800`, `rotinas_vmp`, `verificador`) e coexistência dos namespaces `nbr8800::v2008` e `nbr8800::v2024`.
- **3.2. Discretização Longitudinal da Viga**: Algoritmo de geração das $X$ seções transversais e classificação automática por trecho do cabo (ancoragem ativa, meio do vão e ancoragem passiva).
- **3.3. Fluxograma do Verificador Multifásico (`verificar_viga_vmp`)**: Detalhamento passo a passo das Fases 1, 2 e 3.
- **3.4. Interface Gráfica (`Dear ImGui` + `Raylib`) e Testes Automatizados**: Apresentação das telas do software e da metodologia de testes unitários de aferição.

### Capítulo 4 — Aferição do Programa e Estudo Comparativo (2008 vs. 2024)
- **4.1. Aferição com a Literatura (NBR 8800:2008)**:
  - *Estudo de Caso 1 (Turini, 2021 — Apêndice B)*: Viga pré-tracionada com cabo reto, laje maciça, perfil W 360 x 57,8 ($L = 12\text{ m}$). Tabelas comparativas mostrando concordância exata (desvio $< 0{,}15\%$);
  - *Estudo de Caso 2 (Turini, 2021 — Apêndice C)*: Viga pós-tracionada, laje mista (*steel deck*), perfil W 460 x 89 ($L = 15\text{ m}$).
- **4.2. Impacto da Atualização para a ABNT NBR 8800:2024**:
  - Comparação dos resultados dos Estudos de Caso 1 e 2 entre `v2008` e `v2024`;
  - Análise da distribuição de esforços solicitantes e resistentes ao longo das $X$ seções da viga;
  - Discussão crítica sobre como as mudanças da NBR 8800:2024 afetam a força máxima de protensão ($P_{0,\text{máx}}$), a resistência do perfil na construção e a capacidade última da VMP.

### Capítulo 5 — Conclusões e Sugestões para Trabalhos Futuros
- Síntese das conclusões técnicas sobre o software e sobre o impacto da NBR 8800:2024 no dimensionamento de VMP;
- Sugestões para trabalhos futuros (vigas contínuas, análise não-linear física/geométrica, otimização automática de perfis e excentricidade do cabo).

---

## 4. Cronograma de Execução Sugerido (Passo a Passo)

| Sprint / Etapa | Foco no Código (`VMP`) | Foco na Escrita do TCC | Entregável |
| :--- | :--- | :--- | :--- |
| **Sprint 1** | Finalizar Rotinas E, F e Traçado Poligonal (ou fechar escopo ELU cabo reto) em `v2008`. | Redigir **Capítulo 1 (Introdução)** e seções 2.1 a 2.3 do **Capítulo 2**. | Motor `v2008` 100% fechado + Cap. 1 e base do Cap. 2. |
| **Sprint 2** | Implementar `namespace nbr8800::v2024` e testes comparativos `v2008` vs `v2024`. | Redigir seção 2.4 (**Quadro Comparativo 2008 vs. 2024**) do **Capítulo 2**. | Motor `v2024` validado + Cap. 2 concluído. |
| **Sprint 3** | Atualizar a interface gráfica (`gui/app_gui.cpp`) e gerador de relatórios (`src/main.cpp`). | Redigir **Capítulo 3 (Metodologia e Software)** com capturas de tela e fluxogramas. | GUI finalizada + Cap. 3 concluído. |
| **Sprint 4** | Rodar os Estudos de Caso 1 e 2 (2008 vs. 2024) e exportar todas as tabelas e gráficos. | Redigir **Capítulo 4 (Resultados)** e **Capítulo 5 (Conclusões)** + revisão final. | Tabelas/gráficos finais + Monografia completa. |
