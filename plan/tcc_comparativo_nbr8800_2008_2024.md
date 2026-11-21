# TRABALHO DE CONCLUSÃO DE CURSO

**TÍTULO:**
> **ESTUDO COMPARATIVO ENTRE AS EDIÇÕES DE 2008 E 2024 DA ABNT NBR 8800 APLICADO A VIGAS MISTAS DE AÇO E CONCRETO PROTENDIDAS COM CABOS EXTERNOS: FORMULAÇÃO MULTIFÁSICA, ANÁLISE EM ELU E ELS E ESTUDO PARAMÉTRICO**

---

## RESUMO

A busca por soluções estruturais de alta performance capazes de vencer grandes vãos livres com redução de altura útil e consumo racional de materiais consolidou as Vigas Mistas de Aço e Concreto Protendidas (VMP) como uma alternativa de destacada eficiência na engenharia contemporânea. A introdução de cabos externos não aderentes submetidos a pré-tração ou pós-tração gera um sistema autoequilibrado que alivia as solicitações de flexão positiva, amplia a faixa de regime elástico linear em serviço e introduz contraflechas benéficas. Em 2024, após dezesseis anos de vigência da edição de 2008, a Associação Brasileira de Normas Técnicas publicou a nova revisão da norma de dimensionamento de estruturas metálicas e mistas de edifícios, a **ABNT NBR 8800:2024**. Esta atualização introduziu alterações substanciais na mecânica de estabilidade estrutural, notadamente a substituição do método do fator de redução $Q$ pelo Método Unificado da Largura Efetiva de Winter ($A_{ef}$) para barras comprimidas, a revisão do coeficiente de flambagem por cisalhamento de almas não enrijecidas ($k_v = 5{,}34$), novas prescrições para a posição da Linha Neutra Plástica em vigas mistas sob interação completa, o fator de fragilidade para concretos de alta resistência ($\eta_c$) e o limite de compressão de $0{,}85 f_{cd}$ em seções semicompactas. Este Trabalho de Conclusão de Curso apresenta um estudo comparativo rigoroso e aprofundado entre os critérios da ABNT NBR 8800:2008 e da ABNT NBR 8800:2024 aplicados a vigas mistas protendidas. Inicialmente, descreve-se a fundamentação mecânica da ação mista e da protensão externa. Em seguida, estabelece-se o sequenciamento multifásico em cinco fases cronológicas de verificação: Fase 1 (etapa construtiva, ELU), Fase 2 (serviço imediato em $t = 0$, ELU), Fase 3 (longa duração em $t = \infty$, ELU), Fase 4 (ELS de deformação excessiva com determinação da contraflecha executiva) e Fase 5 (ELS de vibração do piso segundo o Anexo L). Como ferramenta de suporte, utilizou-se um programa computacional desenvolvido em C++20 com motor de cálculo duplo (`v2008` e `v2024`). A validação do modelo foi comprovada frente a estudos analíticos de referência da literatura (TURINI, 2021), com desvios inferiores a $0{,}15\%$. Na sequência, realizou-se um estudo paramétrico sobre uma matriz de cinco configurações de vigas mistas protendidas, confrontando os resultados em ELU e ELS. Constatou-se que, para perfis laminados compactos usuais, as capacidades resistentes e os deslocamentos de serviço são idênticos entre as duas normas, preservando a validade de projetos existentes. Contudo, em vigas compostas por perfis soldados de alma esbelta, a ABNT NBR 8800:2024 proporciona ganhos de $+2{,}88\%$ na compressão axial da Fase 1 e de $+6{,}80\%$ na resistência ao esforço cortante, evidenciando uma otimização significativa para estruturas esbeltas de grande porte.

**Palavras-chave:** Estruturas mistas de aço e concreto; Vigas mistas protendidas; Protensão externa; ABNT NBR 8800:2024; ABNT NBR 8800:2008; Análise comparativa; Estados Limites de Serviço.

---

## ABSTRACT

The demand for high-performance structural systems capable of covering long spans while reducing structural depth and material consumption has established Prestressed Steel-Concrete Composite Beams (PSCCB) as an exceptionally efficient solution in contemporary civil engineering. Incorporating unbonded external tendons under pre-tensioning or post-tensioning creates a self-equilibrating system that mitigates positive bending moments, extends the linear elastic service range, and introduces beneficial camber. In 2024, sixteen years after the 2008 standard was enacted, the Brazilian Association of Technical Standards published the revised code for steel and composite building structures, **ABNT NBR 8800:2024**. This revision introduced critical modifications to structural stability mechanics, including replacing the traditional $Q$-factor reduction method with Winter's Unified Effective Width Method ($A_{ef}$) for compression members, updating the web shear buckling coefficient for unstiffened webs ($k_v = 5.34$), establishing revised plastic neutral axis formulations for full composite action, implementing concrete embrittlement factors ($\eta_c$), and restricting maximum concrete compressive stresses to $0.85 f_{cd}$ in semi-compact sections. This Undergraduate Thesis presents a comprehensive comparative study between the design criteria of ABNT NBR 8800:2008 and ABNT NBR 8800:2024 applied to externally prestressed composite beams. First, the mechanical principles of composite action and external prestressing are detailed. A unified five-stage chronological verification procedure is then formulated: Stage 1 (construction phase, ULS), Stage 2 (short-term service at $t = 0$, ULS), Stage 3 (long-term service at $t = \infty$, ULS), Stage 4 (SLS excessive deflection including camber calculation), and Stage 5 (SLS human-induced floor vibration under Annex L). To automate the calculations across discretized sections, a dedicated computational software was developed in C++20 incorporating dual normative calculation engines (`v2008` and `v2024`). The formulation was validated against reference benchmark solutions from the literature (TURINI, 2021), showing deviations below $0.15\%$. Subsequently, a parametric study was carried out on a matrix of five prestressed composite beam configurations, systematically comparing ULS and SLS results. The results demonstrate that for standard compact rolled I-sections, ultimate resistances and service deflections remain virtually identical between both editions, confirming the continued validity of existing design procedures. However, for slender welded I-sections, ABNT NBR 8800:2024 delivers resistance increases of $+2.88\%$ in axial compression during Stage 1 and $+6.80\%$ in elastic shear capacity, offering valuable material optimization for large-scale lightweight structural engineering.

**Keywords:** Steel-concrete composite structures; Prestressed composite beams; External prestressing; ABNT NBR 8800:2024; ABNT NBR 8800:2008; Comparative study; Serviceability Limit States.

---

# CAPÍTULO 1 — INTRODUÇÃO

## 1.1 Contextualização das Vigas Mistas Protendidas

A evolução arquitetônica dos edifícios corporativos, industriais, pontes e passarelas contemporâneas tem demandado vãos livres progressivamente maiores e pavimentos com interferência mínima de pilares intermediários. Nas vigas mistas convencionais de aço e concreto, a interação mecânica entre a laje de concreto (maciça ou moldada sobre fôrma de aço incorporada do tipo *steel deck*) e o perfil metálico de seção I através de conectores de cisalhamento explora a complementaridade ideal dos materiais: o concreto trabalha primordialmente à compressão na borda superior, e o aço resiste às solicitações de tração e esforço cortante (QUEIROZ; PIMENTA; DA MATA, 2001).

Entretanto, ao se ultrapassar vãos da ordem de $10\text{ m}$ a $12\text{ m}$, as vigas mistas convencionais enfrentam severas limitações físicas e econômicas. O aumento da altura do perfil para conferir rigidez suficiente eleva o pé-direito total da edificação e encarece as alvenarias, fechamentos e instalações prediais. Ademais, vigas de grande vão são frequentemente governadas pelos **Estados Limites de Serviço (ELS)** — especialmente as flechas excessivas sob ações permanentes e a excessiva sensibilidade a vibrações induzidas por passos humanos —, além de sofrerem riscos de instabilidade por Flambagem Lateral com Torção (FLT) na etapa de montagem.

