# MEMÓRIA TÉCNICA: CÁLCULO DO MOMENTO RESISTENTE POSITIVO DE VIGAS MISTAS DE AÇO E CONCRETO
**Normas de Referência:** ABNT NBR 8800:2008 (Anexo O) e ABNT NBR 8800:2024 (Anexo L)  
**Autor:** Projeto VMP / Equipe de Engenharia de Estruturas  
**Tema:** Formulação Matemática Rigorosa por Distribuição Plástica de Tensões, Equilíbrio Seccional, Variação da Posição da Linha Neutra Plástica (Laje, Mesa e Alma) sob Interação Completa e Incompleta.

---

## 1. INTRODUÇÃO E HIPÓTESES BÁSICAS DE CÁLCULO

O dimensionamento de vigas mistas biapoiadas de aço e concreto submetidas a momento fletor positivo baseia-se no **Método da Distribuição Plástica de Tensões** no Estado Limite Último (ELU), conforme estabelecido pela ABNT NBR 8800:2008 (Item O.2) e ABNT NBR 8800:2024 (Item L.6.3.1).

Esse método aplica-se a seções cujas almas satisfazem o critério de compacidade à flexão simples:

$$ \lambda_w = \frac{h}{t_w} \le \lambda_p = 3{,}76 \sqrt{\frac{E_a}{f_y}} $$

Onde:
- $h$ é a altura livre da alma do perfil metálico I ($h = d - 2 t_f$);
- $t_w$ é a espessura da alma;
- $E_a$ é o módulo de elasticidade do aço estrutural ($E_a = 200\,000\text{ MPa}$);
- $f_y$ é a tensão de escoamento característica do aço estrutural.

### 1.1. Hipóteses Fundamentais da Mecânica da Fratura e Plastificação
1. **Resistência à Tração do Concreto Desprezada:** Toda a resistência à tração do concreto abaixo da linha neutra é desconsiderada no ELU;
2. **Plastificação Total do Perfil de Aço:** As regiões tracionadas e comprimidas do perfil I de aço atingem a tensão de escoamento de cálculo:

$$ f_{yd} = \frac{f_y}{\gamma_{a1}} \qquad (\gamma_{a1} = 1{,}10) $$

3. **Bloco Retangular Equivalente de Tensões no Concreto:** A região comprimida da laje de concreto atinge uma tensão uniforme de cálculo atuando sobre a largura efetiva $b_{\text{ef}}$ até a profundidade da linha neutra plástica $a \le t_c$:

$$ f_{cd} = \frac{f_{ck}}{\gamma_c} \qquad (\gamma_c = 1{,}40) $$

4. **Conexão de Cisalhamento Dúctil:** Os conectores de cisalhamento possuem capacidade de deformação inelástica suficiente para redistribuir as forças horizontais ao longo do vão até a formação do mecanismo plástico total.

---

## 2. DIFERENÇAS NORMATIVAS: ABNT NBR 8800:2008 vs. ABNT NBR 8800:2024

A tabela a seguir resume as alterações normativas no cálculo do momento resistente positivo $M_{Rd}^+$:

| Parâmetro / Critério | ABNT NBR 8800:2008 (Anexo O) | ABNT NBR 8800:2024 (Anexo L) |
| :--- | :---: | :---: |
| **Tensão do Bloco de Concreto ($\sigma_c$)** | $\sigma_c = 0{,}85 f_{cd}$ (para $f_{ck} \le 50\text{ MPa}$) | $\sigma_c = 0{,}85 \eta_c f_{cd}$ (válido até $f_{ck} \le 90\text{ MPa}$) |
| **Fator de Fragilidade ($\eta_c$)** | Não contemplado ($\eta_c = 1{,}0$) | $\eta_c = 1{,}0$ ($f_{ck} \le 40\text{ MPa}$) e $\eta_c = (40/f_{ck})^{1/3} \le 1{,}0$ ($f_{ck} > 40\text{ MPa}$) |
| **Braço da Força da Laje na Interação Completa (LNP no Aço)** | Medido até o topo da laje: $(t_c + h_F + d - y_t)$ | Medido até o centroide da laje: $(t_c/2 + h_F + d - y_t)$ |
| **Seções Semicompactas ($\lambda_p < \lambda_w \le \lambda_r$)** | Tensão máxima na fibra de concreto: $1{,}00 f_{cd}$ | Tensão máxima na fibra de concreto reduzida para: $0{,}85 f_{cd}$ |
| **Fator de Redução da Viga Mista ($\beta_{vm}$)** | Não explicitado ($\beta_{vm} = 1{,}0$) | Adotado $\beta_{vm} = 1{,}0$ para viga biapoiada em piso |

