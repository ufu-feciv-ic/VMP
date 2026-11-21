# DIFERENÇAS ENTRE A ABNT NBR 8800:2008 E A ABNT NBR 8800:2024 APLICADAS NO CÓDIGO DO PROJETO VMP

> **MEMORIAL TÉCNICO E DOCUMENTAÇÃO DE ENGENHARIA DAS ATUALIZAÇÕES NORMATIVAS IMPLEMENTADAS EM C++20 (`nbr8800::v2008` vs. `nbr8800::v2024`)**

---

# CAPÍTULO 1 — ARQUITETURA DE COEXISTÊNCIA E SELEÇÃO NORMATIVA NO CÓDIGO

## 1.1 Estratégia de Versionamento Normativo em C++20

Com a publicação da **ABNT NBR 8800:2024** após dezesseis anos de vigência da **ABNT NBR 8800:2008**, o projeto **VMP** foi estruturado para manter ambas as edições normativas ativas simultaneamente no código-fonte. Essa arquitetura permite:
1. Preservar integralmente a aferição original com as dissertações e trabalhos de referência de **Ribeiro (2018)** e **Turini (2021)**, formulados sobre a edição de 2008 (Anexo O de Vigas Mistas);
2. Avaliar a mesma estrutura sob os novos critérios da edição de 2024 (Capítulo 5 e Anexo L de Vigas Mistas);
3. Confrontar diretamente as duas edições em tempo de execução, tanto no terminal (`src/main.cpp`), quanto na suíte de testes unitários (`tests/`) e no modal comparativo da interface gráfica (`gui/app_gui.cpp`).

Para viabilizar essa coexistência sem duplicação desnecessária de estruturas de dados, o código utiliza o recurso de `inline namespace` do C++20 nos cabeçalhos `include/rotinas_nbr8800.h` e `include/rotinas_vmp.h`:
- **`namespace nbr8800 { inline namespace v2008 { ... } }`**: O namespace `v2008` contém todas as structs de retorno (`ResultadoNormalResistentePerfil`, `ResultadoMomentoResistentePerfil`, `ResultadoMomentoPositivoVM`, `ResultadoMomentoNegativoVM`, `ResultadoCortanteResistentePerfil`, `ResultadoMomentoPositivoVMP`, `ResultadoMomentoNegativoVMP`) e as funções da edição de 2008;
- **`namespace nbr8800::v2024 { ... }`**: O namespace `v2024` reutiliza as mesmas structs de retorno definidas em `v2008`, mas fornece sobrecargas próprias para todas as rotinas cujas equações, coeficientes ou critérios físicos sofreram alteração na **ABNT NBR 8800:2024**.

## 1.2 Despacho Dinâmico no Verificador Multifásico

No arquivo `include/modelos.h` (`linhas 57-60`), foi introduzida a enumeração `EdicaoNBR8800` e o campo correspondente dentro do modelo agregado `VigaVMP` (`linha 217`):
- `NBR8800_2008 = 0`: Aciona as rotinas de `nbr8800::v2008`;
- `NBR8800_2024 = 1`: Aciona as rotinas de `nbr8800::v2024`.

Dentro do orquestrador `verificar_viga_vmp(const VigaVMP& viga)` em `src/verificador.cpp` (`linha 154`), a flag booleana `const bool usar_2024 = (viga.norma == NBR8800_2024);` governa o despacho de todas as verificações resistentes ao longo das três fases construtivas (ELU 1, ELU 2 em $t=0$ e ELU 3 em $t=\infty$).

**Tabela 1.1 — Mapa geral das rotinas e alterações entre `nbr8800::v2008` e `nbr8800::v2024` no código**

| Rotina / Função C++ | Arquivo Fonte | Edição 2008 (`v2008`) | Edição 2024 (`v2024`) | Status da Alteração |
| :--- | :--- | :--- | :--- | :--- |
| **Rotina A**: `calcularNormalResistentePerfil` | `src/rotinas_nbr8800.cpp` | Fator de redução $Q = Q_s \cdot Q_a$ aplicado dentro de $\lambda_0$. | Método unificado da Largura/Área Efetiva ($A_{ef}$) com $\lambda_0$ na área bruta $A_g$. | **Reescrita em `v2024`** |
| **Rotina B**: `calcularMomentoResistentePerfil` | `src/rotinas_nbr8800.cpp` | Estados limites FLT, FLM e FLA para perfis I duplamente simétricos. | Mantém as curvas de FLT, FLM e FLA para perfis I compactos/semicompactos. | Delega para `v2008` |
| **Rotina D**: `calcularCortanteResistentePerfil` | `src/rotinas_nbr8800.cpp` | Coeficiente $k_v = 5{,}0$ para almas sem enrijecedores transversais. | Coeficiente $k_v = 5{,}34$ e correção da expressão elástica com $(\lambda_p/\lambda_w)^2$. | **Reescrita em `v2024`** |
| **Rotina C.1 / 3.1**: Momento Positivo Compacto (`VM` e `VMP`) | `src/rotinas_nbr8800.cpp` e `src/rotinas_vmp.cpp` | Bloco de concreto com $0{,}85 f_{cd}$ constante e equação clássica para LNP no aço. | Fator de fragilidade $\eta_c = (40/f_{ck})^{1/3} \le 1{,}0$ e binário com $\sum Q_{Rd}$ para LNP no aço (Item L.6.3.1.1.1). | **Reescrita em `v2024`** |
| **Rotina C.2 / 3.2**: Momento Positivo Semicompacto (`VM` e `VMP`) | `src/rotinas_nbr8800.cpp` e `src/rotinas_vmp.cpp` | Tensão elástica limite na fibra superior da laje igual a $1{,}00 f_{cd}$. | Tensão elástica limite na fibra superior da laje reduzida para $0{,}85 f_{cd}$ (Item L.6.3.1.2-a). | **Reescrita em `v2024`** |
| **Rotina C.3 / 3.3**: Momento Negativo e FLD (`calcularCdist`) | `src/rotinas_nbr8800.cpp` e `src/rotinas_vmp.cpp` | Fator geométrico $\alpha_g$, tabela antiga de $C_{dist}$ ($21{,}9$ a $7{,}6$) e $M_{cr}$ clássico. | Novo fator $\kappa_g$, rigidez adimensional $\eta_b$, número de semiondas $n_{ondas}$ e nova Tabela L.1 de $C_{dist}$ ($1{,}92$ a $1{,}24$). | **Reescrita em `v2024`** |