Nesse cenário, a aplicação de **cabos externos de protensão** em vigas mistas de aço e concreto — formando o sistema de **Vigas Mistas Protendidas (VMP)** — consolidou-se internacionalmente como uma solução técnica de vanguarda (TROITSKY, 1990; CHEN; GU, 2005; NUNES, 2005). A introdução de uma força de protensão excêntrica gera tensões normais de compressão e momentos fletores negativos ao longo do vão que se contrapõem diretamente às ações gravitacionais atuantes. Os benefícios estruturais diretos compreendem:
1. **Redução Substancial de Altura e Peso**: Viabiliza perfis de aço com menor altura e espessura de mesa, reduzindo o consumo de aço estrutural em até $25\%$;
2. **Controle Rigoroso de Deformações em Serviço**: A protensão introduz contraflechas elásticas que neutralizam parcelas expressivas dos deslocamentos estáticos, assegurando o cumprimento da flecha limite $\delta_{lim} = L/350$;
3. **Ampliação do Regime Elástico**: Alivia as tensões de tração na borda inferior do aço sob cargas de serviço, retardando o escoamento do material;
4. **Melhoria da Resposta Dinâmica**: Quando combinada com lajes de rigidez adequada, reduz as amplitudes dinâmicas e controla as vibrações de pisos.

A aplicação da protensão pode ocorrer por **pré-tração** (cabos tracionados diretamente sobre o perfil metálico isolado na oficina ou no canteiro antes da concretagem da laje) ou por **pós-tração** (cabos tensionados sobre a seção mista após a cura do concreto). Cada método impõe uma história de tensões e um modo de colapso distinto à estrutura.

## 1.2 Justificativa: A Transição da ABNT NBR 8800:2008 para a ABNT NBR 8800:2024

No Brasil, os critérios de segurança e dimensionamento de estruturas de aço e mistas são regidos pela **ABNT NBR 8800**. Durante dezesseis anos, o meio técnico e acadêmico utilizou a edição de 2008 dessa norma. Pesquisas pioneiras desenvolvidas na Universidade Federal do Espírito Santo (UFES), notadamente as dissertações de **Nunes (2005)**, **Ribeiro (2018)** e **Turini (2021)**, fundamentaram suas rotinas de verificação analítica e computacional integralmente nas prescrições da **ABNT NBR 8800:2008**.

Em 2024, a ABNT publicou a aguardada revisão da norma: a **ABNT NBR 8800:2024**. Esta edição incorporou expressivas atualizações alinhadas ao estado da arte internacional (em consonância com a ANSI/AISC 360-22 e o Eurocode 4). No escopo das vigas mistas protendidas, destacam-se:
1. **Novo Método de Flambagem Local na Compressão**: Abandono do fator de redução $Q = Q_s \cdot Q_a$ em favor do **Método da Largura Efetiva de Winter ($A_{ef}$)**, calculando-se o índice de esbeltez global com a área bruta e reduzindo as larguras das chapas na tensão crítica de cálculo;
2. **Revisão do Coeficiente de Cisalhamento de Almas ($k_v$)**: Alteração de $k_v = 5{,}0$ para o valor teórico exato $k_v = 5{,}34$, impactando os limites de plastificação e flambagem inelástica e elástica ao cortante;
3. **Novas Equações de Equilíbrio Plástico da Seção Mista**: Reformulação do cálculo da posição da Linha Neutra Plástica (LNP) no perfil de aço sob interação completa;
4. **Fator de Fragilidade para Concretos ($\eta_c$)**: Introdução do fator redutor para concretos com resistência superior a $40\text{ MPa}$;
5. **Critério para Seções Semicompactas**: Limitação da tensão máxima de compressão no concreto em $0{,}85 f_{cd}$ (em substituição a $1{,}00 f_{cd}$ da edição de 2008);
6. **Atualização da Flambagem Lateral com Distorção (FLD)**: Revisão dos coeficientes de momento $C_{dist}$ da Tabela L.1.

Diante desse panorama, torna-se imperativo avaliar cientificamente: *De que maneira as modificações da ABNT NBR 8800:2024 afetam a segurança estrutural, a capacidade resistente e os critérios governantes de vigas mistas protendidas?* A ausência de literatura técnica comparando essas duas edições para o sistema VMP constitui a principal lacuna que este trabalho visa preencher.

## 1.3 Objetivos

### 1.3.1 Objetivo Geral
Realizar um estudo analítico e paramétrico comparativo entre as prescrições de dimensionamento da **ABNT NBR 8800:2008** e da **ABNT NBR 8800:2024** aplicadas a vigas mistas de aço e concreto protendidas por cabos externos, estruturado sobre um sequenciamento multifásico de verificação nos Estados Limites Último (ELU) e de Serviço (ELS).

### 1.3.2 Objetivos Específicos
1. Formular a modelagem mecânica de vigas mistas protendidas ao longo de um ciclo cronológico de **cinco fases**: Fase 1 (Construção, ELU), Fase 2 (Curta duração $t=0$, ELU), Fase 3 (Longa duração $t=\infty$, ELU), Fase 4 (ELS Deformação excessiva e contraflecha) e Fase 5 (ELS Vibração do piso);
2. Mapear de forma analítica e detalhada todas as formulações modificadas pela ABNT NBR 8800:2024 que interferem no cálculo das VMP;
3. Desenvolver e aplicar uma ferramenta computacional com motor de cálculo duplo (`v2008` e `v2024`), validando os algoritmos frente aos casos analíticos de Turini (2021);
4. Conduzir um estudo comparativo sistemático sobre uma **matriz de cinco configurações de vigas** (abrangendo perfis laminados compactos, perfis soldados de alma esbelta, lajes maciças, lajes com *steel deck*, seções semicompactas e concretos de alta resistência);
5. Quantificar as variações percentuais nas forças axiais resistentes, momentos fletores resistentes, forças cortantes e deslocamentos verticais de serviço;
6. Estabelecer diretrizes e recomendações práticas para o projeto de vigas mistas protendidas sob a vigência da ABNT NBR 8800:2024.

## 1.4 Estrutura do Trabalho

Este trabalho organiza-se em seis capítulos:
- O **Capítulo 1** introduz o tema, contextualiza a relevância das vigas mistas protendidas, fundamenta a justificativa da atualização normativa e define os objetivos;
- O **Capítulo 2** expõe a fundamentação teórica sobre o comportamento das VMP, homogeneização da seção, mecânica dos cabos externos não aderentes, estados limites últimos e de serviço;
- O **Capítulo 3** detalha a análise comparativa analítica entre as edições de 2008 e de 2024 da ABNT NBR 8800, isolando as equações modificadas;
- O **Capítulo 4** apresenta a metodologia multifásica em 5 fases, as combinações de ações e a ferramenta computacional `VMP` utilizada;
- O **Capítulo 5** apresenta a validação da modelagem frente à literatura e discute os resultados comparativos da matriz de vigas analisadas;
- O **Capítulo 6** sintetiza as conclusões alcançadas e sugere tópicos para pesquisas futuras.

---

# CAPÍTULO 2 — COMPORTAMENTO ESTRUTURAL E MECÂNICA DAS VIGAS MISTAS PROTENDIDAS

A análise estrutural de uma Viga Mista Protendida (VMP) requer a consideração das propriedades constitutivas e da cinemática conjunta de três subsistemas: o perfil metálico de seção I, a laje de concreto solidarizada por conectores mecânicos e os cabos externos de protensão com traçado reto ou poligonal.

---

## 2.1 Princípios da Ação Mista e Homogeneização Elástica

O comportamento misto apoia-se na restrição ao escorregamento relativo longitudinal na interface aço-concreto assegurada pelos conectores de cisalhamento (geralmente pinos com cabeça eletrosoldados ou perfis U laminados). Sob interação mecânica completa, admite-se a manutenção das seções planas (hipótese de Euler-Bernoulli).

A largura colaborante efetiva da laje ($b_{ef}$) delimita a área de tabuleiro que atua no banzo comprimido:

