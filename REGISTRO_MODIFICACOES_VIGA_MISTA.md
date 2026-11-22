# Registro de Modificações e Inclusões - Módulo Viga Mista (NBR 8800)

Este documento registra todas as alterações, inclusões e extensões de código realizadas no repositório para viabilizar o dimensionamento completo (ELU e ELS) de **Vigas Mistas de Aço e Concreto** (sem protensão) conforme a **ABNT NBR 8800:2024** e **ABNT NBR 8800:2008**.

---

## [2026-11-22] - Versão 1.0 do Dimensionamento de Vigas Mistas

### 1. Arquitetura e Modelos de Dados
- **Criado:** `include/modelos_viga_mista.h`
  - Estruturas de dados agnósticas de protensão encapsuladas no namespace `viga_mista`:
    - `NormaEdicao` (enum: `NBR8800_2008`, `NBR8800_2024`).
    - `TipoLajeMista` (enum: `MACICA`, `STEEL_DECK`).
    - `OrientacaoDeck` (enum: `PERPENDICULAR_A_VIGA`, `PARALELA_A_VIGA`).
    - `DadosLaje`: espessura total $t_c$, geometria da chapa trapezoidal ($h_F, b_F, e_F$), espessura $t_f$ e orientação.
    - `DadosStudBolt`: diâmetro $d_{cs}$, altura $h_{cs}$, tensão $f_u$, conectores por linha/nervura, coeficientes $R_g$ e $R_p$.
    - `GeometriaVigaMista`: vão $L$, comprimento destravado $L_b$, distâncias às vigas adjacentes / balanço de borda, flag `eh_viga_borda` e tipo `escorada`.
    - `AcoesPiso`: cargas de Fase 1 construtiva ($g_1, q_1$) e Fase 2 de serviço ($g_2, q_2$), coeficientes de combinação ($\gamma_{g1}, \gamma_{q1}, \gamma_{g2}, \gamma_{q2}, \psi_1, \psi_2, \psi_{vib}$) e ocupação do piso.
    - `VigaMistaConfig`: agregação completa de entrada de dados para uma viga mista.

- **Modificado:** `include/catalogo.h`
  - Adicionado perfil laminado `perfil_w360x51()` padronizado Gerdau com propriedades geométricas completas ($d = 355\text{ mm}$, $b_f = 171\text{ mm}$, $t_f = 11.6\text{ mm}$, $t_w = 7.2\text{ mm}$, $A_g = 63.56\text{ cm}^2$, $I_x = 14222\text{ cm}^4$, $W_x = 801.2\text{ cm}^3$, $Z_x = 897.4\text{ cm}^3$, $I_y = 967\text{ cm}^4$, $I_t = 22.1\text{ cm}^4$, $C_w = 2.85\times 10^{11}\text{ mm}^6$).

---