---

# CAPÍTULO 2 — COMPRESSÃO AXIAL DO PERFIL METÁLICO (ROTINA A)

A **Rotina A** (`calcularNormalResistentePerfil`, implementada em `src/rotinas_nbr8800.cpp`) calcula a força normal de compressão resistente de cálculo do perfil metálico isolado ($N_{Rd,I}$). Nas vigas mistas protendidas, essa resistência governa diretamente o limite máximo da força inicial de protensão na pré-tração ($P_{0,\max,1}$ na Rotina 2) e o índice de interação à flexo-compressão ($I_a$) em todas as fases.

## 2.1 Formulação da ABNT NBR 8800:2008 (`nbr8800::v2008`, linhas 12–76)

Na edição de 2008 (Item 5.3.2 e Anexo F da NBR 8800:2008), a interação entre a flambagem global por flexão e a flambagem local das chapas componentes do perfil I (mesas — elementos AL; alma — elemento AA) era tratada por meio do **fator de redução total $Q = Q_s \cdot Q_a$**:
1. **Fator $Q_s$ das mesas (elementos AL)**: Comparava-se a esbeltez da mesa $\lambda_f = b_f / (2 t_f)$ com o limite $\lambda_{f,p} = 0{,}56\sqrt{E_a / f_y}$ (perfis laminados) ou $0{,}64\sqrt{E_a / (f_y / k_c)}$ (perfis soldados, com $k_c = 4/\sqrt{\lambda_w}$). Se $\lambda_f > \lambda_{f,p}$, aplicava-se uma redução linear ou hiperbólica para obter $Q_s < 1{,}0$;
2. **Fator $Q_a$ da alma (elemento AA)**: Comparava-se $\lambda_w = d_1 / t_w$ com $\lambda_{w,p} = 1{,}49\sqrt{E_a / f_y}$. Se $\lambda_w > \lambda_{w,p}$, estimava-se a tensão atuante $\sigma = \chi_1 f_y$ (com $\lambda_{0,1} = \sqrt{A_g f_y / N_e}$) e calculava-se a largura efetiva da alma pela fórmula empírica de Winter de 2008:

$$b_{ef,a} = 1{,}92 \, t_w \sqrt{\frac{E_a}{\sigma}} \left(1 - \frac{0{,}34}{\lambda_w} \sqrt{\frac{E_a}{\sigma}}\right) \le d_1, \qquad Q_a = \frac{A_{ef}}{d_1 t_w}$$

3. **Acoplamento de $Q$ dentro de $\lambda_0$ e de $N_{Rd}$**: O fator $Q = Q_s Q_a$ modificava simultaneamente o índice de esbeltez reduzido global $\lambda_0$ e a força resistente final:

$$\lambda_{0,2008} = \sqrt{\frac{Q \cdot A_g \cdot f_y}{N_e}}, \qquad N_{Rd,2008} = \frac{\chi \cdot Q \cdot A_g \cdot f_y}{\gamma_{a1}}$$

## 2.2 Nova Formulação da ABNT NBR 8800:2024 (`nbr8800::v2024`, linhas 475–541)

Alinhada às revisões modernas do ANSI/AISC 360, a **ABNT NBR 8800:2024** aboliu os fatores $Q_s$, $Q_a$ e $Q$, unificando toda a verificação de flambagem local na compressão pelo **Método da Largura/Área Efetiva ($A_{ef}$)** baseado na curva generalizada de Winter com coeficientes $(c_1, c_2)$. A implementação em `nbr8800::v2024::calcularNormalResistentePerfil` executa quatro passos:

1. **Índice de Esbeltez Global sempre na Área Bruta ($A_g$)**:
   O índice de esbeltez reduzido global $\lambda_0$, o fator de redução global $\chi$ e a tensão crítica global de flambagem $f_n$ são determinados diretamente a partir da área bruta $A_g$, sem qualquer fator $Q$:

$$\lambda_{0,2024} = \sqrt{\frac{A_g \cdot f_y}{N_e}}, \qquad \chi = \begin{cases} 0{,}658^{\lambda_0^2}, & \lambda_0 \le 1{,}5 \\ \dfrac{0{,}877}{\lambda_0^2}, & \lambda_0 > 1{,}5 \end{cases}, \qquad f_n = \chi \cdot f_y$$

2. **Limites de Esbeltez Local Escalados pela Tensão Crítica Global ($\lambda_{r,ef}$)**:
   Em vez de comparar $\lambda_f$ e $\lambda_w$ com limites fixos baseados em $f_y$, a norma de 2024 corrige o limite de esbeltez de cada elemento pela razão $\sqrt{f_y / f_n}$ (pois uma barra mais longa, que flamba globalmente sob tensão $f_n < f_y$, é menos suscetível à flambagem local):

$$\lambda_{f,ef} = \lambda_{f,r} \sqrt{\frac{f_y}{f_n}}, \qquad \lambda_{w,ef} = \lambda_{w,r} \sqrt{\frac{f_y}{f_n}}$$

   onde $\lambda_{f,r} = 0{,}56\sqrt{E_a/f_y}$ (laminado) ou $0{,}64\sqrt{k_c E_a/f_y}$ (soldado, com $k_c \in [0{,}35; \; 0{,}76]$) e $\lambda_{w,r} = 1{,}49\sqrt{E_a/f_y}$.

3. **Curva Unificada de Winter $(c_1, c_2)$ para Mesas (AL) e Alma (AA)**:
   Para cada elemento cuja esbeltez supere o limite efetivo ($\lambda > \lambda_{ef}$), calcula-se a tensão de flambagem elástica local $f_{el}$ e a largura efetiva $b_{ef}$ (ou altura efetiva da alma $h_{ef}$):

$$f_{el} = \left(c_2 \frac{\lambda_r}{\lambda}\right)^2 f_y, \qquad b_{ef} = b \left(1 - c_1 \sqrt{\frac{f_{el}}{f_n}}\right) \sqrt{\frac{f_{el}}{f_n}} \le b$$

   - Para as **4 meias-mesas (elementos AL)** (`linhas 503-513`): utilizam-se os coeficientes unificados $c_1 = 0{,}22$ e $c_2 = 1{,}49$, resultando na redução de área das mesas $\Delta A_{mesas} = 4 (b_f/2 - b_{ef,\text{meia-mesa}}) t_f$;
   - Para a **alma (elemento AA)** (`linhas 521-530`): utilizam-se os coeficientes unificados $c_1 = 0{,}18$ e $c_2 = 1{,}31$, resultando na redução de área da alma $\Delta A_{alma} = (d_1 - h_{ef,alma}) t_w$.

4. **Área Efetiva Total ($A_{ef}$) e Compatibilidade de Relatório**:
   Obtém-se a área efetiva da seção comprimida $A_{ef} = \max(0, \; A_g - \Delta A_{mesas} - \Delta A_{alma})$ e a força normal resistente:

$$N_{Rd,2024} = \frac{\chi \cdot A_{ef} \cdot f_y}{\gamma_{a1}}$$

   Para manter total rastreabilidade comparativa na interface gráfica e nos relatórios com a edição de 2008, o código preenche os campos da struct `ResultadoNormalResistentePerfil` com as razões de área equivalentes (`linhas 535-537`):
   - `res.Qs = (Ag - delta_A_mesas) / Ag;`
   - `res.Qa = (Ag - delta_A_alma) / Ag;`
   - `res.Q  = Aef_mm2 / Ag;`

---

# CAPÍTULO 3 — FORÇA CORTANTE RESISTENTE DA ALMA (ROTINA D)

A **Rotina D** (`calcularCortanteResistentePerfil`, em `src/rotinas_nbr8800.cpp`) verifica a resistência ao cisalhamento da alma do perfil I ($V_{Rd}$) a partir da força cortante de plastificação $V_{pl} = 0{,}60 \, d \, t_w \, f_y$ e dos parâmetros de esbeltez limite $\lambda_{w,p} = 1{,}10\sqrt{k_v E_a / f_y}$ e $\lambda_{w,r} = 1{,}37\sqrt{k_v E_a / f_y}$.

## 3.1 Atualização do Coeficiente $k_v$ ($5{,}00 \to 5{,}34$)

1. **Em `nbr8800::v2008` (`linhas 78-96`)**: Para almas sem enrijecedores transversais (ou quando a razão $a / h$ supera $3{,}0$ ou $[260/(h/t_w)]^2$), adotava-se o valor aproximado **$k_v = 5{,}0$**;
2. **Em `nbr8800::v2024` (`linhas 543-571`)**: A norma de 2024 adotou a solução teórica exata da estabilidade elástica de placas longas simplesmente apoiadas sob cisalhamento puro, alterando o coeficiente de almas não enrijecidas para **$k_v = 5{,}34$** (`linha 554`):