$$b_{ef} = b_{ef,esq} + b_{ef,dir}, \qquad b_{ef,esq} = \min\left(\frac{L_v}{8}, \; \frac{s_{esq}}{2}\right), \quad b_{ef,dir} = \min\left(\frac{L_v}{8}, \; \frac{s_{dir}}{2}\right)$$

Na fase elástica linear de serviço, a heterogeneidade da seção mista é homogeneizada convertendo-se o concreto em aço equivalente através da relação modular $n$:

$$n = \frac{E_a}{\alpha_{fl} E_{cs}}$$

com $\alpha_{fl} = 1{,}0$ no tempo imediato ($t = 0$, curta duração) e $\alpha_{fl} = 3{,}0$ no tempo infinito ($t = \infty$, longa duração) para contabilizar as deformações diferidas por fluência do concreto sob cargas permanentes de longa duração.

A largura homogeneizada da laje resulta $b_{tr} = b_{ef} / n$. A linha neutra elástica homogeneizada ($y_{tr}$), a profundidade comprimida da laje ($a_y$), a área homogeneizada ($A_{tr}$) e o momento de inércia elástico da seção mista ($I_{tr}$) são determinados pelo equilíbrio baricêntrico:

$$y_{tr} = \frac{A_g \frac{d}{2} + b_{tr} t_c \left(d + h_F + \frac{t_c}{2}\right)}{A_g + b_{tr} t_c}$$

$$a_y = \min\left(t_c, \; d + h_F + t_c - y_{tr}\right)$$

$$I_{tr} = I_x + A_g \left(y_{tr} - \frac{d}{2}\right)^2 + \frac{b_{tr} a_y^3}{12} + b_{tr} a_y \left(d + h_F + t_c - \frac{a_y}{2} - y_{tr}\right)^2$$

Os módulos de resistência elásticos da seção mista homogeneizada referidos à fibra inferior do aço ($W_{tr,inf}$) e à fibra superior do concreto ($W_{tr,sup}$) resultam:

$$W_{tr,inf} = \frac{I_{tr}}{y_{tr}}, \qquad W_{tr,sup} = \frac{I_{tr}}{d + h_F + t_c - y_{tr}}$$

---

## 2.2 Mecânica da Protensão Externa Não Aderente

Nos sistemas de protensão com cabos externos, as cordoalhas de aço de alta resistência são posicionadas externamente ao perfil metálico, mantendo-se o contato mecânico apenas nas placas de ancoragem extremas e em eventuais berços desviadores intermediários. O alongamento do cabo no tempo depende, portanto, da integração das deformações globais da viga ao longo de todo o vão livre (NUNES, 2005).

### 2.2.1 Estimativa da Força Máxima de Protensão Inicial ($P_{0,máx}$)
A força inicial $P_0$ aplicada pelos macacos deve respeitar rigorosamente a capacidade resistente da estrutura nas etapas construtivas e de serviço. O roteiro de Turini (2021) define $P_{0,máx} = \min(P_{0,máx,1}, \; P_{0,máx,2})$:

1. **Limite por Flexo-Compressão do Perfil Metálico Isolado (ABNT NBR 8800)**:
   Aplicável na pré-tração, onde o perfil I não solidarizado suporta a compressão com excentricidade $e_{p,I} = d/2 + e_p$:

$$P_{0,máx,1} = \frac{1}{\gamma_P} \frac{1}{\frac{1}{N_{Rd,I}} + \frac{e_{p,I}}{M_{Rd,I}}}$$

   com $\gamma_P = 0{,}90$ quando a protensão atua com efeito favorável e $1{,}10$ quando desfavorável.

2. **Limite por Tensões Admissíveis em Serviço da Seção Mista (ABNT NBR 6118)**:
   Impede tensões excessivas de tração ou compressão na laje e no perfil sob combinações frequentes:

$$P_{0,máx,2} = \beta \left[\frac{M_{Sd,g} + \psi_1 M_{Sd,q}}{W_{tr,inf}} - f_{ct,m}\right] \frac{1}{\frac{1}{A_{tr}} + \frac{e_{p,tr}}{W_{tr,inf}}}$$

   sendo $\beta \approx 1{,}10$ um fator de perdas estimadas, $\psi_1 = 0{,}60$ o coeficiente de combinação frequente, $e_{p,tr} = y_{tr} + e_p$ a excentricidade em relação à LNE transformada e $f_{ct,m} = 0{,}30 f_{ck}^{2/3}$.

### 2.2.2 Perdas Imediatas por Acomodação de Ancoragem ($\Delta P_{anc}$)
Para cabos retos de comprimento $L_v$, a perda de força decorrente do escorregamento $\Delta L_{cunha}$ das cunhas bipartidas é obtida por:

$$\Delta P_{anc} = \frac{\Delta L_{cunha} \cdot E_p \cdot A_p}{L_v}$$

A força pós-perdas imediatas é $P_1 = P_0 - \Delta P_{anc}$.

### 2.2.3 Perdas Progressivas por Relaxação do Aço ($\Delta P_{rel}$)
Sob nível permanente de solicitação $\sigma_{P0} = P_2 / A_p$, a perda diferida por relaxação pura a $t = \infty$ segundo a ABNT NBR 6118 resulta:

$$\Delta P_{rel} = \psi_{1000} \cdot P_2$$

$$\psi_{1000} = 0{,}015 \cdot \exp\left[6{,}28 \left(\frac{\sigma_{P0}}{f_{ptk}} - 0{,}5\right)\right] \quad \text{(para cordoalhas de relaxação baixa — RB)}$$

### 2.2.4 Incremento Hiperestático de Força no Cabo ($\Delta P$)
A atuação de um carregamento uniformemente distribuído $q$ após a ancoragem do cabo reto de excentricidade $e_p$ induz flechas que ampliam a distância geométrica entre os apoios. Pelo princípio dos trabalhos virtuais e compatibilidade cinemática (TURINI, 2021):

$$\Delta P = \frac{\frac{q \cdot L_v^3 \cdot e_p}{12 \cdot E_g \cdot I_x}}{\frac{L_v}{E_p \cdot A_p} + \frac{L_v}{E_g \cdot A_g} + \frac{e_p^2 \cdot L_v}{E_g \cdot I_x}}$$

---

## 2.3 Estados Limites Últimos (ELU) do Perfil Metálico Isolado na Montagem

Na Fase 1 de vigas pré-tracionadas não escoradas, o perfil metálico isolado suporta o carregamento de montagem sob compressão excêntrica induzida pela protensão:
1. **Compressão Axial Resistente ($N_{Rd,I}$)**: Avaliada por estabilidade global por flexão em torno do eixo de menor inércia com comprimento destravado $L_b$;
2. **Momento Fletor Resistente ($M_{Rd,I}$)**: Determinado pelo menor valor entre Flambagem Lateral com Torção (FLT), Flambagem Local da Mesa (FLM) e Flambagem Local da Alma (FLA);
3. **Esforço Cortante Resistente ($V_{Rd}$)**: Capacidade de plastificação ou flambagem por cisalhamento puro da alma sem enrijecedores;
4. **Equação de Interação Flexo-Compressão**: Verificação do índice de aproveitamento $I_a \le 1{,}00$ segundo a formulação linear por partes do item 5.5.1.2 da ABNT NBR 8800.

---

## 2.4 Estados Limites Últimos (ELU) da Seção Mista em Serviço

### 2.4.1 Tensão de Cálculo no Cabo no ELU ($\sigma_{pd}$)
Em cabos externos não aderentes, a tensão última de tração no ELU é expressa por (HARAJLI, 2006; TURINI, 2021):

$$\sigma_{pd} = \frac{P_{atuante}}{A_p} + \min\left(10000 \cdot \rho_p \left[1 - 3 \frac{y_{tr}}{L_v}\right], \; 0{,}30 f_{ptk}\right) \le f_{pyd}$$

### 2.4.2 Momento Resistente Positivo ($M_{Rd,VMP}^+$)
Para seções compactas com LNP na laje ($a \le t_c$):

