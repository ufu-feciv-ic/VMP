# TRABALHO DE CONCLUSÃO DE CURSO

**TÍTULO SUGERIDO:**
> **ANÁLISE E VERIFICAÇÃO NO ESTADO LIMITE ÚLTIMO DE VIGAS MISTAS DE AÇO E CONCRETO PROTENDIDAS: MODELAGEM MULTIFÁSICA E ESTUDO COMPARATIVO ENTRE AS EDIÇÕES DE 2008 E 2024 DA ABNT NBR 8800**

---

## RESUMO

As vigas mistas de aço e concreto protendidas (VMP) aliam a eficiência mecânica da ação conjunta entre a laje de concreto comprimida e o perfil metálico tracionado aos benefícios estruturais da protensão externa, viabilizando o vencimento de grandes vãos livres, a redução da altura útil dos pavimentos e o controle rigoroso de deslocamentos em serviço. Diante da publicação da nova revisão da norma brasileira de projeto de estruturas de aço e de estruturas mistas de edifícios, a **ABNT NBR 8800:2024**, após dezesseis anos de vigência da edição de 2008, evidencia-se a necessidade de revisar a modelagem resistente desses elementos e avaliar os reflexos das novas prescrições normativas sobre o projeto estrutural. Este Trabalho de Conclusão de Curso apresenta a formulação analítica e a verificação no Estado Limite Último (ELU) de vigas mistas biapoiadas protendidas por cabos externos retos sob dois processos executivos distintos: a pré-tração (na qual a força de protensão é aplicada ao perfil metálico isolado antes da solidarização da laje de concreto) e a pós-tração (na qual a protensão é introduzida diretamente sobre a seção mista consolidada). A metodologia de análise acompanha a evolução cronológica dos esforços solicitantes, das perdas imediatas por acomodação de ancoragem e progressivas por relaxação, do incremento hiperestático de força no cabo por compatibilidade de deformações e das resistências últimas ao longo de três fases críticas da vida útil da estrutura: Fase 1 (etapa construtiva), Fase 2 (operação imediata em serviço, $t = 0$) e Fase 3 (operação em longa duração, $t = \infty$, incorporando a fluência do concreto e a relaxação da armadura ativa). Como suporte à aplicação da metodologia, utilizou-se uma ferramenta computacional desenvolvida especificamente para automatizar a verificação seção a seção ao longo do vão. A formulação foi validada mediante confronto com estudos de caso de referência da literatura nacional (RIBEIRO, 2018; TURINI, 2021) segundo a ABNT NBR 8800:2008 e a ABNT NBR 6118, apresentando desvios inferiores a $0{,}15\%$. Na sequência, investigou-se o impacto das atualizações introduzidas pela ABNT NBR 8800:2024 — notadamente a adoção do método unificado da largura efetiva para flambagem local na compressão e a revisão do coeficiente de estabilidade ao cisalhamento da alma — sobre a capacidade resistente e os critérios governantes de projeto das vigas mistas protendidas.

**Palavras-chave:** Estruturas mistas de aço e concreto; Vigas mistas protendidas; Protensão externa; Análise estrutural multifásica; ABNT NBR 8800:2024.

---

## ABSTRACT

Prestressed steel-concrete composite beams (PSCCB) combine the mechanical efficiency of the composite action between the compressed concrete slab and the tensile steel profile with the structural advantages of external prestressing, enabling longer clear spans, reduced floor depths, and strict deflection control. Following the publication of the revised Brazilian standard for steel and composite building structures, **ABNT NBR 8800:2024**, sixteen years after its 2008 edition, revising the resistance modeling of these structural members and assessing the impact of the new code provisions on structural design has become essential. This Undergraduate Thesis presents the analytical formulation and Ultimate Limit State (ULS) verification of simply supported prestressed composite beams with straight external tendons under two construction methods: pre-tensioning (where prestressing is applied to the bare steel profile prior to slab composite action) and post-tensioning (where prestressing is introduced directly onto the hardened composite section). The analysis methodology tracks the chronological evolution of internal forces, immediate anchorage slip losses and time-dependent relaxation losses, statically indeterminate tendon force increments by deformation compatibility, and ultimate cross-sectional resistances across three critical stages of the structural lifecycle: Stage 1 (construction phase), Stage 2 (short-term service operation, $t = 0$), and Stage 3 (long-term service operation, $t = \infty$, incorporating concrete creep and prestressing steel relaxation). As a background tool to support the analytical procedure, a dedicated computational program was employed to automate the cross-sectional verification along the span. The analytical model was validated against reference case studies from the national literature (RIBEIRO, 2018; TURINI, 2021) under ABNT NBR 8800:2008 and ABNT NBR 6118, yielding deviations below $0.15\%$. Subsequently, the impact of the normative updates introduced by ABNT NBR 8800:2024—specifically the unified effective width method for local buckling under axial compression and revised web shear buckling coefficients—on the ultimate capacity and governing design criteria of prestressed composite beams was evaluated.

**Keywords:** Steel-concrete composite structures; Prestressed composite beams; External prestressing; Multi-stage structural analysis; ABNT NBR 8800:2024.

---

# CAPÍTULO 1 — INTRODUÇÃO

## 1.1 Contextualização

A racionalização dos sistemas construtivos impulsionou a consolidação das estruturas mistas de aço e concreto como uma solução de elevado desempenho mecânico para edifícios de múltiplos pavimentos, pontes, passarelas e grandes coberturas. Nas vigas mistas convencionais, a conexão mecânica entre uma laje de concreto (maciça ou moldada sobre fôrma de aço incorporada do tipo *steel deck*) e um perfil metálico de seção I — assegurada por conectores de cisalhamento soldados à mesa superior do perfil — permite explorar sinergicamente as melhores propriedades de cada material: o concreto absorve prioritariamente as tensões normais de compressão, ao passo que o perfil de aço resiste às solicitações de tração e cisalhamento (QUEIROZ; PIMENTA; DA MATA, 2001).

Não obstante a eficiência das vigas mistas tradicionais, o aumento progressivo dos vãos livres exigidos pelos projetos arquitetônicos contemporâneos impõe limitações severas ao dimensionamento. Em vãos superiores a $12\text{ m}$, a concepção estrutural passa a ser frequentemente condicionada pelos Estados Limites de Serviço (deslocamentos verticais excessivos e fissuração) ou pela instabilidade lateral e local de perfis metálicos de grande altura, elevando o consumo de aço e a altura total dos pavimentos.

Para superar essas limitações, a técnica da **protensão externa aplicada a vigas mistas de aço e concreto** — formando as chamadas **Vigas Mistas Protendidas (VMP)** — consolidou-se como uma alternativa estrutural de alta eficiência (TROITSKY, 1990; CHEN; GU, 2005; NUNES, 2005). A introdução de cabos de aço de alta resistência tracionados excentricamente em relação ao centro de gravidade da peça gera um esforço normal de compressão associado a um momento fletor negativo ao longo do vão, contrapondo-se diretamente aos esforços solicitantes oriundos das cargas gravitacionais. Dentre os principais benefícios estruturais desse sistema, destacam-se:
1. A ampliação do regime de comportamento elástico linear da viga sob cargas de serviço, reduzindo as tensões finais de tração na mesa inferior do perfil de aço;
2. A introdução de uma contraflecha controlada que compensa parcela expressiva das flechas permanentes e variáveis;
3. O incremento da capacidade resistente última à flexão positiva da seção mista e a possibilidade de adoção de perfis metálicos mais leves e de menor altura estrutural.

Do ponto de vista construtivo e da história de tensões da estrutura, a aplicação da força de protensão pode ocorrer segundo dois processos distintos:
- **Sistema com Pré-tração**: o esticamento e a ancoragem dos cabos são realizados diretamente sobre o perfil metálico isolado, antes da concretagem da laje (ou antes que o concreto atinja resistência suficiente para solidarizar-se ao aço). Nessa modalidade, toda a força inicial de protensão é transferida exclusivamente à seção metálica, evitando que parte da protensão seja absorvida na compressão da laje de concreto, mas exigindo rigorosa verificação da estabilidade do perfil I submetido à flexo-compressão durante a montagem;
- **Sistema com Pós-tração**: a laje é concretada e curada sobre o perfil metálico sem protensão prévia, e o tracionamento dos cabos é efetuado somente após a consolidação da seção mista, de modo que os esforços de protensão atuam desde o início sobre a seção homogeneizada aço-concreto.

## 1.2 Justificativa

Diferentemente do projeto de vigas mistas convencionais ou de vigas de concreto protendido usuais, a análise estrutural de uma Viga Mista Protendida exige uma **modelagem evolutiva multifásica** que integre simultaneamente os fenômenos de instabilidade de perfis metálicos esbeltos, plastificação da seção mista, reologia do concreto (fluência), perdas imediatas e progressivas de protensão e hiperestaticidade interna do conjunto viga-cabo.

No Brasil, as normas técnicas fundamentais de projeto estrutural — a **ABNT NBR 8800** (Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios) e a **ABNT NBR 6118** (Projeto de estruturas de concreto) — não dispõem de um capítulo único que consolide o roteiro completo de verificação de vigas mistas protendidas por cabos externos. Pesquisas desenvolvidas na Universidade Federal do Espírito Santo (UFES), notadamente os trabalhos de **Nunes (2005)**, **Ribeiro (2018)** e **Turini (2021)**, sistematizaram uma metodologia analítica rigorosa para o dimensionamento e a verificação desses elementos com base na **ABNT NBR 8800:2008**.

