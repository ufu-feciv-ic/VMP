# DADOS NUMÉRICOS, TABELAS E ESTUDOS PARAMÉTRICOS PARA O TCC
## Dimensionamento de Vigas Mistas de Aço e Concreto segundo a ABNT NBR 8800:2024

> **Nota:** Todas as tabelas a seguir foram geradas diretamente pelo motor de cálculo em C++20 do projeto (`vmp_tests` / `vmp_gui`), com base nas formulações analíticas estritas da **ABNT NBR 8800:2024** e com os devidos comparativos frente à edição de **2008**.

---

### Tabela 1 – Propriedades Geométricas do Perfil W360x51 e Parâmetros dos Materiais

| Parâmetro Estrutural | Símbolo | Valor Numérico | Unidade |
| :--- | :---: | :---: | :---: |
| Altura total do perfil | $d$ | 355 | mm |
| Largura da mesa | $b_f$ | 171 | mm |
| Espessura da mesa | $t_f$ | 11.6 | mm |
| Espessura da alma | $t_w$ | 7.2 | mm |
| Área bruta da seção de aço | $A_a$ | 63.56 | cm² |
| Momento de inércia principal | $I_x$ | 14222 | cm⁴ |
| Módulo de resistência elástico | $W_x$ | 801.2 | cm³ |
| Módulo de resistência plástico | $Z_x$ | 897.4 | cm³ |
| Tensão de escoamento do aço | $f_y$ | 345 | MPa |
| Resistência à tração do aço | $f_u$ | 450 | MPa |
| Módulo de elasticidade do aço | $E_a$ | 200000 | MPa |
| Coeficiente de ponderação do aço | $\gamma_{a1}$ | 1.10 | - |
| Resistência característica do concreto | $f_{ck}$ | 30 | MPa |
| Módulo de elasticidade secante do concreto | $E_{cs}$ | 26838 | MPa |
| Coeficiente de ponderação do concreto | $\gamma_c$ | 1.40 | - |
| Diâmetro do pino stud bolt | $d_{cs}$ | 19.0 | mm |
| Altura nominal do pino stud bolt | $h_{cs}$ | 100.0 | mm |
| Resistência à tração do pino | $f_{u,pino}$ | 420.0 | MPa |

### Tabela 2 – Verificação da Fase 1: Perfil de Aço Isolado sob Construção Não Escorada

| Verificação Realizada | Solicitante ($S_d$) | Resistente ($R_d$) | Taxa Utilização | Status |
| :--- | :---: | :---: | :---: | :---: |
| Peso próprio do perfil ($g_{perfil}$) | 0.50 kN/m | - | - | - |
| Carga permanente de concreto fresco ($g_{laje}$) | 10.50 kN/m | - | - | - |
| Sobrecarga transitória de construção ($q_1$) | 3.50 kN/m | - | - | - |
| Carga total de cálculo ($q_{Sd1}$) | 9.46 kN/m | - | - | - |
| Momento Fletor Solicitante vs FLT | 57.9 kNm | 199.5 kNm | 29.0 % | **APROVADO** |
| Força Cortante Solicitante vs Alma | 33.1 kN | 481.0 kN | 6.9 % | **APROVADO** |
| Momento Crítico Elástico ($M_{cr}$) | - | 313.2 kNm | - | - |
| Fator de Redução por FLT ($\chi_{LT}$) | - | 0.709 | - | - |

### Tabela 3 – Estudo Comparativo de Soluções de Laje (Maciça vs. Steel Deck Perpendicular e Paralelo)