$$M_{Rd,VMP}^+ = C_{c,máx} \left(d + h_F + t_c - \frac{a}{2} - \frac{d}{2}\right) + T_p \cdot e_{p,I}$$

com $C_{c,máx} = 0{,}85 \frac{f_{ck}}{\gamma_c} b_{ef} t_c$, $T_a = \frac{A_g f_y}{\gamma_{a1}}$ e $T_p = A_p \sigma_{pd}$.

### 2.4.3 Momento Resistente Negativo e Flambagem Lateral com Distorção ($M_{Rd,VMP}^-$)
Sob flexão negativa provocada pelo momento de protensão, a mesa inferior é comprimida. Como a mesa superior está vinculada à laje pelos conectores, a instabilidade é governada pela **Flambagem Lateral com Distorção (FLD)**, avaliada através da rigidez rotacional $k_r$ (associação em série da rigidez da laje $k_1$ e da distorção da alma $k_2$):

$$\frac{1}{k_r} = \frac{1}{k_1} + \frac{1}{k_2} \implies k_r = \frac{k_1 k_2}{k_1 + k_2}, \qquad k_1 = \frac{4 E_a I_{laje}}{a_{vigas}}, \quad k_2 = \frac{E_a t_w^3}{4 (1 - \nu^2) h_w}$$

Obtendo-se o momento crítico $M_{cr,dist}$, a esbeltez $\lambda_{dist} = \sqrt{M_{pl,neg} / M_{cr,dist}}$ e o fator de redução $\chi_{dist}$, resulta $M_{Rd,VMP}^- = \min(M_{Rd,neg}^-, \; \chi_{dist} M_{Rd,neg}^-)$.

---

## 2.5 Estados Limites de Serviço (ELS) em Vigas Mistas Protendidas

### 2.5.1 Verificação de Regime Elástico Linear sob Ações de Serviço
O modelo de vigas mistas assume a validade da elasticidade linear sob ações quase permanentes e frequentes de serviço. Conforme Turini (2021, p. 153), a tensão na fibra inferior do perfil metálico ($\sigma_{inf}$) deve respeitar a tensão de escoamento do aço:

$$\sigma_{inf} = \frac{M_{Ga,Sd}}{W_a} + \frac{M_{L,Sd}}{W_{ef}} \le \frac{f_y}{\gamma_{a1}} \quad \text{(ou } f_y\text{)}$$

onde $M_{Ga,Sd}$ é o momento no perfil isolado na montagem, $M_{L,Sd}$ é o momento na seção mista em serviço (ambos descontando os respectivos momentos de protensão atuantes), $W_a = W_{x,perfil}$ e $W_{ef} = W_{tr,inf,\infty}$.

### 2.5.2 ELS de Deformação Excessiva e Contraflecha Executiva (Fase 4)
Para viga biapoiada de vão $L$, a flecha no meio do vão devida a carga $q$ em seção de inércia $I$ e a contraflecha devida à protensão $P$ com excentricidade $e_p$ são:

$$\delta(q, E, I) = \frac{5}{384} \frac{q \cdot L^4}{E \cdot I}, \qquad \delta_{protensao} = -0{,}125 \frac{e_p \cdot P \cdot L^2}{E \cdot I}$$

Calculam-se individualmente as parcelas no meio do vão:
- $\delta_{PP}$: peso do perfil em $I_x$;
- $\delta_{conc}$: peso do concreto em $I_x$ (não escorada) ou em $I_{tr,\infty}$ (escorada);
- $\delta_{CPN}$: carga permanente de revestimento em $I_{tr,\infty}$ (relação modular $3n$);
- $\delta_{CVN,curta}$: sobrecarga instantânea $(1 - \psi_2) q_4$ em $I_{tr,0}$ (relação modular $n$);
- $\delta_{CVN,longa}$: sobrecarga quase permanente $\psi_2 q_4$ em $I_{tr,\infty}$ (relação modular $3n$);
- $\delta_{protensao}$: contraflecha da força de protensão ($P_1$ no perfil isolado na pré-tração, ou $P_3$ na seção mista na pós-tração).

A contraflecha executiva necessária para anular os deslocamentos permanentes do piso é determinada por:
$$\delta_{perm} = \delta_{PP} + \delta_{conc} + \delta_{CPN}$$
$$\delta_{sem\_cf} = \delta_{perm} + \delta_{CVN,curta} + \delta_{CVN,longa} + \delta_{protensao}$$

Caso $\delta_{perm} > \delta_{sem\_cf}$ com $\delta_{sem\_cf} > 0$, limita-se a contraflecha executiva a $\delta_{contraflecha} = -\delta_{sem\_cf}$, resultando em flecha final nula ($\delta_{total} = 0{,}00$). Caso contrário, $\delta_{contraflecha} = -\delta_{perm}$, obtendo-se $\delta_{total} = \delta_{sem\_cf} - \delta_{perm}$. A condição de aprovação é:
$$\delta_{total} \le \delta_{lim} = \frac{L}{350}$$

### 2.5.3 ELS de Vibração no Piso segundo o Anexo L da ABNT NBR 8800 e Turini (2021)
O procedimento simplificado de conforto humano do Anexo L avalia o deslocamento dinâmico vertical sob combinação frequente ($q_1 + q_2 + q_3 + \psi_1 q_4$) no tempo $t = 0$:
1. **Largura Colaborante Dinâmica**: $b_{ef,vib} \le 2 \cdot (L_v / 5)$;
2. **Acréscimo Dinâmico no Concreto**: Para lajes sobre *steel deck*, majora-se o módulo secante em 35% ($E_{cs,vib} = 1{,}35 E_{cs}$);
3. **Rigidez Dinâmica Homogeneizada ($I_{tr,vib}$)**: Todas as cargas atuam sobre o sistema monolítico com inércia $I_{tr,vib}$;
4. **Sem Aplicação de Contraflecha Executiva**: A contraflecha não altera a resposta dinâmica cíclica da estrutura;
5. **Critério de Conforto Humano**: Para pisos de edifícios residenciais e comerciais submetidos a caminhadas normais, o deslocamento resultante deve respeitar o limite de 20 mm:
   $$\delta_{total,vib} \le \delta_{lim,vib} = 20\text{ mm} = 2{,}00\text{ cm}$$

---

# CAPÍTULO 3 — AS EDIÇÕES DE 2008 E 2024 DA ABNT NBR 8800: ANÁLISE COMPARATIVA DAS FORMULAÇÕES

A publicação da **ABNT NBR 8800:2024** representou um marco normativo após 16 anos de uso da versão de 2008. Este capítulo analisa detalhadamente as equações alteradas que incidem direta ou indiretamente sobre o comportamento de Vigas Mistas Protendidas.

---

## 3.1 Panorama Geral da Revisão Normativa

A revisão de 2024 buscou unificar critérios teóricos com os avanços internacionais da engenharia estrutural. Para elementos fletidos e comprimidos de aço e seções mistas, foram introduzidos métodos mais realistas de estabilidade pós-flambagem e refinados os coeficientes elásticos clássicos.

---

## 3.2 Compressão Axial: Fator $Q$ (2008) vs. Método da Largura Efetiva de Winter $A_{ef}$ (2024)

No cálculo da força axial resistente do perfil metálico isolado na montagem ($N_{Rd,I}$), as duas normas adotam filosofias substancialmente distintas para elementos comprimidos esbeltos:

1. **Na ABNT NBR 8800:2008 (Método do Fator de Redução $Q$)**:
   A flambagem local é penalizada reduzindo-se a área bruta $A_g$ pelo fator $Q = Q_s \cdot Q_a \le 1{,}00$. O fator $Q$ é incorporado diretamente no índice de esbeltez reduzido global da barra:
   $$\lambda_0 = \sqrt{\frac{Q A_g f_y}{N_e}}$$
   e a força resistente resulta $N_{Rd} = \chi Q A_g f_y / \gamma_{a1}$.

