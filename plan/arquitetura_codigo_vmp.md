# ARQUITETURA E DOCUMENTAÇÃO COMPLETA DO CÓDIGO — PROJETO VMP

> **MANUAL TÉCNICO DE ARQUITETURA DE SOFTWARE, ESTRUTURAS DE DADOS, ROTINAS DE CÁLCULO (ABNT NBR 8800:2008 E 2024), VERIFICADOR MULTIFÁSICO, INTERFACE GRÁFICA E TESTES AUTOMATIZADOS**

---

# CAPÍTULO 1 — VISÃO GERAL E ARQUITETURA DO SISTEMA

## 1.1 Propósito e Escopo do Software

O sistema **VMP (Verificação de Vigas Mistas Protendidas)** é uma aplicação de engenharia estrutural desenvolvida em **C++20** para a modelagem, análise multifásica e verificação no Estado Limite Último (ELU) de vigas mistas de aço e concreto protendidas por cabos externos (em sistemas de pré-tração e pós-tração). O software reproduz e valida a metodologia analítica proposta por **Ribeiro (2018)** e **Turini (2021)** originalmente fundamentada na **ABNT NBR 8800:2008** e na **ABNT NBR 6118**, expandindo-a com um módulo normativo dedicado à nova **ABNT NBR 8800:2024**.

A arquitetura do projeto foi concebida sob o princípio de **desacoplamento estrito entre o motor matemático-normativo e as interfaces de usuário**. Dessa forma, o núcleo de cálculo (`MOTOR_SOURCES`) é compilado como uma biblioteca lógica pura, sem dependências gráficas, sendo consumido simultaneamente por três alvos executáveis distintos gerenciados pelo **CMake**:
1. **`vmp_demo`**: Executável de console (`src/main.cpp`) voltado à geração rápida de relatórios comparativos no terminal entre as edições de 2008 e 2024 da ABNT NBR 8800 para os estudos de caso de referência;
2. **`vmp_tests`**: Suíte de testes unitários automatizados (`tests/`) construída sobre um framework próprio de Desenvolvimento Orientado a Testes (TDD), responsável por aferir cada equação e rotina frente aos valores de referência da literatura;
3. **`vmp_gui`**: Aplicação desktop interativa em tempo real (`gui/app_gui.cpp`) construída com **Raylib** (renderização vetorial 2D estilo CAD) e **Dear ImGui / rlImGui** (painéis, tabelas de inspeção, modais e barra de ferramentas estilo Ribbon inspirada no software *NLtool*).

## 1.2 Organização de Diretórios e Arquivos

A estrutura física do repositório é dividida em cinco diretórios principais, conforme sintetizado na Tabela 1.1.

**Tabela 1.1 — Estrutura de diretórios e arquivos do projeto VMP**

| Arquivo / Caminho | Camada Arquitetural | Responsabilidade Principal |
| :--- | :--- | :--- |
| `CMakeLists.txt` | Build System | Configuração do compilador C++20 e definição dos 3 alvos (`vmp_demo`, `vmp_tests`, `vmp_gui`). |
| `include/modelos.h` | Domínio e Dados | Declaração de todos os `enum` e `struct` que representam a viga mista protendida (`VigaVMP`). |
| `include/catalogo.h` | Catálogo e Presets | Funções `inline` com perfis metálicos, materiais e dados completos dos Exemplos 1 e 2 de Turini (2021). |
| `include/geometria.h` | Motor Geométrico | Interface para o cálculo automático das propriedades geométricas e torcionais do Perfil I. |
| `src/geometria.cpp` | Motor Geométrico | Implementação analítica de $A_g, I_x, W_x, Z_x, r_x, I_y, W_y, Z_y, r_y, I_t, C_w, \lambda_f, \lambda_w$. |
| `include/rotinas_nbr8800.h` | Motor Normativo Base | Assinaturas e structs de retorno das Rotinas A, B, C, D, E e F (`v2008` e `v2024`). |
| `src/rotinas_nbr8800.cpp` | Motor Normativo Base | Implementação das resistências do perfil metálico isolado e da viga mista convencional (2008 e 2024). |
| `include/rotinas_vmp.h` | Motor Especializado VMP | Assinaturas e structs das Rotinas 1, 2, 3, 4, 5, 6, G e H exclusivas de vigas mistas protendidas. |
| `src/rotinas_vmp.cpp` | Motor Especializado VMP | Implementação da seção homogeneizada, $P_{0,\max}$, perdas, incremento $\Delta P$, tensão $\sigma_{pd}$ e $M_{Rd,VMP}$. |
| `include/verificador.h` | Orquestrador Multifásico | Estruturas de malha (`SecaoTransversalViga`), resultados por fase (`ResultadoFaseELU`) e `RelatorioVMP`. |
| `src/verificador.cpp` | Orquestrador Multifásico | Algoritmo de discretização em $X$ seções e fluxo completo das Fases 1, 2 ($t=0$) e 3 ($t=\infty$). |
| `src/main.cpp` | CLI / Console | Ponto de entrada do `vmp_demo` com quadros comparativos entre NBR 8800:2008 e NBR 8800:2024. |
| `gui/app_gui.cpp` | Interface Gráfica (GUI) | Aplicação interativa *VMPCalc* com Viewport CAD 2D (Raylib) e painéis estilo Ribbon/NLtool (ImGui). |
| `tests/test_framework.h` | Infraestrutura TDD | Micro-framework de testes unitários com registro automático e macros de tolerância percentual. |
| `tests/test_main.cpp` | Runner de Testes | Ponto de entrada do executável `vmp_tests` que invoca `TestRegistry::instance().run_all()`. |
| `tests/test_rotinas_nbr8800.cpp` | Testes Unitários | Testes de aferição das Rotinas A, B, C.1, C.2, C.3 e D em `v2008` e `v2024`. |
| `tests/test_rotinas_vmp.cpp` | Testes Unitários | Testes das Rotinas 1 a 6, G, H, verificador multifásico completo e comparativo 2008 vs. 2024. |

## 1.3 Coexistência Normativa via Namespaces C++20 (`v2008` vs `v2024`)

Um dos pilares arquiteturais mais elegantes do código é o mecanismo de versionamento normativo em nível de linguagem utilizando `inline namespace` do C++20 dentro do escopo `nbr8800`:
- **`namespace nbr8800 { inline namespace v2008 { ... } }`**: O sub-namespace `v2008` é declarado como `inline`, o que significa que chamadas não qualificadas como `nbr8800::calcularNormalResistentePerfil(...)` resolvem por padrão para a formulação de 2008, mantendo 100% de retrocompatibilidade com os testes originais de aferição com Turini (2021);
- **`namespace nbr8800::v2024 { ... }`**: Contém as implementações atualizadas segundo a **ABNT NBR 8800:2024**. As rotinas cuja formulação física não sofreu alteração entre as duas edições (como a homogeneização elástica da seção na Rotina 1 ou as perdas de protensão da NBR 6118 na Rotina 5) são reutilizadas diretamente, enquanto as rotinas alteradas (Rotinas A, C.1, C.2, C.3, D, 3.1, 3.2 e 3.3) possuem implementação própria em `nbr8800::v2024` e são despachadas dinamicamente pelo verificador conforme o campo `viga.norma`.

---

# CAPÍTULO 2 — MODELAGEM DE DADOS, GEOMETRIA E CATÁLOGO

## 2.1 Tipos Enumerados e Estruturas de Domínio (`include/modelos.h`)

O arquivo `include/modelos.h` centraliza todas as estruturas de dados de entrada que descrevem fisicamente uma viga mista protendida. Não há lógica de negócio acoplada às structs, garantindo objetos leves (estilo *Plain Old Data* — POD) de fácil serialização e manipulação pela interface gráfica.

