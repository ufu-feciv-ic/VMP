# Dimensionamento de uma viga mista de piso

*Viga mista de aço e concreto · NBR 8800:2024*

Viga biapoiada de $7,0\text{ m}$, em seção mista de aço e concreto sem escoramento, verificada nas fases construtiva e mista segundo a ABNT NBR 8800:2024.

> **Escopo do estudo:** caso reproduzível destinado a demonstrar o encadeamento do cálculo. Não representa obra executada nem substitui a avaliação do responsável técnico.

---

## 1. Modelo estrutural e materiais

Uma viga de piso biapoiada vence o vão $L = 7,0\text{ m}$. As vigas paralelas adjacentes estão a $1,75\text{ m}$ à esquerda e a $1,75\text{ m}$ à direita. O perfil de aço é laminado, a laje é maciça e a conexão de cisalhamento é feita por pinos com cabeça soldados à mesa superior. A construção é não escorada: o perfil de aço atua isoladamente até a cura do concreto.

* **Sistema estrutural:** Viga biapoiada, vão $L = 7,0\text{ m}$, não escorada; comprimento destravado $L_b = 3,5\text{ m}$; ocupação garagem.
* **Perfil de aço:** W360×51,0, $f_y = 345\text{ MPa}$ — $d = 355\text{ mm}$, $b_f = 171\text{ mm}$, $t_f = 11,6\text{ mm}$, $t_w = 7,2\text{ mm}$, $A = 63,56\text{ cm}^2$, $I_x = 14\,222\text{ cm}^4$.
* **Laje:** Maciça, $h_c = 120\text{ mm}$, concreto C30 ($f_{ck} = 30\text{ MPa}$; $E_{cs} = 26\,838\text{ MPa}$); largura efetiva $b_{ef} = 1750\text{ mm}$.
* **Conexão de cisalhamento:** Pino com cabeça (stud) $d_{cs} = 19\text{ mm}$, altura de $100\text{ mm}$ e $f_u = 420\text{ MPa}$.
* **Geometria de vibração:** Espaçamento entre vigas secundárias $b = 1,75\text{ m}$; $k_b = 2$ (“demais casos”); $n_p = 3$ vãos principais; $L_p = 7,0\text{ m}$; critério selecionado $a_p$.

---

## 2. Ações e combinações por fase

As ações são separadas em permanentes ($g$) e variáveis ($q$), por fase construtiva. A Fase 1 corresponde ao perfil de aço isolado, antes da cura; a Fase 2, à seção mista em serviço. Cargas superficiais, expressas em $\text{kN/m}^2$, são convertidas em cargas lineares pela largura de influência da laje, expressa em metros.

$$b_{\text{inf}} = \frac{d_{\text{esq}} + d_{\text{dir}}}{2} = \frac{1,75 + 1,75}{2} = 1,75\text{ m}$$

$$q_{\text{viga}}\ [\text{kN/m}] = p_{\text{laje}}\ [\text{kN/m}^2] \cdot b_{\text{inf}}\ [\text{m}]$$

A largura de influência é aplicada às ações superficiais. A largura efetiva da seção mista é determinada separadamente:

### 2.2 Largura Efetiva (L.6.2)

*(Viga de Borda / Viga Central)*

$$b_{ef} = \begin{cases} \min\left(\dfrac{L}{8}, d_{\text{borda}}\right) + \min\left(\dfrac{L}{8}, \dfrac{d_{\text{prox}}}{2}\right), & \text{Viga de Borda} \\ \min\left(\dfrac{L}{8}, \dfrac{d_{\text{esq}}}{2}\right) + \min\left(\dfrac{L}{8}, \dfrac{d_{\text{dir}}}{2}\right), & \text{Viga Central} \end{cases}$$

$$\frac{L}{8} = \frac{7000,00}{8} = 875,00\text{ mm}$$

$$\text{Lado esquerdo: } \min(875,00;\ 875) = 875\text{ mm}$$

$$\text{Lado direito: } \min(875,00;\ 875) = 875\text{ mm}$$

$$b_{ef} = 875 + 875 = 1750\text{ mm}$$

Largura efetiva da laje: $b_{ef} = 1750\text{ mm}$.