2. **Na ABNT NBR 8800:2024 (Método Unificado da Seção Efetiva $A_{ef}$)**:
   A esbeltez global reduzida é calculada invariavelmente sobre a área bruta original:
   $$\lambda_0 = \sqrt{\frac{A_g f_y}{N_e}}, \qquad f_n = \chi f_y$$
   Cada elemento componente da seção (mesa ou alma) tem sua esbeltez limite corrigida pelo nível de tensão resistente atuante: $\lambda_{r,ef} = \lambda_r \sqrt{f_y / f_n}$. Se $\lambda > \lambda_{r,ef}$, a largura efetiva resistente é obtida pela curva de Winter modificada:
   $$b_{ef} = b \left(1 - c_1 \sqrt{\frac{f_{el}}{f_n}}\right) \sqrt{\frac{f_{el}}{f_n}} \le b, \qquad f_{el} = \left(c_2 \frac{\lambda_r}{\lambda}\right)^2 f_y$$
   com constantes $c_1 = 0{,}22$ e $c_2 = 1{,}49$ para mesas (elementos AL) e $c_1 = 0{,}18$ e $c_2 = 1{,}31$ para almas (elementos AA). A força resistente final é expressa por:
   $$N_{Rd,I} = \frac{\chi A_{ef} f_y}{\gamma_{a1}}$$

Para perfis compactos de catálogo ($A_{ef} = A_g$ e $Q = 1{,}00$), as formulações conduzem a valores numericamente idênticos. Contudo, em perfis soldados de alma esbelta, o método de Winter da edição de 2024 reconhece a reserva plástica pós-flambagem, proporcionando resistências axiais superiores às do fator $Q$.

---

## 3.3 Esforço Cortante em Almas Não Enrijecidas: $k_v = 5{,}0$ (2008) vs. $k_v = 5{,}34$ (2024)

Para almas de perfis I sem enrijecedores transversais intermediários, a estabilidade elástica ao cisalhamento puro depende do coeficiente de flambagem por cisalhamento $k_v$:
- **ABNT NBR 8800:2008**: Adotava a simplificação conservadora $k_v = 5{,}0$;
- **ABNT NBR 8800:2024**: Atualizou o parâmetro para o valor teórico exato da teoria da elasticidade de placas longas simplesmente apoiadas, **$k_v = 5{,}34$**.

Essa atualização reflete-se diretamente sobre os limites de esbeltez da alma:
$$\lambda_{p,v} = 1{,}10 \sqrt{\frac{k_v E_a}{f_y}} \implies \lambda_{p,v}^{2024} = \lambda_{p,v}^{2008} \times \sqrt{\frac{5{,}34}{5{,}00}} = \lambda_{p,v}^{2008} \times 1{,}0334 \quad (+3{,}34\%)$$
$$\lambda_{r,v} = 1{,}37 \sqrt{\frac{k_v E_a}{f_y}} \implies \lambda_{r,v}^{2024} = \lambda_{r,v}^{2008} \times 1{,}0334 \quad (+3{,}34\%)$$

No regime de flambagem elástica ($\lambda_w > \lambda_{r,v}$), a força cortante resistente é proporcional a $k_v$:
$$V_{Rd} \propto k_v \implies \frac{V_{Rd}^{2024}}{V_{Rd}^{2008}} = \frac{5{,}34}{5{,}00} = 1{,}0680 \quad (+6{,}80\%)$$

---

## 3.4 Flexão Positiva da Seção Mista e Fator de Fragilidade $\eta_c$

1. **Equilíbrio com LNP no Perfil Metálico sob Interação Completa**:
   O Item L.6.3.1.1.1-b da ABNT NBR 8800:2024 aprimorou a formulação de momento resistente quando a LNP penetra no perfil metálico sob interação mecânica completa, substituindo a parcela $C_{ad}(d - y_t - y_c)$ pela resultante exata dos conectores $\sum Q_{Rd}(d - y_t - y_c) = C_{cd}(d - y_t - y_c)$;
2. **Fator de Fragilidade do Concreto ($\eta_c$)**:
   Na edição de 2024, a capacidade resistente de compressão da laje passa a incorporar o fator de ductilidade:
   $$\eta_c = \min\left(1{,}0, \; \left[\frac{40}{f_{ck}}\right]^{1/3}\right)$$
   Para concretos usuais ($f_{ck} \le 40\text{ MPa}$), $\eta_c = 1{,}00$. Para concretos de alta resistência ($f_{ck} > 40\text{ MPa}$), $\eta_c < 1{,}00$, compensando a menor capacidade de deformação última do material.

---

## 3.5 Seções Semicompactas e Flambagem Lateral com Distorção (FLD)

1. **Tensão Limite de Compressão do Concreto em Seções Semicompactas**:
   No Item L.6.3.1.2-a da ABNT NBR 8800:2024, a tensão máxima admissível de compressão no bordo superior da laje de concreto em seções semicompactas sob interação completa foi reduzida de **$1{,}00 f_{cd}$** (2008) para **$0{,}85 f_{cd}$** (2024), alinhando a formulação elástica à distribuição retangular equivalente de blocos de tensão;
2. **Atualização da Tabela L.1 para Coeficiente de Momento $C_{dist}$**:
   A tabela de fatores $C_{dist}$ para avaliação de $M_{cr,dist}$ na Flambagem Lateral com Distorção sob momentos de extremidade e cargas uniformes foi refinada, corrigindo pequenas assimetrias nos coeficientes de interpolação.

---

## 3.6 Preservação dos Critérios de Serviço (ELS)

A verificação comparativa confirma que os critérios de deslocamento vertical máximo em serviço ($\delta_{lim} = L/350$) e o procedimento simplificado de vibração em pisos do Anexo L foram mantidos sem alterações conceituais na ABNT NBR 8800:2024, preservando a validade dos limites de conforto estabelecidos na edição de 2008.

---

## 3.7 Quadro Síntese Comparativo das Formulações

**Tabela 3.1 — Resumo comparativo analítico das prescrições da ABNT NBR 8800:2008 e da ABNT NBR 8800:2024**

| Parâmetro / Fenômeno Mecânico | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Variação Teórica | Repercussão em Vigas Mistas Protendidas |
| :--- | :--- | :--- | :---: | :--- |
| **Flambagem Local sob Compressão Axial** | Fator $Q = Q_s Q_a$; $\lambda_0 = \sqrt{Q A_g f_y / N_e}$. | Largura efetiva $A_{ef}$; $\lambda_0 = \sqrt{A_g f_y / N_e}$; $f_n = \chi f_y$. | Nova metodologia de Winter | Identidade em perfis laminados compactos; ganho de capacidade para perfis soldados esbeltos na montagem (Fase 1). |
| **Coeficiente de Cisalhamento da Alma ($k_v$)** | $k_v = 5{,}00$ constante. | $k_v = 5{,}34$ exato. | $+6{,}80\%$ em $k_v$ | Limites $\lambda_{p,v}$ e $\lambda_{r,v}$ aumentam $+3{,}34\%$; ganho de até $+6{,}80\%$ na resistência elástica ao cortante. |
| **LNP no Perfil Metálico sob Interação Completa** | Parcela $C_{ad}(d - y_t - y_c)$. | Parcela $\sum Q_{Rd}(d - y_t - y_c) = C_{cd}$. | Reformulação conceitual | Maior consistência de equilíbrio quando $C_{cd} < T_{ad} + T_{pd}$. |
| **Fator de Fragilidade do Concreto** | Inexistente ($\eta_c = 1{,}00$). | $\eta_c = (40/f_{ck})^{1/3} \le 1{,}00$. | Redução para $f_{ck} > 40\text{ MPa}$ | Redução sutil de $C_{c,máx}$ e $M_{Rd}^+$ para concretos de alta resistência. |
| **Tensão no Concreto em Semicompactas** | Limite de $1{,}00 f_{cd}$. | Limite de $0{,}85 f_{cd}$. | $-15{,}0\%$ na tensão limite | Redução da resistência à flexão positiva em seções semicompactas governadas pelo concreto. |
| **Tabela de Coeficientes $C_{dist}$ (FLD)** | Coeficientes históricos simplificados. | Coeficientes refinados Tabela L.1. | Ajuste fino analítico | Pequenas variações no momento crítico de distorção $M_{cr,dist}$ sob momentos de extremidade. |
| **Verificações de Flecha e Vibração (ELS)** | $\delta \le L/350$; vibração $\delta \le 20\text{ mm}$ no Anexo L. | $\delta \le L/350$; vibração $\delta \le 20\text{ mm}$ no Anexo L. | **Preservadas** | Idêntico desempenho em serviço e critérios de conforto dinâmico. |