### 2.1.1 Enumerações de Configuração
O cabeçalho define onze tipos enumerados que controlam as ramificações de cálculo:
- **`TipoFabricacao`**: `LAMINADO` (perfis laminados a quente, ex.: Série W) ou `SOLDADO` (perfis compostos por chapas soldadas, ex.: Série VS/CS);
- **`TipoLaje`**: `LAJE_MACICA` ($h_F = 0$) ou `LAJE_STEEL_DECK` (laje moldada sobre fôrma de aço incorporada com nervuras trapezoidais);
- **`TipoProtensao`**: `PRE_TRACAO` (protensão aplicada ao perfil de aço isolado na Fase 1) ou `POS_TRACAO` (protensão aplicada à seção mista já consolidada na Fase 2);
- **`TipoTracado`**: `CABO_RETO` (excentricidade constante ao longo de todo o vão), `CABO_POLIGONAL` (cabo desviado com trechos inclinados nos apoios e reto no terço central) ou `CABO_PARABOLICO`;
- **`TipoElementoProtensao`** (`CORDOALHA`, `FIO`, `BARRA`), **`TipoRelaxacao`** (`RELAXACAO_BAIXA` — RB, ou `RELAXACAO_NORMA` — RN), **`TipoAncoragem`** (`ATIVA_PASSIVA` ou `ATIVA_ATIVA`), **`TipoCabo`** (`CP_190_RB`, `CP_210_RB`, `CP_175_RB`) e **`TipoCordoalha`** (`CORDOALHA_15_2` com $140\text{ mm}^2$ ou `CORDOALHA_12_7` com $98{,}7\text{ mm}^2$);
- **`TipoConector`**: `CONECTOR_PERFIL_U` ou `CONECTOR_STUD_BOLT` (pino com cabeça);
- **`EdicaoNBR8800`**: `NBR8800_2008` (valor `0`) ou `NBR8800_2024` (valor `1`), responsável por chavear o motor normativo no verificador.

### 2.1.2 Estruturas dos Componentes Estruturais
Cada subsistema físico da viga é modelado por uma `struct` dedicada:

**Tabela 2.1 — Estruturas de dados de domínio definidas em `include/modelos.h`**

| Estrutura (`struct`) | Principais Campos e Unidades | Papel no Modelo Estrutural |
| :--- | :--- | :--- |
| `PerfilI` | Dimensões (`d`, `bf`, `tf`, `tw`, `r`, `h`, `d1` em $\text{mm}$) e propriedades (`Ag`, `Ix`, `Wx`, `rx`, `Zx`, `Iy`, `Wy`, `ry`, `Zy`, `rt`, `It`, `Cw`, `lbf`, `lbw`). | Representa a geometria e as constantes elásticas, plásticas e torcionais da seção metálica duplo T. |
| `Aco` | `nome`, `fy_MPa`, `fu_MPa`, `E_MPa` ($200\,000\text{ MPa}$), `G_MPa` ($7\,700\text{ MPa}$), `nu`. | Propriedades mecânicas do aço estrutural do perfil I (ex.: ASTM A572 Gr. 50). |
| `Concreto` | `classe`, `fck_MPa`, `Ecs_MPa` (módulo secante), `fctm_MPa` (tração média). | Propriedades mecânicas do concreto da laje segundo a ABNT NBR 6118. |
| `Laje` | `tipo`, `tc_mm` (espessura de concreto acima da fôrma), `bef_mm` (largura efetiva), `hF_mm`, `bF_mm`, `eF_mm`, `nervura_paralela`. | Geometria da laje maciça ou mista com fôrma de aço incorporada (*steel deck*). |
| `ArmacaoLaje` | `As` ($\text{mm}^2/\text{m}$), `Ads` (área total na largura $b_{ef}$ em $\text{mm}^2$), `fsd` ($\text{MPa}$), `d_linha_mm` (cobrimento superior). | Armadura passiva longitudinal negativa disposta na laje para momentos negativos e FLD. |
| `CaboProtensao` | `tipo`, `tracado`, `ancoragem`, `fpk`, `fpyk`, `sigma_Pd`, `Ep`, `diametro`, `area_unit`, `n_cordoalhas`, `Ap`, `ep` (negativo acima da face inferior), `delta_cunha`, `X_ep_mm`. | Caracterização física, geométrica e mecânica dos cabos externos de protensão. |
| `ConectorCisalhamento` | `tipo`, `fu`, `h`, `tf`, `tw`, `Lcs` (perfil U), `d`, `nt`, `Rg`, `Rp` (*stud bolt*). | Parâmetros geométricos e resistentes dos conectores de cisalhamento aço-concreto. |
| `CargasVMP` | `q1_perfil`, `q2_concreto`, `qs_construcao`, `q3_revestimento`, `q4_uso` (em $\text{kN/m}$), `psi1`, `psi2`, `escorada`. | Carregamentos lineares permanentes e variáveis atuantes nas fases de construção e serviço. |
| `VigaVMP` | Agrega todas as structs acima + `L` (vão), `Lb` (destravamento), `a_vigas_mm`, `alpha_viga`, `Cb_fase1`, `P0_adotado`, `num_secoes`, `norma`. | Objeto raiz (agregado completo) recebido pela função `verificar_viga_vmp`. |

## 2.2 Motor de Propriedades Geométricas (`include/geometria.h` e `src/geometria.cpp`)

Quando o usuário define um perfil soldado ou customizado informando apenas as dimensões nominais de chapa ($d, b_f, t_f, t_w, r$), a função `calcular_propriedades_secao_i(PerfilI& p)` em `src/geometria.cpp` calcula automaticamente todas as grandezas geométricas exigidas pela ABNT NBR 8800, preservando valores de catálogo caso já tenham sido preenchidos (`if (p.Ag <= 0.0)`):
1. **Alturas da alma**: Calcula a altura entre faces internas das mesas $h = d - 2 t_f$ e a altura livre plana da alma $d_1 = d - 2(t_f + r)$ para perfis laminados com raio de concordância $r > 0$, ou $d_1 = d - 2 t_f$ para perfis soldados;
2. **Área bruta ($A_g$)**: Soma a área das duas mesas ($2 b_f t_f$), da alma ($h t_w$) e dos quatro filetes circulares de laminação $(4 - \pi) r^2$;
3. **Momentos de inércia principais ($I_x$ e $I_y$)**: Aplica o Teorema dos Eixos Paralelos (Steiner) para as mesas e alma em torno do eixo de maior inércia ($x$-$x$) e de menor inércia ($y$-$y$):

$$I_x = 2 \left[ \frac{b_f t_f^3}{12} + (b_f t_f) \left(\frac{d - t_f}{2}\right)^2 \right] + \frac{t_w h^3}{12}, \qquad I_y = 2 \left(\frac{t_f b_f^3}{12}\right) + \frac{h t_w^3}{12}$$

4. **Módulos elásticos ($W_x, W_y$), módulos plásticos ($Z_x, Z_y$) e raios de giração ($r_x, r_y$)**:

$$W_x = \frac{2 I_x}{d}, \qquad Z_x = b_f t_f (d - t_f) + \frac{t_w h^2}{4}, \qquad r_x = \sqrt{\frac{I_x}{A_g}}, \qquad r_y = \sqrt{\frac{I_y}{A_g}}$$

5. **Esbeltezes locais ($\lambda_f$ e $\lambda_w$) e constantes de torção ($I_t$ e $C_w$)**: Determina a esbeltez da mesa $\lambda_f = b_f / (2 t_f)$, a esbeltez da alma $\lambda_w = d_1 / t_w$, a constante de torção de Saint-Venant $I_t = (2 b_f t_f^3 + h t_w^3)/3$ e a constante de empenamento ao redor do eixo menor $C_w = I_y (d - t_f)^2 / 4$.