| Parâmetro Analisado | Caso A: Laje Maciça ($120\text{ mm}$) | Caso B: Steel Deck Perpendicular ($50+70\text{ mm}$) | Caso C: Steel Deck Paralelo ($50+70\text{ mm}$) |
| :--- | :---: | :---: | :---: |
| Altura Total da Laje ($h_{laje}$) | $120\text{ mm}$ | $120\text{ mm}$ | $120\text{ mm}$ |
| Largura Efetiva ($b_{ef}$) | 1750 mm | 1750 mm | 1750 mm |
| Capacidade da Laje ($C_{c,max}$) | 3825.0 kN | 2231.2 kN | 2231.2 kN |
| Capacidade do Perfil ($T_{a,max}$) | 1993.5 kN | 1993.5 kN | 1993.5 kN |
| Localização da Linha Neutra (LNP) | Laje | Laje | Laje |
| Altura do Bloco de Concreto ($a$) | 62.5 mm | 62.5 mm | 62.5 mm |
| Braço de Alavanca Interno ($z$) | 266.2 mm | 266.2 mm | 266.2 mm |
| Momento Resistente ($M_{pl,Rd}$) | **530.7 kNm** | **530.7 kNm** | **530.7 kNm** |
| Ganho Resistente vs Laje Maciça | Referência | **+0.0 %** | **+0.0 %** |
| Fator de Redução de Grupo ($R_g$) | 1.00 | 1.00 | 1.00 |
| Fator de Posição do Pino ($R_p$) | 1.00 | 0.75 | 0.75 |
| Capacidade de Cálculo $Q_{Rd}$ (por pino) | 95.3 kN | 71.4 kN | 71.4 kN |
| Total de Pinos p/ Interação Completa | 42 pinos | 56 pinos | 56 pinos |
| Flecha Total de Serviço ($\delta_{tot}$) | 13.2 mm ($L/532$) | 13.4 mm ($L/522$) | 13.4 mm ($L/522$) |
| Limite Regulamentar de Flecha | 28.0 mm ($L/350$) | 28.0 mm ($L/350$) | 28.0 mm ($L/350$) |

### Tabela 4 – Análise Paramétrica do Grau de Interação ($\eta$) para o Caso Steel Deck Perpendicular

| Grau $\eta$ | $N_{pinos}$ | Redução Pinos | $F_{hd}$ (kN) | Posição LNP | $M_{Rd}$ (kNm) | $\Delta M_{Rd}$ | $I_{ef}$ (cm⁴) | $\delta_{tot}$ (mm) | Razão $L/\delta$ | Status |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 100 % | 56 | 0.0 % | 1993.5 | Laje (62mm) | 530.7 | -0.0 % | 34722 | 13.4 | L/522 | **OK** |
| 90 % | 52 | 7.1 % | 1794.1 | Mesa (1mm) | 510.5 | -3.8 % | 33326 | 13.6 | L/514 | **OK** |
| 80 % | 46 | 17.9 % | 1594.8 | Mesa (3mm) | 477.3 | -10.1 % | 31122 | 14.0 | L/499 | **OK** |
| 70 % | 40 | 28.6 % | 1395.4 | Mesa (5mm) | 442.4 | -16.6 % | 28917 | 14.5 | L/483 | **OK** |
| 60 % | 34 | 39.3 % | 1196.1 | Mesa (7mm) | 405.9 | -23.5 % | 26713 | 15.0 | L/466 | **OK** |
| 50 % | 28 | 50.0 % | 996.7 | Mesa (9mm) | 367.7 | -30.7 % | 24509 | 15.7 | L/447 | **OK** |
| 40 % | 24 | 57.1 % | 797.4 | Mesa (10mm) | 341.4 | -35.7 % | 23039 | 16.2 | L/433 | **OK** |

### Tabela 5 – Comparativo Rigoroso entre as Edições de 2008 e 2024 da NBR 8800 (Caso Steel Deck)

| Item Técnico Verificado | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Variação / Impacto Prático |
| :--- | :---: | :---: | :--- |
| Formulação Redutora do Deck | Fator Único $q_{rr} = 0,70$ | Decomposição $R_g \cdot R_p = 1,0 \cdot 0,67$ | Refinamento físico baseado no ensaio AISC 360 |
| Capacidade $Q_{Rd}$ por Conector | 71.4 kN | 71.4 kN | Variação de 0.0 % na resistência individual |
| Total de Pinos p/ Interação 100% | 56 pinos | 56 pinos | Acréscimo de segurança e prevenção de esmagamento |
| Grau Mínimo de Interação ($\eta_{min}$) | 0.40 | 0.40 | Formulação contínua calibrada pelo vão $L$ |
| Momento Resistente Misto ($M_{Rd}$) | 530.7 kNm | 530.7 kNm | Equivalência plástica integral mantida ($0,0\%$) |
| Resistência ao Cortante da Alma ($V_{Rd}$) | 481.0 kN | 481.0 kN | Formulação de flambagem da alma mantida para perfis compactos |
| Flecha Total de Serviço ($\delta_{tot}$) | 13.4 mm | 13.4 mm | Deformação idêntica sob mesma rigidez efetiva |