$$k_{v,2024} = \begin{cases} 5{,}34, & \text{para almas sem enrijecedores transversais ou } \dfrac{a}{h} > 3{,}0 \text{ ou } \dfrac{a}{h} > \left(\dfrac{260}{h/t_w}\right)^2 \\[10pt] 5{,}0 + \dfrac{5{,}0}{(a/h)^2}, & \text{para almas com enrijecedores transversais efetivos} \end{cases}$$

Essa mudança eleva os limites de esbeltez plástica ($\lambda_{w,p}$) e elástica ($\lambda_{w,r}$) da alma pelo fator $\sqrt{5{,}34 / 5{,}00} = 1{,}0334$ (**$+3{,}34\%$**). Para o aço ASTM A572 Grau 50 ($f_y = 345\text{ MPa}, E_a = 200\,000\text{ MPa}$), conforme aferido no teste unitário `Rotina D - NBR 8800:2024` (`tests/test_rotinas_nbr8800.cpp`, `linhas 275-291`):
- $\lambda_{w,p}$ aumenta de **$59{,}22$** (em 2008) para **$61{,}20$** (em 2024);
- $\lambda_{w,r}$ aumenta de **$73{,}75$** (em 2008) para **$76{,}22$** (em 2024).

## 3.2 Regime de Flambagem Elástica por Cisalhamento ($\lambda_w > \lambda_{w,r}$)

Além da atualização de $k_v = 5{,}34$, a implementação em `nbr8800::v2024` (`linha 567`) expressa o regime de flambagem elástica por cisalhamento ($\lambda_w > \lambda_{w,r}$) em função direta da esbeltez da alma $\lambda_w$:

$$V_{Rd,2024} = 1{,}24 \left(\frac{\lambda_{w,p}}{\lambda_w}\right)^2 \frac{V_{pl}}{\gamma_{a1}} \qquad (\text{para } \lambda_w > \lambda_{w,r})$$

Como $\lambda_{w,p}^2$ é diretamente proporcional a $k_v$, almas esbeltas submetidas à flambagem elástica por cisalhamento apresentam um ganho direto de **$+6{,}80\%$** ($5{,}34 / 5{,}00 = 1{,}068$) na força cortante resistente $V_{Rd}$ sob a ABNT NBR 8800:2024.

---

# CAPÍTULO 4 — MOMENTO FLETOR POSITIVO EM VIGAS MISTAS E VMP (ROTINAS C.1, C.2, 3.1 E 3.2)

No Anexo L da **ABNT NBR 8800:2024** (que sucedeu o Anexo O da edição de 2008 para vigas mistas de aço e concreto), foram introduzidas três modificações importantes na determinação do momento fletor resistente positivo, aplicadas no código tanto para a **viga mista convencional sem protensão** (`calcularMomentoPositivoVM` em `src/rotinas_nbr8800.cpp`, `linhas 579-654`) quanto para a **viga mista protendida** (`calcularMomentoPostivoVMP` em `src/rotinas_vmp.cpp`, `linhas 637-722`).

## 4.1 Fator de Fragilidade do Concreto de Alta Resistência ($\eta_c$)

Na edição de 2008, a tensão plástica uniforme equivalente do bloco comprimido de concreto na laje era admitida igual a $0{,}85 f_{cd}$ independentemente da classe do concreto. Em consonância com a atualização da ABNT NBR 6118 para concretos do Grupo II ($f_{ck} > 40\text{ MPa}$), a **ABNT NBR 8800:2024** (Anexo L, Item L.5.3.4 e Item L.6.3.1.1.1, pp. 197 e 202) introduziu o **fator redutor de fragilidade do concreto ($\eta_c$)**:

$$\eta_c = \begin{cases} 1{,}0, & \text{para } f_{ck} \le 40\text{ MPa} \\[8pt] \left(\dfrac{40}{f_{ck}}\right)^{1/3} \le 1{,}0, & \text{para } f_{ck} > 40\text{ MPa} \end{cases}$$

No código (`src/rotinas_nbr8800.cpp`, `linhas 587-589`, e `src/rotinas_vmp.cpp`, `linhas 651-653`), $\eta_c$ é calculado por `std::cbrt(40.0 / concreto.fck_MPa)` e multiplica diretamente a força máxima de compressão da laje ($C_{c,\max}$) e o denominador da altura do bloco comprimido ($a$):

$$C_{c,\max,2024} = 0{,}85 \cdot \eta_c \cdot f_{cd} \cdot b_{ef} \cdot t_c, \qquad a_{2024} = \frac{T_{aço} + T_{cabo}}{0{,}85 \cdot \eta_c \cdot f_{cd} \cdot b_{ef}}$$

- **Impacto prático no código**: Para os Estudos de Caso 1 e 2 com concreto C30 ($f_{ck} = 30\text{ MPa} \le 40\text{ MPa}$), tem-se $\eta_c = 1{,}0$, mantendo $C_{c,\max}$ e $M_{Rd}^+$ idênticos aos de 2008 quando a Linha Neutra Plástica (LNP) situa-se na laje. Contudo, para concretos de maior resistência (por exemplo, C50 com $f_{ck} = 50\text{ MPa}$, validado no teste comparativo em `tests/test_rotinas_vmp.cpp`, `linhas 531-541`), obtém-se $\eta_c = (40/50)^{1/3} \approx 0{,}9283$, reduzindo $C_{c,\max}$ em $7{,}17\%$ e aumentando a profundidade da linha neutra $a$, o que reduz de forma a favor da segurança o momento resistente positivo $M_{Rd,VMP}^+$.