## 2.3 Catálogo de Perfis e Presets de Aferição (`include/catalogo.h`)

O cabeçalho `include/catalogo.h` fornece funções fábrica `inline` que retornam instâncias pré-configuradas e aferidas para uso imediato nos testes unitários, no console e na interface gráfica:
- **Estudo de Caso 1 (Turini, 2021 — Apêndice B)**: `perfil_w360x57_8()`, `aco_a572_gr50()`, `concreto_c30()`, `laje_macica_130()` ($t_c = 130\text{ mm}, b_{ef} = 3000\text{ mm}$), `laje_5cm_m()` ($A_s = 500\text{ mm}^2/\text{m}, A_{ds} = 1500\text{ mm}^2$) e `cabo_190RB()` (pré-tração, cabo reto, $e_p = -50\text{ mm}$, acomodação $\Delta_{cunha} = 6\text{ mm}$);
- **Estudo de Caso 2 (Turini, 2021 — Apêndice C)**: `perfil_w460x89()`, `laje_mista_exemplo2()` (laje com *steel deck* $h_F = 50\text{ mm}, b_F = 130\text{ mm}, e_F = 305\text{ mm}, b_{ef} = 2000\text{ mm}$), `laje_exemplo2_armacao()` ($A_{ds} = 1000\text{ mm}^2$) e `cabo_exemplo2()` (pós-tração, 6 cordoalhas CP-190 RB, $A_p = 840\text{ mm}^2$).

---

# CAPÍTULO 3 — MOTOR NORMATIVO BASE: PERFIL DE AÇO E VIGA MISTA (`rotinas_nbr8800`)

O par de arquivos `include/rotinas_nbr8800.h` e `src/rotinas_nbr8800.cpp` implementa as rotinas fundamentais de verificação do perfil metálico isolado e da seção mista aço-concreto sem protensão (denominadas **Rotinas A, B, C e D** na metodologia de Turini, 2021), tanto para o namespace `nbr8800::v2008` quanto para o namespace `nbr8800::v2024`.

## 3.1 Rotina A — Força Normal de Compressão Resistente do Perfil (`calcularNormalResistentePerfil`)

Esta rotina determina a força normal de compressão axial resistente de cálculo ($N_{Rd,I}$) do perfil I metálico, indispensável para verificar a estabilidade à flexo-compressão do perfil quando submetido à força excêntrica dos cabos de protensão ($P_0$ e $P_1$).

1. **Cargas Críticas Elásticas de Euler ($N_{ex}, N_{ey}, N_e$)**: Em ambas as edições normativas, calculam-se as forças críticas de flambagem global por flexão em torno dos eixos $x$ (vão $L_v$) e $y$ (comprimento destravado $L_b$), adotando-se a menor delas:

$$N_{ex} = \frac{\pi^2 E_a I_x}{(k_x L_v)^2}, \qquad N_{ey} = \frac{\pi^2 E_a I_y}{(k_y L_b)^2}, \qquad N_e = \min(N_{ex}, \; N_{ey})$$

2. **Tratamento da Flambagem Local na Edição 2008 (`nbr8800::v2008`)**: Utiliza o método clássico do **fator de redução total $Q = Q_s \cdot Q_a$**. O fator $Q_s$ (elementos AL — mesas) é avaliado comparando $\lambda_f$ com $\lambda_{f,p} = 0{,}56\sqrt{E_a/f_y}$ e $\lambda_{f,r} = 1{,}03\sqrt{E_a/f_y}$ (para perfis laminados) ou utilizando o coeficiente $k_c = 4/\sqrt{\lambda_w}$ (para perfis soldados). O fator $Q_a$ (elemento AA — alma) compara $\lambda_w$ com $\lambda_{w,p} = 1{,}49\sqrt{E_a/f_y}$; se a alma for esbelta ($\lambda_w > \lambda_{w,p}$), calcula-se a largura efetiva de Winter $b_{ef,a}$ e a razão $Q_a = A_{ef} / (d_1 t_w)$. Por fim, obtêm-se o índice de esbeltez reduzido $\lambda_0 = \sqrt{Q A_g f_y / N_e}$, o fator de redução global $\chi$ ($0{,}658^{\lambda_0^2}$ para $\lambda_0 \le 1{,}5$ ou $0{,}877/\lambda_0^2$ para $\lambda_0 > 1{,}5$) e a resistência:

$$N_{Rd,2008} = \frac{\chi \cdot Q \cdot A_g \cdot f_y}{\gamma_{a1}}$$

3. **Tratamento da Flambagem Local na Edição 2024 (`nbr8800::v2024`)**: A nova norma extinguiu os fatores $Q_s$ e $Q_a$, adotando o **Método Unificado da Largura/Área Efetiva ($A_{ef}$)** baseado na curva generalizada de Winter com coeficientes $(c_1, c_2)$. No código (`linhas 475-541` de `src/rotinas_nbr8800.cpp`):
   - O índice de esbeltez reduzido global $\lambda_0 = \sqrt{A_g f_y / N_e}$ é calculado diretamente com a área bruta $A_g$, obtendo-se a tensão crítica global $f_n = \chi f_y$;
   - Os limites de esbeltez local das mesas ($\lambda_{f,r}$) e da alma ($\lambda_{w,r}$) são corrigidos pelo nível de tensão atuante na flambagem global: $\lambda_{ef} = \lambda_r \sqrt{f_y / f_n}$;
   - Caso $\lambda > \lambda_{ef}$, calcula-se a tensão elástica de flambagem local $f_{el} = (c_2 \lambda_r / \lambda)^2 f_y$ (com $c_1 = 0{,}22, c_2 = 1{,}49$ para as mesas AL e $c_1 = 0{,}18, c_2 = 1{,}31$ para a alma AA) e subtraem-se as perdas de área $\Delta A_{mesas}$ e $\Delta A_{alma}$ de $A_g$ para obter $A_{ef}$ e $N_{Rd,2024} = (\chi A_{ef} f_y)/\gamma_{a1}$. Para manter rastreabilidade comparativa na interface gráfica, o código preenche `res.Q = Aef_mm2 / perfil.Ag`.

## 3.2 Rotina B — Momento Fletor Resistente do Perfil de Aço (`calcularMomentoResistentePerfil`)

Avalia a resistência à flexão simples do perfil metálico isolado ($M_{Rd,I}$), governante na Fase 1 (construção) de vigas pré-tracionadas. A função principal calcula o momento de plastificação $M_{pl} = \min(Z_x f_y, \; 1{,}5 W_x f_y)$ e o momento de início de escoamento com tensões residuais $M_r = 0{,}7 W_x f_y$, delegando a verificação aos três estados limites de instabilidade:
- **`calcularMRdFLT` (Flambagem Lateral com Torção)**: Compara a esbeltez lateral $\lambda = L_b / r_y$ com o limite plástico $\lambda_p = 1{,}76\sqrt{E_a/f_y}$ e o limite elástico $\lambda_r$ (função de $\beta_1 = 0{,}7 f_y W_x / (E_a I_t)$, $I_y$, $I_t$ e $C_w$). Aplica o fator de modificação para diagrama de momento não-uniforme $C_b$ (limitado ao teto plástico $M_{pl}/\gamma_{a1}$);
- **`calcularMRdFLM` (Flambagem Local da Mesa)**: Avalia a esbeltez da mesa comprimida $\lambda_f$ frente a $\lambda_{f,p} = 0{,}38\sqrt{E_a/f_y}$ e $\lambda_{f,r}$, diferenciando perfis laminados e soldados;
- **`calcularMRdFLA` (Flambagem Local da Alma)**: Avalia $\lambda_w$ frente a $\lambda_{w,p} = 3{,}76\sqrt{E_a/f_y}$ e $\lambda_{w,r} = 5{,}70\sqrt{E_a/f_y}$. Caso $\lambda_w > \lambda_{w,r}$, sinaliza `res.perfil_esbelto = true`. O momento resistente final é o mínimo entre os três estados: $M_{Rd,I} = \min(M_{Rd,FLT}, \; M_{Rd,FLM}, \; M_{Rd,FLA})$.