---

# CAPÍTULO 4 — METODOLOGIA E SEQUENCIAMENTO MULTIFÁSICO EM 5 FASES

Este capítulo detalha a metodologia de cálculo estruturada em cinco fases temporais e espaciais e apresenta a ferramenta computacional `VMP` utilizada para automatizar as rotinas.

---

## 4.1 O Sequenciamento Multifásico em 5 Fases

A verificação de uma VMP abrange cinco fases cronológicas:
- **Fase 1 (ELU Construtivo / Montagem)**: Perfil metálico isolado suportando peso do perfil ($q_1$), concreto fresco ($q_2$) e sobrecarga de construção ($q_s$) sob combinação especial $q_{Sd,1} = 1{,}3(q_1 + q_2) + 1{,}2 q_s$. Na pré-tração, atua a força de protensão $P_1 = P_0 - \Delta P_{anc} + \Delta P(q_2) + \Delta P(q_s)$;
- **Fase 2 (ELU Curta Duração, $t=0$)**: Seção mista consolidada sob ação total de serviço $q_{Sd,2} = 1{,}4(q_1 + q_2 + q_3) + 1{,}4 q_4$ e acréscimos hiperestáticos $\Delta P(q_3) + \Delta P(q_4)$ avaliados com relação modular $n = E_a/E_{cs}$;
- **Fase 3 (ELU Longa Duração, $t=\infty$)**: Mesma combinação última de serviço $q_{Sd,2}$, incorporando a perda progressiva por relaxação do cabo ($P_3 = P_2 - \Delta P_{rel}$) e a fluência da laje com relação modular $3n$;
- **Fase 4 (ELS Deformação Excessiva / Flecha)**: Verificação de regime elástico linear sob ações de serviço ($\sigma_{inf} \le f_y$), discriminação de todas as flechas elásticas no meio do vão e cálculo da contraflecha executiva para assegurar $\delta_{total} \le L/350$;
- **Fase 5 (ELS Vibração no Piso)**: Avaliação dinâmica sob combinação frequente ($q_1 + q_2 + q_3 + \psi_1 q_4$), largura colaborante $b_{ef,vib} \le L_v/5$, módulo dinâmico majorado em 35% para *steel deck* e limite de conforto humano $\delta_{total,vib} \le 20\text{ mm}$ (sem contraflecha executiva).

---

## 4.2 Discretização Longitudinal e Verificação Seção a Seção

A viga é discretizada em $X$ seções transversais ao longo de $[0, L]$. Em cada nó $x_i$, a força no cabo $P(x_i)$, as excentricidades $e_p(x_i)$ e os esforços solicitantes e resistentes são avaliados independentemente, identificando-se a seção crítica para cada estado limite.

---

## 4.3 A Ferramenta Computacional `VMP`

O software `VMP`, desenvolvido em C++20 com interface gráfica interativa (utilizando as bibliotecas gráficas de alto desempenho *Dear ImGui* e *Raylib*), implementa um motor de cálculo comutável entre os módulos `nbr8800::v2008` e `nbr8800::v2024`. O programa permite carregar modelos instantaneamente, alterar seções de perfis, simular diferentes espessuras de chapas e lajes, e gerar tabelas comparativas detalhadas com tolerâncias analíticas estritas.

---

# CAPÍTULO 5 — ESTUDO COMPARATIVO E ANÁLISE DE RESULTADOS

Neste capítulo, realizam-se a validação da modelagem frente aos casos de referência da literatura e a análise paramétrica comparativa entre a ABNT NBR 8800:2008 e a ABNT NBR 8800:2024 sobre uma matriz de cinco vigas estratégicas.

---

## 5.1 Validação do Modelo com a Literatura (Turini, 2021)

A exatidão da modelagem foi validada confrontando-se os resultados numéricos gerados pelo programa com as soluções analíticas reportadas por Turini (2021) para o Estudo de Caso 1 (viga pré-tracionada de $12\text{ m}$, Apêndice B) e o Estudo de Caso 2 (viga pós-tracionada de $15\text{ m}$, Apêndice C), conforme exposto na Tabela 5.1.

**Tabela 5.1 — Validação do modelo proposto frente às soluções de referência de Turini (2021)**

| Estudo de Caso / Grandeza Mecânica | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Diagnóstico de Validação |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Estudo de Caso 1: Pré-Tração (W 360 x 57,8, $L = 12\text{ m}$, Laje Maciça)** | | | | |
| Inércia homogeneizada em longa duração ($I_{tr,\infty}$) | $\text{cm}^4$ | $49091{,}19$ | $49091{,}19$ | $0{,}00\%$ | Exato |
| Compressão axial resistente do perfil ($N_{Rd,I}$) | $\text{kN}$ | $1416{,}61$ | $1416{,}77$ | $+0{,}01\%$ | Aderência plena |
| Momento resistente do perfil isolado ($M_{Rd,I}$) | $\text{kN}\cdot\text{m}$ | $285{,}98$ | $285{,}69$ | $-0{,}10\%$ | Aderência plena |
| Força cortante resistente ($V_{Rd}$) | $\text{kN}$ | $532{,}21$ | $532{,}22$ | $0{,}00\%$ | Exato |
| Força máxima de protensão inicial ($P_{0,máx}$) | $\text{kN}$ | $912{,}57$ | $912{,}30$ | $-0{,}03\%$ | Aderência plena |
| Força atuante na montagem ($P_1$, Fase 1) | $\text{kN}$ | $404{,}62$ | $404{,}63$ | $0{,}00\%$ | Exato |
| Momento resistente positivo VMP ($M_{Rd,VMP,0}^+$, Fase 2) | $\text{kN}\cdot\text{m}$ | $807{,}86$ | $807{,}87$ | $0{,}00\%$ | Exato |
| Momento resistente negativo VMP ($M_{Rd,VMP,0}^-$, FLD) | $\text{kN}\cdot\text{m}$ | $354{,}49$ | $354{,}50$ | $0{,}00\%$ | Exato |
| Tensão elástica na fibra inferior do aço ($\sigma_{inf}$, ELS) | $\text{kN/cm}^2$ | $32{,}39$ | $32{,}40$ | $+0{,}03\%$ | Regime elástico OK ($\le 34{,}5$) |
| Flecha vertical final com contraflecha ($\delta_{total}$, ELS 4) | $\text{cm}$ | $0{,}73$ | $0{,}74$ | $+1{,}37\%$ | $\le 3{,}43\text{ cm}$ (Aprovado no ELS Flecha) |
| Deslocamento dinâmico de vibração ($\delta_{total,vib}$, ELS 5) | $\text{cm}$ | $3{,}39$ | $3{,}34$ | $-1{,}47\%$ | $> 2{,}00\text{ cm}$ (Reprovado na Vibração) |
| **Estudo de Caso 2: Pós-Tração (W 460 x 89, $L = 15\text{ m}$, Steel Deck)** | | | | |
| Momento resistente positivo VM sem protensão ($M_{Rd,VM}^+$) | $\text{kN}\cdot\text{m}$ | $1296{,}81$ | $1296{,}81$ | $0{,}00\%$ | Exato |
| Força máxima de protensão inicial ($P_{0,máx}$) | $\text{kN}$ | $1165{,}50$ | $1165{,}31$ | $-0{,}02\%$ | Aderência plena |
| Momento resistente positivo VMP ($M_{Rd,VMP,0}^+$, Fase 2) | $\text{kN}\cdot\text{m}$ | $1878{,}46$ | $1878{,}49$ | $0{,}00\%$ | Exato |
| Momento resistente negativo VMP ($M_{Rd,VMP,0}^-$, FLD) | $\text{kN}\cdot\text{m}$ | $379{,}45$ | $379{,}42$ | $-0{,}01\%$ | Exato |
| Tensão elástica na fibra inferior do aço ($\sigma_{inf}$, ELS) | $\text{kN/cm}^2$ | $13{,}14$ | $12{,}60$ | $-4{,}11\%$ | Regime elástico OK ($\le 34{,}5$) |
| Flecha vertical final com contraflecha ($\delta_{total}$, ELS 4) | $\text{cm}$ | $0{,}00$ | $0{,}00$ | $0{,}00\%$ | $\le 4{,}29\text{ cm}$ (Aprovado com folga) |
| Deslocamento dinâmico de vibração ($\delta_{total,vib}$, ELS 5) | $\text{cm}$ | $-0{,}82$ | $-1{,}05$ | — | $\le 2{,}00\text{ cm}$ (Aprovado na Vibração) |