### 2. Motor Normativo de Cálculo
- **Criado:** `include/rotinas_viga_mista.h` e `src/rotinas_viga_mista.cpp`
  - **Largura Efetiva ($b_{ef}$) e Largura de Influência ($b_{inf}$):**
    - Automação conforme NBR 8800:2024 L.6.2 para vigas centrais e de borda.
  - **Fase 1 (Perfil de Aço Isolado - ELU):**
    - Ações e combinações construtivas ($M_{Sd1}, V_{Sd1}$).
    - Flambagem lateral com torção (FLT) suportando NBR 8800:2024 Anexo D ($\lambda_{LT} = \sqrt{M_{pl}/M_{cr}}$ e curva $\chi_{LT}$) e NBR 8800:2008.
    - Flambagens locais da mesa (FLM) e da alma (FLA).
    - Esforço cortante resistente com $k_v = 5.34$ (2024) ou $k_v = 5.00$ (2008).
  - **Conexão de Cisalhamento Stud Bolt (ELU):**
    - Resistência de cálculo do pino $Q_{Rd} = \min(Q_{Rd1}, Q_{Rd2})$ para laje maciça e steel deck.
    - Determinação automática de $R_g$ e $R_p$ para nervuras perpendiculares ou paralelas.
    - Força horizontal rasante na interface $F_{hd} = \min(A_a f_{yd}, 0.85 \eta_c f_{cd} b_{ef} t_c)$.
    - Quantidade necessária por semi-vão e total, espaçamento uniforme $s$, limites normativos ($6d \le s \le s_{max}$) e grau de interação $\eta \ge \eta_{min}$.
  - **Fase 2 (Seção Mista - ELU):**
    - Combinação de ações de uso ($M_{Sd2}, V_{Sd2}$).
    - Momento resistente positivo plástico $M_{Rd}$ considerando interação completa ou parcial ($\eta < 1.0$) conforme NBR 8800:2024 L.6.3.1.1-c.
    - Verificação de tensões nominais em serviço na mesa inferior para construção não escorada conforme NBR 8800:2024 L.6.3.2 ($\frac{M_{Ga,S}}{W_a} + \frac{M_{L,S}}{W_{ef}} \le f_y$).
    - Força cortante resistente da alma de aço na fase mista.
  - **Armadura de Costura contra Fendilhamento (ELU):**
    - Cisalhamento longitudinal na interface $\nu_{Sd}$ conforme NBR 8800:2024 L.5.3, verificação de esmagamento de bielas ($\nu_{Rd,max}$) e área de aço transversal necessária $A_{sf} / s$ ($\text{cm}^2/\text{m}$).
  - **ELS - Deformações / Flechas com Fluência:**
    - Razão modular de curta ($n = E_a/E_{cs}$) e longa duração ($n_{ld} = 3n$).
    - Posição da linha neutra elástica e momentos de inércia homogeneizados ($I_{tr,cd}$, $I_{tr,ld}$) e efetivos com interação parcial ($I_{ef,cd}, I_{ef,ld}$).
    - Parcelas de flecha: $\delta_{fase1}$ (perfil isolado sob $g_1 + q_1$), $\delta_{perm,ld}$ (permanentes pós-cura com fluência), $\delta_{var,cd}$ e $\delta_{var,ld}$ (parcela quase-permanente com fluência $\psi_2$).
    - Flecha total e flecha ativa pós-cura frente aos limites normativos ($L/250, L/350, L/500$).
  - **ELS - Conforto e Vibração do Piso (NBR 8800:2024 Anexo I):**
    - Módulo de elasticidade dinâmico do concreto ($E_{c,din} = 38000\text{ MPa}$), razão $n_{din}$ e inércia dinâmica $I_{ef,din}$.
    - Rigidez transversal e flexibilidade da laje $I_s = \frac{t_c^3}{12 n_{din}}$ e $\delta_{laje}$.
    - Flecha dinâmica total $\delta_{vib} = \delta_{viga} + \delta_{laje}$.
    - Frequência natural fundamental $f_0 = \frac{18}{\sqrt{\delta_{vib}}}\text{ Hz}$.
    - Massa modal Modo A ($M_A$) e largura colaborante do piso ($B_b$).
    - Aceleração de pico $a_p = \frac{0.55 P_0 e^{-0.35 f_0}}{2 M_A \beta}$ comparada com o limite normativo $a_{p,lim}$ para a categoria de ocupação.
  - **Verificador Consolidado:**
    - Função `verificar_viga_mista(cfg)` gerando relatório estruturado com taxa de utilização ($Solicitante / Resistente$) e status APROVADO / NÃO ATENDE para cada estado limite.

---