## 3.3 Rotina C — Momento Fletor Resistente da Viga Mista Convencional (`calcularMomentoPositivoVM` e `calcularMomentoNegativoVM`)

Utilizada na Fase 1 (construção) de vigas pós-tracionadas, nas quais a laje já se encontra solidarizada ao perfil, mas os cabos ainda não foram protendidos ($P_1 = 0$).

### 3.3.1 Momento Positivo (`calcularMomentoPositivoVM` — Rotinas C.1 e C.2)
- **Seção Compacta ($\lambda_w \le 3{,}76\sqrt{E_a/f_y}$ — Rotina C.1)**: Calcula a capacidade máxima de compressão da laje $C_{c,\max} = 0{,}85 \eta_c f_{cd} b_{ef} t_c$ e a capacidade plástica de tração do perfil $T_{aço} = A_g f_{yd}$. Se $C_{c,\max} \ge T_{aço}$, a Linha Neutra Plástica (LNP) situa-se na laje com altura comprimida $a = T_{aço} / (0{,}85 \eta_c f_{cd} b_{ef})$. Caso contrário ($C_{c,\max} < T_{aço}$), a LNP corta o perfil metálico (na mesa superior ou na alma), determinando-se a força de compressão no aço $C_{aço} = 0{,}5 (A_g f_{yd} - C_{c,\max})$, a posição $y_p$ da LNP e os centroides das regiões comprimida ($y_c$) e tracionada ($y_t$);
- **Seção Semicompacta ($3{,}76\sqrt{E_a/f_y} < \lambda_w \le 5{,}70\sqrt{E_a/f_y}$ — Rotina C.2)**: A verificação é realizada em regime elástico utilizando os módulos de resistência da seção homogeneizada em $t=0$ ($W_{tr,inf}$ e $W_{tr,sup}$);
- **Diferenças implementadas em `nbr8800::v2024`**:
  1. Introdução do **fator de fragilidade do concreto de alta resistência** $\eta_c = \min(1{,}0; \; (40/f_{ck})^{1/3})$ para concretos com $f_{ck} > 40\text{ MPa}$ (Item L.5.3.4 da NBR 8800:2024);
  2. Ajuste da equação do binário interno quando a LNP está no perfil de aço sob interação completa (Item L.6.3.1.1.1-b, p. 203), utilizando o somatório dos conectores $\sum Q_{Rd} = C_{cd}$ multiplicado pelo braço $(d - y_t - y_c)$ e o braço da laje referido ao centroide $t_c/2$;
  3. Redução da tensão limite de compressão na fibra superior da laje em seções semicompactas de $1{,}00 f_{cd}$ (em 2008) para **$0{,}85 f_{cd}$** (em 2024, Item L.6.3.1.2-a).

### 3.3.2 Momento Negativo e Flambagem Lateral com Distorção (`calcularMomentoNegativoVM` — Rotina C.3)
Na região de momentos negativos, a laje de concreto encontra-se tracionada (fissurada), de modo que a resistência plástica da seção mista ($M_{Rd}^-$) provém do equilíbrio entre a força de tração na armadura passiva da laje ($T_{ds} = A_{ds} f_{sd}$) e a plastificação do perfil metálico ($y_p$ na mesa superior ou na alma, verificando a compactação da região comprimida da alma $\lambda \le 3{,}76\sqrt{E_a/f_y}$). Em seguida, avalia-se a **Flambagem Lateral com Distorção (FLD)** da mesa inferior comprimida do perfil I:
- Calcula-se a rigidez rotacional do conjunto laje-alma $k_r = (k_1 k_2)/(k_1 + k_2)$, onde $k_1 = \alpha_{viga} E_a I_{laje} / a_{vigas}$ é a rigidez à flexão transversal da laje fissurada e $k_2 = E_a t_w^3 / (3{,}64 h_0)$ é a rigidez à flexão da alma do perfil;
- Na edição de **2008 (`v2008`)**, o momento crítico elástico $M_{cr}$ utiliza o fator geométrico $\alpha_g$ e a tabela clássica de $C_{dist}$ (que varia de $21{,}9$ para $\psi = 0{,}50$ a $7{,}6$ para $\psi = 2{,}50$);
- Na edição de **2024 (`v2024`)**, foi implementada a nova formulação completa dos Itens L.6.5.5 e L.6.5.6 da ABNT NBR 8800:2024 (`linhas 667-720` de `src/rotinas_nbr8800.cpp`), que introduz o novo fator geométrico exponencial $\kappa_g$, o parâmetro adimensional de rigidez rotacional $\eta_b$, o número discreto de semiondas de flambagem $n_{ondas}$ e a nova Tabela L.1 de $C_{dist}$ (cujos valores passaram a variar entre $1{,}92$ e $1{,}24$):

$$\kappa_g = \left(0{,}31 + 0{,}69 \cdot 0{,}05^{\bar{y}/h_0}\right) \frac{I_{tr}}{I_x}, \qquad \eta_b = \sqrt{\frac{k_r L_v^4}{E_a I_{af,y} h_0^2}}$$

$$M_{cr,2024} = C_{dist} \frac{\kappa_g}{h_0} \left\{ G_a I_t + \frac{E_a I_{af,y} h_0^2}{L_v^2} \left[ (n \pi)^2 + \left(\frac{\eta_b}{n \pi}\right)^2 \right] \right\}$$

## 3.4 Rotina D — Força Cortante Resistente da Alma (`calcularCortanteResistentePerfil`)

Determina a força cortante resistente de cálculo ($V_{Rd}$) da alma do perfil I a partir da força plástica de cisalhamento $V_{pl} = 0{,}60 d t_w f_y$ e dos limites de esbeltez $\lambda_{w,p} = 1{,}10\sqrt{k_v E_a / f_y}$ e $\lambda_{w,r} = 1{,}37\sqrt{k_v E_a / f_y}$. A diferença fundamental entre os dois módulos reside no coeficiente de flambagem por cisalhamento $k_v$ para almas sem enrijecedores transversais:
- Em **`nbr8800::v2008`**: adota $k_v = 5{,}0$;
- Em **`nbr8800::v2024`**: adota **$k_v = 5{,}34$** (alinhado ao AISC 360), elevando o patamar limite de plastificação por cisalhamento $\lambda_{w,p}$ em cerca de $3{,}34\%$ (de $59{,}22$ para $61{,}20$ no aço ASTM A572 Gr. 50), além de corrigir a equação do regime elástico ($\lambda_w > \lambda_{w,r}$) para $1{,}24 (\lambda_{w,p}/\lambda_w)^2 V_{pl}/\gamma_{a1}$.

---

# CAPÍTULO 4 — MOTOR ESPECIALIZADO DE VIGAS MISTAS PROTENDIDAS (`rotinas_vmp`)

Os arquivos `include/rotinas_vmp.h` e `src/rotinas_vmp.cpp` contêm o núcleo científico específico de vigas mistas protendidas (**Rotinas 1 a 6, G e H**).

## 4.1 Rotina 1 — Seção Transformada / Homogeneizada (`calcularSecaoTransformada`)