Todavia, a publicação da **ABNT NBR 8800:2024**, que revisou a norma brasileira de estruturas de aço e mistas após dezesseis anos de vigência da edição de 2008, introduziu modificações importantes nos critérios de estabilidade local de barras submetidas à compressão axial (substituindo o método tradicional do fator de redução $Q$ pelo método unificado da largura efetiva $A_{ef}$) e nos parâmetros de resistência à flambagem por cisalhamento de almas não enrijecidas. Torna-se, portanto, fundamental investigar como a modelagem estrutural das vigas mistas protendidas se comporta frente às prescrições da **ABNT NBR 8800:2024**, utilizando uma ferramenta computacional de apoio para realizar verificações precisas ao longo do vão e comparar quantitativamente os resultados obtidos entre as edições de 2008 e de 2024.

## 1.3 Objetivos

### 1.3.1 Objetivo Geral
Desenvolver a modelagem estrutural multifásica para verificação no Estado Limite Último (ELU) de vigas mistas de aço e concreto protendidas (pré-tracionadas e pós-tracionadas) e realizar um estudo comparativo entre os critérios de dimensionamento da **ABNT NBR 8800:2008** e da **ABNT NBR 8800:2024**, com o auxílio de um programa computacional de verificação.

### 1.3.2 Objetivos Específicos
1. Apresentar a formulação analítica do comportamento estrutural de vigas mistas protendidas ao longo de suas três fases críticas: construção (Fase 1), serviço no tempo imediato ($t = 0$, Fase 2) e serviço no tempo infinito ($t = \infty$, Fase 3);
2. Modelar a evolução da força de protensão nos cabos externos considerando as perdas imediatas por acomodação de ancoragem, as perdas progressivas por relaxação do aço e os acréscimos hiperestáticos decorrentes da deformação da viga sob carregamentos externos;
3. Avaliar os Estados Limites Últimos de flexão positiva, flexão negativa (incluindo a Flambagem Lateral com Distorção da seção transversal — FLD), esforço cortante e interação flexo-compressão ao longo de seções discretizadas no comprimento do vão;
4. Validar o modelo de cálculo implementado no programa de apoio mediante confronto direto com os estudos de caso de pré-tração e pós-tração apresentados por Turini (2021);
5. Analisar comparativamente o impacto das atualizações normativas da **ABNT NBR 8800:2024** em relação à **ABNT NBR 8800:2008** sobre a estabilidade do perfil metálico e a capacidade resistente última da seção mista protendida.

## 1.4 Estrutura do Trabalho

O presente trabalho organiza-se em cinco capítulos:
- O **Capítulo 1** contextualiza o sistema de vigas mistas protendidas, fundamenta a justificativa da atualização para a ABNT NBR 8800:2024 e estabelece os objetivos da pesquisa;
- O **Capítulo 2** desenvolve a revisão da literatura e a fundamentação teórica da modelagem estrutural das vigas mistas protendidas, abrangendo a homogeneização elástica, a mecânica da protensão externa, os estados limites últimos na fase construtiva e em serviço, e o comparativo teórico entre a ABNT NBR 8800:2008 e a ABNT NBR 8800:2024;
- O **Capítulo 3** expõe a metodologia de análise multifásica ao longo do vão, as combinações de ações, a caracterização física e geométrica dos estudos de caso e a apresentação da ferramenta computacional utilizada como plano de fundo para o processamento dos modelos;
- O **Capítulo 4** apresenta a validação numérica da modelagem frente à literatura e discute os resultados do estudo comparativo entre as edições de 2008 e de 2024 da ABNT NBR 8800;
- O **Capítulo 5** sintetiza as conclusões obtidas e propõe recomendações para investigações futuras.

---

# CAPÍTULO 2 — REVISÃO DA LITERATURA E MODELAGEM ESTRUTURAL DE VIGAS MISTAS PROTENDIDAS

A verificação da segurança estrutural de uma Viga Mista de Aço e Concreto Protendida (VMP) requer a formulação do equilíbrio e da compatibilidade de deformações entre três componentes estruturais: o perfil metálico de seção I, a laje de concreto (maciça ou moldada sobre fôrma de aço incorporada) e os cabos externos de protensão. Este capítulo apresenta a modelagem mecânica e normativa do problema sob a perspectiva da engenharia de estruturas.

---

## 2.1 Comportamento Estrutural Misto e Homogeneização da Seção Transversal

O comportamento conjunto de uma viga mista fundamenta-se na transferência de fluxo de cisalhamento longitudinal na interface entre a mesa superior do perfil metálico e a laje de concreto por meio de conectores mecânicos. Sob interação completa, despreza-se o deslizamento relativo na interface aço-concreto, admitindo-se a validade da hipótese cinemática de Navier-Bernoulli (manutenção das seções planas) na fase elástica e a plastificação integral das seções compactas no Estado Limite Último.

### 2.1.1 Largura Efetiva Colaborante e Fluência do Concreto

Em virtude do fenômeno de atraso de cisalhamento (*shear lag*) no plano da laje, as tensões normais longitudinais de compressão decrescem à medida que se afastam do eixo da alma do perfil metálico. Para viabilizar a aplicação da teoria técnica de flexão de vigas, a ABNT NBR 8800 substitui a largura real tributária da laje por uma **largura efetiva colaborante ($b_{ef}$)** sobre a qual o campo de tensões normais longitudinais é considerado uniforme.

No regime elástico linear — indispensável para a determinação da posição da Linha Neutra Elástica (LNE), da excentricidade efetiva do cabo de protensão e do incremento hiperestático de força na armadura ativa —, a heterogeneidade entre o concreto (módulo de deformação secante $E_{cs}$) e o aço estrutural (módulo de elasticidade $E_a$) é equacionada pelo método da **seção homogeneizada (ou transformada)**. Transforma-se a área transversal de concreto comprimido em uma área equivalente de aço por meio do coeficiente de homogeneização modular $n$.

Para incorporar os efeitos reológicos da deformação lenta do concreto sob tensões permanentes (**fluência**), a ABNT NBR 8800 determina que o módulo de elasticidade efetivo do concreto seja reduzido a um terço de seu valor inicial para solicitações de longa duração. Dessa forma, o coeficiente modular $n$ assume dois valores distintos ao longo da vida útil da viga:

$$n = \begin{cases} \dfrac{E_{cs}}{E_a}, & \text{para ações de curta duração } (t = 0) \\ \dfrac{E_{cs}}{3 E_a}, & \text{para ações de longa duração } (t = \infty) \end{cases}$$

A largura homogeneizada da laje é definida por $b_{tr} = n \cdot b_{ef}$. Adotando-se a face inferior da mesa inferior do perfil metálico como eixo horizontal de referência ($y = 0$), e considerando um perfil I simétrico de altura total $d$, área bruta $A_g$ e momento de inércia $I_x$ solidarizado a uma laje de espessura de concreto $t_c$ sobre fôrma metálica de altura $h_F$ ($h_F = 0$ no caso de laje maciça), a ordenada da Linha Neutra Elástica ($y_{tr}$) é determinada pela condição de momento estático nulo:

$$y_{tr} = \frac{b_{tr} \cdot t_c \left(\dfrac{t_c}{2} + h_F + d\right) + A_g \cdot \dfrac{d}{2}}{b_{tr} \cdot t_c + A_g}$$

Definida a espessura efetivamente comprimida da laje de concreto $a_y = \min(d + h_F + t_c - y_{tr}, \; t_c)$, obtêm-se a área homogeneizada ($A_{tr}$), o momento de inércia homogeneizado em relação ao eixo baricêntrico ($I_{tr}$) e os módulos de resistência elásticos referidos às fibras extremas inferior ($W_{tr,inf}$) e superior ($W_{tr,sup}$):

$$A_{tr} = b_{tr} \cdot a_y + A_g$$

$$I_{tr} = \frac{b_{tr} \cdot a_y^3}{12} + b_{tr} \cdot a_y \left(d + h_F + t_c - \frac{a_y}{2} - y_{tr}\right)^2 + I_x + A_g \left(y_{tr} - \frac{d}{2}\right)^2$$

$$W_{tr,inf} = \frac{I_{tr}}{y_{tr}}, \qquad W_{tr,sup} = \frac{I_{tr}}{d + h_F + t_c - y_{tr}}$$

A evolução da fluência entre o instante inicial ($t = 0$) e o tempo infinito ($t = \infty$) reduz a largura transformada da laje a um terço ($b_{tr,\infty} = b_{tr,0} / 3$), promovendo a descida da Linha Neutra Elástica em direção ao perfil metálico ($y_{tr,\infty} < y_{tr,0}$). Consequentemente, a excentricidade do cabo de protensão em relação ao centroide da seção mista ($e_{p,tr} = y_{tr} + e_p$, onde $e_p$ é negativo quando o cabo se posiciona acima da face inferior do perfil) sofre redução ao longo do tempo ($e_{p,tr,\infty} < e_{p,tr,0}$).

---

## 2.2 Mecânica da Protensão Externa e Evolução Multifásica

O comportamento estrutural de uma viga mista protendida é intrinsecamente dependente do histórico evolutivo de construção e carregamento, devendo ser equacionado em três fases cronológicas:
- **Fase 1 (Etapa Construtiva)**: corresponde ao estágio anterior à entrada em serviço da seção mista protendida. No sistema com **pré-tração**, o perfil de aço isolado recebe a força de protensão inicial e resiste, sem a colaboração da laje, ao seu peso próprio ($q_1$), ao peso do concreto fresco ($q_2$) e à sobrecarga de construção ($q_s$). No sistema com **pós-tração**, a protensão ainda é nula ($P_1 = 0$) e a seção mista recém-curada suporta as ações construtivas como viga mista ordinária;
- **Fase 2 (Operação em Curta Duração, $t = 0$)**: a estrutura atua como viga mista protendida com rigidez elástica instantânea ($n = E_{cs}/E_a$), submetida à totalidade das cargas permanentes ($q_1 + q_2 + q_3$, incluindo revestimentos) e à sobrecarga acidental de utilização ($q_4$);
- **Fase 3 (Operação em Longa Duração, $t = \infty$)**: a estrutura permanece submetida às ações permanentes e variáveis de serviço, porém com a seção mista afetada pela fluência do concreto ($3n$) e a força no cabo reduzida pela perda progressiva de relaxação do aço de protensão.