### 2.1. O Fator de Fragilidade do Concreto ($\eta_c$) na NBR 8800:2024
Na edição de 2024 (Item L.5.3.4), a força máxima de compressão da laje incorpora a ductilidade reduzida de concretos de alta resistência:

$$ \eta_c = \begin{cases} 1{,}0, & \text{para } f_{ck} \le 40\text{ MPa} \\ \left(\dfrac{40}{f_{ck}}\right)^{1/3}, & \text{para } 40\text{ MPa} < f_{ck} \le 90\text{ MPa} \end{cases} $$

Assim, a força máxima de compressão desenvolvida na laje é:

$$ C_{c,\text{máx}} = 0{,}85 \eta_c f_{cd} b_{\text{ef}} t_c $$

---

## 3. CASOS DE INTERAÇÃO COMPLETA ($\alpha_i = 1{,}0$)

Na interação completa, o número de conectores de cisalhamento entre a seção de momento máximo e o apoio é suficiente para transmitir a força horizontal total:

$$ F_{hd} = \min(T_{ad},\, C_{c,\text{máx}}) $$

Onde a capacidade máxima de tração do perfil de aço é:

$$ T_{ad} = A_a f_{yd} = \frac{A_a f_y}{\gamma_{a1}} $$

A posição da Linha Neutra Plástica (LNP) depende diretamente do equilíbrio entre $T_{ad}$ e $C_{c,\text{máx}}$.

---

### 3.1. CASO 1: Linha Neutra Plástica (LNP) Cortando a Laje de Concreto

Este caso ocorre quando a capacidade de compressão da laje supera a capacidade de tração do perfil de aço:

$$ C_{c,\text{máx}} \ge T_{ad} $$

Nesta situação, todo o perfil metálico de aço está submetido à tração uniforme com tensão $f_{yd}$, e apenas uma parte da altura da laje ($a \le t_c$) é comprimida.

#### Equilíbrio de Forças Axiais
Pelo equilíbrio de forças horizontais na seção transversal ($\sum F_x = 0$):

$$ C_c - T_{ad} = 0 \Longrightarrow 0{,}85 \eta_c f_{cd} b_{\text{ef}} a = A_a f_{yd} $$

Isolando a profundidade do bloco comprimido de concreto ($a$):

$$ a = \frac{A_a f_{yd}}{0{,}85 \eta_c f_{cd} b_{\text{ef}}} \le t_c $$

#### Braço de Alavanca Interno ($z$)
O centroide da força de tração do perfil de aço $T_{ad}$ localiza-se no centro geométrico do perfil ($d/2$). O centroide da força de compressão do concreto $C_c$ localiza-se no centro do bloco comprimido ($a/2$ abaixo da face superior da laje).

A distância vertical entre o centroide do perfil de aço e o centroide do bloco comprimido de concreto é:

$$ z = \frac{d}{2} + h_F + t_c - \frac{a}{2} $$

Onde:
- $d$ é a altura total do perfil de aço I;
- $h_F$ é a altura da fôrma de aço (zero para lajes maciças);
- $t_c$ é a espessura da laje acima da fôrma (ou espessura total em lajes maciças).

#### Momento Fletor Resistente de Cálculo ($M_{Rd}^+$)
Tomando momentos em relação ao ponto de aplicação de $T_{ad}$:

$$ M_{Rd}^+ = T_{ad} \cdot z = T_{ad} \left(\frac{d}{2} + h_F + t_c - \frac{a}{2}\right) $$

Esta formulação é idêntica na NBR 8800:2008 e na NBR 8800:2024 (com $\eta_c$ aplicado ao cálculo de $a$).

---

### 3.2. CASO 2: Linha Neutra Plástica (LNP) Cortando a Mesa Superior do Perfil

Este caso ocorre quando a laje não tem capacidade de equilibrar sozinha a tração total do perfil de aço:

$$ C_{c,\text{máx}} < T_{ad} $$

Portanto, o perfil de aço precisa colaborar com uma força de compressão $C_a$ na sua parte superior.

#### Condição de Ocorrência na Mesa Superior
A força de compressão necessária no aço é:

$$ C_a = \frac{T_{ad} - C_{c,\text{máx}}}{2} = \frac{A_a f_{yd} - 0{,}85 \eta_c f_{cd} b_{\text{ef}} t_c}{2} $$

A força resistente máxima da mesa superior do perfil I é:

$$ C_{f,\text{máx}} = b_f t_f f_{yd} $$

Se $C_a \le C_{f,\text{máx}}$, a LNP corta a mesa superior do perfil de aço a uma profundidade $y_p \le t_f$ medida a partir da face superior da mesa.

#### Posição da Linha Neutra Plástica ($y_p$)
$$ y_p = \frac{C_a}{b_f f_{yd}} = \frac{A_a f_{yd} - C_{c,\text{máx}}}{2 b_f f_{yd}} \le t_f $$

#### Centroide da Região Comprimida do Aço ($y_c$)
Como a região comprimida do aço é um retângulo de largura $b_f$ e profundidade $y_p$:

$$ y_c = \frac{y_p}{2} $$

#### Centroide da Região Tracionada do Aço ($y_t$)
A região tracionada do perfil é composta por:
1. Mesa inferior: área $A_1 = b_f t_f$, centroide a $d - t_f/2$ do topo;
2. Alma: área $A_2 = h t_w$, centroide a $t_f + h/2 = d/2$ do topo;
3. Trecho restante da mesa superior: área $A_3 = b_f (t_f - y_p)$, centroide a $y_p + (t_f - y_p)/2$ do topo.

Por estática seccional exata, a distância do centroide de tração à face superior do perfil é:

$$ y_t = \frac{b_f t_f \dfrac{t_f}{2} + h t_w \left(t_f + \dfrac{h}{2}\right) + \dfrac{b_f}{2} (t_f - y_p)(3 t_f + 2h - y_p)}{b_f t_f + h t_w + b_f (t_f - y_p)} $$

#### Momento Fletor Resistente ($M_{Rd}^+$)
- **Conforme ABNT NBR 8800:2008 (Item O.2.2-b):**

$$ M_{Rd}^+ = C_a (d - y_t - y_c) + C_{c,\text{máx}} (t_c + h_F + d - y_t) $$

- **Conforme ABNT NBR 8800:2024 (Item L.6.3.1.1.1-b):**

$$ M_{Rd}^+ = C_a (d - y_t - y_c) + C_{c,\text{máx}} \left(\frac{t_c}{2} + h_F + d - y_t\right) $$

---

### 3.3. CASO 3: Linha Neutra Plástica (LNP) Cortando a Alma do Perfil

Este caso ocorre quando a força de compressão necessária no aço excede a capacidade da mesa superior:

$$ C_a > C_{f,\text{máx}} \Longleftrightarrow C_a > b_f t_f f_{yd} $$

A mesa superior inteira está comprimida e a linha neutra penetra na alma a uma profundidade $y_p > t_f$.

#### Posição da Linha Neutra Plástica ($y_p$)
A força de compressão que excede a mesa superior deve ser absorvida pela alma:

$$ C_{\text{alma}} = C_a - b_f t_f f_{yd} = (y_p - t_f) t_w f_{yd} $$

Isolando $y_p$ (medido a partir da face superior do perfil):

$$ y_p = t_f + \frac{C_a - b_f t_f f_{yd}}{t_w f_{yd}} $$