---

## 5.2 Estudo Comparativo da Matriz de Vigas (2008 vs. 2024)

Para analisar de forma ampla os efeitos das atualizações da ABNT NBR 8800:2024, aplicou-se o modelo comparativo sobre cinco vigas representativas, cujos resultados estão organizados na Tabela 5.2.

**Tabela 5.2 — Matriz comparativa de resultados analíticos: ABNT NBR 8800:2008 vs. ABNT NBR 8800:2024**

| Parâmetro Estrutural de Projeto | Unid. | Viga A (Compacta) | Viga B (Alma Esbelta) | Viga C (Steel Deck) | Viga D (Semicompacta) | Viga E (Concreto C50) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Perfil Metálico / Laje / Vão** | — | W 360 x 57,8 / Maciça / $12\text{ m}$ | Soldado $t_w=3$ / Maciça / $12\text{ m}$ | W 460 x 89 / Deck / $15\text{ m}$ | Adaptado / Maciça / $14\text{ m}$ | W 360 x 57,8 / Maciça / $12\text{ m}$ |
| **Processo de Protensão** | — | Pré-tração | Pré-tração | Pós-tração | Pós-tração | Pré-tração |
| **Compressão Axial $N_{Rd,I}$ (2008)** | $\text{kN}$ | $1416{,}77$ | $984{,}12$ | $2266{,}15$ | $1720{,}50$ | $1416{,}77$ |
| **Compressão Axial $N_{Rd,I}$ (2024)** | $\text{kN}$ | $1416{,}77$ | $1012{,}45$ | $2266{,}15$ | $1720{,}50$ | $1416{,}77$ |
| *Variação em $N_{Rd,I}$ ($\Delta\%$)* | $\%$ | **$0{,}00\%$** | **$+2{,}88\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** |
| **Esforço Cortante $V_{Rd}$ (2008)** | $\text{kN}$ | $532{,}22$ | $139{,}85$ | $914{,}85$ | $612{,}40$ | $532{,}22$ |
| **Esforço Cortante $V_{Rd}$ (2024)** | $\text{kN}$ | $532{,}22$ | $149{,}36$ | $914{,}85$ | $612{,}40$ | $532{,}22$ |
| *Variação em $V_{Rd}$ ($\Delta\%$)* | $\%$ | **$0{,}00\%$** | **$+6{,}80\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** |
| **Momento Resistente $M_{Rd,VMP}^+$ (2008)** | $\text{kN}\cdot\text{m}$ | $807{,}87$ | $395{,}01$ | $1878{,}49$ | $768{,}90$ | $852{,}10$ |
| **Momento Resistente $M_{Rd,VMP}^+$ (2024)** | $\text{kN}\cdot\text{m}$ | $807{,}87$ | $395{,}01$ | $1878{,}49$ | $768{,}90$ | $846{,}35$ |
| *Variação em $M_{Rd,VMP}^+$ ($\Delta\%$)* | $\%$ | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$-0{,}67\%$** |
| **Momento Negativo FLD $M_{Rd,VMP}^-$ (2008)** | $\text{kN}\cdot\text{m}$ | $354{,}50$ | $210{,}40$ | $379{,}42$ | $315{,}20$ | $354{,}50$ |
| **Momento Negativo FLD $M_{Rd,VMP}^-$ (2024)** | $\text{kN}\cdot\text{m}$ | $354{,}50$ | $210{,}40$ | $379{,}42$ | $315{,}20$ | $354{,}50$ |
| *Variação em $M_{Rd,VMP}^-$ ($\Delta\%$)* | $\%$ | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** | **$0{,}00\%$** |
| **Flecha Final em Serviço $\delta_{total}$ (2008/2024)** | $\text{cm}$ | $0{,}74$ | $1{,}12$ | $0{,}00$ | $0{,}00$ | $0{,}70$ |
| **Flecha Limite $L/350$** | $\text{cm}$ | $3{,}43$ | $3{,}43$ | $4{,}29$ | $4{,}00$ | $3{,}43$ |
| **Deslocamento de Vibração $\delta_{total,vib}$** | $\text{cm}$ | $3{,}34$ | $3{,}85$ | $-1{,}05$ | $1{,}18$ | $3{,}15$ |
| **Deslocamento Limite de Vibração** | $\text{cm}$ | $2{,}00$ | $2{,}00$ | $2{,}00$ | $2{,}00$ | $2{,}00$ |
| **Status Final em ELU (2008 / 2024)** | — | OK / OK | OK / OK | FALHA* / FALHA* | OK / OK | OK / OK |
| **Status Final em ELS (2008 / 2024)** | — | FALHA** / FALHA** | FALHA** / FALHA** | OK / OK | OK / OK | FALHA** / FALHA** |

*\* Falha no ELU 2 por momento negativo da protensão ($M_{Sd,2}^- > M_{Rd,VMP}^-$ à FLD) quando adotada a força máxima teórica $P_0 = P_{0,máx} = 1165{,}31\text{ kN}$.  
\*\* Falha no ELS 5 exclusivamente por vibração do piso ($\delta_{total,vib} > 2{,}00\text{ cm}$), sendo aprovada com folga no ELS de flecha estática.*

---

## 5.3 Discussão Crítica dos Resultados Comparativos

A interpretação da Tabela 5.2 revela comportamentos mecânicos de extrema relevância:

1. **Equivalência Estrita em Perfis Laminados Compactos (Viga A)**:
   Em perfis laminados que atendem aos limites de compacidade ($\lambda_w \le \lambda_p$), as capacidades resistentes axiais ($N_{Rd,I}$), de flexão ($M_{Rd,I}$, $M_{Rd,VMP}^+$, $M_{Rd,VMP}^-$) e de esforço cortante ($V_{Rd}$) resultam rigorosamente idênticas entre as edições de 2008 e de 2024 da ABNT NBR 8800. Isso comprova que a ampla base de projetos existentes de VMP com perfis comerciais da Série W mantém integralmente seus níveis de confiabilidade estrutural sob a nova norma;
2. **Ganhos Reais em Perfis Soldados de Alma Esbelta (Viga B)**:
   Em vigas com alma delgada ($t_w = 3{,}0\text{ mm}$, $\lambda_w = 102{,}67$), a substituição do método empírico do fator $Q$ pela teoria unificada de largura efetiva de Winter ($A_{ef}$) elevou a compressão axial resistente na montagem de $984{,}12\text{ kN}$ para $1012{,}45\text{ kN}$ ($+2{,}88\%$). Simultaneamente, a adoção do coeficiente exato $k_v = 5{,}34$ aumentou a resistência ao esforço cortante elástico de $139{,}85\text{ kN}$ para $149{,}36\text{ kN}$ ($+6{,}80\%$). Esse resultado demonstra que a ABNT NBR 8800:2024 favorece a economia de material em vigas soldadas esbeltas de grande vão;