Recebe o perfil I, a laje, o concreto, o aço, a posição do cabo $e_p$ e a flag booleana `tempo_imediato`:
- Quando `tempo_imediato == true` ($t = 0$, Fase 2), calcula a razão modular de curta duração $n = E_{cs} / E_a$;
- Quando `tempo_imediato == false` ($t = \infty$, Fase 3), incorpora a fluência do concreto adotando $n = E_{cs} / (3 E_a)$;
- Retorna a struct `ResultadoSecaoTransformada` contendo a largura transformada $b_{tr} = n \cdot b_{ef}$, a distância da Linha Neutra Elástica à face inferior do perfil $y_{tr}$, a altura comprimida de concreto $a_y$, a área homogeneizada $A_{tr}$, a inércia homogeneizada $I_{tr}$, os módulos elásticos $W_{tr,inf}$ e $W_{tr,sup}$, e a excentricidade efetiva do cabo em relação à LNE transformada ($e_{p,tr} = y_{tr} + e_p$).

## 4.2 Rotina 2 — Estimativa da Força Máxima de Protensão Inicial (`calcularEstimativaP0max`)

Calcula a força máxima de protensão inicial ($P_{0,\max}$) que pode ser aplicada ao cabo sem violar os estados limites na montagem e em serviço:
1. **Critério da ABNT NBR 8800 (`P0_max_1`)**: Na **pré-tração**, o perfil metálico isolado fica sujeito à força normal $\gamma_p \beta P_0$ e ao momento de protensão $\gamma_p \beta P_0 e_{p,I}$ (onde $e_{p,I} = d/2 + e_p$). Isolando $P_0$ nas duas equações de interação de flexo-compressão da NBR 8800 (para $N_{Sd}/N_{Rd} \ge 0{,}2$ e $< 0{,}2$), obtém-se:

$$P_{0,\max,1} = \begin{cases} \dfrac{1}{\gamma_p \beta \left(\dfrac{1}{N_{Rd,I}} + \dfrac{8 e_{p,I}}{9 M_{Rd,I}}\right)}, & \text{se } \dfrac{P_{0,\max,1}}{N_{Rd,I}} \ge 0{,}2 \\[12pt] \dfrac{1}{\gamma_p \beta \left(\dfrac{1}{2 N_{Rd,I}} + \dfrac{e_{p,I}}{M_{Rd,I}}\right)}, & \text{se } \dfrac{P_{0,\max,1}}{N_{Rd,I}} < 0{,}2 \end{cases}$$

   Na **pós-tração**, como o perfil isolado não é protendido na Fase 1, o código define `res.P0_max_1 = std::numeric_limits<double>::infinity()`, fazendo com que apenas o segundo critério governe.
2. **Critério da ABNT NBR 6118 (`P0_max_2`)**: Limita a força de protensão pelo estado de descompressão/tensão na seção mista em $t = \infty$ sob os momentos característicos permanente ($M_{Sd,g}$) e variável ($M_{Sd,q}$) e verifica se a tensão de compressão no topo da laje ($\sigma_c / \alpha_E$) não ultrapassa o limite de microfissuração por fluência não-linear de $0{,}60 f_{ck}$. O valor final retornado é $P_{0,\max} = \min(|P_{0,\max,1}|, \; |P_{0,\max,2}|)$.

## 4.3 Rotina 4 — Dimensionamento da Armadura Ativa (`calcularArmaduraProtensao`)

Determina a tensão máxima admissível no ato da protensão ($\sigma_{adm}$) conforme a ABNT NBR 6118, diferenciando a modalidade executiva:
- **Pré-tração**: $\sigma_{adm} = \min(0{,}77 f_{pk}, \; 0{,}85 f_{pyk})$;
- **Pós-tração**: $\sigma_{adm} = \min(0{,}80 f_{pk}, \; 0{,}88 f_{pyk})$.

Com base na força $P_0$ adotada, calcula a área teórica necessária $A_{p,calc} = P_0 / \sigma_{adm}$, arredonda para cima o número inteiro de cordoalhas $n_{cord} = \lceil A_{p,calc} / A_{unit} \rceil$ e retorna a área efetiva real $A_{p,real} = n_{cord} \cdot A_{unit}$.

## 4.4 Rotina 5 — Perdas Imediatas e Progressivas de Protensão

O módulo modela a redução da força no cabo ao longo do tempo através de três funções:
1. **`calcularPerdaAncoragemTracadoReto` (Perda Imediata por Encunhamento)**: Quando o macaco hidráulico libera o cabo, ocorre a acomodação da cunha $\Delta_{cunha}$ (tipicamente $6\text{ mm}$). Para ancoragem ativa-passiva, o encurtamento distribui-se pelo comprimento $L_v$; para ancoragem ativa-ativa (protendida pelas duas extremidades), a perda é dividida por 2 no código:

$$\Delta P_{anc} = \frac{\Delta_{cunha} E_p A_p}{L_v} \quad (\text{Ativa-Passiva}), \qquad P_1 = P_0 - \Delta P_{anc}$$

2. **`calcularPsi1000` e `calcularPerdaRelaxacaoTracadoReto` (Perda Progressiva por Relaxação em $t = \infty$)**: Calcula a razão de tensão inicial $\chi = \sigma_{P0} / f_{pk}$. Se $\chi \le 0{,}50$, a relaxação é nula. Para $\chi > 0{,}50$, interpola linearmente sobre a tabela normativa da ABNT NBR 6118 para $1\,000\text{ horas}$ ($\psi_{1000}$) conforme o tipo de elemento (cordoalha, fio ou barra) e o grau de relaxação (RB ou RN). A perda no tempo infinito é obtida extrapolando $\psi_{1000}$ pelo fator $2{,}5$:

$$\Delta P_{relax} = 2{,}5 \cdot \psi_{1000} \cdot P_{atuante}, \qquad P_3 = P_{atuante} - \Delta P_{relax}$$

3. **`calcularPerdaImediataTracadoPoligonal` (Rotina 5.1 em Stand-by)**: Retorna a estrutura `ForcasProtensaoTrechos` (`P_ativa_kN`, `P_meio_kN`, `P_passiva_kN`), delegando temporariamente ao modelo de cabo reto enquanto o atrito nos desviadores encontra-se em *stand-by*.

## 4.5 Rotina 6 — Incremento Hiperestático de Força no Cabo (`calcularIncrementoDpTracadoReto`)

Como o cabo externo é ancorado nas extremidades da própria viga, qualquer carregamento gravitacional $q$ que provoque flexão positiva na viga impõe um alongamento adicional ao cabo posicionado abaixo da Linha Neutra Elástica, gerando um **acréscimo autoequilibrado (hiperestático) de força de protensão ($\Delta P$)**. Aplicando o Princípio dos Trabalhos Virtuais (compatibilidade de deslocamentos entre a viga de inércia $I_x$, área $A_g$, módulo $E_g$ e o cabo de área $A_p$, módulo $E_p$ e excentricidade $e_p$ em relação ao CG da seção vigente na fase), a função calcula:

$$\Delta P = \frac{q \cdot e_p \cdot L_v^2}{12 \left( e_p^2 + \dfrac{I_x}{A_g} + \dfrac{E_g I_x}{E_p A_p} \right)}$$

Na Fase 1 da pré-tração, essa função é invocada usando as propriedades do perfil metálico isolado ($I_x, A_g, e_{p,I}$). Na Fase 2 ($t=0$), é invocada usando as propriedades da seção mista homogeneizada ($I_{tr,0}, A_{tr,0}, e_{p,tr,0}$).

## 4.6 Rotina H — Tensão de Cálculo no Cabo no ELU (`calcularTensaoCaboELU`)

No Estado Limite Último de flexão da viga mista protendida, a rotação plástica da seção transversal provoca um acréscimo de tensão última no cabo não-aderente ($\Delta \sigma_p$) que se soma à pré-deformação elástica instalada ($\varepsilon_{pre}$). A função `calcularTensaoCaboELU` implementa a formulação de Turini (2021):
1. Calcula a taxa geométrica de armadura ativa $\rho_p = A_p / A_{tr}$ e a pré-deformação específica do cabo sob a força minorada $0{,}9 P_{atuante}$:
   - Na **pré-tração**: $\varepsilon_{pre} = \dfrac{0{,}9 P_{atuante}}{E_p A_p}$;
   - Na **pós-tração**: $\varepsilon_{pre} = \dfrac{0{,}9 P_{atuante}}{E_p A_p} \left[ 1 + \alpha_p \rho_p \left(1 + \dfrac{e_{p,tr}^2 A_{tr}}{I_{tr}}\right) \right]$, onde $\alpha_p = E_p / E_{cs}$.