## 4.2 Equação do Momento Positivo quando a LNP Corta o Perfil de Aço (`Item L.6.3.1.1.1-b`)

Quando a força máxima de compressão que a laje de concreto pode oferecer é inferior à força plástica de tração total do perfil e do cabo ($C_{c,\max} < T_{aço} + T_{cabo}$), a Linha Neutra Plástica (LNP) desce para o interior do perfil metálico ($y_p > 0$, cortando a mesa superior ou a alma).

1. **Formulação em `nbr8800::v2008`**:
   - Na **Rotina C.1 (Viga Mista sem protensão, `src/rotinas_nbr8800.cpp`, `linhas 280-281`)**, o momento resistente era tomado em torno do centroide da região tracionada do perfil ($y_t$ medido a partir da face inferior), mas com a força do aço comprimido $C_{aço} = 0{,}5(A_g f_{yd} - C_{c,\max})$ e o braço da laje sem subtrair metade da espessura $t_c$:

$$M_{Rd,VM,2008}^+ = C_{aço} (d - y_t - y_c) + C_{c,\max} (t_c + h_F + d - y_t)$$

   - Na **Rotina 3.1 (VMP com protensão, `src/rotinas_vmp.cpp`, `linhas 320-323`)**, o momento resistente era calculado tomando os momentos de todas as quatro resultantes plásticas em torno da própria Linha Neutra Plástica ($y_p$):

$$M_{Rd,VMP,2008}^+ = C_{c,\max} \left(\frac{t_c}{2} + h_F + y_p\right) + C_{aço} (y_p - y_c) + T_{aço} (d - y_p - y_t) + T_{cabo} (d + e_p - y_p)$$

2. **Formulação em `nbr8800::v2024` (Item L.6.3.1.1.1-b, p. 203 da ABNT NBR 8800:2024)**:
   Na edição de 2024, a expressão normativa unificada para interação completa com LNP no perfil de aço toma os momentos em relação ao centroide da região tracionada do perfil ($y_t$), utilizando a resistência dos conectores de cisalhamento no trecho de momento positivo ($\sum Q_{Rd}$, que sob interação completa e LNP no aço iguala a capacidade da laje $\sum Q_{Rd} = C_{c,\max} = 0{,}85 \eta_c f_{cd} b_{ef} t_c$):
   - Em **`nbr8800::v2024::calcularMomentoPositivoVM`** (`src/rotinas_nbr8800.cpp`, `linhas 638-640`):

$$M_{Rd,VM,2024}^+ = \sum Q_{Rd} (d - y_t - y_c) + C_{c,\max} \left(\frac{t_c}{2} + h_F + d - y_t\right)$$

   - Em **`nbr8800::v2024::calcularMomentoPostivoVMP`** (`src/rotinas_vmp.cpp`, `linhas 705-708`):

$$M_{Rd,VMP,2024}^+ = \sum Q_{Rd} (d - y_t - y_c) + C_{c,\max} \left(\frac{t_c}{2} + h_F + d - y_t\right) + T_{cabo} (e_p + y_t)$$

   Essa diferença explica exatamente o comportamento observado no **Estudo de Caso 2** (perfil W460x89 com laje *steel deck* $b_{ef} = 2000\text{ mm}$, onde a soma $T_{aço} + T_{cabo}$ na Fase 2 supera $C_{c,\max}$, levando a LNP para o perfil de aço): enquanto em `v2008` o momento por equilíbrio em torno de $y_p$ resulta em $1878{,}49\text{ kN}\cdot\text{m}$, a expressão de `v2024` com $\sum Q_{Rd} = C_{c,\max}$ resulta em $2975{,}23\text{ kN}\cdot\text{m}$.

## 4.3 Redução da Tensão Limite no Concreto em Seções Semicompactas ($1{,}00 f_{cd} \to 0{,}85 f_{cd}$)

Para perfis cuja alma classifica-se como **semicompacta** ($3{,}76\sqrt{E_a/f_y} < \lambda_w \le 5{,}70\sqrt{E_a/f_y}$, Rotinas C.2 e 3.2), a seção mista não pode plastificar totalmente por risco de flambagem local da alma antes da plastificação total, devendo o momento resistente ser obtido por análise elástica sobre a seção homogeneizada em $t = 0$ ($W_{tr,inf}$ e $W_{tr,sup}$):

1. **Em `nbr8800::v2008`** (`src/rotinas_nbr8800.cpp`, `linhas 289-290` e `src/rotinas_vmp.cpp`, `linhas 331-332`): A tensão normal máxima de compressão na fibra extrema superior da laje era limitada a **$1{,}00 \, f_{cd}$**:

$$M_{Rd,2008}^+ = \min\left( W_{tr,inf} \cdot f_{yd}, \; \frac{W_{tr,sup} \cdot f_{cd}}{n} \right)$$