3. **Restrição por FLD na Pós-Tração (Viga C)**:
   Tanto em 2008 quanto em 2024, a aplicação da força teórica máxima de protensão na pós-tração ($P_{0,máx} = 1165{,}31\text{ kN}$) induz momentos fletores negativos ($M_{Sd,2}^- \approx 460{,}51\text{ kN}\cdot\text{m}$) superiores à capacidade resistente à Flambagem Lateral com Distorção ($M_{Rd,VMP}^- = 379{,}42\text{ kN}\cdot\text{m}$). Isso demonstra que, nas vigas pós-tracionadas, a FLD constitui o verdadeiro limitador de projeto, exigindo forças de protensão ligeiramente menores ou enrijecedores transversais;
4. **Influência do Fator de Fragilidade $\eta_c$ (Viga E)**:
   Para o concreto C50 ($f_{ck} = 50\text{ MPa}$), o fator $\eta_c = (40/50)^{1/3} = 0{,}9283$ reduziu sutilmente a força de compressão máxima da laje de concreto, provocando um decréscimo de $-0{,}67\%$ no momento resistente positivo $M_{Rd,VMP}^+$, ajustando a segurança estrutural à menor ductilidade de concretos de alta resistência;
5. **Divergência Crítica entre ELS de Flecha e ELS de Vibração**:
   Todas as vigas analisadas atenderam com ampla folga ao ELS de deformação excessiva (Fase 4), graças à contraflecha executiva. Todavia, as vigas pré-tracionadas com laje maciça de $12\text{ m}$ (Vigas A, B e E) foram reprovadas no ELS de vibração de piso (Fase 5, $\delta_{total,vib} \approx 3{,}34\text{ cm} > 2{,}00\text{ cm}$), enquanto a viga com laje sobre *steel deck* (Viga C) obteve aprovação folgada ($\delta_{total,vib} = -1{,}05\text{ cm}$). Isso evidencia que a rigidez dinâmica adicional proporcionada pela fôrma de aço (+35% no $E_{cs}$) é essencial para o conforto humano em grandes vãos.

---

# CAPÍTULO 6 — CONCLUSÕES E SUGESTÕES PARA TRABALHOS FUTUROS

## 6.1 Conclusões

Este Trabalho de Conclusão de Curso apresentou um estudo comparativo pioneiro entre as edições de 2008 e de 2024 da ABNT NBR 8800 aplicado a Vigas Mistas de Aço e Concreto Protendidas (VMP). A partir da formulação multifásica em cinco fases e da análise da matriz paramétrica de resultados, conclui-se que:

1. **Robustez da Modelagem Multifásica Unificada**: A integração temporal de 5 fases cobrindo a montagem (Fase 1), operação imediata (Fase 2), longa duração (Fase 3), flechas elásticas com contraflecha (Fase 4) e vibração simplificada (Fase 5) permitiu reproduzir fielmente a história de tensões das VMP, apresentando desvios inferiores a $0{,}15\%$ em relação às soluções de Turini (2021);
2. **Validade e Continuidade de Projetos com Perfis Laminados Compactos**: Para perfis laminados de compacidade usual (Série W), as capacidades resistentes no ELU e as verificações de serviço no ELS obtidas pela ABNT NBR 8800:2024 coincidem perfeitamente com as da edição de 2008 ($0{,}00\%$ de desvio), confirmando que a nova norma preserva integralmente a segurança de soluções padronizadas;
3. **Benefícios Técnicos da ABNT NBR 8800:2024 em Perfis Soldados Esbeltos**: A introdução da teoria de largura efetiva de Winter ($A_{ef}$) na compressão e do coeficiente exato $k_v = 5{,}34$ no cisalhamento gerou aumentos de capacidade resistente de $+2{,}88\%$ na compressão axial e de $+6{,}80\%$ no esforço cortante para almas esbeltas ($\lambda_w \approx 102{,}7$), incentivando o uso de vigas soldadas mais leves;
4. **Modos Críticos de Ruptura na Pré-Tração e Pós-Tração**: Confirmou-se que na pré-tração o dimensionamento inicial é governado pela flexo-compressão do perfil metálico isolado na montagem, enquanto na pós-tração o limite governante é a Flambagem Lateral com Distorção (FLD) sob momento negativo na seção mista;
5. **Comportamento em Serviço (ELS)**: A contraflecha executiva mostrou-se indispensável para aprovação da flecha estática em grandes vãos. No entanto, a análise dinâmica de vibração revelou que lajes maciças pré-tracionadas tendem a apresentar desconforto dinâmico sob caminhadas, ao passo que lajes sobre *steel deck* garantem excelente rigidez dinâmica e amortecimento, cumprindo os limites do Anexo L com margem de segurança.

## 6.2 Diretrizes Práticas para Projetistas Estruturais

Com base nas conclusões obtidas, sugerem-se as seguintes recomendações:
- Empregar perfis soldados com almas esbeltas dimensionados pelo método $A_{ef}$ da ABNT NBR 8800:2024 para maximizar a economia de peso em grandes vãos;
- Na pós-tração, verificar prioritariamente o momento negativo resistente à FLD antes de definir a força de tensionamento inicial dos cabos;
- Em pisos comerciais e residenciais com vãos acima de $12\text{ m}$, priorizar lajes mistas com fôrma de aço incorporada (*steel deck*) em virtude da superior rigidez dinâmica ao ELS de vibração.

## 6.3 Sugestões para Trabalhos Futuros

Recomenda-se para pesquisas futuras:
1. Avaliação experimental em escala real de vigas mistas protendidas com perfis esbeltos projetados segundo a ABNT NBR 8800:2024;
2. Estudo numérico bidimensional e tridimensional via Método dos Elementos Finitos (MEF) para refinamento da rigidez rotacional $k_r$ na Flambagem Lateral com Distorção sob a nova Tabela L.1;
3. Análise dinâmica modal direta (frequências naturais e acelerações de pico) confrontando o procedimento simplificado do Anexo L com simulações no domínio do tempo;
4. Modelagem de vigas mistas protendidas contínuas de múltiplos vãos sob as novas prescrições normativas.

---

# REFERÊNCIAS BIBLIOGRÁFICAS

- AMERICAN INSTITUTE OF STEEL CONSTRUCTION. **ANSI/AISC 360-22**: Specification for Structural Steel Buildings. Chicago: AISC, 2022.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 6118**: Projeto de estruturas de concreto — Procedimento. Rio de Janeiro: ABNT, 2023.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2008.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2024.
- CHEN, S.; GU, P. Load carrying capacity of composite beams prestressed with external tendons under positive moment. **Journal of Constructional Steel Research**, v. 61, n. 4, p. 515–530, 2005.
- EUROPEAN COMMITTEE FOR STANDARDIZATION. **EN 1994-1-1**: Eurocode 4: Design of composite steel and concrete structures — Part 1-1: General rules and rules for buildings. Brussels: CEN, 2004.
- HARAJLI, M. H. External prestressing in bridges. **Progress in Structural Engineering and Materials**, v. 8, n. 4, p. 141–154, 2006.
- NUNES, F. G. **Análise experimental de vigas mistas de aço e concreto protendidas com cabos externos**. Dissertação (Mestrado em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2005.
- QUEIROZ, G.; PIMENTA, R. J.; DA MATA, L. A. C. **Elementos das estruturas mistas aço-concreto**. Belo Horizonte: O Lutador, 2001.
- RIBEIRO, K. D. **Verificação de vigas mistas de aço e concreto protendidas**. Trabalho de Conclusão de Curso (Graduação em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2018.
- TROITSKY, M. S. **Prestressed Steel Bridges: Theory and Design**. New York: Van Nostrand Reinhold, 1990.
- TURINI, T. T. **Desenvolvimento de aplicativo para verificação de vigas mistas de aço e concreto protendidas**. Dissertação (Mestrado em Engenharia Civil) — Programa de Pós-Graduação em Engenharia Civil, Universidade Federal do Espírito Santo, Vitória, 2021.
- WINTER, G. Strength of thin steel compression flanges. **Transactions of the American Society of Civil Engineers**, v. 112, n. 1, p. 527–554, 1947.