2. Determina o acréscimo empírico de tensão na ruptura $\Delta \sigma_p$ conforme a esbeltez longitudinal $L_v / h_{total}$:
   - Se $L_v / (d + h_F + t_c) \le 35$: $\Delta \sigma_p = \min\left(70 + \dfrac{f_{ck}}{100 \rho_p}, \; 420\text{ MPa}\right)$;
   - Se $L_v / (d + h_F + t_c) > 35$: $\Delta \sigma_p = \min\left(70 + \dfrac{f_{ck}}{300 \rho_p}, \; 210\text{ MPa}\right)$.
3. Retorna a tensão última de cálculo no cabo limitada ao escoamento de cálculo:

$$\sigma_{pd} = \min\left( \frac{\varepsilon_{pre} E_p + \Delta \sigma_p}{1{,}15}, \; \frac{f_{pyk}}{1{,}15} \right)$$

## 4.7 Rotinas 3 e G — Momento Resistente Positivo e Negativo da VMP e Inércia da Laje

- **Rotinas 3.1 e 3.2 (`calcularMomentoPostivoVMP`)**: Generaliza o cálculo do momento resistente positivo da seção mista para incluir a força de tração última do cabo $T_{cabo} = A_p \sigma_{pd}$. Quando a seção é compacta ($\lambda_w \le \lambda_{w,p}$) e $C_{c,\max} \ge T_{aço} + T_{cabo}$, a LNP permanece na laje ($a = (T_{aço} + T_{cabo})/(0{,}85 \eta_c f_{cd} b_{ef})$) e o momento resistente soma a contribuição do perfil e do cabo. Quando $C_{c,\max} < T_{aço} + T_{cabo}$, a LNP desce para o perfil metálico ($C_{aço} = 0{,}5(A_g f_{yd} + T_{cabo} - C_{c,\max})$). A versão `nbr8800::v2024::calcularMomentoPostivoVMP` incorpora o fator de fragilidade $\eta_c$, a formulação do Item L.6.3.1.1.1-b com $\sum Q_{Rd}$ para LNP no aço e o limite de $0{,}85 f_{cd}$ para seções semicompactas;
- **Rotina 3.3 (`calcularMomentoNegativoVMP`)**: Calcula a resistência ao momento negativo provocado pela protensão excêntrica nas seções próximas aos apoios e no meio do vão, incorporando a tração do cabo $T_{pd} = A_p \sigma_{pd}$ e verificando a Flambagem Lateral com Distorção (FLD) pelas formulações de 2008 (`v2008`) e 2024 (`v2024`);
- **Rotina G (`calcularInerciaLaje`)**: Calcula o momento de inércia fissurado homogeneizado por unidade de largura da laje ($I_{laje}$, retornado em $\text{mm}^4/\text{m}$) resolvendo a equação do 2º grau da posição da linha neutra fissurada ($x_{ln}$) tanto para **laje maciça** quanto para **laje mista com *steel deck*** (diferenciando quando $x_{ln} \le h_F$ ou $x_{ln} > h_F$).

---

# CAPÍTULO 5 — ORQUESTRADOR E VERIFICADOR MULTIFÁSICO (`verificador`)

O módulo `include/verificador.h` e `src/verificador.cpp` é o coração algorítmico que integra todas as rotinas anteriores em uma simulação cronológica e espacial completa da viga.

## 5.1 Discretização Espacial em $X$ Seções (`gerarSecoesViga`)

A função `gerarSecoesViga(const VigaVMP& viga, ...)` divide o vão $L \in [0, L]$ em `n_div = std::max(3, viga.num_secoes)` pontos igualmente espaçados. Caso `num_secoes` seja par e o ponto médio $x = L/2$ não pertença à partição uniforme, o algoritmo insere automaticamente a coordenada $x = L/2$ e reordena o vetor, garantindo que a seção crítica do meio do vão esteja sempre presente na malha.

Cada seção gerada (`SecaoTransversalViga`) recebe:
- Um índice sequencial e um rótulo descritivo (ex.: `"Seção 0 (x = 0 mm - Apoio Esq.)"`, `"Seção 2 (x = 6000 mm - Meio do Vão)"`);
- A classificação do trecho do cabo (`TRECHO_ATIVA`, `TRECHO_MEIO_VAO` ou `TRECHO_PASSIVA`) conforme a coordenada longitudinal $x$ e o tipo de ancoragem (`ATIVA_PASSIVA` ou `ATIVA_ATIVA`), permitindo associar a cada seção a força de protensão exata de seu trecho através do helper `obterForcaNoTrecho`;
- A posição vertical do cabo na seção (`ep_perfil_mm`).

## 5.2 Fluxo Sequencial Completo de `verificar_viga_vmp`

A função `RelatorioVMP verificar_viga_vmp(const VigaVMP& viga)` executa cinco grandes etapas sequenciais:

**Tabela 5.1 — Pipeline de execução da função `verificar_viga_vmp` (`src/verificador.cpp`)**

| Etapa | Descrição Técnica e Rotinas Invocadas | Diferenciação Pré-Tração vs. Pós-Tração |
| :--- | :--- | :--- |
| **1. Resistências Base do Aço** | Verifica se o perfil precisa de cálculo geométrico (`calcular_propriedades_secao_i`), lê a flag `usar_2024 = (viga.norma == NBR8800_2024)` e executa as **Rotinas A, B e D** no namespace correspondente (`v2008` ou `v2024`). | Comum a ambos os sistemas construtivos. |
| **2. Homogeneização, $P_{0,\max}$ e $A_p$** | Executa a **Rotina 1** para $t=0$ (`secao_t0`) e $t=\infty$ (`secao_t_inf`), calcula os momentos característicos $M_{Sd,g}$ e $M_{Sd,q}$, obtém $P_{0,\max}$ pela **Rotina 2**, define $P_{0,efetivo}$ e dimensiona as cordoalhas pela **Rotina 4**. | Na pré-tração, $P_{0,\max}$ é limitado por $\min(P_{0,8800}, P_{0,6118})$; na pós-tração, apenas $P_{0,6118}$ governa. |
| **3. Fase 1 (ELU 1 — Construção)** | Combinação construtiva: $q_{Sd,1} = 1{,}3(q_1 + q_2) + 1{,}2 q_s$. Percorre todas as $X$ seções calculando $M_{Sd}^+(x)$, $M_{Sd}^-(x)$, $V_{Sd}(x)$ e os índices de interação à flexo-compressão (`calcularIndiceFlexoCompressao`). | **Pré-tração**: Perfil I isolado protendido com $P_1 = P_0 - \Delta P_{anc} + \Delta P(q_2) + \Delta P(q_s)$, resistente via $M_{Rd,I}$. **Pós-tração**: Seção mista sem protensão ($P_1 = 0$), resistente via **Rotina C.1** ($M_{Rd,VM}^+$). |
| **4. Fase 2 (ELU 2 — Serviço $t=0$)** | Combinação normal última: $q_{Sd,2} = 1{,}4(q_1 + q_2 + q_3) + 1{,}4 q_4$. Em cada seção $x$, calcula $\sigma_{pd}$ pela **Rotina H** (com `secao_t0`) e os momentos resistentes $M_{Rd,VMP}^+$ e $M_{Rd,VMP}^-$ pelas **Rotinas 3.1/3.2 e 3.3**. | **Pré-tração**: Parte de $P_1$, remove o incremento transitório $\Delta P(q_s)$ da construção e soma $\Delta P(q_3) + \Delta P(q_4)$. **Pós-tração**: Aplica $P_0$ agora na seção mista, desconta $\Delta P_{anc}$ e soma $\Delta P(q_3) + \Delta P(q_4)$. |
| **5. Fase 3 (ELU 3 — Serviço $t=\infty$)** | Desconta a perda progressiva por relaxação do aço (**Rotina 5**: $P_3 = P_2 - 2{,}5 \psi_{1000} P_2$). Em cada seção $x$, recalcula a excentricidade $e_{p,LNE} = y_{tr,\infty} + e_p$, a tensão $\sigma_{pd}$ (**Rotina H** com `secao_t_inf`) e as resistências últimas. | Comum a ambos: avalia o efeito combinado da relaxação do cabo e da descida da LNE por fluência do concreto ($3n$). |