### 2.2.1 Critérios Limites para a Força Inicial de Protensão ($P_{0,máx}$)

A magnitude máxima da força inicial de protensão ($P_0$) introduzida pelos equipamentos de tracionamento é condicionada por dois limites normativos que previnem tanto o colapso prematuro na montagem quanto a deterioração em serviço (RIBEIRO, 2018; TURINI, 2021):

1. **Limite de Estabilidade à Flexo-Compressão do Perfil Metálico na Pré-Tração ($P_{0,máx,1}$)**:
   Quando os cabos são tracionados sobre o perfil I isolado antes do lançamento do concreto, a peça metálica fica submetida exclusivamente à força normal de compressão excêntrica decorrente da protensão. Majorando-se o efeito da protensão pelo produto $\gamma_p \beta P_0$ (com $\gamma_p = 0{,}9$ e fator de sobretensão $\beta = 1{,}1$) e substituindo-se a força normal e o momento fletor de protensão ($\gamma_p \beta P_0 e_{p,I}$, sendo $e_{p,I} = d/2 + e_p$) nas equações de interação de flexo-compressão da ABNT NBR 8800, isola-se a força máxima admissível para o perfil metálico:

$$P_{0,máx,1} = \begin{cases} \dfrac{1}{\gamma_p \beta \left(\dfrac{1}{N_{Rd,I}} + \dfrac{8 e_{p,I}}{9 M_{Rd,I}}\right)}, & \text{para } \dfrac{P_{0,máx,1}}{N_{Rd,I}} \ge 0{,}20 \\ \dfrac{1}{\gamma_p \beta \left(\dfrac{1}{2 N_{Rd,I}} + \dfrac{e_{p,I}}{M_{Rd,I}}\right)}, & \text{para } \dfrac{P_{0,máx,1}}{N_{Rd,I}} < 0{,}20 \end{cases}$$

   em que $N_{Rd,I}$ e $M_{Rd,I}$ representam a força normal resistente de compressão e o momento fletor resistente do perfil metálico isolado. Para vigas pós-tracionadas, como o perfil isolado não é protendido na Fase 1, esse limite inexiste ($P_{0,máx,1} \to \infty$).

2. **Limite de Tensões Normais na Seção Mista em Serviço ($P_{0,máx,2}$)**:
   Baseado nas condições de serviço da ABNT NBR 6118, estabelece-se a força de protensão capaz de compensar as tensões de tração na borda inferior da seção mista homogeneizada em longa duração ($t = \infty$) sob os momentos fletores característicos permanentes ($M_g$) e variáveis ($M_q$), respeitando concomitantemente a tensão limite de compressão de $0{,}60 f_{ck}$ na face superior da laje de concreto:

$$P_{0,máx,2} = \frac{\dfrac{M_g + M_q}{W_{tr,inf}}}{\dfrac{1{,}1}{A_{tr}} + \dfrac{1{,}1 \cdot e_{p,tr}}{W_{tr,inf}}}$$

A força inicial máxima de projeto resulta de $P_{0,máx} = \min(|P_{0,máx,1}|, \; |P_{0,máx,2}|)$. Definida a força $P_0 \le P_{0,máx}$, dimensiona-se a área de armadura ativa ($A_p$) limitando-se a tensão normal na saída do cilindro hidráulico ($\sigma_{p,adm}$) aos valores limites da ABNT NBR 6118 para cordoalhas de baixa relaxação (RB):

$$\sigma_{p,adm} = \begin{cases} \min(0{,}77 f_{ptk}, \; 0{,}85 f_{pyk}), & \text{na Pré-tração} \\ \min(0{,}80 f_{ptk}, \; 0{,}88 f_{pyk}), & \text{na Pós-tração} \end{cases}$$

### 2.2.2 Perdas Imediatas e Progressivas de Protensão

Para cabos externos retos desprovidos de desviadores intermediários, a perda imediata restringe-se ao deslizamento ou **acomodação das cunhas de ancoragem ($\Delta L_{cunha}$)** durante a cravação nas placas de extremidade. Pela relação constitutiva elástica do aço de protensão ao longo do vão $L_v$, a perda imediata de força ($\Delta P_{anc}$) vale:

$$\Delta P_{anc} = \begin{cases} \dfrac{\Delta L_{cunha} \cdot E_p \cdot A_p}{L_v}, & \text{em ancoragem Ativa-Passiva} \\ \dfrac{\Delta L_{cunha} \cdot E_p \cdot A_p}{2 L_v}, & \text{em ancoragem Ativa-Ativa} \end{cases}$$

No decorrer do tempo ($t \to \infty$), os cabos tensionados sofrem perda progressiva de força por **relaxação do aço de protensão ($\Delta P_{rel}$)**. Determinando-se a razão de tensão inicial $\chi_p = \sigma_{p0} / f_{ptk}$, obtém-se por interpolação na Tabela 8.4 da ABNT NBR 6118 o coeficiente de relaxação pura após 1000 horas ($\psi_{1000}$), extrapolando-se a perda estabilizada no tempo infinito por:

$$\Delta P_{rel} = 2{,}5 \cdot \psi_{1000} \cdot P_{atuante}, \qquad P_3 = P_{atuante} - \Delta P_{rel}$$

### 2.2.3 Hiperestaticidade Interna e Incremento de Força no Cabo ($\Delta P$)

Em vigas protendidas com cabos externos ancorados nas extremidades do próprio elemento estrutural, o conjunto viga-cabo constitui um sistema **internamente hiperestático de grau um**. Sob a ação de um carregamento vertical distribuído $q$, a rotação das seções de extremidade e o alongamento da fibra longitudinal situada no nível do cabo ($e$) impõem uma variação de comprimento entre as ancoragens. A restrição oferecida pela rigidez axial do cabo ($E_p A_p$) gera um acréscimo hiperestático de tração ($\Delta P$) autogerado pela deformação da viga.

Aplicando-se o Princípio dos Trabalhos Virtuais para compatibilizar o deslocamento relativo entre as ancoragens de uma viga biapoiada de vão $L_v$, área $A$, momento de inércia $I$ e módulo de elasticidade $E$ com o alongamento elástico de um cabo reto de área $A_p$, módulo $E_p$ e excentricidade constante $e$, deduz-se a expressão fechada do incremento hiperestático (NUNES, 2005; RIBEIRO, 2018; TURINI, 2021):

$$\Delta P(q) = \frac{q \cdot e \cdot L_v^2}{12 \left( e^2 + \dfrac{I}{A} + \dfrac{E \cdot I}{E_p \cdot A_p} \right)}$$

Na análise evolutiva por fases:
- Na **Fase 1 do sistema pré-tracionado**, os incrementos $\Delta P(q_2)$ e $\Delta P(q_s)$ são avaliados com as propriedades geométricas do **perfil metálico isolado** ($A = A_g$, $I = I_x$, $E = E_a$, $e = e_{p,I}$);
- Na **Fase 2 ($t = 0$)**, retira-se o efeito transitório da sobrecarga de construção ($-\Delta P(q_s)$ na pré-tração) e adicionam-se as parcelas $\Delta P(q_3)$ e $\Delta P(q_4)$ avaliadas sobre as propriedades da **seção mista homogeneizada em curta duração** ($A = A_{tr,0}$, $I = I_{tr,0}$, $E = E_a$, $e = e_{p,tr,0}$).

---

## 2.3 Estados Limites Últimos do Perfil Metálico Isolado na Construção

Na etapa construtiva de vigas pré-tracionadas (Fase 1), o perfil I de aço encontra-se submetido à flexo-compressão e ao cisalhamento sem a contribuição resistente da laje de concreto, devendo atender integralmente aos critérios de estabilidade da ABNT NBR 8800.

### 2.3.1 Resistência à Compressão Axial e Estabilidade Global-Local

A força normal resistente de cálculo à compressão centrada ($N_{Rd,I}$) depende da carga crítica elástica de flambagem global por flexão ($N_e$) em torno dos eixos principais de inércia ($x$ com comprimento de flambagem $K_x L_v$ e $y$ com comprimento destravado $K_y L_b$):

$$N_e = \min\left( \frac{\pi^2 E_a I_x}{(K_x L_v)^2}, \; \frac{\pi^2 E_a I_y}{(K_y L_b)^2} \right)$$

Na formulação da **ABNT NBR 8800:2008**, a interação com a flambagem local das chapas da mesa e da alma é expressa pelo fator de redução $Q = Q_s \cdot Q_a$, definindo-se o índice de esbeltez reduzido $\lambda_0 = \sqrt{Q A_g f_y / N_e}$ e o fator de redução de resistência à compressão $\chi$:

$$\chi = \begin{cases} 0{,}658^{\lambda_0^2}, & \text{se } \lambda_0 \le 1{,}50 \\ \dfrac{0{,}877}{\lambda_0^2}, & \text{se } \lambda_0 > 1{,}50 \end{cases} \quad \Longrightarrow \quad N_{Rd,I} = \frac{\chi \cdot Q \cdot A_g \cdot f_y}{\gamma_{a1}}$$

### 2.3.2 Resistência à Flexão Simples (FLT, FLM e FLA) e Interação Flexo-Compressão

O momento fletor resistente de cálculo do perfil metálico isolado ($M_{Rd,I}$) é determinado pelo estado limite mais crítico entre a **Flambagem Lateral com Torção (FLT)**, a **Flambagem Local da Mesa Comprimida (FLM)** e a **Flambagem Local da Alma (FLA)**. Para seções compactas ($\lambda \le \lambda_p$), a resistência atinge o momento de plastificação $M_{pl} = \min(Z_x f_y, \; 1{,}5 W_x f_y)$ dividido por $\gamma_{a1}$. No intervalo de comportamento inelástico ($\lambda_p < \lambda \le \lambda_r$), interpola-se linearmente entre $M_{pl}$ e o momento de início de escoamento com tensões residuais $M_r = 0{,}70 W_x f_y$:

$$M_{Rd} = \min\left( \frac{C_b}{\gamma_{a1}} \left[ M_{pl} - (M_{pl} - M_r) \frac{\lambda - \lambda_p}{\lambda_r - \lambda_p} \right], \; \frac{M_{pl}}{\gamma_{a1}} \right)$$

A atuação combinada da força normal de cálculo ($N_{Sd} = 0{,}9 P_1$) e do momento fletor solicitante ($M_{Sd}$) é verificada pela expressão bilinear de interação da ABNT NBR 8800:

$$\begin{cases} \dfrac{N_{Sd}}{N_{Rd,I}} + \dfrac{8}{9} \dfrac{M_{Sd}}{M_{Rd,I}} \le 1{,}0, & \text{para } \dfrac{N_{Sd}}{N_{Rd,I}} \ge 0{,}20 \\ \dfrac{N_{Sd}}{2 N_{Rd,I}} + \dfrac{M_{Sd}}{M_{Rd,I}} \le 1{,}0, & \text{para } \dfrac{N_{Sd}}{N_{Rd,I}} < 0{,}20 \end{cases}$$

### 2.3.3 Resistência ao Esforço Cortante da Alma

A força cortante resistente de cálculo ($V_{Rd}$) é governada pela plastificação ou pela flambagem por cisalhamento da alma de área $A_w = d \cdot t_w$ e força plástica $V_{pl} = 0{,}60 d t_w f_y$. Em função da esbeltez da alma $\lambda_w = d_1 / t_w$ e dos limites $\lambda_{p,v} = 1{,}10\sqrt{k_v E_a / f_y}$ e $\lambda_{r,v} = 1{,}37\sqrt{k_v E_a / f_y}$, tem-se:

$$V_{Rd} = \begin{cases} \dfrac{V_{pl}}{\gamma_{a1}}, & \text{se } \lambda_w \le \lambda_{p,v} \\ \dfrac{\lambda_{p,v}}{\lambda_w} \dfrac{V_{pl}}{\gamma_{a1}}, & \text{se } \lambda_{p,v} < \lambda_w \le \lambda_{r,v} \\ 1{,}24 \left(\dfrac{\lambda_{p,v}}{\lambda_w}\right)^2 \dfrac{V_{pl}}{\gamma_{a1}}, & \text{se } \lambda_w > \lambda_{r,v} \end{cases}$$

---

## 2.4 Estados Limites Últimos da Seção Mista Protendida em Serviço

Nas Fases 2 ($t = 0$) e 3 ($t = \infty$), a seção mista protendida deve apresentar capacidade resistente última superior aos esforços solicitantes de cálculo tanto sob flexão positiva quanto sob flexão negativa.

### 2.4.1 Tensão Última de Cálculo nos Cabos de Protensão ($\sigma_{pd}$)

Na iminência da plastificação da seção mista no ELU, a deformação específica total da armadura de protensão resulta da soma entre a pré-deformação instalada ($\varepsilon_{pré}$) e o acréscimo último decorrente da curvatura da viga ($\Delta \sigma_p / E_p$). Conforme sistematizado por Ribeiro (2018) e Turini (2021):

$$\varepsilon_{pré} = \begin{cases} \dfrac{0{,}9 P_{atuante}}{E_p A_p}, & \text{na Pré-tração} \\ \dfrac{0{,}9 P_{atuante}}{E_p A_p} \left[ 1 + \alpha_p \rho_p \left(1 + \dfrac{e_{p,tr}^2 A_{tr}}{I_{tr}}\right) \right], & \text{na Pós-tração} \end{cases}$$

sendo $\rho_p = A_p / A_{tr}$ e $\alpha_p = E_p / E_{cs}$. O acréscimo de tensão no estado limite último para cabos não aderentes é estimado em função da relação vão/altura total da seção mista:

$$\Delta \sigma_p = \begin{cases} \min\left(70 + \dfrac{f_{ck}}{100 \rho_p}, \; 420\text{ MPa}\right), & \text{para } \dfrac{L_v}{d + h_F + t_c} \le 35 \\ \min\left(70 + \dfrac{f_{ck}}{300 \rho_p}, \; 210\text{ MPa}\right), & \text{para } \dfrac{L_v}{d + h_F + t_c} > 35 \end{cases}$$

A tensão de cálculo na armadura ativa no ELU ($\sigma_{pd}$) é então obtida aplicando-se o coeficiente de minoração $\gamma_p = 1{,}15$:

$$\sigma_{pd} = \min\left( \frac{\varepsilon_{pré} E_p + \Delta \sigma_p}{1{,}15}, \; \frac{f_{pyk}}{1{,}15} \right)$$

### 2.4.2 Momento Fletor Resistente Positivo da Seção Mista Protendida ($M_{Rd,VMP}^+$)

Para seções mistas cujo perfil metálico possui alma compacta ($\lambda_w = d_1 / t_w \le 3{,}76\sqrt{E_a / f_y}$), o momento fletor resistente positivo no ELU é determinado pela teoria rígido-plástica, admitindo-se bloco retangular de tensões de compressão $0{,}85 f_{cd}$ na laje de concreto, escoamento uniforme $f_{yd}$ no perfil metálico e força de tração última $T_{cabo} = A_p \sigma_{pd}$ nos cabos de protensão.

A partir da capacidade máxima de compressão da laje $C_{c,máx} = 0{,}85 f_{cd} b_{ef} t_c$ e da força plástica de tração do perfil isolado $T_{a,máx} = A_g f_{yd}$, identificam-se dois casos de equilíbrio plástico:
1. **Linha Neutra Plástica (LNP) contida na laje de concreto ($C_{c,máx} \ge T_{a,máx} + T_{cabo}$)**:
   O perfil de aço permanece integralmente tracionado e a espessura do bloco comprimido de concreto ($a$) é dada por:
   $$a = \frac{T_{a,máx} + T_{cabo}}{0{,}85 f_{cd} b_{ef}}$$
   O momento fletor resistente positivo resulta do equilíbrio de momentos em relação ao centro de pressão do concreto comprimido:
   $$M_{Rd,VMP}^+ = T_{a,máx} \left(\frac{d}{2} + h_F + t_c - \frac{a}{2}\right) + T_{cabo} \left(d + e_p + h_F + t_c - \frac{a}{2}\right)$$
2. **Linha Neutra Plástica (LNP) situada no perfil metálico ($C_{c,máx} < T_{a,máx} + T_{cabo}$)**:
   A laje de concreto trabalha totalmente comprimida ($C_c = C_{c,máx}$) e a região superior do perfil metálico absorve a força complementar de compressão $C_{aço} = 0{,}5 (A_g f_{yd} + T_{cabo} - C_{c,máx})$, restando na porção inferior do perfil a força de tração $T_{aço} = C_{c,máx} + C_{aço} - T_{cabo}$. Calculada a posição da LNP ($y_p$) e os baricentros das zonas comprimida ($y_c$) e tracionada ($y_t$) do perfil, obtém-se:
   $$M_{Rd,VMP}^+ = C_{c,máx} \left(\frac{t_c}{2} + h_F + y_p\right) + C_{aço} (y_p - y_c) + T_{aço} (d - y_p - y_t) + T_{cabo} (d + e_p - y_p)$$

Para seções classificadas como semicompactas ($3{,}76\sqrt{E_a / f_y} < \lambda_w \le 5{,}70\sqrt{E_a / f_y}$), a resistência limita-se ao momento elástico da seção homogeneizada:

$$M_{Rd,VMP}^+ = \min\left( W_{tr,inf} \cdot f_{yd}, \; \frac{1}{n} W_{tr,sup} \cdot f_{cd} \right)$$

### 2.4.3 Momento Fletor Resistente Negativo e Flambagem Lateral com Distorção ($M_{Rd,VMP}^-$)

Sob a atuação do momento fletor negativo de protensão ($M_{Sd}^- = 0{,}9 P_{atuante} \cdot e_{p,tr}$), a laje de concreto encontra-se tracionada (fissurada) e a mesa inferior do perfil metálico fica submetida à compressão longitudinal. Como o travamento lateral contínuo proporcionado pela laje atua apenas sobre a mesa superior, a mesa inferior comprimida pode deslocar-se lateralmente mediante flexão transversal da alma — modo de instabilidade denominado **Flambagem Lateral com Distorção da seção transversal (FLD)** (Anexo O da ABNT NBR 8800).

A rigidez rotacional do vínculo elástico contínuo fornecido pelo conjunto laje-alma ($k_r$) é modelada pela associação em série entre a rigidez flexural transversal da laje fissurada ($k_1 = \alpha_{viga} E_a I_{laje} / a_{vigas}$) e a rigidez flexural da alma metálica ($k_2 = E_a t_w^3 / [3{,}64 (d - t_f)]$):

$$k_r = \frac{k_1 \cdot k_2}{k_1 + k_2}$$

O momento crítico elástico à flambagem lateral com distorção ($M_{cr}$) é expresso por:

$$M_{cr} = \frac{\alpha_g \cdot C_{dist}(\psi)}{L_v} \sqrt{\left(G I_t + \frac{k_r L_v^2}{\pi^2}\right) E_a I_{af,y}}$$

onde $I_{af,y} = b_f^3 t_f / 12$ é o momento de inércia da mesa inferior comprimida em torno do eixo vertical da seção, $\alpha_g$ é o fator geométrico da seção mista fissurada e $C_{dist}(\psi)$ é o coeficiente dependente da relação de momentos $\psi = |M_{Sd}^-| / M_{Sd}^+$. Determinado o índice de esbeltez reduzido $\lambda_{dist}$ e o fator de redução correspondente $\chi_{dist}$ pela curva de estabilidade da ABNT NBR 8800, o momento resistente negativo de cálculo resulta de:

$$M_{Rd,VMP}^- = \min\left(M_{Rd,neg}^-, \; \chi_{dist} \cdot M_{Rd,neg}^-\right)$$

---

## 2.5 Evolução Normativa: Da ABNT NBR 8800:2008 à ABNT NBR 8800:2024

A revisão da norma brasileira **ABNT NBR 8800:2024** incorporou avanços teóricos e experimentais consolidados internacionalmente no dimensionamento de estruturas metálicas e mistas. No âmbito do comportamento de vigas mistas protendidas, duas alterações normativas merecem destaque especial:

1. **Método da Largura Efetiva Unificada ($A_{ef}$) para Flambagem Local na Compressão**:
   A edição de 2008 penalizava a flambagem local de elementos esbeltos comprimidos por meio do fator $Q = Q_s \cdot Q_a$, que reduzia diretamente a tensão de escoamento efetiva no índice de esbeltez global ($\lambda_0 = \sqrt{Q A_g f_y / N_e}$). A **ABNT NBR 8800:2024** substituiu esse procedimento pelo **Método da Largura Efetiva de Winter**: o índice de esbeltez reduzido global $\lambda_0 = \sqrt{A_g f_y / N_e}$ e a tensão crítica global $f_n = \chi f_y$ são calculados sempre com a área bruta $A_g$. Cada elemento componente (mesa ou alma) tem seu limite de esbeltez corrigido pelo nível de tensão atuante ($\lambda_{r,ef} = \lambda_r \sqrt{f_y / f_n}$). Se a esbeltez do elemento superar $\lambda_{r,ef}$, sua largura efetiva $b_{ef}$ é determinada pela curva de resistência pós-flambagem:

$$b_{ef} = b \left(1 - c_1 \sqrt{\frac{f_{el}}{f_n}}\right) \sqrt{\frac{f_{el}}{f_n}} \le b, \qquad f_{el} = \left(c_2 \frac{\lambda_r}{\lambda}\right)^2 f_y$$

   com constantes $c_1 = 0{,}22$ e $c_2 = 1{,}49$ para mesas de perfis I (elementos AL) e $c_1 = 0{,}18$ e $c_2 = 1{,}31$ para almas (elementos AA), resultando na resistência axial $N_{Rd,I} = \chi A_{ef} f_y / \gamma_{a1}$;

2. **Coeficiente de Flambagem por Cisalhamento de Almas Não Enrijecidas ($k_v$)**:
   Na verificação ao esforço cortante de almas sem enrijecedores transversais, a edição de 2008 adotava de forma aproximada $k_v = 5{,}0$. A **ABNT NBR 8800:2024** adota o valor teórico exato de estabilidade elástica de placas longas sob cisalhamento puro, **$k_v = 5{,}34$**, ampliando os limites de esbeltez plástico ($\lambda_{p,v}$) e elástico ($\lambda_{r,v}$) da alma em $3{,}34\%$ e elevando em $6{,}80\%$ a resistência ao cortante no regime de flambagem elástica.

**Tabela 2.1 — Quadro comparativo das prescrições da ABNT NBR 8800:2008 e da ABNT NBR 8800:2024 aplicáveis às VMP**

| Critério Normativo | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Efeito sobre o Projeto de VMP |
| :--- | :--- | :--- | :--- |
| **Flambagem Local sob Compressão Axial** | Fator $Q = Q_s Q_a$ aplicado sobre $\lambda_0 = \sqrt{Q A_g f_y / N_e}$; $N_{Rd} = \chi Q A_g f_y / \gamma_{a1}$. | Esbeltez global com área bruta $\lambda_0 = \sqrt{A_g f_y / N_e}$ e seção efetiva $A_{ef}$ avaliada na tensão $f_n = \chi f_y$. | Valores idênticos de $N_{Rd,I}$ para perfis laminados compactos ($A_{ef} = A_g$); ganho de capacidade para perfis soldados esbeltos na Fase 1. |
| **Cisalhamento em Almas sem Enrijecedores** | Coeficiente $k_v = 5{,}0$; $\lambda_{p,v} = 1{,}10\sqrt{5{,}0 E_a / f_y}$. | Coeficiente $k_v = 5{,}34$; $\lambda_{p,v} = 1{,}10\sqrt{5{,}34 E_a / f_y}$. | Aumento de $3{,}34\%$ nos limites $\lambda_{p,v}$ e $\lambda_{r,v}$ e de $6{,}80\%$ na resistência elástica ao cortante. |
| **Flexão do Perfil Metálico (FLT, FLM, FLA)** | Momento plástico $M_{pl} = \min(Z_x f_y, 1{,}5 W_x f_y)$ e transição linear com $M_r = 0{,}70 W_x f_y$. | Manutenção das curvas de resistência para perfis I duplamente simétricos compactos e semicompactos. | Manutenção de $M_{Rd,I}$ e do limite de protensão $P_{0,máx,1}$ em perfis laminados Série W. |
| **Resistência Última Positiva e Negativa da Seção Mista** | Modelo rígido-plástico ($\lambda_w \le 3{,}76\sqrt{E_a/f_y}$) e verificação de FLD pelo modelo de pórtico U invertido. | Preservação do equilíbrio de blocos plásticos e do modelo de rigidez rotacional $k_r$ na FLD. | Compatibilidade integral dos momentos resistentes $M_{Rd,VMP}^+$ e $M_{Rd,VMP}^-$. |

---

# CAPÍTULO 3 — METODOLOGIA DE ANÁLISE E ROTEIRO DE VERIFICAÇÃO MULTIFÁSICA

Este capítulo apresenta a metodologia estrutural adotada para a verificação das vigas mistas protendidas ao longo de seu vão, as combinações últimas de ações para cada fase construtiva, a caracterização dos estudos de caso analisados e a descrição sucinta da ferramenta computacional utilizada como plano de fundo para o processamento dos cálculos.

---

## 3.1 Roteiro de Verificação Multifásica e Combinações de Ações

Para avaliar com fidelidade as condições críticas de segurança ao longo de toda a extensão da viga biapoiada de comprimento $L$, o domínio longitudinal $[0, L]$ é discretizado em $X$ seções transversais igualmente espaçadas ($x_0 = 0, x_1, \dots, x_{X-1} = L$), incluindo obrigatoriamente os apoios extremos e a seção central do meio do vão ($x = L/2$).

Em cada seção transversal $x_i$, a verificação do Estado Limite Último é conduzida sequencialmente nas três fases temporais definidas na modelagem teórica:

### 3.1.1 Fase 1 — Verificação na Etapa Construtiva (ELU 1)
Na etapa de montagem e concretagem, atuam o peso próprio do perfil metálico ($q_1$), o peso do concreto fresco ($q_2$) e a sobrecarga variável de construção ($q_s$). Adota-se a combinação última especial de construção prescrita pela ABNT NBR 8800:

$$q_{Sd,1} = 1{,}3 (q_1 + q_2) + 1{,}2 q_s$$

- **No sistema Pré-Tracionado**: a força atuante no cabo na Fase 1 é $P_1 = P_0 - \Delta P_{anc} + \Delta P(q_2) + \Delta P(q_s)$. Em cada seção $x_i$, o momento fletor positivo solicitante das cargas gravitacionais é aliviado pelo momento de protensão minorado por $\gamma_p = 0{,}9$:

$$M_{Sd,1}^+(x_i) = \max\left(0, \; \frac{q_{Sd,1}}{2}(L x_i - x_i^2) - 0{,}9 P_1 e_{p,I}\right), \qquad M_{Sd,1}^-(x_i) = 0{,}9 P_1 e_{p,I}$$

  Verifica-se o perfil metálico isolado frente ao momento resistente $M_{Rd,I}$, ao esforço cortante máximo $V_{Sd,1} = q_{Sd,1} L / 2 \le V_{Rd}$ e à equação de interação de flexo-compressão sob força normal $N_{Sd,1} = 0{,}9 P_1$.
- **No sistema Pós-Tracionado**: como os cabos ainda não foram protendidos ($P_1 = 0$), verifica-se a viga mista sem protensão para o momento solicitante $M_{Sd,1}^+(x_i) = \frac{1}{2} q_{Sd,1}(L x_i - x_i^2) \le M_{Rd,VM}^+$.

### 3.1.2 Fase 2 — Verificação em Serviço de Curta Duração ($t = 0$, ELU 2)
Com a estrutura em operação sob a totalidade das ações permanentes ($q_g = q_1 + q_2 + q_3$) e a ação variável de uso ($q_4$), aplica-se a combinação última normal de serviço:

$$q_{Sd,2} = 1{,}4 (q_1 + q_2 + q_3) + 1{,}4 q_4$$

A força de protensão atuante em curta duração ($P_2$) incorpora os acréscimos hiperestáticos $\Delta P(q_3)$ e $\Delta P(q_4)$ avaliados sobre a seção mista homogeneizada com $n = E_{cs}/E_a$. Em cada seção $x_i$, verificam-se:
1. **Flexão Positiva da Seção Mista**: $M_{Sd,2}^+(x_i) = \frac{1}{2} q_{Sd,2}(L x_i - x_i^2) \le M_{Rd,VMP,0}^+(x_i)$;
2. **Flexão Negativa Introduzida pela Protensão (FLD)**: $M_{Sd,2}^-(x_i) = 0{,}9 P_2(x_i) e_{p,tr,0}(x_i) \le M_{Rd,VMP,0}^-(x_i)$;
3. **Esforço Cortante Máximo**: $V_{Sd,2} = q_{Sd,2} L / 2 \le V_{Rd}$.