### 3. Interface Gráfica Interativa (Raylib + Dear ImGui)
- **Modificado:** `gui/app_gui.cpp`
  - **Repensada a UI/UX (User Interface / User Experience):**
    - **Tipografia e Escala:** `FontGlobalScale = 1.18f` aumentado para proporcionar leitura confortável e nítida de parâmetros e valores numéricos em monitores de alta resolução.
    - **Tema Moderno "Slate / Dark Blue":** paleta de cores escura profissional estilo software CAD/BIM de engenharia com cantos arredondados (`Rounding = 6.0f ~ 8.0f`) e espaçamentos internos ampliados.
    - **Layout Fluido e Responsivo:** dimensionamento dinâmico baseado em `GetScreenWidth()` e `GetScreenHeight()`, eliminando sobreposições ou cortes em qualquer resolução de tela.
    - **Dashboard de Engenharia (KPI Cards):** cards visuais no topo da síntese indicando Status Geral (`APROVADO` / `NÃO ATENDE`), Momento Fletor Misto ($M_{Sd2} / M_{Rd2}$), Flecha Total em Serviço e Frequência Fundamental de Vibração.
    - **Barras de Utilização Inteligentes:** coloração dinâmica conforme a margem de segurança (Verde $< 75\%$, Âmbar/Laranja $75\% \sim 100\%$, Vermelho $> 100\%$).
    - **Renderização Gráfica 2D Aprimorada com Suporte Visual a Steel Deck:**
      - Perfil I de aço em cinza metálico elegante com eixos guia.
      - Pinos com cabeça (Stud Bolts) dourados com cabeça cilíndrica modelada e distribuição transversal.
      - **Diferenciação Visual Paramétrica para Cada Tipo de Laje:**
        * **Laje Maciça:** Camada retangular contínua de espessura $t_c$ assentada diretamente sobre o topo da mesa superior.
        * **Steel Deck Perpendicular:** Ondulação periódica com nervuras trapezoidais repetidas conforme o passo $e_F$, preenchidas de concreto e contornadas pela linha da chapa de aço em ciano metálico (`0x50E3C2`), com o pino assentado dentro da nervura sobre a mesa.
        * **Steel Deck Paralelo:** Canal trapezoidal longitudinal contínuo apoiado sobre a mesa da viga, com abas laterais estendidas e capa superior contínua.
      - Linha Neutra Plástica (LNP) com destaque e badge em vermelho.
      - Legendas com tipografia aumentada (14px) e tarjas semi-transparentes de alto contraste.

---

### 4. Suíte de Testes Automatizados (TDD)
- **Criado:** `tests/test_viga_mista.cpp`
  - `TEST_CASE("Viga Mista NBR 8800:2024 - Estudo de Referencia (W360x51, Vao 7m, Garagem)")`: Calibração completa e validação de 100% dos valores numéricos contra o documento de referência `referencias/dimensionamento_de_viga_mista_de_piso.md`:
    - $b_{ef} = 1750.0\text{ mm}$
    - Fase 1: $M_{Sd1} = 57.95\text{ kNm}$, $M_{Rd,FLT} = 199.46\text{ kNm}$, $V_{Rd} = 480.99\text{ kN}$
    - Conector Stud: $Q_{Rd} = 95.27\text{ kN}$, $F_{hd} = 1993.52\text{ kN}$, $N_{semi} = 21$
    - Fase 2: $M_{Sd2} = 157.56\text{ kNm}$, $M_{Rd,VM} = 530.73\text{ kNm}$ ($a = 62.5\text{ mm}$), $V_{Sd2} = 90.03\text{ kN}$
    - ELS Flechas: $I_{tr,cd} = 46857.95\text{ cm}^4$, $I_{tr,ld} = 36732.64\text{ cm}^4$, $\delta_{total} = 13.17\text{ mm}$
    - ELS Vibração: $f_0 = 8.97\text{ Hz}$, $B_b = 7.79\text{ m}$, $M_A = 20249\text{ kg}$, $a_p = 0.0176\text{ m/s}^2$
  - `TEST_CASE("Viga Mista com Steel Deck - Verificacao de Coeficientes Rg e Rp")`: Validação dos fatores de fôrma nervurada para 1 e 2 conectores por nervura.
  - `TEST_CASE("Comparativo NBR 8800:2008 vs NBR 8800:2024 - Cortante e FLT")`: Validação de $k_v = 5.34$ vs $5.00$.
- **Modificado:** `CMakeLists.txt`
  - Inclusão de `src/rotinas_viga_mista.cpp` em `MOTOR_SOURCES` e `tests/test_viga_mista.cpp` em `vmp_tests`.
  - Resultado: Todos os 22 testes unitários passando com sucesso.


---

## [2026-11-22] - Otimização de Espaços, Proporções e Geometria do Steel Deck na GUI