Ao término de cada fase, o verificador popula o vetor `rel.verificacoes` com objetos `ItemVerificacao` e consolida os flags booleanos `aprovado` de cada seção em `fase.aprovado_fase` e `rel.aprovado_global`.

---

# CAPÍTULO 6 — INTERFACE GRÁFICA INTERATIVA (`gui/app_gui.cpp`) E CLI (`src/main.cpp`)

## 6.1 Arquitetura da Interface Gráfica *VMPCalc* (`gui/app_gui.cpp`)

O arquivo `gui/app_gui.cpp` (com cerca de $1\,990$ linhas) implementa uma estação de trabalho gráfica completa em $60\text{ FPS}$ utilizando **Raylib** e **Dear ImGui** (via ponte `rlImGui`), projetada segundo a linguagem visual e ergonômica do software acadêmico **NLtool (FECIV/UFU)**.

### 6.1.1 Gerenciamento de Estado e Sincronização em Tempo Real
Toda a interação do usuário é armazenada na estrutura `EstadoInterfaceVMP ui` (`linhas 111-185`), que guarda os índices de combos, flags de visualização, textos, escalas de diagramas e estados de abertura de modais. A cada quadro do loop principal (`while (!WindowShouldClose())`), antes de desenhar a tela:
1. A função `sincronizar_modelo_viga(viga, ui)` (`linhas 238-354`) traduz as escolhas da interface para a estrutura de domínio `VigaVMP` — calculando automaticamente a largura colaborante $b_{ef}$ a partir dos vãos transversais esquerdo/direito ($L_{esq}, L_{dir}$) e condições de borda, o módulo secante do concreto $E_{cs}$ em função do tipo de agregado (granito, basalto, calcário ou arenito) segundo a NBR 6118, as dimensões da fôrma *steel deck* (Metform MF-50, MF-75 ou Polydeck 59S), as propriedades do catálogo Série W (`CATALOGO_SERIE_W` com 8 perfis comerciais da ABNT NBR 15980) e convertendo cargas por área ($\text{kN/m}^2$) em cargas lineares ($\text{kN/m}$);
2. Em seguida, `RelatorioVMP rel = verificar_viga_vmp(viga)` é executado instantaneamente em memória, atualizando todos os diagramas e tabelas em tempo real a cada alteração de parâmetro.

### 6.1.2 Layout de Quatro Zonas e Diálogos Modais
A janela gráfica é particionada dinamicamente em quatro regiões coordenadas:
1. **Barra de Menu Superior (`BeginMainMenuBar`)**: Menus *Arquivo* (com carregamento em 1 clique dos dois Modelos de Referência de Turini, 2021), *Materiais e Seções*, *Visualizar* (controle de grade, cotas, pontos de discretização e escala global de fonte UI de $90\%$ a $130\%$), *Norma* (chaveamento instantâneo entre NBR 8800:2008 e NBR 8800:2024) e *Sobre*;
2. **Top Toolbar — Ribbon por Abas (`TopToolbar`)**: Barra superior de $132\text{ px}$ dividida em 6 abas de fluxo de trabalho:
   - *Página Inicial*: Nome do projeto, seletor de norma, botões de acesso aos modais de materiais, vão $L_v$, $L_b$ e checkboxes de auxílios visuais;
   - *Protensão*: Escolha entre cabo retilíneo ou poligonal, excentricidade $e_p$, $X_{ep}$, pré-tração vs. pós-tração, ancoragem ativa-ativa, acomodação de cunha $\Delta_p$, tipo de aço ativo (CP-190/210/175 RB), diâmetro da cordoalha e entrada manual ou automática de $P_0$;
   - *Carregamentos*: Modo de entrada por área ($\text{kN/m}^2$) ou linear direto ($\text{kN/m}$) para $q_1, q_2, q_s, q_3, q_4$ e controle de escala visual das setas de carga;
   - *Discretização*: Slider do número de seções $X$ ($3$ a $11$), vãos transversais de influência e combo de seleção da seção monitorada;
   - *Análise*: Coeficientes $\psi_1, \psi_2$, botão de execução, botão de abertura do quadro comparativo 2008 vs. 2024 e diagnóstico global;
   - *Resultados*: Seletor da fase ativa (`ELU 1`, `ELU 2`, `ELU 3`) e do diagrama plotado sobre a barra (*Momento Fletor $M_{Sd}^+$ vs $M_{Rd}^+$*, *Esforço Cortante $V_{Sd}$ vs $V_{Rd}$*, *Força de Protensão $P(x)$* ou *Índice de Flexo-Compressão $I_a(x) \le 1{,}00$*).
3. **Left Panel — Inspetor Lateral (`LeftPanel`)**: Painel esquerdo fixo contendo duas abas:
   - *Propriedades*: Três tabelas técnicas formatadas (`SectionPropertyTable`, `MaterialPropertyTable` e `ProtensaoPropertyTable`) exibindo todas as propriedades calculadas do perfil I, da laje homogeneizada e do dimensionamento da protensão;
   - *Resultados da Seção*: Inspetor detalhado do nó selecionado $S_k$ (`ResultsLimitTable`) e tabela resumo clicável de todas as seções da barra (`AllSectionsTable`).
4. **Viewport CAD 2D Interativo (Raylib)**: Ocupa toda a área central/direita da tela, dividida em duas vistas técnicas desenhadas com primitivas vetoriais anti-aliased:
   - *Vista Longitudinal (60% à esquerda — `desenhar_vista_longitudinal_nltool`)*: Desenha a viga mista com textura de brita no concreto, conectores de cisalhamento, apoios fixo e móvel, trem de setas de carregamento distribuído, traçado do cabo (reto ou poligonal com desviadores), diagrama de esforços preenchido com linha tracejada do patamar resistente e nós discretizados $S_0 \dots S_n$ **interativos ao mouse** (exibindo *Tooltip* completo ao passar o cursor e selecionando a seção no inspetor lateral ao clicar);
   - *Corte Transversal Cotado (40% à direita — `desenhar_secao_transversal_tecnica`)*: Desenha em escala a seção transversal contendo a laje (maciça ou com ondas trapezoidais da fôrma *steel deck*), barras de armadura passiva, perfil I metálico, linha tracejada da Linha Neutra Elástica ($y_{tr}$), cabos externos de protensão e cotas técnicas automáticas ($b_{ef}, t_c, h_F, d, b_f, e_p, t_w, t_f$).
5. **Cinco Diálogos Modais (`BeginPopupModal`)**: Janelas dedicadas para edição aprofundada do *Perfil I e Aço* (`SectionDialog`), *Laje e Steel Deck* (`SlabDialog`), *Armaduras Passivas* (`RebarDialog`), *Sobre o Aplicativo* (`AboutDialog`) e o **Modal Comparativo ABNT NBR 8800:2008 vs 2024** (`CompareDialog`), que executa o verificador nas duas normas simultaneamente e exibe a variação percentual ($\Delta\%$) de cada grandeza estrutural.