### Fase 1 — Construção (perfil de aço isolado)

| Ação | Símbolo | Composição | Resultado |
| :--- | :---: | :--- | :---: |
| Peso próprio do perfil e da laje | $g_1$ | $0,499\text{ kN/m} + (25\text{ kN/m}^3 \cdot 0,12\text{ m}) \cdot 1,75\text{ m}$ | $5,749\text{ kN/m}$ |
| Ação variável de construção | $q_1$ | $100\text{ kgf/m}^2 \approx 1,00\text{ kN/m}^2;\ 1,00 \cdot 1,75$ | $1,75\text{ kN/m}$ |

O peso próprio da laje é uma ação superficial e é multiplicado pela largura de influência. O peso próprio do perfil, obtido da massa por metro do W360×51,0, já é uma ação linear e entra diretamente em $g_1$, sem nova multiplicação pela largura.

* **Combinação última:** $1,25 g_1 + 1,30 q_1$

### Fase 2 — Seção mista (após a cura)

| Ação | Símbolo | Composição | Resultado |
| :--- | :---: | :--- | :---: |
| Ações permanentes após a cura | $g_2$ | $4,00\text{ kN/m}^2 \cdot 1,75\text{ m}$ | $7,00\text{ kN/m}$ |
| Ação variável de uso — garagem | $q_2$ | $3,00\text{ kN/m}^2 \cdot 1,75\text{ m}$ | $5,25\text{ kN/m}$ |

Em $g_2$ entram as ações permanentes aplicadas depois da cura — como revestimentos, alvenarias, impermeabilização e instalações permanentes — conforme o uso e a solução construtiva. Em $q_2$ entra a ação variável correspondente ao uso, definida conforme a ABNT NBR 6120. Cada parcela superficial é convertida separadamente por $p_{\text{laje}} b_{\text{inf}}$; cargas pontuais permanecem em $\text{kN}$.

* **Combinação última:** $1,40 (g_1 + g_2) + 1,50 q_2$

---

## 3. Verificação à flexão

Na seção mista, a resistência de cálculo decorre da distribuição plástica de tensões: a resultante de tração no aço equilibra a resultante de compressão no concreto, fixando a posição da linha neutra plástica e o momento fletor resistente correspondente à distribuição plástica.

### 2.4 Resistência à Flexão (L.6.3 e L.6.4)

$$f_{yd} = \frac{f_y}{\gamma_{a1}} = \frac{345}{1,10} = 313,64\text{ MPa}$$

$$f_{cd} = \frac{f_{ck}}{\gamma_c} = \frac{30}{1,40} = 21,43\text{ MPa}$$

$$h_c = t_c = 120\text{ mm}, \quad d = 355\text{ mm}$$

$$F_a = f_{yd} \times A_a = 313,64 \times 6356 \times 10^{-3} = 1993,52\text{ kN}$$

$$F_c = 0,85 \times f_{cd} \times b_{ef} \times h_c = 0,85 \times 21,43 \times 1750 \times 120 \times 10^{-3}$$

$$F_c = 3825,00\text{ kN}$$

$$\text{LNP na Laje } (F_c \geq F_a): \quad a = \frac{F_a \times 1000}{0,85 \times f_{cd} \times b_{ef}} = 62,5\text{ mm}$$

$$M_{Rd,\text{plástico}} = 530,73\text{ kN}\cdot\text{m} \quad (\text{resistência pela distribuição plástica, antes dos limites de instabilidade})$$

$$M_{Rd} = 530,73\text{ kN}\cdot\text{m} \quad (\text{momento fletor resistente de cálculo final})$$

Equilíbrio com $F_a = 1993,52\text{ kN}$, linha neutra na laje em $a = 62,5\text{ mm}$ e $M_{Rd} = 530,73\text{ kN}\cdot\text{m}$.

---

## 4. Conexão de cisalhamento

A resistência de cálculo de um conector é o menor valor entre a ruptura do concreto que o envolve e a ruptura do aço do pino. A quantidade e o espaçamento decorrem da força a transferir, respeitados os limites geométricos da norma. A força horizontal na interface é limitada pela soma das resistências dos conectores do trecho.