### 1. Eliminação de Transbordamento de Textos e Otimização dos Espaços
- **Painel de Parâmetros (Esquerdo):**
  - Fixada largura compacta de `145px` para caixas numéricas (`InputDouble` e `InputInt`), liberando mais de `450px` para os rótulos de texto explicativos.
  - Seletores dropdown (`Combo`) expandidos para preenchimento de largura total (`-1.0f`), permitindo leitura integral das opções normativas sem corte de texto.
- **Tabela de Síntese de Resultados (Inferior Direito):**
  - Ajustadas larguras de colunas fixas (`Solicitante`: 105px, `Resistente`: 130px, `Utilização`: 115px, `Status`: 65px).
  - A coluna "Item Verificado" agora conta com até `350px` livres, evitando truncamento das descrições dos estados limites.

### 2. Ancoragem Rígida e Proteção do Cabeçalho da Área Gráfica
- Definida zona vertical livre estrita com folga de segurança de `75px` abaixo do cabeçalho da seção transversal (`canvas_topo_y + 75px`), garantindo que o topo da laje, linha de cota e legendas **nunca invadam a barra de título**.
- Centralização vertical paramétrica dinâmica de todo o conjunto estrutural (perfil + deck + laje) na área útil do canvas.

### 3. Geometria Unificada da Laje de Concreto e Steel Deck (Mesmo Comprimento)
- **Mesmo Comprimento Rigoroso:** O trecho de concreto e a chapa perfilada do steel deck agora compartilham com exatidão matemática o mesmo intervalo horizontal $[x_{esq\_laje}, x_{dir\_laje}]$.
- **Preenchimento Contínuo e Estanque:**
  - A chapa de aço e as faixas de concreto são geradas por amostragem trapezoidal contígua, eliminando frestas, linhas soltas ou nervuras que ultrapassavam os limites da laje.
  - As bordas laterais (esquerda e direita) fecham verticalmente a laje até a cota exata da chapa nas extremidades.
- **Orientação Perpendicular vs Paralela:**
  - **Perpendicular:** Perfil ondulado periódico com nervuras trapezoidais repetidas conforme o passo $e_F$, com o pino conector assentado no fundo da nervura sobre a mesa superior da viga.
  - **Paralela:** Calha longitudinal assentada diretamente sobre a mesa da viga I com os conectores em seu interior, e nervuras paralelas regulares ao longo da largura da laje.

### 4. Proporções Visuais Realistas (Sensação Real de Piso de Edifício)
- Calibração de escala com aspecto 1:1 real: a laje passa a apresentar largura proporcional ampla ($W \approx 2.8 \sim 4.4 \times d$), transmitindo a sensação física real de uma laje de piso apoiada sobre um perfil de aço laminado.


---

## [2026-11-22] - Correção Geométrica dos Cortes Transversais de Steel Deck

- **Ajuste Físico e Descritivo dos Cortes Transversais da Viga:**
  - **Nervuras Perpendiculares à Viga:** Como as nervuras vencem o vão entre vigas transversais (eixo $X$), o corte transversal da viga corta **ao longo do sentido das nervuras**. Portanto, na seção transversal, a laje é representada como uma **faixa contínua** (espessura total $t_c + h_F$), com a chapa de aço contínua na face inferior sobre a mesa da viga e uma linha tracejada demarcando a crista da fôrma (separando a capa $t_c$ de concreto resistente do concreto da nervura desprezado no cálculo conforme NBR 8800:2024 L.6.6.2-a).
  - **Nervuras Paralelas à Viga:** Como as nervuras correm paralelas ao eixo longitudinal da viga (eixo $Z$), o corte transversal da viga corta **perpendicularmente às nervuras**. Portanto, visualizam-se as **nervuras trapezoidais periódicas cortadas**, com a calha longitudinal central de largura $b_F$ assentada na mesa de aço e as nervuras vizinhas distribuídas na laje até os limites exatos $[x_{esq\_laje}, x_{dir\_laje}]$.


---

## [2026-11-22] - Suporte a Interação Incompleta (Parcial) e Verificação Normativa da Altura do Stud Bolt