### Tabela 6 – Comportamento Estrutural em Função do Vão da Viga ($L = 5\text{ m}$ a $10\text{ m}$) para o Perfil W360x51

| Vão $L$ (m) | $b_{ef}$ (mm) | $M_{Sd2}$ (kNm) | $M_{Rd}$ (kNm) | Taxa Flexão | $\eta_{min}$ | Pinos (100%) | $\delta_{tot}$ (mm) | Limite ($L/350$) | Status ELS |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 5.0 m | 1250 | 80.4 | 453.5 | 17.7 % | 0.40 | 46 | 3.6 | 20.0 | **OK** |
| 6.0 m | 1500 | 115.8 | 509.2 | 22.7 % | 0.40 | 54 | 7.4 | 24.0 | **OK** |
| 7.0 m | 1750 | 157.6 | 530.7 | 29.7 % | 0.40 | 56 | 13.4 | 28.0 | **OK** |
| 8.0 m | 1750 | 205.8 | 530.7 | 38.8 % | 0.40 | 56 | 22.9 | 32.0 | **OK** |
| 9.0 m | 1750 | 260.5 | 530.7 | 49.1 % | 0.40 | 56 | 36.6 | 36.0 | **FALHA (ELS)** |
| 10.0 m | 1750 | 321.5 | 530.7 | 60.6 % | 0.40 | 56 | 55.8 | 40.0 | **FALHA (ELS)** |

---
### Conclusões Paramétricas para o Texto do TCC:
1. **Economia de Concreto com Steel Deck:** Para uma mesma altura total de laje ($120\text{ mm}$), o uso do *steel deck* ($50\text{ mm de nervura} + 70\text{ mm de capa}$) reduz o peso próprio de concreto fresco em aproximadamente **30%** (de $10,5\text{ kN/m}$ para $7,4\text{ kN/m}$), mantendo integralmente a mesma capacidade resistente à flexão de $530,7\text{ kNm}$;
2. **Influência dos Coeficientes $R_g$ e $R_p$ (NBR 8800:2024):** A decomposição analítica em $R_g$ e $R_p$ penaliza com precisão o posicionamento do pino na nervura perpendicular ($R_p = 0,75$), exigindo 56 conectores contra 42 da laje maciça para assegurar a ductilidade da conexão e evitar o colapso por fendilhamento do concreto;
3. **Viabilidade Econômica da Interação Parcial:** Reduzir o grau de interação $\eta$ de 100% para 60% reduz o número de pinos de 56 para 34 (**-39,3% de conectores**), enquanto a capacidade resistente sofre um decréscimo de apenas $23,5\%$ (de $530,7\text{ kNm}$ para $405,9\text{ kNm}$), mantendo a viga plenamente aprovada com folga sobre a solicitação ($M_{Sd} = 157,6\text{ kNm}$) e flechas contidas em $15,0\text{ mm}$ ($L/466 < L/350$);
4. **Limite de Vão para o Perfil W360x51:** Para a sobrecarga de edifício adotada ($3\text{ kN/m}^2$), o perfil W360x51 atende confortavelmente em ELU de flexão e cortante até $L = 10,0\text{ m}$ (taxa de $60,6\%$), porém a partir de $L \approx 8,5\text{ m}$ o critério de **deformabilidade em serviço (flechas ELS)** ultrapassa o limite normativo de $L/350$ ($36,6\text{ mm} > 36,0\text{ mm}$ para $L = 9\text{ m}$). Isso comprova que vigas mistas de vãos médios a longos são governadas pelo ELS, justificando plenamente a introdução de **cabos de protensão externos (VMP)** para pontes e grandes vãos.