### 2.3 Resistência dos Conectores (L.8)

**Dados do conector (pino com cabeça — stud):**
* Diâmetro: $d = 19\text{ mm}$
* Resistência à tração: $f_u = 420\text{ MPa}$
* Coeficiente de ponderação: $\gamma_{cs} = 1,25$
* Concreto: $f_{ck} = 30\text{ MPa}$; $E_{cs} = 26838\text{ MPa}$

**Área da Seção Transversal do Pino:**

$$A_{cs} = \frac{\pi \cdot d^2}{4} = \frac{\pi \times 19^2}{4} = 283,5\text{ mm}^2$$

**Coeficientes aplicáveis ao modo de ruptura do aço do pino (L.8.2.1.2 e L.8.2.1.3):**
* Fôrma de aço: Laje Maciça
* Conectores por nervura: $n = 1$
* Coeficientes adotados: $R_g = 1,00$; $R_p = 1,00$

**Resistência por Ruptura do Concreto (L.8.2.1.1-a):**

$$Q_{Rd1} = \frac{A_{cs} \sqrt{f_{ck} \cdot E_{cs}}}{2 \gamma_{cs}} = \frac{283,5 \times \sqrt{30 \times 26838}}{2 \times 1,25} = 101,76\text{ kN}$$

**Resistência por Ruptura do Aço do Pino (L.8.2.1.1-b):**

$$Q_{Rd2} = \frac{R_g \cdot R_p \cdot A_{cs} \cdot f_u}{\gamma_{cs}} = \frac{1,00 \times 1,00 \times 283,5 \times 420}{1,25} = 95,27\text{ kN}$$

**Resistência de Cálculo Adotada:**

$$Q_{Rd} = \min(Q_{Rd1}, Q_{Rd2}) = \min(101,76;\ 95,27) = 95,27\text{ kN}$$

Ruptura do concreto e do aço do pino; com $f_u = 420\text{ MPa}$, adota-se $Q_{Rd} = 95,27\text{ kN}$.

### 2.3.1 Disposição, Quantidade e Espaçamento dos Conectores (L.8.3)

* **Método adotado:** Uniforme (contínuo)

**Espaçamentos normativos (L.8.3.3 e L.8.3.4):**

$$s_{\min,\text{long}} = 6 d_{cs} = 6 \times 19 = 114\text{ mm} \quad (\text{direção longitudinal})$$

$$s_{\min,\text{transv}} = 4 d_{cs} = 4 \times 19 = 76\text{ mm} \quad (\text{direção transversal})$$

$$s_{\max} = 8 (t_c + h_f) = 8 \times (120 + 0) = 960\text{ mm}$$

**Espaçamento adotado:**

$$s_{\text{adotado}} = 150\text{ mm}$$

$46$ conectores uniformemente espaçados a $150\text{ mm}$ em todo o vão.

Espaçamentos mínimo e máximo verificados; $46$ conectores a $150\text{ mm}$ ao longo do vão.

### 5.1 Cisalhamento Longitudinal na Interface Laje–Viga (L.5.3)

Solicitação de cálculo na região de momento positivo, determinada diretamente por L.5.3.4. O fator de assimetria é $n = 2 b_1 / (b_1 + b_2)$, com $b_1 \geq b_2$.

$$\Sigma Q_{m,Rd} = \min(\Sigma Q_{Rd,\text{tramo}}; F_{hd}) = \min(2191,11;\ 1993,52) = 1993,52\text{ kN}$$

$$n = \frac{2 b_1}{b_1 + b_2} = \frac{2 \times 875}{875 + 875} = 1,000$$

$$\nu_{Sd,a-a} = \frac{n \Sigma Q_{m,Rd} - A_{\text{long}} f_{sd} - 0,85 \eta_c f_{cd} A_{blc}}{L_m} = 462,79\text{ kN/m}$$

Superfície governante: `a-a`; $\nu_{Sd} = 462,79\text{ kN/m}$.

Força efetiva do trecho: $1993,52\text{ kN}$; na superfície crítica, $\nu_{Sd} = 462,79\text{ kN/m}$.

---

## 5. Estabilidade do perfil de aço e cortante