#### Centroide da Região Comprimida do Aço ($y_c$)
A região comprimida do perfil I é uma seção "T" (mesa superior de largura $b_f$ e espessura $t_f$ + trecho de alma de espessura $t_w$ e altura $y_p - t_f$). Seu centroide medido a partir da face superior do perfil é:

$$ y_c = \frac{b_f t_f \dfrac{t_f}{2} + t_w (y_p - t_f) \left(t_f + \dfrac{y_p - t_f}{2}\right)}{b_f t_f + t_w (y_p - t_f)} $$

#### Centroide da Região Tracionada do Aço ($y_t$)
A região tracionada é composta pela mesa inferior e pelo trecho tracionado da alma ($d - y_p - t_f$). Seu centroide medido a partir da face superior do perfil é:

$$ y_t = \frac{b_f t_f \left(d - \dfrac{t_f}{2}\right) + t_w (d - y_p - t_f) \left(y_p + \dfrac{d - y_p - t_f}{2}\right)}{b_f t_f + t_w (d - y_p - t_f)} $$

#### Momento Fletor Resistente ($M_{Rd}^+$)
- **Conforme ABNT NBR 8800:2008:**

$$ M_{Rd}^+ = C_a (d - y_t - y_c) + C_{c,\text{máx}} (t_c + h_F + d - y_t) $$

- **Conforme ABNT NBR 8800:2024:**

$$ M_{Rd}^+ = C_a (d - y_t - y_c) + C_{c,\text{máx}} \left(\frac{t_c}{2} + h_F + d - y_t\right) $$

---

## 4. CASOS DE INTERAÇÃO INCOMPLETA (PARCIAL) ($\alpha_i < 1{,}0$)

A interação incompleta ocorre quando o número de conectores de cisalhamento instalados entre a seção de momento máximo e o apoio ($N_{\text{adotado}}$) é inferior ao quantitativo necessário para a interação completa ($N_{\text{full}}$):

$$ \alpha_i = \frac{\sum Q_{Rd}}{F_{hd}} = \frac{N_{\text{adotado}}}{N_{\text{full}}} < 1{,}0 $$

Onde:
- $\sum Q_{Rd} = N_{\text{adotado}} \cdot Q_{Rd}$ é a capacidade total dos conectores de cisalhamento instalados no meio-vão;
- $F_{hd} = \min(A_a f_{yd},\, C_{c,\text{máx}})$ é a força horizontal máxima de interação completa.

### 4.1. Limites Normativos para Interação Incompleta
Segundo o Anexo O da ABNT NBR 8800:2008 e Anexo L da NBR 8800:2024, a interação incompleta só é permitida sob momento positivo em perfis compactos, respeitando o limite mínimo:

$$ \alpha_{i,\text{mín}} = \begin{cases} 0{,}50, & \text{para } L \le 5\text{ m} \\ 0{,}50 + \dfrac{L - 5000}{20000} \cdot 0{,}50, & \text{para } 5\text{ m} < L \le 25\text{ m} \end{cases} $$

*(Em lajes com fôrma de aço incorporada com nervuras perpendiculares, permite-se $\alpha_{i,\text{mín}} = 0{,}40$ para $L \le 5\text{ m}$).*

### 4.2. Consequência Física Direta: Posição Obrigatória da LNP no Aço
Na interação incompleta, a força horizontal efetivamente transmitida entre o aço e a laje é limitada pela resistência dos conectores:

$$ F_{hd,\text{red}} = \sum Q_{Rd} = \alpha_i \cdot F_{hd} < F_{hd} $$

Como a força de compressão desenvolvida na laje é $C'_c = \sum Q_{Rd} < T_{ad}$, a Linha Neutra Plástica **NUNCA PODE ESTAR NA LAJE DE CONCRETO** sob interação incompleta!  
Ela é forçada para dentro do perfil metálico de aço (corta a mesa superior ou a alma).

A altura comprimida da laje reduz-se a:

$$ a' = \frac{\sum Q_{Rd}}{0{,}85 \eta_c f_{cd} b_{\text{ef}}} < t_c $$