### 3.1.3 Fase 3 — Verificação em Serviço de Longa Duração ($t = \infty$, ELU 3)
Mantendo-se o carregamento último de serviço $q_{Sd,2}$, contabiliza-se a perda progressiva por relaxação pura da armadura ativa ($P_3 = P_2 - \Delta P_{rel}$) e a migração da Linha Neutra Elástica pela fluência do concreto ($n_{\infty} = E_{cs} / [3 E_a]$), reduzindo a excentricidade para $e_{p,tr,\infty} = y_{tr,\infty} + e_p$. Reavaliam-se então a tensão última no cabo $\sigma_{pd,\infty}$ e as condições $M_{Sd,3}^+(x_i) \le M_{Rd,VMP,\infty}^+(x_i)$ e $M_{Sd,3}^-(x_i) = 0{,}9 P_3(x_i) e_{p,tr,\infty}(x_i) \le M_{Rd,VMP,\infty}^-(x_i)$.

---

## 3.2 Caracterização dos Estudos de Caso e Ferramenta Computacional de Apoio

Para aplicar a metodologia descrita, validar a formulação frente à literatura (TURINI, 2021) e comparar as edições de 2008 e de 2024 da ABNT NBR 8800, foram selecionados dois estudos de caso completos que contemplam as duas tipologias construtivas e de laje mais recorrentes na prática:

**Tabela 3.1 — Parâmetros geométricos, mecânicos e de carregamento dos Estudos de Caso 1 e 2**

| Parâmetro de Projeto | Símbolo | Unidade | Estudo de Caso 1 (Pré-Tração) | Estudo de Caso 2 (Pós-Tração) |
| :--- | :---: | :---: | :---: | :---: |
| Sistema de aplicação da protensão | — | — | Pré-tração (no perfil I) | Pós-tração (na seção mista) |
| Vão teórico biapoiado | $L$ | $\text{mm}$ | $12\,000$ | $15\,000$ |
| Comprimento destravado na construção | $L_b$ | $\text{mm}$ | $3\,000$ | $3\,000$ |
| Espaçamento transversal entre vigas | $a_{vigas}$ | $\text{mm}$ | $3\,000$ | $2\,000$ |
| Perfil metálico laminado (ASTM A572 Gr. 50) | — | — | W 360 x 57,8 | W 460 x 89 |
| Resistência ao escoamento e módulo do aço | $f_y \; / \; E_a$ | $\text{MPa}$ | $345 \; / \; 200\,000$ | $345 \; / \; 200\,000$ |
| Área bruta e inércia principal do perfil I | $A_g \; / \; I_x$ | $\text{cm}^2 \; / \; \text{cm}^4$ | $72{,}50 \; / \; 16\,143$ | $114{,}10 \; / \; 41\,105$ |
| Tipologia da laje de concreto (Classe C30) | — | — | Maciça ($h_F = 0$) | Com fôrma de aço ($h_F = 50\text{ mm}$) |
| Espessura efetiva e largura colaborante | $t_c \; / \; b_{ef}$ | $\text{mm}$ | $130 \; / \; 3\,000$ | $130 \; / \; 2\,000$ |
| Resistência característica e módulo secante | $f_{ck} \; / \; E_{cs}$ | $\text{MPa}$ | $30 \; / \; 26\,838$ | $30 \; / \; 26\,838$ |
| Armadura longitudinal negativa da laje | $A_s$ | $\text{cm}^2/\text{m}$ | $5{,}00$ | $5{,}00$ |
| Cordoalhas de protensão CP-190 RB ($\varnothing 15{,}2\text{ mm}$) | $f_{ptk} \; / \; A_{unit}$ | $\text{MPa} \; / \; \text{mm}^2$ | $1\,864 \; / \; 140{,}0$ | $1\,864 \; / \; 140{,}0$ |
| Posição vertical do cabo e acomodação de cunha | $e_p \; / \; \Delta L_{cunha}$ | $\text{mm}$ | $-50{,}0 \; / \; 6{,}0$ | $-50{,}0 \; / \; 6{,}0$ |
| Força inicial de protensão adotada | $P_0$ | $\text{kN}$ | $400{,}00$ | $P_{0,máx}$ ($1\,165{,}31$) |
| Peso próprio do perfil e do concreto fresco | $q_1 \; / \; q_2$ | $\text{kN/m}$ | $0{,}56 \; / \; 9{,}75$ | $0{,}88 \; / \; 6{,}81$ |
| Sobrecarga transitória de construção | $q_s$ | $\text{kN/m}$ | $3{,}00$ | $2{,}00$ |
| Carga permanente complementar e sobrecarga | $q_3 \; / \; q_4$ | $\text{kN/m}$ | $6{,}00 \; / \; 15{,}00$ | $4{,}00 \; / \; 10{,}00$ |

Como plano de fundo para eliminar erros de arredondamento manual e permitir a avaliação instantânea das equações ao longo das $X$ seções sob ambas as edições normativas (`ABNT NBR 8800:2008` e `ABNT NBR 8800:2024`), desenvolveu-se o programa computacional `VMP` (escrito em C++ com interface gráfica interativa em *Dear ImGui* e *Raylib*). O programa recebe os parâmetros físicos da Tabela 3.1, desenha em escala a seção transversal mista com a posição da Linha Neutra Elástica e dos cabos de protensão, e gera as tabelas completas de verificação apresentadas no Capítulo 4.

---

# CAPÍTULO 4 — VALIDAÇÃO DO MODELO E ESTUDO COMPARATIVO (ABNT NBR 8800:2008 vs. 2024)

Este capítulo apresenta os resultados numéricos obtidos pela aplicação da modelagem multifásica aos Estudos de Caso 1 e 2. Inicialmente, realiza-se a validação do modelo confrontando seus resultados com aqueles reportados por Turini (2021) segundo a ABNT NBR 8800:2008. Em seguida, analisa-se o comportamento das estruturas frente às novas prescrições da **ABNT NBR 8800:2024**.

---

## 4.1 Validação da Modelagem — Estudo de Caso 1 (Viga Mista Pré-Tracionada)

As Tabelas 4.1 e 4.2 apresentam o confronto entre os valores analíticos de referência de Turini (2021) e os resultados obtidos pela modelagem implementada neste trabalho para o Estudo de Caso 1.

**Tabela 4.1 — Validação das propriedades homogeneizadas, resistências do perfil metálico e protensão máxima (Estudo de Caso 1)**

| Grandeza Estrutural | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio Relativo (%) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| Coeficiente de homogeneização ($t = \infty$) | $n_{\infty}$ | — | $0{,}04473$ | $0{,}04473$ | $0{,}00\%$ |
| Ordenada da LNE em curta duração ($t = 0$) | $y_{tr,0}$ | $\text{mm}$ | $393{,}30$ | $393{,}30$ | $0{,}00\%$ |
| Ordenada da LNE em longa duração ($t = \infty$) | $y_{tr,\infty}$ | $\text{mm}$ | $351{,}30$ | $351{,}30$ | $0{,}00\%$ |
| Área homogeneizada ($t = \infty$) | $A_{tr,\infty}$ | $\text{cm}^2$ | $246{,}96$ | $246{,}94$ | $-0{,}01\%$ |
| Momento de inércia homogeneizado ($t = \infty$) | $I_{tr,\infty}$ | $\text{cm}^4$ | $49091{,}19$ | $49091{,}19$ | $0{,}00\%$ |
| Momento de inércia fissurado da laje | $I_{laje}$ | $\text{cm}^4/\text{m}$ | $431{,}15$ | $431{,}15$ | $0{,}00\%$ |
| Compressão axial resistente do perfil I | $N_{Rd,I}$ | $\text{kN}$ | $1416{,}61$ | $1416{,}77$ | $+0{,}01\%$ |
| Momento fletor resistente do perfil I (FLT) | $M_{Rd,I}$ | $\text{kN}\cdot\text{m}$ | $285{,}98$ | $285{,}69$ | $-0{,}10\%$ |
| Força cortante resistente do perfil I | $V_{Rd}$ | $\text{kN}$ | $532{,}21$ | $532{,}22$ | $0{,}00\%$ |
| Limite de protensão pela estabilidade do aço | $P_{0,máx,1}$ | $\text{kN}$ | $912{,}57$ | $912{,}30$ | $-0{,}03\%$ |
| Limite de protensão por tensões na seção mista | $P_{0,máx,2}$ | $\text{kN}$ | $1431{,}63$ | $1431{,}59$ | $0{,}00\%$ |
| Força máxima de protensão inicial | $P_{0,máx}$ | $\text{kN}$ | $912{,}57$ | $912{,}30$ | $-0{,}03\%$ |
| Área efetiva da armadura ativa ($3$ cordoalhas) | $A_p$ | $\text{mm}^2$ | $420{,}00$ | $420{,}00$ | $0{,}00\%$ |

**Tabela 4.2 — Validação das verificações do ELU nas Fases 1, 2 e 3 no meio do vão ($x = 6000\text{ mm}$) — Estudo de Caso 1**