Na Fase 1, o perfil de aço é verificado isoladamente quanto à flambagem lateral com torção (FLT) e às flambagens locais da mesa (FLM) e da alma (FLA), pelo Anexo D. O momento resistente é o menor das três verificações. A resistência ao cortante é dada pela alma.

### Perfil W/I duplamente simétrico

O cálculo segue o procedimento do Anexo D.2.1, que é um caso particular da fórmula geral com $\beta_x = 0$, resultando na expressão clássica para $M_{cr}$.

**Passo 1: Propriedades Geométricas**

$$I_y = 9,67 \times 10^6\text{ mm}^4$$

$$J = 2,21 \times 10^5\text{ mm}^4$$

$$C_w = 2,85 \times 10^{11}\text{ mm}^6$$

**Passo 2: Momento Crítico Elástico**

$$M_{cr} = \frac{C_b \pi^2 E I_y}{L_b^2} \sqrt{\frac{C_w}{I_y} \left(1 + 0,039 \frac{J L_b^2}{C_w}\right)} \quad (\text{Anexo D})$$

Parâmetros: $C_b = 1,00$, $E = 200\,000\text{ MPa}$, $L_b = 3500\text{ mm}$

$$\frac{C_b \pi^2 E I_y}{L_b^2} = \frac{1,00 \times \pi^2 \times 200\,000 \times 9,67 \times 10^6}{3500^2} = 1,56 \times 10^6\text{ N}$$

$$\sqrt{\frac{C_w}{I_y} \left(1 + 0,039 \frac{J L_b^2}{C_w}\right)} = \sqrt{\frac{2,85 \times 10^{11}}{9,67 \times 10^6} \left(1 + 0,039 \frac{2,21 \times 10^5 \times 3500^2}{2,85 \times 10^{11}}\right)} = 4,04 \times 10^4\text{ mm}^2$$

$$M_{cr} = 1,56 \times 10^6 \times 4,04 \times 10^4 = 313,01\text{ kN}\cdot\text{m}$$

**Passo 3: Regime de FLT**

$$M_{pl} = 309,60\text{ kN}\cdot\text{m}$$

Regime: Inelástico (Anexo D.2.1 — curva $\chi_{LT}$, $\alpha = 0,49$; limites $\lambda_{LT} = 0,4$ e $1,4$)

**Passo 4: Momento Resistente**

Método: equação linear de redução $\chi_{LT}$ (Anexo D.2.1)

$$\lambda_{LT} = \sqrt{\frac{M_{pl}}{M_{cr}}} = 0,995$$

$$\chi_{LT} = 1,0 - 0,49 (\lambda_{LT} - 0,4) = 0,709$$

$$M_{Rd,FLT} = \frac{\chi_{LT} \cdot M_{pl}}{\gamma_{a1}} = 199,46\text{ kN}\cdot\text{m}$$

FLT: $M_{cr} = 313,01\text{ kN}\cdot\text{m}$, regime inelástico e $M_{Rd,FLT} = 199,46\text{ kN}\cdot\text{m}$.

### 2.5.3 FLA — Flambagem Local da Alma (5.4.2.1; Tabela D.1)

**Parâmetros de esbeltez:**

$$\lambda_{FLA} = \frac{h_w}{t_w} = \frac{331,8}{7,2} = 46,1$$

$$\lambda_p = 3,76 \sqrt{\frac{E}{f_y}} = 90,5$$

$$\lambda_r = 5,70 \sqrt{\frac{E}{f_y}} = 137,2$$

**Critério de verificação:**

$$\lambda_{FLA} = 46,1 \leq \lambda_p = 90,5 \implies \text{Alma compacta}$$

$$M_{Rd,FLA} = \frac{M_{pl}}{\gamma_{a1}} = 281,45\text{ kN}\cdot\text{m}$$

**Momento Resistente do Perfil Isolado:**

$$M_{Rd,\text{aço}} = \min(M_{Rd,FLT},\ M_{Rd,FLM},\ M_{Rd,FLA})$$

$$M_{Rd,\text{aço}} = \min(199,46;\ 281,45;\ 281,45) = 199,46\text{ kN}\cdot\text{m}$$