## 6.2 Executável de Console (`src/main.cpp`)

O arquivo `src/main.cpp` implementa o alvo `vmp_demo`. Ele define duas funções construtoras (`criar_exemplo1_turini()` para a viga pré-tracionada W360x57,8 de $12\text{ m}$ e `criar_exemplo2_turini()` para a viga pós-tracionada W460x89 de $15\text{ m}$ com *steel deck*) e a função `imprimir_resumo_comparativo`, que roda `verificar_viga_vmp` com `NBR8800_2008` e `NBR8800_2024` e imprime no terminal uma tabela alinhada lado a lado confrontando $N_{Rd,I}$, $M_{Rd,I}$, $V_{Rd}$, $P_{0,\max}$, $P_1$, $P_2$, $P_3$, $M_{Sd}^+$, $M_{Rd,VMP}^+$, $M_{Sd}^-$, $M_{Rd,VMP}^-$ e o status global de aprovação no ELU.

---

# CAPÍTULO 7 — SUÍTE DE TESTES AUTOMATIZADOS E VALIDAÇÃO (`tests/`)

Para garantir rigor numérico e impedir regressões durante a evolução do código para a ABNT NBR 8800:2024, o projeto conta com uma suíte completa de testes unitários automatizados na pasta `tests/`.

## 7.1 Micro-Framework de Testes (`tests/test_framework.h` e `tests/test_main.cpp`)

O cabeçalho `tests/test_framework.h` implementa um framework TDD leve, sem dependências externas, baseado no padrão *Singleton* (`TestRegistry::instance()`):
- A macro `TEST_CASE("Nome do Teste")` cria uma função estática e instancia um objeto global `AutoRegister` que registra automaticamente o teste no vetor `tests_` antes da execução da função `main()`;
- O método `run_all()` habilita cores ANSI no console do Windows (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`), executa cada teste dentro de um bloco `try/catch`, contabiliza sucessos e falhas e retorna código `0` ao CTest se todos passarem;
- São fornecidas quatro macros de asserção científica com mensagens diagnósticas detalhadas (valor obtido, valor esperado, erro absoluto, erro relativo percentual, arquivo e linha):
  - `ASSERT_TRUE(expr)`: Verifica condições booleanas;
  - `ASSERT_NEAR(actual, expected, tol)`: Verifica se $|x - x_{ref}| \le \text{tol}$;
  - `ASSERT_PERCENT(actual, expected, max_percent)`: Verifica se o desvio relativo percentual $|x - x_{ref}| / |x_{ref}| \times 100\%$ está abaixo do limite tolerado (usualmente entre $0{,}01\%$ e $0{,}20\%$, ideal para absorver arredondamentos de duas casas decimais das tabelas de Turini, 2021);
  - `ASSERT_APPROX(actual, expected, rel_tol, abs_tol)`: Tolerância mista relativa e absoluta.

## 7.2 Casos de Teste Implementados (`test_rotinas_nbr8800.cpp` e `test_rotinas_vmp.cpp`)

A suíte totaliza **27 casos de teste unitários e de integração**, divididos em dois arquivos:

**Tabela 7.1 — Catálogo completo dos testes automatizados do projeto VMP**

| Arquivo de Teste | Casos de Teste Implementados | O que é Validado e Aferido |
| :--- | :--- | :--- |
| `test_rotinas_nbr8800.cpp` (13 testes) | • Rotinas A, B, C.1, C.2 (Ex. 1 e Ex. 2 adaptados), C.3 e D em `nbr8800::v2008`<br>• Rotinas A, B, C.1 (LNP na laje e no aço), C.2, C.3 e D em `nbr8800::v2024` | Aferição exata de $N_{Rd,I} = 1416{,}61\text{ kN}$, $M_{Rd,I} = 285{,}98\text{ kN.m}$, $V_{Rd} = 532{,}21\text{ kN}$, $M_{Rd,VM}^+ = 1296{,}81\text{ kN.m}$, $M_{Rd,VM}^- = 789{,}50\text{ kN.m}$, mudança de $k_v = 5{,}0 \to 5{,}34$, método da largura efetiva e LNP no perfil de aço na NBR 8800:2024. |
| `test_rotinas_vmp.cpp` (14 testes) | • Rotina 1 ($t=\infty$), Rotina 2 ($P_{0,\max}$)<br>• Rotina 3.1, 3.2 e 3.3 em `v2008` e `v2024`<br>• Rotina 4 ($A_p$), Rotina 5 (ancoragem e relaxação), Rotina 6 ($\Delta P$)<br>• Rotina G ($I_{laje}$ maciça e *steel deck*), Rotina H ($\sigma_{pd}$)<br>• Verificador Completo: Exemplo 1 (Pré-tração, 5 seções)<br>• Verificador Completo: Exemplo 2 (Pós-tração, 5 seções)<br>• Comparativo Direto NBR 8800:2008 vs. 2024 | Aferição integral das Tabelas 12 a 15 e 20 a 21 de Turini (2021) com erro $< 0{,}15\%$, validação da distribuição parabólica de momentos nas $X$ seções, reprovação esperada da Fase 2 no Exemplo 2 com $P_0 = P_{0,\max}$ por momento negativo de protensão, redução de resistência com $\eta_c$ para concreto C50 ($f_{ck} = 50\text{ MPa}$) e nova formulação de FLD ($C_{dist}$ Tabela L.1) na NBR 8800:2024. |

---

# CAPÍTULO 8 — COMO COMPILAR, EXECUTAR E ESTENDER O SISTEMA

## 8.1 Compilação e Execução via CMake (`CMakeLists.txt`)

O arquivo `CMakeLists.txt` na raiz do projeto requer **CMake 3.20+** e um compilador compatível com **C++20** (como GCC/MinGW-w64 no `w64devkit`). O sistema detecta automaticamente a presença da biblioteca **Raylib** em `C:/raylib/` para habilitar a compilação do alvo gráfico `vmp_gui`:
- Para configurar e compilar todos os alvos a partir da raiz do projeto:
  - `cmake -B build`
  - `cmake --build build`
- Para executar a suíte de testes unitários:
  - `./build/vmp_tests.exe` (ou `ctest --test-dir build --output-on-failure`)
- Para executar o comparativo de console entre a ABNT NBR 8800:2008 e 2024:
  - `./build/vmp_demo.exe`
- Para iniciar a estação gráfica interativa *VMPCalc*:
  - `./build/vmp_gui.exe`

## 8.2 Pontos de Extensão Previstos na Arquitetura

O código-fonte já dispõe de estruturas de dados e assinaturas preparadas (`[STAND-BY]`) para extensões futuras sem necessidade de refatoração arquitetural:
1. **Traçado Poligonal com Desviadores (`CABO_POLIGONAL`)**: As assinaturas de `calcularPerdaImediataTracadoPoligonal` (Rotina 5.1, para perda por atrito nos desviadores + encunhamento nos 3 trechos) e `calcularIncrementoDpTracadoPoligonal` (Rotina 6.2), bem como a estrutura `ForcasProtensaoTrechos` e a classificação espacial em `gerarSecoesViga`, já estão integradas ao pipeline de `verificar_viga_vmp` e à renderização gráfica em `gui/app_gui.cpp`;
2. **Conectores de Cisalhamento e Armadura de Costura (Rotinas E e F)**: As estruturas `ConectorCisalhamento`, `ResultadoRotinaE` (`calcular_rotina_e_perfil_u`) e `ResultadoRotinaF` (`calcular_rotina_f_armadura_costura`) já estão declaradas em `include/modelos.h`, `include/rotinas_nbr8800.h` e `include/verificador.h`, prontas para receberem a verificação de interação parcial e fendilhamento longitudinal da laje.