| Fase / Grandeza Estrutural | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Verificação no ELU |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Fase 1 — Etapa Construtiva (Perfil I Protendido)** | | | | | | |
| Perda imediata por acomodação da cunha | $\Delta P_{anc}$ | $\text{kN}$ | $42{,}00$ | $42{,}00$ | $0{,}00\%$ | — |
| Incremento hiperestático ($\Delta P$ concreto fresco) | $\Delta P(q_2)$ | $\text{kN}$ | $35{,}65$ | $35{,}65$ | $0{,}00\%$ | — |
| Força de protensão atuante na construção | $P_1$ | $\text{kN}$ | $404{,}62$ | $404{,}63$ | $0{,}00\%$ | — |
| Excentricidade do cabo no perfil isolado | $e_{p,I}$ | $\text{mm}$ | $129{,}00$ | $129{,}00$ | $0{,}00\%$ | — |
| Momento solicitante positivo líquido | $M_{Sd,1}^+$ | $\text{kN}\cdot\text{m}$ | $259{,}09$ | $259{,}09$ | $0{,}00\%$ | $\le 285{,}69$ (Atende) |
| Momento solicitante negativo de protensão | $M_{Sd,1}^-$ | $\text{kN}\cdot\text{m}$ | $46{,}98$ | $46{,}98$ | $0{,}00\%$ | $\le 285{,}69$ (Atende) |
| Força cortante solicitante máxima | $V_{Sd,1}$ | $\text{kN}$ | $102{,}00$ | $102{,}02$ | $+0{,}02\%$ | $\le 532{,}22$ (Atende) |
| **Fase 2 — Serviço em Curta Duração ($t = 0$)** | | | | | | |
| Força de protensão atuante em $t = 0$ | $P_2$ | $\text{kN}$ | $448{,}41$ | $448{,}42$ | $0{,}00\%$ | — |
| Excentricidade em relação à LNE ($t = 0$) | $e_{p,tr,0}$ | $\text{mm}$ | $343{,}30$ | $343{,}30$ | $0{,}00\%$ | — |
| Tensão de cálculo no cabo no ELU | $\sigma_{pd,0}$ | $\text{MPa}$ | $923{,}80$ | $923{,}78$ | $0{,}00\%$ | — |
| Momento solicitante positivo máximo | $M_{Sd,2}^+$ | $\text{kN}\cdot\text{m}$ | $789{,}01$ | $789{,}01$ | $0{,}00\%$ | — |
| Momento resistente positivo da VMP ($t = 0$) | $M_{Rd,VMP,0}^+$ | $\text{kN}\cdot\text{m}$ | $807{,}86$ | $807{,}87$ | $0{,}00\%$ | $M_{Sd}^+ \le M_{Rd}^+$ (Atende) |
| Momento solicitante negativo de protensão | $M_{Sd,2}^-$ | $\text{kN}\cdot\text{m}$ | $138{,}68$ | $138{,}55$ | $-0{,}09\%$ | — |
| Momento resistente negativo da VMP (FLD) | $M_{Rd,VMP,0}^-$ | $\text{kN}\cdot\text{m}$ | $354{,}49$ | $354{,}50$ | $0{,}00\%$ | $M_{Sd}^- \le M_{Rd}^-$ (Atende) |
| Força cortante solicitante máxima | $V_{Sd,2}$ | $\text{kN}$ | $263{,}00$ | $263{,}02$ | $+0{,}01\%$ | $\le 532{,}22$ (Atende) |
| **Fase 3 — Serviço em Longa Duração ($t = \infty$)** | | | | | | |
| Coeficiente de relaxação a 1000 horas | $\psi_{1000}$ | $\%$ | $0{,}946\%$ | $0{,}946\%$ | $0{,}00\%$ | — |
| Força de protensão final após relaxação | $P_3$ | $\text{kN}$ | $437{,}81$ | $437{,}82$ | $0{,}00\%$ | — |
| Excentricidade em relação à LNE ($t = \infty$) | $e_{p,tr,\infty}$ | $\text{mm}$ | $301{,}30$ | $301{,}30$ | $0{,}00\%$ | — |
| Momento resistente positivo da VMP ($t = \infty$) | $M_{Rd,VMP,\infty}^+$ | $\text{kN}\cdot\text{m}$ | $802{,}53$ | $802{,}51$ | $0{,}00\%$ | $\ge 789{,}01$ (Atende) |
| Momento solicitante negativo ($t = \infty$) | $M_{Sd,3}^-$ | $\text{kN}\cdot\text{m}$ | $118{,}75$ | $118{,}72$ | $-0{,}03\%$ | — |
| Momento resistente negativo da VMP ($t = \infty$) | $M_{Rd,VMP,\infty}^-$ | $\text{kN}\cdot\text{m}$ | $358{,}60$ | $358{,}61$ | $0{,}00\%$ | $\ge 118{,}72$ (Atende) |

Verifica-se nas Tabelas 4.1 e 4.2 uma aderência virtualmente exata entre a modelagem proposta e os valores de referência da literatura, com desvios máximos inferiores a $0{,}10\%$. Do ponto de vista estrutural, nota-se que na pré-tração o incremento hiperestático produzido pelo peso do concreto fresco ($\Delta P(q_2) = 35{,}65\text{ kN}$) e pela sobrecarga de construção ($\Delta P(q_s) = 10{,}97\text{ kN}$) compensa integralmente a perda imediata de ancoragem ($\Delta P_{anc} = 42{,}00\text{ kN}$), elevando a força no cabo de $400{,}00\text{ kN}$ para $404{,}63\text{ kN}$ já na Fase 1 e para $448{,}42\text{ kN}$ na Fase 2.

A Tabela 4.3 ilustra a variação longitudinal dos momentos solicitantes e das taxas de solicitação ($M_{Sd} / M_{Rd}$) ao longo das $X = 5$ seções discretizadas na Fase 2 ($t = 0$).

**Tabela 4.3 — Verificação ao longo das $X = 5$ seções transversais na Fase 2 ($t = 0$) — Estudo de Caso 1**

| Seção | Coordenada $x$ ($\text{mm}$) | Posição na Viga | $P(x)$ ($\text{kN}$) | $M_{Sd}^+(x)$ ($\text{kN}\cdot\text{m}$) | $M_{Rd,VMP}^+(x)$ ($\text{kN}\cdot\text{m}$) | Relação $M_{Sd}^+/M_{Rd}^+$ | $M_{Sd}^-(x)$ ($\text{kN}\cdot\text{m}$) | $M_{Rd,VMP}^-(x)$ ($\text{kN}\cdot\text{m}$) | Relação $M_{Sd}^-/M_{Rd}^-$ |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **0** | $0$ | Apoio Esquerdo | $448{,}42$ | $0{,}00$ | $807{,}87$ | $0{,}0\%$ | $138{,}55$ | $354{,}50$ | $39{,}1\%$ |
| **1** | $3000$ | $L/4$ | $448{,}42$ | $591{,}76$ | $807{,}87$ | $73{,}2\%$ | $138{,}55$ | $354{,}50$ | $39{,}1\%$ |
| **2** | $6000$ | Meio do Vão ($L/2$) | $448{,}42$ | $789{,}01$ | $807{,}87$ | **$97{,}7\%$** | $138{,}55$ | $354{,}50$ | $39{,}1\%$ |
| **3** | $9000$ | $3L/4$ | $448{,}42$ | $591{,}76$ | $807{,}87$ | $73{,}2\%$ | $138{,}55$ | $354{,}50$ | $39{,}1\%$ |
| **4** | $12000$ | Apoio Direito | $448{,}42$ | $0{,}00$ | $807{,}87$ | $0{,}0\%$ | $138{,}55$ | $354{,}50$ | $39{,}1\%$ |

---

## 4.2 Validação da Modelagem — Estudo de Caso 2 (Viga Mista Pós-Tracionada)

A Tabela 4.4 apresenta os resultados da modelagem para o Estudo de Caso 2 (viga mista pós-tracionada com vão de $15{,}0\text{ m}$ e laje sobre fôrma de aço incorporada).

**Tabela 4.4 — Validação do Estudo de Caso 2 (Pós-Tração) com Turini (2021)**

| Grandeza Estrutural / Fase | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Verificação no ELU |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| Força inicial máxima na pós-tração | $P_{0,máx}$ | $\text{kN}$ | $1165{,}50$ | $1165{,}31$ | $-0{,}02\%$ | Governa critério de serviço |
| Área efetiva da armadura ativa ($6$ cordoalhas) | $A_p$ | $\text{mm}^2$ | $840{,}00$ | $840{,}00$ | $0{,}00\%$ | $6 \times 140\text{ mm}^2$ |
| **Fase 1 — Construção (Viga Mista sem protensão)** | | | | | | |
| Momento solicitante positivo na construção | $M_{Sd,1}^+$ | $\text{kN}\cdot\text{m}$ | $348{,}66$ | $348{,}66$ | $0{,}00\%$ | — |
| Momento resistente positivo da VM sem protensão | $M_{Rd,VM}^+$ | $\text{kN}\cdot\text{m}$ | $1296{,}81$ | $1296{,}81$ | $0{,}00\%$ | Atende ($26{,}9\%$) |
| Força cortante resistente do perfil W460x89 | $V_{Rd}$ | $\text{kN}$ | $914{,}84$ | $914{,}85$ | $0{,}00\%$ | Atende ($10{,}2\%$) |
| **Fase 2 ($t = 0$) e Fase 3 ($t = \infty$)** | | | | | | |
| Momento solicitante positivo máximo em serviço | $M_{Sd,2}^+$ | $\text{kN}\cdot\text{m}$ | $854{,}18$ | $854{,}04$ | $-0{,}02\%$ | — |
| Momento resistente positivo da VMP | $M_{Rd,VMP}^+$ | $\text{kN}\cdot\text{m}$ | $1878{,}46$ | $1878{,}49$ | $0{,}00\%$ | Atende ($45{,}5\%$) |
| Momento resistente negativo da VMP (FLD) | $M_{Rd,VMP}^-$ | $\text{kN}\cdot\text{m}$ | $379{,}45$ | $379{,}42$ | $-0{,}01\%$ | Não atende para $P_0 = P_{0,máx}$ |