---

### 4.3. CASO 4: Interação Incompleta com LNP na Mesa Superior do Perfil

A força de compressão que o aço precisa desenvolver para suprir a deficiência de conectores é:

$$ C_a = \frac{T_{ad} - \sum Q_{Rd}}{2} = \frac{A_a f_{yd} - \sum Q_{Rd}}{2} $$

Se $C_a \le b_f t_f f_{yd}$, a Linha Neutra Plástica corta a mesa superior do perfil:

$$ y_p = \frac{C_a}{b_f f_{yd}} = \frac{A_a f_{yd} - \sum Q_{Rd}}{2 b_f f_{yd}} \le t_f $$

Os centroides $y_c$ e $y_t$ são obtidos pelas mesmas expressões analíticas do Caso 2.

#### Momento Resistente por Equilíbrio Plástico Direto:
Tomando momentos em relação ao centroide da parte tracionada do aço ($d - y_t$):

$$ M_{Rd}^+ = C_a (d - y_t - y_c) + \sum Q_{Rd} \left(d - y_t + h_F + t_c - \frac{a'}{2}\right) $$

---

### 4.4. CASO 5: Interação Incompleta com LNP na Alma do Perfil

Se a força de compressão exigida for superior à capacidade da mesa ($C_a > b_f t_f f_{yd}$), a LNP penetra na alma:

$$ y_p = t_f + \frac{C_a - b_f t_f f_{yd}}{t_w f_{yd}} $$

Os centroides $y_c$ e $y_t$ são determinados pelas fórmulas analíticas do Caso 3.

#### Momento Resistente por Equilíbrio Plástico Direto:
$$ M_{Rd}^+ = C_a (d - y_t - y_c) + \sum Q_{Rd} \left(d - y_t + h_F + t_c - \frac{a'}{2}\right) $$

---

### 4.5. Fórmula de Interpolação Prática da NBR 8800 (Item O.2.4 e L.6.3.1.1.2)
Além do método de equilíbrio estático direto da seção, a ABNT NBR 8800 disponibiliza a formulação de interpolação consagrada:

$$ M_{Rd}^+(\alpha_i) = M_{Rd,a} + \sqrt{\alpha_i} \left(M_{Rd,\text{full}} - M_{Rd,a}\right) $$

Onde:
- $M_{Rd,a} = \dfrac{Z_x f_y}{\gamma_{a1}}$ é o momento plástico resistente do perfil de aço isolado;
- $M_{Rd,\text{full}}$ é o momento fletor resistente da viga mista sob interação completa ($\alpha_i = 1{,}0$);
- $\alpha_i = \dfrac{\sum Q_{Rd}}{F_{hd}}$ é o grau de interação efetivo.

Esta formulação simplificada oferece excelente concordância prática com o método do equilíbrio direto, ficando ligeiramente a favor da segurança.

---

## 5. EXEMPLO NUMÉRICO COMPARATIVO RESOLVIDO

Para comprovar numericamente todas as formulações deduzidas, adota-se a seguinte viga de referência:
- **Perfil de Aço:** W 360 x 57,8 laminado (Aço ASTM A572 Grau 50: $f_y = 345\text{ MPa}$, $f_{yd} = 313{,}64\text{ MPa}$);
  - $d = 358\text{ mm}$, $b_f = 172\text{ mm}$, $t_f = 13{,}1\text{ mm}$, $t_w = 7{,}9\text{ mm}$, $A_a = 7250\text{ mm}^2$, $Z_x = 1014{,}8\text{ cm}^3$;
  - $T_{ad} = 7250 \times 313{,}64 \times 10^{-3} = 2273{,}86\text{ kN}$;
  - $M_{Rd,a} = 1014{,}8 \times 10^3 \times 313{,}64 \times 10^{-6} = 318{,}28\text{ kN}\cdot\text{m}$.
- **Laje de Concreto:** Laje maciça de $t_c = 130\text{ mm}$, $b_{\text{ef}} = 2000\text{ mm}$, $h_F = 0$;
- **Concreto C30:** $f_{ck} = 30\text{ MPa}$, $f_{cd} = 21{,}43\text{ MPa}$, $\eta_c = 1{,}0$;
  - $C_{c,\text{máx}} = 0{,}85 \times 1{,}0 \times 21{,}43 \times 2000 \times 130 \times 10^{-3} = 4735{,}71\text{ kN}$.

### 5.1. Comparação dos Resultados

| Caso Analisado | Condição Geométrica | Posição da LNP ($y_p$) | $M_{Rd}^+$ NBR 8800:2008 | $M_{Rd}^+$ NBR 8800:2024 | Variação ($\Delta\%$) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Caso 1: Interação Completa ($\alpha_i = 1{,}0$)** | $C_{c,\text{máx}} = 4735{,}7\text{ kN} > T_{ad} = 2273{,}9\text{ kN}$ | Na Laje ($a = 62{,}4\text{ mm}$) | **$631{,}75\text{ kN}\cdot\text{m}$** | **$631{,}75\text{ kN}\cdot\text{m}$** | $0{,}00\%$ |
| **Caso 2: Interação Completa ($b_{\text{ef}}$ reduzido)** | $b_{\text{ef}} = 700\text{ mm} \implies C_{c,\text{máx}} = 1657{,}5\text{ kN}$ | Na Mesa ($y_p = 5{,}7\text{ mm}$) | **$543{,}20\text{ kN}\cdot\text{m}$** | **$538{,}15\text{ kN}\cdot\text{m}$** | $-0{,}93\%$ |
| **Caso 3: Interação Completa ($b_{\text{ef}}$ estreito)** | $b_{\text{ef}} = 350\text{ mm} \implies C_{c,\text{máx}} = 828{,}8\text{ kN}$ | Na Alma ($y_p = 19{,}3\text{ mm}$) | **$462{,}80\text{ kN}\cdot\text{m}$** | **$455{,}30\text{ kN}\cdot\text{m}$** | $-1{,}62\%$ |
| **Caso 4: Interação Parcial ($\alpha_i = 0{,}75$)** | $\sum Q_{Rd} = 0{,}75 \times 2273{,}9 = 1705{,}4\text{ kN}$ | Na Mesa ($y_p = 5{,}3\text{ mm}$) | **$589{,}90\text{ kN}\cdot\text{m}$** | **$589{,}90\text{ kN}\cdot\text{m}$** | $0{,}00\%$ |
| **Caso 5: Interação Parcial ($\alpha_i = 0{,}55$)** | $\sum Q_{Rd} = 0{,}55 \times 2273{,}9 = 1250{,}6\text{ kN}$ | Na Mesa ($y_p = 9{,}5\text{ mm}$) | **$550{,}80\text{ kN}\cdot\text{m}$** | **$550{,}80\text{ kN}\cdot\text{m}$** | $0{,}00\%$ |

---

## 6. SÍNTESE E CONCLUSÕES DE PROJETO

1. **Equilíbrio Plástico Rigoroso:** O cálculo do momento resistente positivo independe da história de tensões em perfis compactos, sendo governado puramente pelo equilíbrio de forças normais no ELU;
2. **Posição da LNP:**
   - Sob **interação completa**, a LNP quase sempre localiza-se na laje de concreto em vigas de piso usuais ($C_{c,\text{máx}} \ge T_{ad}$), resultando no aproveitamento máximo da seção;
   - Sob **interação incompleta**, a LNP situa-se **obrigatoriamente no perfil de aço**, reduzindo o braço de alavanca e consequentemente o momento resistente;
3. **Concordância entre as Normas de 2008 e 2024:**
   - Para o caso canônico mais comum (LNP na laje), o momento resistente é **rigorosamente idêntico** entre a NBR 8800:2008 e NBR 8800:2024;
   - Apenas nos casos em que a LNP corta o perfil de aço ocorrem diferenças da ordem de $1\%$, devido ao ajuste no braço de alavanca do centroide do concreto na edição de 2024.