FLA e envoltória do perfil isolado: $\min(\text{FLT}, \text{FLM}, \text{FLA}) = 199,46\text{ kN}\cdot\text{m}$.

### 2.6 Resistência ao Cortante (5.4.3)

$$h_w = d - 2 t_f = 355 - 2 \times 11,6 = 331,8\text{ mm}$$

$$A_w = d \times t_w = 355 \times 7,2 = 2556\text{ mm}^2 \quad (5.4.3.1.2)$$

$$\lambda = \frac{h_w}{t_w} = \frac{331,8}{7,2} = 46,1$$

$$\lambda_p = 1,10 \sqrt{\frac{5 E_a}{f_y}}, \quad \lambda_r = 1,37 \sqrt{\frac{5 E_a}{f_y}}$$

$$V_{pl} = 0,6 A_w f_y$$

$$\lambda = 46,08 \leq \lambda_p = 59,22 \implies V_{pl,Rd} = \frac{0,6 f_y A_w}{\gamma_{a1}} = 480,99\text{ kN}$$

Esbeltez da alma $\lambda = 46,08$ e $V_{Rd} = 480,99\text{ kN}$.

---

## 6. Deslocamentos em serviço

No estado-limite de serviço, a flecha é obtida a partir da seção homogeneizada, com razão modular distinta para curta e longa duração, incorporando a fluência do concreto. Os deslocamentos somam as parcelas de cada fase.

### 2.7 Inércia Equivalente (LNE + Steiner) – Seção Mista

**Módulos elásticos e razão modular:**

$$n = \frac{E_a}{E_c} = \frac{200000}{26838} = 7,45$$

$$n_{ld} = 3n = 3 \times 7,45 = 22,36 \quad (\text{longa duração})$$

**Geometrias e áreas ($\text{mm}, \text{mm}^2$):**

$$A_a = 63,5616\text{ cm}^2 = 6356\text{ mm}^2$$

$$A_c = b_{ef} \cdot t_c = 1750 \cdot 120 = 210000\text{ mm}^2$$

**Posição da LNE (origem na base do perfil de aço):**

$$y_a = \frac{d}{2} = \frac{355}{2} = 177,5\text{ mm}$$

$$y_c = d + \frac{t_c}{2} = 355 + \frac{120}{2} = 415,0\text{ mm}$$

$$\bar{y} = \frac{A_a y_a + (A_c / n) y_c}{A_a + (A_c / n)} = 371,3\text{ mm}$$

**Inércias ($\text{mm}^4$) e termos de Steiner:**

$$I_a = 14222\text{ cm}^4 = 142220000\text{ mm}^4$$

$$I_c = \frac{b_{ef} t_c^3}{12} = \frac{1750 \cdot 120^3}{12} = 252000000\text{ mm}^4$$

**Inércia transformada (curta e longa duração):**

$$I_{tr,cd} = I_a + A_a (y_a - \bar{y})^2 + \frac{I_c + A_c (y_c - \bar{y})^2}{n}$$

$$I_{tr,cd} = 46857,95\text{ cm}^4$$

$$I_{tr,ld} = I_a + A_a (y_a - \bar{y})^2 + \frac{I_c + A_c (y_c - \bar{y})^2}{n_{ld}}$$

$$I_{tr,ld} = 36732,64\text{ cm}^4$$

**Grau de interação:** $\eta = 1,000$

$$I_{ef,cd} = I_a + \eta (I_{tr,cd} - I_a) = 46857,95\text{ cm}^4$$

$$I_{ef,ld} = I_a + \eta (I_{tr,ld} - I_a) = 36732,64\text{ cm}^4$$

Seção homogeneizada: razão modular, posição da linha neutra e momentos de inércia de curta duração $I_{tr,cd}$ e de longa duração $I_{tr,ld}$.

### 2.8 Cálculo de Flechas com Fluência (L.5.2.1)

**Consideração da fluência do concreto:**

$$k_{ld} = 3,00 \quad (\text{multiplicador simplificado para longa duração})$$

$$n_{ld} = n_{\text{serv}} \cdot k_{ld} = 7,45 \times 3,00 = 22,36$$

$$I_{tr,ld} = 36732,64\text{ cm}^4 \quad (\text{longa duração})$$