2. **Em `nbr8800::v2024`** (Item L.6.3.1.2-a, p. 207 da NBR 8800:2024 — `src/rotinas_nbr8800.cpp`, `linhas 646-647` e `src/rotinas_vmp.cpp`, `linhas 714-715`): A tensão limite de cálculo na fibra superior do concreto comprimido no regime elástico foi reduzida para **$0{,}85 \, f_{cd}$**, harmonizando o teto de tensão elástica com o diagrama parábola-retângulo da NBR 6118:

$$M_{Rd,2024}^+ = \min\left( W_{tr,inf} \cdot f_{yd}, \; \frac{W_{tr,sup} \cdot (0{,}85 \, f_{cd})}{n} \right)$$

Quando o escoamento da mesa inferior de aço ($W_{tr,inf} f_{yd}$) governa a seção semicompacta (o que ocorre na maioria das vigas mistas usuais com laje de grande largura colaborante $b_{ef}$), os valores de `v2008` e `v2024` coincidem; porém, em lajes estreitas onde o esmagamento elástico do concreto governa, a edição de 2024 reduz a resistência elástica em $15\%$.

---

# CAPÍTULO 5 — MOMENTO FLETOR NEGATIVO E FLAMBAGEM LATERAL COM DISTORÇÃO — FLD (ROTINAS C.3 E 3.3)

A mudança analítica mais profunda introduzida pelo Anexo L da **ABNT NBR 8800:2024** ocorreu na formulação da **Flambagem Lateral com Distorção da seção transversal (FLD)** nas regiões submetidas a momento fletor negativo (Itens L.6.5.5 e L.6.5.6, pp. 212–215 da NBR 8800:2024). Nas vigas mistas protendidas, esse estado limite é de importância crítica, pois o momento negativo introduzido pela força de protensão excêntrica ($M_{Sd}^- = 0{,}9 \, P \, e_{p,LNE}$) comprime a mesa inferior do perfil metálico ao longo de todo o vão destravado $L_b$.

Em ambas as versões, a capacidade plástica da seção mista fissurada sob momento negativo ($M_{Rd}^-$, composta pelo perfil de aço, pela armadura passiva da laje $T_{ds} = A_{ds} f_{sd}$ e, na VMP, pela força do cabo $T_{pd} = A_p \sigma_{pd}$) e a rigidez rotacional do pórtico U invertido ($k_r = k_1 k_2 / [k_1 + k_2]$) permanecem idênticas. Contudo, a expressão do **momento crítico elástico de flambagem lateral com distorção ($M_{cr}$)** foi integralmente reformulada em `nbr8800::v2024`.

## 5.1 Formulação Clássica da ABNT NBR 8800:2008 (`v2008`)

Em `nbr8800::v2008` (`src/rotinas_nbr8800.cpp`, `linhas 425-451`, e `src/rotinas_vmp.cpp`, `linhas 491-518`), o cálculo de $M_{cr}$ seguia a formulação original de Roik, Hanswille e Kina adotada no Anexo O de 2008:
1. Calculava-se o parâmetro auxiliar $e = \dfrac{A \, I_x}{A_g \, y_c \, (A - A_g)}$ (com $A = A_g + A_{ds}$) e o **fator geométrico $\alpha_g$**:

$$\alpha_g = \frac{\dfrac{h_0 I_{tr}}{I_x}}{\dfrac{\dfrac{h_0^2}{4} + \dfrac{I_x + I_y}{A_g}}{e + h_0}}$$

2. Interpolava-se o coeficiente de gradiente de momento **$C_{dist,2008}$** na função `nbr8800::v2008::calcularCdist(psi)` (`src/rotinas_vmp.cpp`, `linhas 534-554`), cujos valores variavam de **$21{,}9$** (para $\psi \le 0{,}50$) até **$7{,}6$** (para $\psi \ge 2{,}50$), onde $\psi = |M_{Sd}^-| / M_{Sd}^+$;
3. O momento crítico elástico $M_{cr,2008}$ era obtido por uma expressão com raiz quadrada dos termos de torção de Saint-Venant e rigidez rotacional $k_r$:

$$M_{cr,2008} = \frac{\alpha_g \, C_{dist,2008}}{L_v} \sqrt{\left(G_a I_t + \frac{k_r L_v^2}{\pi^2}\right) E_a I_{af,y}}$$

   onde $I_{af,y} = b_f^3 t_f / 12$ é o momento de inércia da mesa inferior do perfil em torno do eixo $y$.

## 5.2 Nova Formulação da ABNT NBR 8800:2024 (`v2024`, Itens L.6.5.5 e L.6.5.6)

Na **ABNT NBR 8800:2024**, a expressão de $M_{cr}$ foi substituída por uma solução energética explícita baseada no número discreto de semiondas de flambagem ($n_{ondas}$) ao longo do comprimento $L_v$. A implementação em `nbr8800::v2024::calcularMomentoNegativoVM` (`src/rotinas_nbr8800.cpp`, `linhas 667-721`) e `nbr8800::v2024::calcularMomentoNegativoVMP` (`src/rotinas_vmp.cpp`, `linhas 745-795`) compreende cinco etapas:

1. **Distância Centroide a Centroide ($\bar{y}$) e Novo Fator Geométrico ($\kappa_g$)**:
   Determina-se a distância vertical $\bar{y} = |y_{tr} - d/2|$ entre o centroide da seção mista fissurada (aço + armadura da laje) e o centroide do perfil de aço isolado (Figura L.9, p. 211 da NBR 8800:2024). Para seções I duplamente simétricas, o antigo fator $\alpha_g$ foi substituído pelo novo fator adimensional $\kappa_g$ (Item L.6.5.5, p. 213):

$$\kappa_g = \left(0{,}31 + 0{,}69 \cdot 0{,}05^{\bar{y}/h_0}\right) \frac{I_{tr}}{I_x}$$

2. **Parâmetro Adimensional de Rigidez Rotacional ($\eta_b$)**:
   Calcula-se o parâmetro adimensional $\eta_b$ que relaciona a rigidez rotacional do conjunto laje-alma ($k_r$) com a rigidez à flexão lateral da mesa inferior comprimida ($E_a I_{af,y}$) ao longo do vão $L_v$:

$$\eta_b = \sqrt{\frac{k_r \cdot L_v^4}{E_a \cdot I_{af,y} \cdot h_0^2}}$$

3. **Número Discreto de Semiondas de Flambagem ($n_{ondas}$)**:
   Diferentemente da edição de 2008 (que assumia implicitamente uma aproximação contínua), a edição de 2024 determina o número ideal contínuo de semiondas $n_{id} = \sqrt{\eta_b} / \pi$, toma os inteiros adjacentes $n_1 = \max(1, \lfloor n_{id} \rfloor)$ e $n_2 = \max(1, \lceil n_{id} \rceil)$, e seleciona o número inteiro crítico de semiondas $n_{ondas}$ pela média geométrica (`linhas 694-697`):

$$n_{ondas} = \begin{cases} n_1, & \text{se } n_{id} \le \sqrt{n_1 \cdot n_2} \\ n_2, & \text{se } n_{id} > \sqrt{n_1 \cdot n_2} \end{cases}$$

4. **Nova Tabela L.1 do Coeficiente $C_{dist,2024}$ (`nbr8800::v2024::calcularCdist`)**:
   Na função `nbr8800::v2024::calcularCdist(psi)` (`src/rotinas_vmp.cpp`, `linhas 613-635`), foi implementada a nova **Tabela L.1 (p. 214 da ABNT NBR 8800:2024)**, cujos valores foram reescalonados para a nova estrutura algébrica de $M_{cr}$:

**Tabela 5.1 — Comparação dos valores tabelados de $C_{dist}$ no código (`v2008` vs. `v2024`)**

| Relação $\psi = |M_{Sd}^-| / M_{Sd}^+$ | $0{,}50$ | $0{,}75$ | $1{,}00$ | $1{,}25$ | $1{,}50$ | $1{,}75$ | $2{,}00$ | $2{,}25$ | $2{,}50$ |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **`v2008::calcularCdist(psi)`** | $21{,}90$ | $13{,}90$ | $11{,}00$ | $9{,}60$ | $8{,}80$ | $8{,}30$ | $8{,}00$ | $7{,}80$ | $7{,}60$ |
| **`v2024::calcularCdist(psi)` (Tabela L.1)** | **$1{,}92$** | **$1{,}67$** | **$1{,}53$** | **$1{,}44$** | **$1{,}38$** | **$1{,}33$** | **$1{,}30$** | **$1{,}27$** | **$1{,}24$** |

5. **Nova Equação do Momento Crítico Elástico ($M_{cr,2024}$)**:
   Com $\kappa_g$, $n_{ondas}$, $\eta_b$ e $C_{dist,2024}$, o momento crítico elástico na edição de 2024 é calculado somando diretamente a rigidez à torção uniforme ($G_a I_t$) e a rigidez ao empenamento restringido pela mola rotacional $k_r$ (`linhas 706-708`):

$$M_{cr,2024} = C_{dist,2024} \cdot \frac{\kappa_g}{h_0} \left\{ G_a I_t + \frac{E_a I_{af,y} h_0^2}{L_v^2} \left[ (n_{ondas} \pi)^2 + \left(\frac{\eta_b}{n_{ondas} \pi}\right)^2 \right] \right\}$$

A partir de $M_{cr}$, ambos os módulos calculam o índice de esbeltez reduzido à distorção $\lambda_{dist} = \sqrt{|M_{Sd}^-| / M_{cr}}$, o fator de redução $\chi_{dist}$ (igual a $1{,}0$ para $\lambda_{dist} \le 0{,}4$, $0{,}658^{\lambda_{dist}^2}$ para $0{,}4 < \lambda_{dist} \le 1{,}5$ e $0{,}877/\lambda_{dist}^2$ para $\lambda_{dist} > 1{,}5$) e o momento resistente final $M_{Rd}^- = \min(M_{Rd,pl}^-, \; \chi_{dist} M_{Rd,pl}^-)$.