### 1. Verificações Normativas da Geometria do Stud Bolt (NBR 8800:2024 L.8.1, L.6.6 e L.8.4)
- **Altura Mínima ($h_{cs,min}$):**
  - Regra geral: $h_{cs} \ge 4 \cdot d_{cs}$ (comprimento mínimo após instalação, NBR 8800 L.8.1).
  - Em Steel Deck: penetração mínima na capa de concreto acima da fôrma de $40\text{ mm}$ ($h_{cs} \ge h_F + 40\text{ mm}$, NBR 8800 L.6.6.1 e Figura L.11).
  - Verificação: $h_{cs} \ge \max(4d_{cs}, \, h_F + 40\text{ mm})$.
- **Cobrimento Superior Mínimo ($c_{sup}$):**
  - Cobrimento de concreto acima do topo da cabeça do pino: $c_{sup} = t_{total} - h_{cs} \ge 10\text{ mm}$ (NBR 8800 L.8.1 e L.8.4.1).
- **Diâmetro Máximo na Mesa ($d_{max}$):**
  - $d_{cs} \le 2.5 \cdot t_f$ (quando soldado diretamente na mesa superior do perfil I, NBR 8800 L.8.1).
- **Integração no Relatório e Diagnóstico da GUI:** Exibição clara dos valores reais e limites de $h_{cs}$, cobrimento e diâmetro com status `[OK]` ou `[INSUFICIENTE]`.

### 2. Controle de Interação Incompleta / Parcial ($\eta < 1.0$)
- **Suporte na GUI:** Adicionado seletor com 3 modos de dimensionamento da conexão:
  1. *Interação Completa Automática ($\eta = 1.00$):* adota o número exato de pinos para 100% de interação.
  2. *Interação Parcial por Grau Alvo ($\eta$):* slider de $\eta_{alvo} \in [\eta_{min}, 1.00]$ com recálculo automático da quantidade de conectores e do espaçamento.
  3. *Espaçamento Adotado Manual ($s$ em mm):* digitação direta do espaçamento, permitindo simular qualquer arranjo de pinos.
- **Visualização da LNP no Aço:** Quando a interação é parcial ($\eta < 1.0$), a Linha Neutra Plástica desce para a mesa superior do perfil de aço. O desenho gráfico em Raylib agora destaca em vermelho a zona comprimida da mesa de aço e plota a linha da LNP e o badge `LNP (Aço)` na cota exata $y_p$.
- **Suíte de Testes:** Adicionado o teste unitário `TEST_CASE("Viga Mista - Interacao Incompleta / Parcial e LNP no Aco")` validando o comportamento físico e normativo (total de 23 testes passando).


---

## [2026-11-22] - Formulação Analítica Exata para LNP na Alma e Diferenciação Visual na GUI

- **Cálculo de $M_{Rd}$ com LNP na Alma (NBR 8800:2024 L.6.3.1.1.1-b e c):**
  - Implementada a integração das resultantes e braços de alavanca dos centros de gravidade das partes comprimidas e tracionadas quando a força $C_{ad}$ excede a capacidade da mesa ($C_{ad} > b_f t_f f_{yd}$).
- **Diferenciação Visual 2D na GUI:**
  - `LNP (Mesa)`: destaca a zona comprimida da mesa superior.
  - `LNP (Alma)`: destaca a mesa superior completa mais o trecho superior comprimido da alma, com badge e linha de cota exata na cota $y_p$.


---

## [2026-11-22] - Limpeza e Alinhamento de Escopo Estrutural (Foco em Vigas Mistas / Pontes)

- **Desativação do ELS de Vibração de Pisos da Verificação Principal:**
  - O cálculo do Anexo I da NBR 8800:2024 (caminhar humano em pisos de edifícios) foi desacoplado do relatório normativo de vigas mistas e da interface gráfica, ficando preservado internamente como módulo auxiliar.
  - O status global `rel.aprovado_global` agora reflete exclusivamente os estados limites estruturais pertinentes: Flexão Fase 1, Cortante Fase 1, Conectores Stud Bolt (capacidade, grau de interação, altura e cobrimento), Flexão Fase 2, Cortante Fase 2, Armadura de Costura e ELS de Deformações (Flecha Total $L/350$ e Flecha Ativa Pós-Cura).