A análise do Estudo de Caso 2 revela um comportamento estrutural de grande importância prática para o projeto de vigas mistas pós-tracionadas: na ausência da restrição de flambagem do perfil isolado na Fase 1, a estimativa de $P_{0,máx}$ baseada exclusivamente na descompressão em serviço ($1165{,}31\text{ kN}$) conduz a um momento fletor negativo solicitante na Fase 2 ($M_{Sd,2}^- = 460{,}51\text{ kN}\cdot\text{m}$ para cabo reto ou $437{,}80\text{ kN}\cdot\text{m}$ para cabo poligonal) superior à capacidade resistente da seção à Flambagem Lateral com Distorção ($M_{Rd,VMP}^- = 379{,}42\text{ kN}\cdot\text{m}$). Conclui-se, portanto, que nas vigas pós-tracionadas a verificação da estabilidade lateral com distorção sob momento negativo na Fase 2 ($t = 0$) constitui o verdadeiro limitador superior da força de protensão inicial.

---

## 4.3 Estudo Comparativo: ABNT NBR 8800:2008 vs. ABNT NBR 8800:2024

A Tabela 4.5 confronta os resultados do dimensionamento segundo a **ABNT NBR 8800:2008** e segundo a **ABNT NBR 8800:2024**, abrangendo tanto o perfil laminado compacto do Estudo de Caso 1 (W 360 x 57,8, $\lambda_w = 38{,}98$) quanto uma viga mista protendida equivalente composta por perfil soldado de alma esbelta ($t_w = 3{,}0\text{ mm}$, $\lambda_w = 102{,}67$).

**Tabela 4.5 — Comparação de parâmetros de estabilidade e resistências últimas entre a ABNT NBR 8800:2008 e a ABNT NBR 8800:2024**

| Caso / Parâmetro Estrutural | Unidade | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Variação ($\Delta\%$) | Justificativa Mecânica e Normativa |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Estudo de Caso 1 (W 360 x 57,8 — Perfil Compacto)** | | | | | |
| Coeficiente de flambagem por cisalhamento ($k_v$) | — | $5{,}00$ | $5{,}34$ | $+6{,}80\%$ | Adoção do coeficiente exato para alma sem enrijecedores |
| Esbeltez limite plástica ao cortante ($\lambda_{p,v}$) | — | $59{,}22$ | $61{,}20$ | $+3{,}34\%$ | Incremento proporcional a $\sqrt{5{,}34 / 5{,}00}$ |
| Esbeltez limite elástica ao cortante ($\lambda_{r,v}$) | — | $73{,}75$ | $76{,}22$ | $+3{,}35\%$ | Ampliação da faixa de flambagem inelástica ao cortante |
| Força cortante resistente ($V_{Rd}$, $\lambda_w = 38{,}98$) | $\text{kN}$ | $532{,}22$ | $532{,}22$ | $0{,}00\%$ | Alma atinge plastificação por cisalhamento ($V_{pl}/\gamma_{a1}$) |
| Compressão axial resistente do perfil ($N_{Rd,I}$) | $\text{kN}$ | $1416{,}77$ | $1416{,}77$ | $0{,}00\%$ | Ausência de flambagem local ($Q = 1{,}00 \Leftrightarrow A_{ef} = A_g$) |
| Limite máximo de protensão inicial ($P_{0,máx}$) | $\text{kN}$ | $912{,}30$ | $912{,}30$ | $0{,}00\%$ | Invariância das resistências $N_{Rd,I}$ e $M_{Rd,I}$ |
| Momento resistente positivo da VMP ($M_{Rd,VMP,0}^+$) | $\text{kN}\cdot\text{m}$ | $807{,}87$ | $807{,}87$ | $0{,}00\%$ | Equilíbrio plástico da seção mista compacta mantido |
| Momento resistente negativo da VMP ($M_{Rd,VMP,0}^-$) | $\text{kN}\cdot\text{m}$ | $354{,}50$ | $354{,}50$ | $0{,}00\%$ | Formulação de rigidez rotacional $k_r$ e FLD preservada |
| **Viga Mista com Perfil Soldado de Alma Esbelta ($t_w = 3{,}0\text{ mm}$, $\lambda_w = 102{,}67$)** | | | | | |
| Compressão axial resistente na Fase 1 ($N_{Rd,I}$) | $\text{kN}$ | $984{,}12$ | $1012{,}45$ | $+2{,}88\%$ | Método da largura efetiva ($A_{ef}$) com $\lambda_0$ na área bruta |
| Força cortante resistente elástica ($V_{Rd}$) | $\text{kN}$ | $139{,}85$ | $149{,}36$ | $+6{,}80\%$ | Ganho direto da atualização $k_v = 5{,}34$ para $\lambda_w > \lambda_{r,v}$ |

Da análise comparativa apresentada na Tabela 4.5, depreende-se que:
1. **Invariância para Perfis Laminados Compactos**: Quando a viga mista protendida é concebida com perfis laminados padrão da Série W (cujas mesas e almas respeitam com folga os limites de compacidade $\lambda_p$), as resistências de cálculo obtidas pela **ABNT NBR 8800:2024** são rigorosamente idênticas às da edição de 2008. Esse resultado demonstra que toda a metodologia de dimensionamento de VMP desenvolvida sobre a versão anterior da norma permanece plenamente válida sob a nova edição;
2. **Desempenho Superior em Perfis Soldados Esbeltos**: Em projetos otimizados que empregam perfis soldados de alma delgada, a substituição do fator $Q$ pelo método unificado da largura efetiva ($A_{ef}$) e a atualização do parâmetro $k_v = 5{,}34$ na **ABNT NBR 8800:2024** resultam em ganhos de resistência de $2{,}88\%$ na compressão axial do perfil na Fase 1 (elevando o limite $P_{0,máx,1}$ na pré-tração) e de até $6{,}80\%$ na resistência ao esforço cortante.

---

# CAPÍTULO 5 — CONCLUSÕES E SUGESTÕES PARA TRABALHOS FUTUROS

## 5.1 Conclusões

O presente trabalho desenvolveu a modelagem estrutural multifásica e a verificação no Estado Limite Último (ELU) de vigas mistas de aço e concreto protendidas por cabos externos, comparando os critérios normativos das edições de 2008 e de 2024 da ABNT NBR 8800 com o suporte de uma ferramenta computacional dedicada. A partir dos resultados alcançados, conclui-se que:

1. **Rigor e Precisão da Modelagem Multifásica**: O equacionamento sequencial das três fases críticas da estrutura (Construção, Serviço imediato em $t = 0$ e Serviço em longa duração em $t = \infty$), integrando a variação da Linha Neutra Elástica por fluência do concreto, as perdas imediatas e por relaxação e o incremento hiperestático de tração nos cabos ($\Delta P$), reproduziu os estudos de caso de referência da literatura (TURINI, 2021) com desvios máximos inferiores a $0{,}10\%$;
2. **Diferenciação dos Modos Governantes entre Pré-Tração e Pós-Tração**: Ficou demonstrado que, enquanto nas vigas pré-tracionadas a força inicial máxima de protensão ($P_{0,máx}$) é governada pela estabilidade à flexo-compressão do perfil metálico isolado na etapa construtiva (Fase 1), nas vigas pós-tracionadas o estado limite crítico desloca-se para a resistência ao momento fletor negativo governada pela Flambagem Lateral com Distorção (FLD) no instante da aplicação da protensão na seção mista (Fase 2, $t = 0$);
3. **Impacto da Atualização para a ABNT NBR 8800:2024**: Comprovou-se que, para perfis laminados compactos, as resistências de cálculo do perfil isolado e da seção mista protendida permanecem inalteradas entre as edições de 2008 e de 2024. Já para vigas mistas protendidas compostas por perfis soldados de alma esbelta, as inovações da ABNT NBR 8800:2024 — método da seção efetiva ($A_{ef}$) na compressão e coeficiente de flambagem por cisalhamento $k_v = 5{,}34$ — proporcionam ganhos reais de capacidade resistente na verificação da etapa construtiva e do esforço cortante.

## 5.2 Sugestões para Trabalhos Futuros

Como continuidade desta pesquisa, sugerem-se os seguintes temas para trabalhos futuros:
1. A ampliação da modelagem analítica para cabos com traçado poligonal e parabólico dotados de múltiplos desviadores, incorporando as verificações completas do Estado Limite de Serviço (deslocamentos verticais imediatos e diferidos e controle de fissuração);
2. O estudo do comportamento de vigas mistas protendidas contínuas de múltiplos vãos, avaliando os momentos hiperestáticos de protensão nos apoios internos;
3. A realização de estudos paramétricos de otimização estrutural visando determinar a relação ótima entre altura do perfil soldado, excentricidade do cabo e força inicial de protensão segundo a ABNT NBR 8800:2024.

---

# REFERÊNCIAS BIBLIOGRÁFICAS

- AMERICAN INSTITUTE OF STEEL CONSTRUCTION. **ANSI/AISC 360-22**: Specification for Structural Steel Buildings. Chicago: AISC, 2022.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 6118**: Projeto de estruturas de concreto — Procedimento. Rio de Janeiro: ABNT, 2023.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2008.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2024.
- CHEN, S.; GU, P. Load carrying capacity of composite beams prestressed with external tendons under positive moment. **Journal of Constructional Steel Research**, v. 61, n. 4, p. 515–530, 2005.
- NUNES, F. G. **Análise experimental de vigas mistas de aço e concreto protendidas com cabos externos**. Dissertação (Mestrado em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2005.
- QUEIROZ, G.; PIMENTA, R. J.; DA MATA, L. A. C. **Elementos das estruturas mistas aço-concreto**. Belo Horizonte: O Lutador, 2001.
- RIBEIRO, K. D. **Verificação de vigas mistas de aço e concreto protendidas**. Trabalho de Conclusão de Curso (Graduação em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2018.
- TROITSKY, M. S. **Prestressed Steel Bridges: Theory and Design**. New York: Van Nostrand Reinhold, 1990.
- TURINI, T. T. **Desenvolvimento de aplicativo para verificação de vigas mistas de aço e concreto protendidas**. Dissertação (Mestrado em Engenharia Civil) — Programa de Pós-Graduação em Engenharia Civil, Universidade Federal do Espírito Santo, Vitória, 2021.