- **Comportamento nos Estudos de Caso**: Quando o momento negativo de protensão $M_{Sd}^-$ é moderado frente à rigidez rotacional $k_r$ da laje (como no Estudo de Caso 1, em que $M_{Sd,2}^- = 138{,}55\text{ kN}\cdot\text{m}$), o índice $\lambda_{dist}$ resulta inferior ao patamar plástico de $0{,}40$ tanto em 2008 quanto em 2024 ($\chi_{dist} = 1{,}0$), fazendo com que $M_{Rd,VMP}^- = 354{,}50\text{ kN}\cdot\text{m}$ coincida nas duas normas. Já para momentos negativos elevados ou vãos destravados maiores ($\lambda_{dist} > 0{,}40$, conforme validado no teste `Comparativo ABNT NBR 8800:2008 vs ABNT NBR 8800:2024` em `tests/test_rotinas_vmp.cpp`, `linhas 543-554`), os valores de $M_{Rd,dist}$ divergem entre as duas edições devido ao novo modelo de semiondas discretas da NBR 8800:2024.

---

# CAPÍTULO 6 — SÍNTESE DOS RESULTADOS COMPARATIVOS NOS ESTUDOS DE CASO E TESTES

A Tabela 6.1 consolida todos os resultados numéricos extraídos diretamente dos executáveis `vmp_demo.exe` e `vmp_tests.exe`, demonstrando como cada uma das seis alterações normativas implementadas no código se manifesta nos diferentes cenários de projeto.

**Tabela 6.1 — Quadro resumo dos resultados numéricos comparativos entre `nbr8800::v2008` e `nbr8800::v2024` no software VMP**

| Cenário de Teste no Código | Grandeza Avaliada | Valor em `v2008` | Valor em `v2024` | Diferença ($\Delta\%$) | Causa da Igualdade ou Divergência no Código |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **Estudo de Caso 1** (W360x57,8, C30, Laje Maciça, Pré-Tração) | Normal resistente $N_{Rd,I}$ | $1416{,}77\text{ kN}$ | $1416{,}77\text{ kN}$ | $0{,}00\%$ | Perfil compacto ($\lambda_f < \lambda_{f,ef}$ e $\lambda_w < \lambda_{w,ef} \Rightarrow Q = 1{,}0$ e $A_{ef} = A_g$). |
| **Estudo de Caso 1** (W360x57,8, $\lambda_w = 38{,}98$) | Coeficiente $k_v$ e limite $\lambda_{w,p}$ ao cortante | $k_v = 5{,}00$<br>$\lambda_{w,p} = 59{,}22$ | $k_v = 5{,}34$<br>$\lambda_{w,p} = 61{,}20$ | $+6{,}80\%$<br>$+3{,}34\%$ | Atualização de $k_v = 5{,}34$ para alma sem enrijecedores na Rotina D. |
| **Estudo de Caso 1** (W360x57,8, $\lambda_w = 38{,}98 \le \lambda_{w,p}$) | Cortante resistente $V_{Rd}$ | $532{,}22\text{ kN}$ | $532{,}22\text{ kN}$ | $0{,}00\%$ | Como $\lambda_w < 59{,}22$, a alma plastifica por cisalhamento ($V_{pl}/\gamma_{a1}$) em ambas. |
| **Estudo de Caso 1** (C30, LNP na laje $a = 48{,}7\text{ mm} < 130\text{ mm}$) | Momento positivo $M_{Rd,VMP}^+$ ($t=0$) | $807{,}87\text{ kN}\cdot\text{m}$ | $807{,}87\text{ kN}\cdot\text{m}$ | $0{,}00\%$ | Para $f_{ck} = 30\text{ MPa} \le 40\text{ MPa}$, tem-se $\eta_c = 1{,}0$ e LNP permanece na laje. |
| **Estudo de Caso 2** (W460x89, C30, Steel Deck, Pós-Tração, LNP no aço) | Momento positivo $M_{Rd,VMP}^+$ ($t=0$) | $1878{,}49\text{ kN}\cdot\text{m}$ | $2975{,}23\text{ kN}\cdot\text{m}$ | $+58{,}38\%$ | LNP corta o perfil de aço ($C_{c,\max} < T_{aço} + T_{cabo}$): `v2024` aplica a expressão do Item L.6.3.1.1.1-b com $\sum Q_{Rd}$. |
| **Concreto de Alta Resistência C50** ($f_{ck} = 50\text{ MPa} > 40\text{ MPa}$) | Fator $\eta_c$, $C_{c,\max}$ e $M_{Rd,VMP}^+$ | $\eta_c = 1{,}000$<br>$C_{c,\max}$ maior | $\eta_c = 0{,}928$<br>$C_{c,\max}$ menor | $-7{,}17\%$ em $C_{c,\max}$ | Fator de fragilidade $\eta_c = (40/50)^{1/3} = 0{,}9283$ reduz o bloco comprimido em `v2024`. |
| **Flexão Negativa Elevada / FLD Inelástica** ($\lambda_{dist} > 0{,}40, \psi = 1{,}0$) | Coeficiente $C_{dist}$ e $M_{Rd,dist}^-$ | $C_{dist} = 11{,}00$ | $C_{dist} = 1{,}53$ | Divergem ($\Delta \neq 0$) | Nova equação de $M_{cr}$ com $\kappa_g$, $\eta_b$, semiondas $n_{ondas}$ e Tabela L.1 em `v2024`. |