**Componentes da flecha (Anexo B, B.3.3; ações conforme §4.8):**

1. **Fase de construção (perfil de aço isolado)**
   * Rigidez: $EI = 2,844 \times 10^{13}\text{ N}\cdot\text{mm}^2$
   * $\delta = 8,24\text{ mm}$ (integração numérica sobre cargas reais)
   * $\delta_{\text{construção}} = 8,24\text{ mm}$

2. **Permanentes longa duração (seção mista)**
   * Rigidez: $EI = 7,347 \times 10^{13}\text{ N}\cdot\text{mm}^2$
   * $\delta = 2,98\text{ mm}$ (integração numérica sobre cargas reais)
   * $\delta_{\text{perm},ld} = 2,98\text{ mm}$

3. **Variáveis curta duração**
   * Rigidez: $EI = 9,372 \times 10^{13}\text{ N}\cdot\text{mm}^2$
   * $\delta = 1,05\text{ mm}$ (integração numérica sobre cargas reais)
   * $\delta_{\text{var},cd} = 1,05\text{ mm}$

4. **Variáveis longa duração**
   * Rigidez: $EI = 7,347 \times 10^{13}\text{ N}\cdot\text{mm}^2$
   * $\delta = 0,89\text{ mm}$ (integração numérica sobre cargas reais)
   * $\delta_{\text{var},ld} = 0,89\text{ mm}$

**Soma bruta das componentes:**

$$\delta_{\text{soma,bruta}} = \delta_{\text{construção}} + \delta_{\text{perm},ld} + \delta_{\text{var},cd} + \delta_{\text{var},ld} = 13,17\text{ mm} \quad (\text{inclui } q_1 \text{ temporária})$$

$$\psi_2 = 0,40 \quad (\text{NBR 8681})$$

Deslocamentos por fase com fluência; $\delta = 13,17\text{ mm}$ frente ao limite adotado de $14,00\text{ mm}$.

---

## 7. Vibração do piso

A verificação de conforto do aplicativo avalia os Modos A e B quando o sistema de piso está completo, identifica a frequência fundamental e compara a resposta selecionada com o limite da ocupação. O fragmento abaixo ilustra o cálculo do Modo A deste estudo; o critério selecionado é a aceleração de pico.

### 4.2 Vibração do Piso (Anexo I)

Avaliação simplificada conforme I.6, válida para pisos com frequência fundamental entre $3\text{ Hz}$ e $9\text{ Hz}$ (I.6.1).

A resposta adotada para aceitação é a aceleração de pico $a_p$. O valor eficaz ponderado $a_{w,\text{rms}}$ e o fator $R$ são apresentados apenas como informação, conforme I.6.4 e I.6.5.

**Quadro normativo único — Tabela 2 e Anexo I:** $\psi_1 / \psi_2$ pertencem às combinações gerais de serviço; $\psi_{\text{vib}}$ é o fator específico de I.5.5. O limite de $a_{w,\text{rms}}$ é apresentado por extenso como $0,005 R_{\text{máx}}$.

| Categoria de uso | $\psi_1$ | $\psi_2$ | $\psi_{\text{vib}}$ | $\beta_{\text{ocup.}}$ | $a_{p,\text{lim}}$ ($\text{m/s}^2$) | $a_{w,\text{rms,lim}}$ ($\text{m/s}^2$) | $R_{\text{máx}}$ |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| Escritórios convencionais | 0,600 | 0,400 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| Escritórios corporativos — andar aberto | 0,600 | 0,400 | 0,100 | 0,005 | 0,050 | 0,040 | 8 |
| Escolas — salas de aula e áreas comuns | 0,600 | 0,400 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| Circulação pública — mall, aeroportos e rodoviárias | 0,600 | 0,400 | 0,000 | 0,000 | 0,150 | 0,120 | 24 |
| Centros comerciais e lojas em geral | 0,600 | 0,400 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| **Garagens — selecionada** | **0,700** | **0,600** | **0,000** | **0,005** | **0,150** | **0,120** | **24** |
| Passarelas e escadas internas | 0,400 | 0,300 | 0,000 | 0,005 | 0,150 | 0,120 | 24 |
| Passarelas e escadas externas | 0,400 | 0,300 | 0,000 | 0,005 | 0,200 | 0,160 | 32 |
| Residências | 0,400 | 0,300 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| Hotéis | 0,600 | 0,400 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| Hospitais — atendimento, internação e apartamentos | 0,600 | 0,400 | 0,100 | 0,010 | 0,050 | 0,040 | 8 |
| Hospitais — pacientes sensíveis a movimentos | 0,600 | 0,400 | 0,100 | 0,010 | 0,025 | 0,020 | 4 |
| Hospitais — cirurgia e laboratórios convencionais | 0,600 | 0,400 | 0,100 | 0,000 | 0,012 | 0,010 | 2 |
| Hospitais — cirurgia/laboratório de maior precisão | 0,600 | 0,400 | 0,100 | 0,000 | 0,006 | 0,005 | 1 |
| Bibliotecas, arquivos e depósitos | 0,700 | 0,600 | 0,100 | 0,000 | — | — | — |
| Coberturas acessíveis | 0,700 | 0,600 | 0,100 | 0,000 | — | — | — |