- **Aprimoramento do Dashboard da GUI ([app_gui.cpp](file:///C:/Users/Arthur/Documents/GitHub/VMP/gui/app_gui.cpp)):**
  - Removidos inputs de ocupação de piso de edifícios ("Garagens", "Escritórios", etc.).
  - Os 4 KPI Cards superiores agora destacam com máxima clareza:
    1. `STATUS GERAL`: APROVADO / NÃO ATENDE
    2. `MOMENTO MISTO`: $M_{Sd2} / M_{Rd}$
    3. `CONEXÃO STUDS`: $\eta$ (%) e total de conectores adotados
    4. `FLECHA SERVIÇO`: $\delta_{total}$ (mm) e relação $L/\delta$


---

## [2026-11-22] - Calibração de Proporções Visuais e Posicionamento Rigoroso da LNP

- **Causa Identificada:** Em perfis laminados reais (ex: W360x51), a espessura da mesa ($t_f \approx 11.5\text{ mm}$) representa apenas $\approx 3\%$ da altura da viga ($d = 355\text{ mm}$). Quando desenhada em escala na tela, a mesa ficava com apenas 4 a 5 pixels. Quando a LNP estava a $y_p \approx 10\text{ mm}$ (ainda dentro da mesa, pois $10.0 \le 11.5\text{ mm}$), a espessura do traço da linha vermelha (1.8px) transbordava a borda inferior da mesa desenhada, dando a ilusão ótica de que a linha já cortava a alma.
- **Correção Geométrica e Visual Implementada:**
  1. *Espessura Visual Estrutural:* Aumentada a espessura visual mínima da mesa para `std::max(7.0f, p.tf * escala)` e da alma para `std::max(4.5f, p.tw * escala)`, conferindo solidez ao perfil I e espaço gráfico suficiente para enxergar a posição da LNP.
  2. *Mapeamento Proporcional da LNP na Mesa:* Quando $y_p \le t_f$, a linha vermelha é posicionada a $y_{topo\_aco} + (y_p / t_f) \cdot t_{f,desenho}$. Isso garante matematicamente que a linha vermelha fica 100% contida dentro do retângulo da mesa desenhada na tela enquanto $y_p \le t_f$.
  3. *Cotas Numéricas Explícitas no Badge:* O badge agora exibe os valores exatos em milímetros:
     - `LNP (Mesa): yp = ... mm (tf = ... mm)`
     - `LNP (Alma): yp = ... mm (tf = ... mm)`
     eliminando qualquer dúvida visual ou ambiguidade.

## [2026-11-22] - Modernização Tipográfica e Otimização da Visualização Numérica

- **Causa da Dificuldade de Leitura dos Números:**
  1. A fonte padrão do Dear ImGui (`ProggyClean.ttf`) é uma fonte bitmap de 13px de 2004 concebida para código de baixa resolução. Seus algarismos numéricos (`0`, `8`, `3`, `6`, `9`) são blocos de pixels muito semelhantes entre si.
  2. O fator de escala global anterior (`FontGlobalScale = 1.05f`) aplicava filtragem bilinear sobre o bitmap de 13px, gerando borrão e perda de nitidez nos decimais e tabelas.
  3. No canvas 2D do Raylib (`desenhar_secao_viga_mista`), o `DrawText` utilizava a fonte raster embutida de 10px esticada, pixelando legendas e cotas numéricas.
- **Melhorias e Atualizações Implementadas:**
  1. *Carregamento Vetorial TrueType de Alta Resolução:* Instalado callback `rlImGuiSetLoadFontsCallback` carregando diretamente a fonte corporativa de engenharia do Windows **Segoe UI** (`segoeui.ttf`) em tamanho 17.5px com rasterização com `PixelSnapH = true` e `OversampleH = 3` (com fallbacks automáticos para Calibri e Arial).
  2. *Escala Nativa 1.0f:* `io.FontGlobalScale` fixado em `1.0f`, garantindo mapeamento de pixels 1:1, contornos nítidos e leitura imediata de números, separadores decimais e unidades.
  3. *Renderização Vetorial Suave no Canvas Raylib:* Carregamento de `segoeui.ttf` a 36px com filtro bilinear (`TEXTURE_FILTER_BILINEAR`) para o Raylib, com funções dedicadas `desenhar_texto_nitido` e `medir_largura_texto`.
  4. *Otimização dos Badges e Tabela de Síntese:*
     - Badges de LNP, Laje e Steel Deck redimensionados dinamicamente com base na medição exata do texto vetorial (`medir_largura_texto`), sem cortes.
     - Colunas numéricas da tabela de resultados alargadas (`Solicitante`: 120px, `Resistente/Limite`: 150px, `Utilização`: 120px) para acomodar valores de alta precisão com folga.


---

## [2026-11-22] - Barra Superior de Menus, Visibilidade Integral dos Studs e Alinhamento Central de Nervuras Paralelas

- **Barra de Menus Superior (Menu Bar) Discreta e Profissional:**
  - Removidos os seletores tipo rádio do topo do painel de parâmetros que ocupavam espaço vertical nobre.
  - Implementada barra superior de menus (`ImGui::BeginMainMenuBar`):
    - Menu `Exibição`: permite alternar discretamente entre **Tema Escuro (Dark Slate)** e **Tema Claro (Clean Engineering)**.
    - Menu `Norma`: permite alternar entre NBR 8800:2024 e NBR 8800:2008.
    - Indicador visual passivo no canto direito informando o tema ativo.
  - O layout da janela Raylib e dos painéis ImGui foi ajustado para iniciar com folga abaixo da barra de menus (`margem_y = margem + 24px`), sem qualquer sobreposição.

- **Visibilidade Integral da Altura Real dos Pinos Stud Bolt:**
  - *Diagnóstico do Problema:* Os conectores stud bolt eram renderizados antes da laje de concreto. No caso de Steel Deck ($h_F = 50\text{ mm}$, $t_c = 130\text{ mm}$, stud $h = 100\text{ mm}$), a laje de concreto desenhada por cima cobria a metade superior do pino (os $50\text{ mm}$ que penetram na capa de concreto $t_c$), gerando a falsa impressão visual de que o pino só tinha $50\text{ mm}$.
  - *Solução Aplicada:* Reordenada a ordem de renderização:
    1. Eixos baricêntricos
    2. Perfil de aço I
    3. Laje de concreto e fôrma do steel deck
    4. Conectores stud bolt desenhados com corpo metálico dourado, contornos e cabeça forjada em primeiro plano sobre a laje
    5. Linha Neutra Plástica (LNP) translúcida
    6. Legendas e cotas numéricas
  - Agora, os $100\text{ mm}$ integrais do pino ficam 100% visíveis, mostrando claramente a haste subindo $50\text{ mm}$ na nervura, cruzando a crista da fôrma e penetrando mais $50\text{ mm}$ dentro da capa de concreto com sua cabeça forjada.

- **Alinhamento Simétrico Perfeito de Nervuras Paralelas:**
  - *Diagnóstico do Problema:* O algoritmo anterior de nervuras paralelas usava divisões ad-hoc que cortavam nervuras de forma assimétrica e truncada sobre a mesa do perfil de aço.
  - *Correção Construtiva e Geométrica:* 
    - Um fundo de nervura ($w_{fundo}$) tem agora seu centro rigorosamente alinhado com o eixo central da mesa da viga (`centro_x`).
    - A partir desse fundo central assentado diretamente sobre a viga (onde os conectores stud bolt são soldados), a forma trapezoidal propaga-se de maneira estritamente periódica e simétrica para a esquerda e para a direita por todo o comprimento da laje.
    - Nenhum trapézio é cortado no meio da mesa: o fundo central apoia perfeitamente na mesa superior e as rampas sobem simetricamente para ambos os lados.


---

## [2026-11-22] - Suporte Completo a Acentuação e Centralização da Norma no Menu Superior

---

## [2026-11-22] - Harmonização Terminológica de Nervuras: Transversais vs Longitudinais

---

## [2026-11-22] - Reestruturação Completa da Monografia do TCC (`docs/TCC_VIGA_MISTA_NBR8800_2024.md`)

- **Reposicionamento Estratégico do Escopo e Objetivos:**
  - **Objetivo Geral:** Centralizado no desenvolvimento, implementação e validação da ferramenta computacional interativa (C++20, Raylib, Dear ImGui) para dimensionamento e verificação de vigas mistas biapoiadas de alma cheia (seção I), sob momentos fletores positivos e forças cortantes, em regime de construção não escorada segundo a ABNT NBR 8800:2024.
  - **Objetivos Específicos:** Reorganizados em 6 eixos claros: formulação do motor multifásico (Fases 1 e 2), conectores e fatores $R_g$ e $R_p$, estudo comparativo analítico das alterações da norma (2008 vs. 2024 com elucidação do erro tipográfico do Caso b no Anexo L), desenvolvimento da interface gráfica em tempo real, validação paramétrica numérica e preparação modular para a futura pesquisa de mestrado em Vigas Mistas Protendidas (VMP).
- **Enriquecimento Teórico e Normativo:**
  - Inclusão dos 4 sistemas de laje admitidos pela norma e distinção construtiva de nervuras transversais vs longitudinais.
  - Demonstração analítica e estática da gralha tipográfica de $\sum Q_{Rd}$ em vez de $C_{ad}$ na fórmula de $M_{Rd}$ do Caso b da NBR 8800:2024.
  - Inclusão de 4 pseudocódigos estruturados e 6 tabelas numéricas exatas com gráficos comparativos de sensibilidade.


- **Ajuste na Interface Gráfica (`gui/app_gui.cpp`):**
  - Unificada a nomenclatura prática com a terminologia normativa da ABNT NBR 8800:
    1. **Nervuras Transversais (Perpendiculares à Viga):** Destaca que as cristas de aço atravessam o vão da viga, interrompendo a continuidade do concreto inferior e confinando os pinos stud bolt em cada nervura transversal ($R_g \le 1.0$, $R_p \le 0.75$).
    2. **Nervuras Longitudinais (Paralelas à Viga):** Destaca a formação de uma calha contínua de concreto sobre a mesa superior da viga ("caminho livre" para soldagem sequencial dos pinos e continuidade do concreto ao longo do vão, $R_g = 1.0$).
  - Atualizados os rótulos do combo de seleção, notas de rodapé técnicas e os badges superiores de visualização no canvas 2D do Raylib.


- **Remoção de Duplicidade da Norma nos Dados de Entrada:**
  - O seletor de norma (`ABNT NBR 8800:2024` vs `ABNT NBR 8800:2008`) foi completamente retirado do painel "Parâmetros da Estrutura", eliminando duplicações e deixando a interface mais limpa.
  - A seleção fica agora exclusivamente no menu superior `Norma` da barra principal (`MainMenuBar`), com indicação permanente do status ativo (`NBR 8800:2024 | Tema: Escuro`) no canto direito da barra superior.

- **Suporte Integral aos Acentos da Língua Portuguesa:**
  - *Diagnóstico no Raylib:* Ao invocar `LoadFontEx(..., nullptr, 0)`, a Raylib carrega por padrão estritamente 95 caracteres ASCII (32..126). Quaisquer caracteres acentuados (`Ç`, `Ã`, `Õ`, `Á`, `É`, `Í`, `Ó`, `Ú`, `ç`, `ã`, `õ`, etc.) eram omitidos ou desenhados como interrogação (`?`).
  - *Solução no Raylib:* Criado vetor de codepoints explícitos abrangendo ASCII básico (32..126), Latin-1 Supplement (128..255), Latin Extended-A (256..383) e símbolos estruturais ($\eta$, $\le$, $\ge$).
  - *Solução no Dear ImGui:* Definido `glyph_ranges` estendido incluindo Latin básico, Latin suplementar, pontuação geral, sobrescritos/subscritos ($^2$, $^3$) e operadores matemáticos, garantindo que palavras como "Seção", "Maciça", "Plástica", "Aço", "Vão" e "Dimensões" sejam renderizadas perfeitamente sem falhas.