Valores efetivamente adotados neste dimensionamento: $\psi_1 = 0,70$; $\psi_2 = 0,40$; $\psi_{\text{vib}} = 0,00$.

#### 1. Rigidez Dinâmica (Módulo e Inércia) – I.5.7

Para concreto de densidade normal, adota-se o valor simplificado de $38\text{ GPa}$ conforme I.5.7.

$$E_{c,\text{din}} = 38000\text{ MPa}$$

$$n_{\text{din}} = \frac{E_a}{E_{c,\text{din}}} = 5,26$$

$$I_{tr,\text{din}} = 49936,12\text{ cm}^4 \quad (\text{calculado com } n_{\text{din}})$$

$$I_{\text{eff,vib}} = I_a + \eta (I_{tr,\text{din}} - I_a) = 49936,12\text{ cm}^4$$

#### 2. Carregamento de Vibração ($W_{\text{vib}}$) – I.5.5

Ocupação Garagens: conforme I.5.5, a carga variável de uso não compõe a massa modal ($\psi_{\text{vib}} = 0$).

A deflexão e a massa modal são calculadas por integração numérica dos carregamentos informados, com a rigidez dinâmica $EI_{\text{vib}}$.

$$W_{\text{vib}} = (g_1 + g_2) + 0,00 q_{\text{uso}} = 12,75\text{ kN/m} \quad (\text{equivalente distribuída})$$

#### 3. Deflexão ($\delta$) – Tabela I.1 Modo A

Deflexão calculada por integração numérica sobre cada carga individual $j$ (distribuída, pontual ou trapezoidal), com rigidez $EI_{\text{vib}}$, acrescida da parcela de flexibilidade da laje ($b^3 / I_s$) quando informado o espaçamento $b$:

$$\delta_{\text{viga}} = \sum_j \delta_{\text{cargas}} = 3,991\text{ mm}$$

**Parcela de flexibilidade da laje ($b^3 / I_s$) — Tabela I.1:**

$$I_s = \frac{h_c^3}{12 n_{\text{din}}} = \frac{120^3}{12 \times 5,26} = 27360,0\text{ mm}^3$$

$$\delta_{\text{laje}} = \frac{w_{\text{vib}} b^3}{384 E_a I_s} = \frac{12,75 \times 1750^3}{384 \times 200000 \times 27360,0} = 0,033\text{ mm}$$

$$\delta = 4,023\text{ mm}$$

#### 4. Frequência Natural ($f_0$) – I.6.2

$$f_0 = \frac{18}{\sqrt{\delta}} = \frac{18}{\sqrt{4,023}} = 8,97\text{ Hz}$$

$$\omega = 2 \pi f_0 = 56,38\text{ rad/s}$$

#### 5. Massa modal e largura colaborante do piso — I.6.3

$$\frac{I_s b}{I_b} = \frac{27360,0 \times 1750}{499361151} = 0,095883$$

$$B_{b,\text{rig}} = k_b L_b \left(\frac{I_s b}{I_b}\right)^{0,25} = 2,0 \times 7,00 \times 0,095883^{0,25} = 7,790\text{ m}$$

$$B_{b,\text{lim}} = \frac{2}{3} n_p L_p = \frac{2}{3} \times 3 \times 7,00 = 14,000\text{ m}$$

$$B_b = \min\{B_{b,\text{rig}};\ B_{b,\text{lim}}\} = 7,790\text{ m}$$

Governa a rigidez relativa $(I_s b / I_b)^{0,25}$.

$$m = \frac{w_{\text{vib}}}{g b} = \frac{12749,0}{9,81 \times 1,750} = 742,62\text{ kg/m}^2$$

$$M_A = 0,5 m L_b B_b = 0,5 \times 742,62 \times 7,00 \times 7,790 = 20248,8\text{ kg}$$

Massa adotada na resposta: calculada pela área efetiva de participação do Modo A, conforme I.6.3.

#### 6. Razão de Amortecimento Crítico ($\beta$) – Tabela I.2

$$\beta = \text{sistema estrutural} + \text{forros e instalações} + \text{ocupação} + \text{divisórias piso-teto}$$

$$\beta = 0,010 + 0,010 + 0,005 + 0,000 = 0,025$$

#### 7. Aceleração de Pico ($a_p$) – I.6.4

$P_0 = 750\text{ N}$ (peso da pessoa, I.6.4); $\zeta = 0,10$ ($f_0 > 5\text{ Hz}$); $W = 0,89$

$$a_p = \frac{0,55 P_0 e^{-0,35 f_0}}{\sqrt{2} M \beta} = \frac{0,55 \times 750 \times e^{-0,35 \times 8,97}}{\sqrt{2} \times 20248,8 \times 0,025} = 0,0176\text{ m/s}^2$$

#### 8. Valor Eficaz Ponderado ($a_{w,\text{rms}}$) e Fator de Resposta – I.6.4 e I.6.5

$$a_{w,\text{rms}} = \frac{\zeta P_0}{2 \sqrt{2} M \beta} W = \frac{0,10 \times 750}{2 \sqrt{2} \times 20248,8 \times 0,025} \times 0,89 = 0,0467\text{ m/s}^2$$

$$R = \frac{a_{w,\text{rms}}}{0,005} = \frac{0,0467}{0,005} = 9,34$$

| Resposta | Valor calculado | Limite | Uso na aceitação | Resultado |
| :--- | :---: | :---: | :---: | :---: |
| Aceleração de pico $a_p$ | $0,0176\text{ m/s}^2$ | $0,150\text{ m/s}^2$ | Selecionada | Satisfeito |
| Valor eficaz $a_{w,\text{rms}} / R$ | $0,0467\text{ m/s}^2\ /\ R = 9,34$ | $0,120\text{ m/s}^2\ /\ R = 24$ | Informativa | Satisfeito |

* **Resposta selecionada:** $a_p = 0,180\%g \leq a_{p,\text{lim}} = 1,53\%g \quad (U = 11,7\%)$
* **Resultado:** a resposta selecionada satisfaz o limite da Tabela I.3. A massa modal do Modo A foi calculada por I.6.3.

Modo A completo: $f_0 = 8,97\text{ Hz}$, $B_b = 7,790\text{ m}$, $M = 20\,249\text{ kg}$, $a_p = 0,0176\text{ m/s}^2$ e $R = 9,34$.

---

## 8. Síntese das verificações

| Verificação | Solicitante / Resistente | Relação |
| :--- | :---: | :---: |
| Flexão — Fase 1 (perfil isolado) | $57,95 / 199,46\text{ kN}\cdot\text{m}$ | $0,29$ |
| Flexão — Fase 2 (seção mista) | $157,56 / 530,73\text{ kN}\cdot\text{m}$ | $0,30$ |
| Esforço cortante | $90,03 / 480,99\text{ kN}$ | $0,19$ |
| Flecha (ELS) | $13,17 / 14,00\text{ mm}$ | $0,94$ |
| Vibração — aceleração de pico | $0,0176 / 0,150\text{ m/s}^2$ | $0,12$ |

A flecha é o critério condicionante deste estudo, com utilização de $94,0\%$. Todas as verificações listadas atendem, mas a proximidade do limite de serviço deve permanecer visível na escolha do perfil.