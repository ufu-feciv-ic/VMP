# TRABALHO DE CONCLUSÃO DE CURSO

**TÍTULO SUGERIDO:**
> **ANÁLISE E VERIFICAÇÃO NOS ESTADOS LIMITES ÚLTIMO E DE SERVIÇO DE VIGAS MISTAS DE AÇO E CONCRETO PROTENDIDAS: MODELAGEM MULTIFÁSICA E ESTUDO COMPARATIVO ENTRE AS EDIÇÕES DE 2008 E 2024 DA ABNT NBR 8800**

---

## RESUMO

As vigas mistas de aço e concreto protendidas (VMP) aliam a eficiência mecânica da ação conjunta entre a laje de concreto comprimida e o perfil metálico tracionado aos benefícios estruturais da protensão externa, viabilizando o vencimento de grandes vãos livres, a redução da altura útil dos pavimentos e o controle rigoroso de deslocamentos em serviço. Diante da publicação da nova revisão da norma brasileira de projeto de estruturas de aço e de estruturas mistas de edifícios, a **ABNT NBR 8800:2024**, após dezesseis anos de vigência da edição de 2008, evidencia-se a necessidade de consolidar a modelagem analítica desses elementos, abrangendo tanto a capacidade resistente no Estado Limite Último (ELU) quanto o desempenho em serviço nos Estados Limites de Serviço (ELS), e avaliar os reflexos das novas prescrições normativas sobre o projeto estrutural. Este Trabalho de Conclusão de Curso apresenta a formulação analítica e a verificação estrutural completa de vigas mistas biapoiadas protendidas por cabos externos retos sob dois processos executivos distintos: a pré-tração (força de protensão aplicada ao perfil metálico isolado antes da concretagem da laje) e a pós-tração (protensão introduzida diretamente sobre a seção mista consolidada). A metodologia de análise acompanha a evolução cronológica dos esforços solicitantes, das perdas imediatas por ancoragem e progressivas por relaxação, do incremento hiperestático de força no cabo por compatibilidade de deformações e do comportamento mecânico ao longo de cinco fases críticas da vida útil: Fase 1 (etapa construtiva, ELU), Fase 2 (serviço imediato, $t = 0$, ELU), Fase 3 (serviço em longa duração, $t = \infty$, ELU), Fase 4 (ELS de deformação excessiva, com verificação de regime elástico linear e aplicação de contraflecha executiva) e Fase 5 (ELS de vibração no piso sob ações dinâmicas de atividades humanas normais conforme a ABNT NBR 8800 Anexo L). Como suporte à aplicação da metodologia, utilizou-se uma ferramenta computacional desenvolvida especificamente para automatizar as verificações ao longo do vão. A formulação foi validada mediante confronto com estudos de caso de referência da literatura nacional (TURINI, 2021) segundo a ABNT NBR 8800:2008 e a ABNT NBR 6118, apresentando desvios inferiores a $0{,}15\%$. Na sequência, investigou-se o impacto das atualizações introduzidas pela ABNT NBR 8800:2024 — notadamente a adoção do método unificado da largura efetiva para flambagem local na compressão e a revisão do coeficiente de estabilidade ao cisalhamento da alma ($k_v = 5{,}34$) — sobre a capacidade resistente e os critérios governantes de projeto das vigas mistas protendidas.

**Palavras-chave:** Estruturas mistas de aço e concreto; Vigas mistas protendidas; Protensão externa; Análise estrutural multifásica; Estados Limites de Serviço; Vibração em pisos; ABNT NBR 8800:2024.

---

## ABSTRACT

Prestressed steel-concrete composite beams (PSCCB) combine the mechanical efficiency of composite action between the compressed concrete slab and the tensile steel profile with the structural advantages of external prestressing, enabling longer clear spans, reduced floor depths, and strict deflection control. Following the publication of the revised Brazilian standard for steel and composite building structures, **ABNT NBR 8800:2024**, sixteen years after its 2008 edition, consolidating the analytical modeling of these structural members across both the Ultimate Limit State (ULS) and Serviceability Limit States (SLS), as well as assessing the impact of the new code provisions, has become essential. This Undergraduate Thesis presents the analytical formulation and comprehensive verification of simply supported prestressed composite beams with straight external tendons under two construction methods: pre-tensioning (prestressing applied to the bare steel profile prior to slab casting) and post-tensioning (prestressing introduced directly onto the hardened composite section). The analysis tracks the chronological evolution of internal forces, immediate anchorage slip losses and time-dependent relaxation losses, statically indeterminate tendon force increments, and mechanical behavior across five critical stages: Stage 1 (construction phase, ULS), Stage 2 (short-term service operation, $t = 0$, ULS), Stage 3 (long-term service operation, $t = \infty$, ULS), Stage 4 (SLS excessive deflection, including elastic linear stress verification and execution camber compensation), and Stage 5 (SLS floor vibration under human-induced dynamic actions according to ABNT NBR 8800 Annex L). A dedicated computational tool was developed to automate cross-sectional verifications along the span. The analytical model was validated against benchmark case studies from the literature (TURINI, 2021) under ABNT NBR 8800:2008 and ABNT NBR 6118, yielding relative deviations below $0.15\%$. Subsequently, the impact of updates introduced by ABNT NBR 8800:2024—specifically the unified effective width method for local compression buckling and the updated web shear buckling coefficient ($k_v = 5.34$)—on structural capacity and governing design criteria was thoroughly investigated.

**Keywords:** Steel-concrete composite structures; Prestressed composite beams; External prestressing; Multi-stage structural analysis; Serviceability Limit States; Floor vibration; ABNT NBR 8800:2024.

---

# CAPÍTULO 1 — INTRODUÇÃO

## 1.1 Contextualização

A racionalização dos sistemas construtivos impulsionou a consolidação das estruturas mistas de aço e concreto como uma solução de elevado desempenho mecânico para edifícios de múltiplos pavimentos, pontes, passarelas e grandes coberturas. Nas vigas mistas convencionais, a conexão mecânica entre uma laje de concreto (maciça ou moldada sobre fôrma de aço incorporada do tipo *steel deck*) e um perfil metálico de seção I — assegurada por conectores de cisalhamento soldados à mesa superior do perfil — permite explorar sinergicamente as melhores propriedades de cada material: o concreto absorve prioritariamente as tensões normais de compressão, ao passo que o perfil de aço resiste às solicitações de tração e cisalhamento (QUEIROZ; PIMENTA; DA MATA, 2001).

Não obstante a eficiência das vigas mistas tradicionais, o aumento progressivo dos vãos livres exigidos pelos projetos arquitetônicos contemporâneos impõe limitações severas ao dimensionamento. Em vãos superiores a $12\text{ m}$, a concepção estrutural passa a ser frequentemente condicionada pelos Estados Limites de Serviço — notadamente os deslocamentos verticais excessivos (flechas) e a excessiva sensibilidade a vibrações induzidas pelo caminhar humano — ou pela instabilidade lateral e local de perfis metálicos de grande altura, elevando o consumo de aço e a altura total dos pavimentos.

Para superar essas limitações, a técnica da **protensão externa aplicada a vigas mistas de aço e concreto** — formando as chamadas **Vigas Mistas Protendidas (VMP)** — consolidou-se como uma alternativa estrutural de alta eficiência (TROITSKY, 1990; CHEN; GU, 2005; NUNES, 2005). A introdução de cabos de aço de alta resistência tracionados excentricamente em relação ao centro de gravidade da peça gera um esforço normal de compressão associado a um momento fletor negativo ao longo do vão, contrapondo-se diretamente aos esforços solicitantes oriundos das cargas gravitacionais. Dentre os principais benefícios estruturais desse sistema, destacam-se:
1. A ampliação do regime de comportamento elástico linear da viga sob cargas de serviço, reduzindo as tensões finais de tração na mesa inferior do perfil de aço;
2. A introdução de uma contraflecha controlada que compensa parcela expressiva das flechas permanentes e variáveis, permitindo atender com folga ao limite de deformação $\delta_{lim} = L/350$;
3. O aumento da rigidez dinâmica do piso frente aos efeitos de vibração humana, assegurando o cumprimento dos limites de conforto estabelecidos por normas internacionais e pela ABNT NBR 8800;
4. O incremento da capacidade resistente última à flexão positiva da seção mista e a possibilidade de adoção de perfis metálicos mais leves e de menor altura estrutural.

Do ponto de vista construtivo e da história de tensões da estrutura, a aplicação da força de protensão pode ocorrer segundo dois processos distintos:
- **Sistema com Pré-tração**: o esticamento e a ancoragem dos cabos são realizados diretamente sobre o perfil metálico isolado, antes da concretagem da laje. Toda a força inicial de protensão é transferida exclusivamente à seção metálica, evitando que parte da protensão seja absorvida na compressão da laje de concreto, mas exigindo rigorosa verificação da estabilidade do perfil I submetido à flexo-compressão durante a montagem;
- **Sistema com Pós-tração**: a laje é concretada e curada sobre o perfil metálico sem protensão prévia, e o tracionamento dos cabos é efetuado somente após a consolidação da seção mista, de modo que os esforços de protensão atuam desde o início sobre a seção homogeneizada aço-concreto.

## 1.2 Justificativa

Diferentemente do projeto de vigas mistas convencionais ou de vigas de concreto protendido usuais, a análise estrutural de uma Viga Mista Protendida exige uma **modelagem evolutiva multifásica** que integre simultaneamente os fenômenos de instabilidade de perfis metálicos esbeltos, plastificação da seção mista, reologia do concreto (fluência e retração), perdas imediatas e progressivas de protensão, hiperestaticidade interna do conjunto viga-cabo e verificações completas em serviço (deformações verticais e resposta dinâmica a vibrações).

No Brasil, as normas técnicas fundamentais de projeto estrutural — a **ABNT NBR 8800** (Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios) e a **ABNT NBR 6118** (Projeto de estruturas de concreto) — não dispõem de um capítulo único que consolide o roteiro completo de verificação de vigas mistas protendidas por cabos externos. Pesquisas desenvolvidas na Universidade Federal do Espírito Santo (UFES), notadamente os trabalhos de **Nunes (2005)**, **Ribeiro (2018)** e **Turini (2021)**, sistematizaram uma metodologia analítica rigorosa para o dimensionamento e a verificação desses elementos com base na **ABNT NBR 8800:2008**.

Todavia, a publicação da **ABNT NBR 8800:2024**, que revisou a norma brasileira de estruturas de aço e mistas após dezesseis anos de vigência da edição de 2008, introduziu modificações importantes nos critérios de estabilidade local de barras submetidas à compressão axial (substituindo o método tradicional do fator de redução $Q$ pelo método unificado da largura efetiva $A_{ef}$) e nos parâmetros de resistência à flambagem por cisalhamento de almas não enrijecidas ($k_v = 5{,}34$). Torna-se, portanto, fundamental investigar como a modelagem estrutural das vigas mistas protendidas se comporta frente às prescrições da **ABNT NBR 8800:2024**, tanto no Estado Limite Último quanto nos Estados Limites de Serviço, utilizando uma ferramenta computacional de apoio para realizar verificações precisas ao longo do vão e comparar quantitativamente os resultados obtidos entre as edições de 2008 e de 2024.

## 1.3 Objetivos

### 1.3.1 Objetivo Geral
Desenvolver e validar a modelagem estrutural multifásica para verificação completa nos Estados Limites Último (ELU) e de Serviço (ELS) de vigas mistas de aço e concreto protendidas (pré-tracionadas e pós-tracionadas), e realizar um estudo comparativo aprofundado entre os critérios de dimensionamento da **ABNT NBR 8800:2008** e da **ABNT NBR 8800:2024**, com o auxílio de um programa computacional de verificação.

### 1.3.2 Objetivos Específicos
1. Formular analiticamente o comportamento estrutural de vigas mistas protendidas ao longo de cinco fases do ciclo de vida: Fase 1 (construção / montagem), Fase 2 (serviço imediato em $t = 0$, ELU), Fase 3 (serviço em longa duração em $t = \infty$, ELU), Fase 4 (ELS de deformação excessiva / flecha no meio do vão) e Fase 5 (ELS de vibração do piso sob ações frequentes);
2. Modelar a evolução temporal da força de protensão nos cabos externos considerando as perdas imediatas por acomodação de ancoragem, as perdas progressivas por relaxação do aço e os acréscimos hiperestáticos decorrentes da deformação elástica da viga sob carregamentos externos;
3. Avaliar os Estados Limites Últimos de flexão positiva, flexão negativa (governados pela Flambagem Lateral com Distorção da seção transversal — FLD), esforço cortante e interação flexo-compressão ao longo de seções discretizadas no comprimento do vão;
4. Implementar as formulações completas de ELS segundo Turini (2021) e o Anexo L da ABNT NBR 8800: verificação de regime elástico linear da seção mista, cálculo discriminado de todas as parcelas de flecha imediata e diferida, determinação da contraflecha executiva necessária e análise dinâmica de vibração considerando a largura colaborante especial ($b_{ef} \le L_v/5$) e o acréscimo dinâmico de 35% no módulo do concreto para lajes mistas;
5. Validar a modelagem implementada no programa computacional de apoio mediante confronto com os estudos de caso analíticos de pré-tração e pós-tração apresentados por Turini (2021);
6. Avaliar comparativamente o impacto das atualizações normativas da **ABNT NBR 8800:2024** em relação à **ABNT NBR 8800:2008** sobre a capacidade resistente de perfis compactos e esbeltos e confirmar a conformidade das verificações de serviço entre as duas normas.

## 1.4 Estrutura do Trabalho

O presente trabalho organiza-se em cinco capítulos:
- O **Capítulo 1** contextualiza o sistema de vigas mistas protendidas, fundamenta a justificativa da atualização para a ABNT NBR 8800:2024 e estabelece os objetivos da pesquisa;
- O **Capítulo 2** desenvolve a revisão da literatura e a fundamentação teórica da modelagem estrutural das vigas mistas protendidas, abrangendo a homogeneização elástica, a mecânica da protensão externa, os estados limites últimos (Fases 1, 2 e 3), os estados limites de serviço de deformação e vibração (Fases 4 e 5), e o comparativo teórico entre a ABNT NBR 8800:2008 e a ABNT NBR 8800:2024;
- O **Capítulo 3** expõe a metodologia de análise multifásica ao longo do vão em 5 fases, as combinações de ações últimas e de serviço, a caracterização física e geométrica dos estudos de caso e a apresentação da ferramenta computacional de cálculo;
- O **Capítulo 4** apresenta a validação numérica da modelagem frente à literatura em ELU e ELS e discute os resultados do estudo comparativo entre as edições de 2008 e de 2024 da ABNT NBR 8800;
- O **Capítulo 5** sintetiza as conclusões obtidas e propõe recomendações para investigações futuras.

---

# CAPÍTULO 2 — REVISÃO DA LITERATURA E MODELAGEM ESTRUTURAL DE VIGAS MISTAS PROTENDIDAS

A verificação da segurança estrutural de uma Viga Mista de Aço e Concreto Protendida (VMP) requer a formulação do equilíbrio e da compatibilidade de deformações entre três componentes estruturais: o perfil metálico de seção I, a laje de concreto (maciça ou moldada sobre fôrma de aço incorporada) e os cabos externos de protensão. Este capítulo apresenta a modelagem mecânica e normativa do problema sob a perspectiva da engenharia de estruturas.

---

## 2.1 Comportamento Estrutural Misto e Homogeneização da Seção Transversal

O comportamento conjunto de uma viga mista fundamenta-se na transferência de fluxo de cisalhamento longitudinal na interface entre a mesa superior do perfil metálico e a laje de concreto por meio de conectores mecânicos. Sob interação completa, despreza-se o deslizamento relativo na interface aço-concreto, admitindo-se a validade da hipótese cinemática de Navier-Bernoulli (manutenção das seções planas) na fase elástica e a plastificação integral das seções compactas no Estado Limite Último.

A largura colaborante efetiva da laje de concreto ($b_{ef}$) quantifica a porção do tabuleiro que participa efetivamente da flexão longitudinal, compensando a não uniformidade das tensões normais induzida pelo fenômeno de *shear lag*. Para vigas biapoiadas de vão teórico $L_v$ com vigas adjacentes dispostas a distâncias $s_{esq}$ e $s_{dir}$, a ABNT NBR 8800 prescreve para a flexão estática:

$$b_{ef} = b_{ef,esq} + b_{ef,dir}, \qquad b_{ef,esq} = \min\left(\frac{L_v}{8}, \; \frac{s_{esq}}{2}\right), \quad b_{ef,dir} = \min\left(\frac{L_v}{8}, \; \frac{s_{dir}}{2}\right)$$

Na análise elástica em serviço, a heterogeneidade da seção mista é tratada pelo **Método da Seção Homogeneizada (ou Seção Transformada)**, convertendo-se a área de concreto comprimido em uma área de aço equivalente por meio da relação modular entre os módulos de elasticidade do aço estrutural ($E_a \approx 200\,000\text{ MPa}$) e do concreto ($E_{cs}$):

$$n = \frac{E_a}{\alpha_{fl} E_{cs}}$$

em que o multiplicador de fluência $\alpha_{fl}$ assume valor unitário ($\alpha_{fl} = 1{,}0$) para análises de curta duração ($t = 0$) e valor $\alpha_{fl} = 3{,}0$ para análises de longa duração ($t = \infty$), em atendimento à ABNT NBR 8800 e à ABNT NBR 6118 para reproduzir as deformações diferidas por fluência do concreto sob cargas permanentes.

A largura da laje homogeneizada resulta $b_{tr} = b_{ef} / n$. A posição da Linha Neutra Elástica (LNE), medida a partir da fibra inferior do perfil de aço de altura total $d$, área bruta $A_g$ e centro geométrico em $d/2$, é determinada pela linha baricêntrica da seção composta:

$$y_{tr} = \frac{A_g \frac{d}{2} + b_{tr} t_c \left(d + h_F + \frac{t_c}{2}\right)}{A_g + b_{tr} t_c}$$

sendo $t_c$ a espessura da capa de concreto sobre a fôrma de aço e $h_F$ a altura das nervuras da fôrma ($h_F = 0$ para lajes maciças). A profundidade comprimida efetiva da laje ($a_y$), a área homogeneizada ($A_{tr}$) e o momento de inércia à flexão em torno do eixo de maior inércia ($I_{tr}$) são dados por:

$$a_y = \min\left(t_c, \; d + h_F + t_c - y_{tr}\right)$$

$$A_{tr} = A_g + b_{tr} a_y$$

$$I_{tr} = I_x + A_g \left(y_{tr} - \frac{d}{2}\right)^2 + \frac{b_{tr} a_y^3}{12} + b_{tr} a_y \left(d + h_F + t_c - \frac{a_y}{2} - y_{tr}\right)^2$$

Os módulos de resistência elásticos da seção mista homogeneizada referidos à fibra inferior do perfil de aço ($W_{tr,inf}$) e à fibra superior da laje de concreto ($W_{tr,sup}$) são expressos por:

$$W_{tr,inf} = \frac{I_{tr}}{y_{tr}}, \qquad W_{tr,sup} = \frac{I_{tr}}{d + h_F + t_c - y_{tr}}$$

---

## 2.2 Mecânica da Protensão Externa e Evolução Multifásica

A protensão externa distingue-se pela ausência de aderência contínua entre as armaduras ativas e a matriz de concreto ao longo do vão, ocorrendo o contato mecânico exclusivamente nas ancoragens de extremidade e nos desviadores intermediários. Essa peculiaridade cinemática implica que as deformações no cabo de protensão não dependem da curvatura local de uma seção específica, mas sim da integral das deformações da viga ao longo de todo o comprimento destravado do cabo (NUNES, 2005; RIBEIRO, 2018).

### 2.2.1 Força Máxima Admissível de Protensão Inicial ($P_{0,máx}$)
A força inicial de protensão introduzida pelos macacos hidráulicos ($P_0$) deve ser estritamente limitada para evitar o colapso precoce do perfil metálico isolado na etapa de montagem e impedir tensões de tração ou compressão inaceitáveis na laje de concreto e no perfil durante o serviço. Segundo o roteiro sistematizado por Turini (2021), a força $P_0$ deve respeitar o menor valor entre dois critérios normativos:

1. **Critério da Estabilidade do Perfil Metálico na Construção (ABNT NBR 8800)**:
   Na pré-tração, o perfil I não solidarizado sofre compressão axial combinada com flexão provocada pela excentricidade $e_{p,I}$ do cabo em relação ao centro de gravidade do aço ($e_{p,I} = d/2 + e_p$, com $e_p$ medido em relação à face inferior). A força limite $P_{0,máx,1}$ decorre da equação de interação linear de flexo-compressão:

$$P_{0,máx,1} = \frac{1}{\gamma_P} \frac{1}{\frac{1}{N_{Rd,I}} + \frac{e_{p,I}}{M_{Rd,I}}}$$

   onde $N_{Rd,I}$ e $M_{Rd,I}$ são as resistências de cálculo do perfil metálico isolado à compressão axial e à flexão com flambagem lateral com torção (FLT), e $\gamma_P = 0{,}90$ é o coeficiente de ponderação favorável da protensão quando atua como ação estabilizante ou $\gamma_P = 1{,}10$ quando desfavorável.

2. **Critério das Tensões em Serviço da Seção Mista (ABNT NBR 6118 / ABNT NBR 8800)**:
   Para assegurar a integridade da laje e a reversibilidade de deformações na fibra inferior do aço sob as combinações de serviço, impõe-se a verificação de descompressão ou tração controlada:

$$P_{0,máx,2} = \beta \left[\frac{M_{Sd,g} + \psi_1 M_{Sd,q}}{W_{tr,inf}} - f_{ct,m}\right] \frac{1}{\frac{1}{A_{tr}} + \frac{e_{p,tr}}{W_{tr,inf}}}$$

   com $\beta \approx 1{,}10$ representando o fator de perdas estimadas de protensão, $\psi_1 = 0{,}60$ o fator de combinação frequente, $e_{p,tr} = y_{tr} + e_p$ a excentricidade do cabo em relação à LNE transformada, e $f_{ct,m} = 0{,}30 f_{ck}^{2/3}$ a resistência média à tração do concreto. A força adotada no projeto é $P_0 \le P_{0,máx} = \min(P_{0,máx,1}, \; P_{0,máx,2})$.

### 2.2.2 Perdas Imediatas por Acomodação de Ancoragem ($\Delta P_{anc}$)
Ao término da operação de tensionamento dos cabos retos de comprimento $L_v$ e área de armadura ativa $A_p$, a acomodação e o escorregamento dos dentes das cunhas bipartidas no interior da placa de ancoragem geram um encurtamento longitudinal $\Delta L_{cunha}$ (tipicamente entre $5\text{ mm}$ e $7\text{ mm}$). Pela Lei de Hooke unidimensional, a perda de força correspondente resulta:

$$\Delta P_{anc} = \frac{\Delta L_{cunha} \cdot E_p \cdot A_p}{L_v}$$

sendo $E_p \approx 200\,000\text{ MPa}$ o módulo de elasticidade do aço de protensão. A força efetiva pós-encunhamento é $P_1 = P_0 - \Delta P_{anc}$.

### 2.2.3 Perdas Progressivas por Relaxação do Aço ($\Delta P_{rel}$)
Sob nível permanente de tensão $\sigma_{P0} = P_2 / A_p$ ao longo do tempo, as cordoalhas sofrem deformações plásticas microscópicas que reduzem a tensão no aço. Conforme a ABNT NBR 6118, a perda diferida por relaxação a $t = \infty$ é quantificada por:

$$\Delta P_{rel} = \psi_{1000} \cdot P_2$$

$$\psi_{1000} = \begin{cases} 0{,}015 \cdot \exp\left[6{,}28 \left(\frac{\sigma_{P0}}{f_{ptk}} - 0{,}5\right)\right], & \text{para relaxação baixa (RB)} \\ 0{,}020 \cdot \exp\left[8{,}00 \left(\frac{\sigma_{P0}}{f_{ptk}} - 0{,}5\right)\right], & \text{para relaxação normal (RN)} \end{cases}$$

sendo $f_{ptk}$ a resistência característica à tração do aço de protensão (ex: $1864\text{ MPa}$ para cordoalhas CP-190 RB).

### 2.2.4 Acréscimo Hiperestático de Força no Cabo ($\Delta P$)
Sob atuação de carregamento uniformemente distribuído $q$ aplicado após a ancoragem do cabo externo reto de excentricidade constante $e_p$, a flexão da viga induz um alongamento elástico relativo entre as ancoragens de extremidade. Pela compatibilidade cinemática global da viga biapoiada, o acréscimo hiperestático de força de tração no cabo ($\Delta P$) é expresso analiticamente por (TURINI, 2021):

$$\Delta P = \frac{\frac{q \cdot L_v^3 \cdot e_p}{12 \cdot E_g \cdot I_x}}{\frac{L_v}{E_p \cdot A_p} + \frac{L_v}{E_g \cdot A_g} + \frac{e_p^2 \cdot L_v}{E_g \cdot I_x}}$$

em que $E_g$, $A_g$ e $I_x$ representam, respectivamente, o módulo de elasticidade, a área e o momento de inércia da seção resistente atuante no instante da aplicação da carga $q$ (o perfil isolado para cargas da Fase 1 na pré-tração, ou a seção mista homogeneizada $E_a, A_{tr}, I_{tr}$ para cargas da Fase 2 e na pós-tração).

---

## 2.3 Estados Limites Últimos do Perfil Metálico Isolado na Construção

Na Fase 1 de estruturas pré-tracionadas com construção não escorada, o perfil metálico de seção I suporta isoladamente o peso próprio, o peso do concreto fresco e a sobrecarga de construção, estando submetido simultaneamente a esforço axial de compressão e a momento fletor negativo induzido pela protensão. As verificações do ELU do perfil de aço compreendem:

1. **Força Axial de Compressão Resistente ($N_{Rd,I}$)**:
   A capacidade resistente à compressão axial considera a instabilidade global por flexão associada à esbeltez reduzida $\lambda_0 = \sqrt{Q A_g f_y / N_e}$ (segundo a ABNT NBR 8800:2008) ou $\lambda_0 = \sqrt{A_g f_y / N_e}$ sob seção efetiva $A_{ef}$ (segundo a ABNT NBR 8800:2024), aplicando-se o fator de redução $\chi$:

$$N_{Rd,I} = \frac{\chi Q A_g f_y}{\gamma_{a1}} \quad \text{(2008)} \qquad \text{ou} \qquad N_{Rd,I} = \frac{\chi A_{ef} f_y}{\gamma_{a1}} \quad \text{(2024)}$$

   sendo $\gamma_{a1} = 1{,}10$ e $N_e = \pi^2 E_a I_y / (K_y L_b)^2$ a força crítica de Euler no eixo de menor inércia, travado lateralmente a cada distância $L_b$.

2. **Momento Fletor Resistente do Perfil ($M_{Rd,I}$)**:
   Avaliado frente aos estados limites de Flambagem Lateral com Torção (FLT), Flambagem Local da Mesa (FLM) e Flambagem Local da Alma (FLA). Para perfis compactos duplamente simétricos:

$$M_{Rd,I} = \begin{cases} \frac{M_{pl}}{\gamma_{a1}} = \frac{Z_x f_y}{\gamma_{a1}}, & \text{se } L_b \le L_p \\ \frac{C_b}{\gamma_{a1}} \left[M_{pl} - (M_{pl} - M_r)\left(\frac{L_b - L_p}{L_r - L_p}\right)\right] \le \frac{M_{pl}}{\gamma_{a1}}, & \text{se } L_p < L_b \le L_r \\ \frac{M_{cr}}{\gamma_{a1}} \le \frac{M_{pl}}{\gamma_{a1}}, & \text{se } L_b > L_r \end{cases}$$

3. **Esforço Cortante Resistente ($V_{Rd}$)**:
   Para almas sem enrijecedores transversais submetidas ao cisalhamento puro:

$$V_{Rd} = \frac{V_{pl}}{\gamma_{a1}} = \frac{0{,}60 A_w f_y}{\gamma_{a1}} \quad \left(\text{se } \frac{h}{t_w} \le 1{,}10 \sqrt{\frac{k_v E_a}{f_y}}\right)$$

   sendo $A_w = d \cdot t_w$ a área da alma e $k_v$ o coeficiente de estabilidade ao cisalhamento ($k_v = 5{,}0$ na edição de 2008 e $k_v = 5{,}34$ na edição de 2024).

4. **Interação Flexo-Compressão (Item 5.5.1.2 da ABNT NBR 8800)**:
   Sob ação combinada de $N_{Sd,1} = 0{,}9 P_1$ e momento solicitante de cálculo $M_{Sd,1}$, a segurança do perfil é assegurada pelo índice de aproveitamento $I_a \le 1{,}00$:

$$I_a = \begin{cases} \frac{N_{Sd}}{N_{Rd}} + \frac{8}{9} \frac{M_{Sd}}{M_{Rd}} \le 1{,}00, & \text{para } \frac{N_{Sd}}{N_{Rd}} \ge 0{,}20 \\ \frac{N_{Sd}}{2 N_{Rd}} + \frac{M_{Sd}}{M_{Rd}} \le 1{,}00, & \text{para } \frac{N_{Sd}}{N_{Rd}} < 0{,}20 \end{cases}$$

---

## 2.4 Estados Limites Últimos da Seção Mista Protendida em Serviço

Na seção mista consolidada (Fases 2 e 3), a verificação no ELU envolve a determinação da tensão última de cálculo no cabo de protensão e a resistência à flexão positiva e negativa.

### 2.4.1 Tensão de Cálculo no Cabo no ELU ($\sigma_{pd}$)
Em cabos externos não aderentes, a tensão de cálculo no ELU não atinge necessariamente a plastificação integral ($f_{pyd} = f_{pyk} / 1{,}15$), dependendo do nível de pré-alongamento e da ductilidade da viga. Conforme a Rotina H sistematizada por Turini (2021) e fundamentada em Harajli (2006):

$$\sigma_{pd} = \sigma_{pe} + \Delta\sigma_p \le f_{pyd}$$

$$\sigma_{pe} = \frac{P_{atuante}}{A_p}, \qquad \Delta\sigma_p = \min\left(10000 \cdot \rho_p \left[1 - 3 \frac{y_{tr}}{L_v}\right], \; 0{,}30 f_{ptk}\right)$$

em que $\rho_p = A_p / A_{tr}$ é a taxa geométrica de armadura ativa.

### 2.4.2 Momento Fletor Resistente Positivo ($M_{Rd,VMP}^+$)
Para seções compactas sob interação completa, o momento resistente último à flexão positiva é determinado pelo modelo de blocos plásticos retangulares de tensões de plastificação:
- Compressão máxima resistente da laje de concreto: $C_{c,máx} = 0{,}85 \frac{f_{ck}}{\gamma_c} b_{ef} t_c$ (com $\gamma_c = 1{,}40$);
- Força máxima de tração do perfil metálico: $T_a = \frac{A_g f_y}{\gamma_{a1}}$;
- Força de tração do cabo de protensão no ELU: $T_p = A_p \sigma_{pd}$.

Quando $C_{c,máx} \ge T_a + T_p$, a Linha Neutra Plástica (LNP) situa-se integralmente no interior da capa de concreto da laje, a uma profundidade comprimida $a$:

$$a = \frac{T_a + T_p}{0{,}85 \frac{f_{ck}}{\gamma_c} b_{ef}} \le t_c$$

O momento resistente plástico $M_{Rd,VMP}^+$ é calculado tomando-se momentos estáticos em relação ao centro de gravidade do perfil de aço ($d/2$):

$$M_{Rd,VMP}^+ = C_{c,máx} \left(d + h_F + t_c - \frac{a}{2} - \frac{d}{2}\right) + T_p \cdot e_{p,I}$$

### 2.4.3 Momento Fletor Resistente Negativo e Flambagem Lateral com Distorção ($M_{Rd,VMP}^-$)
Sob efeito predominante da protensão sem carregamento gravitacional suficiente para anular o momento fletor negativo, a mesa inferior do perfil metálico torna-se comprimida. Como a mesa superior está vinculada à laje de concreto pelos conectores, a instabilidade ocorre sob a forma de **Flambagem Lateral com Distorção (FLD)**, na qual a alma sofre distorção transversal e a mesa inferior comprime-se e desloca-se lateralmente (Item L.6 da ABNT NBR 8800).

O momento resistente à FLD é avaliado pelo modelo analítico de rigidez rotacional $k_r$ formada pela rigidez à flexão da laje ($k_1$) associada à rigidez à distorção da alma do perfil ($k_2$):

$$\frac{1}{k_r} = \frac{1}{k_1} + \frac{1}{k_2} \implies k_r = \frac{k_1 k_2}{k_1 + k_2}$$

$$k_1 = \frac{4 E_a I_{laje}}{a_{vigas}}, \qquad k_2 = \frac{E_a t_w^3}{4 (1 - \nu^2) h_w}$$

sendo $I_{laje}$ o momento de inércia da laje fissurada por unidade de comprimento e $a_{vigas}$ o espaçamento transversal entre vigas. O momento crítico elástico de distorção ($M_{cr,dist}$) e a esbeltez reduzida $\lambda_{dist} = \sqrt{M_{pl,neg} / M_{cr,dist}}$ fornecem o fator de redução por instabilidade distorcional $\chi_{dist}$, resultando na resistência de cálculo:

$$M_{Rd,VMP}^- = \min\left(M_{Rd,neg}^-, \; \chi_{dist} \cdot M_{Rd,neg}^-\right)$$

---

## 2.5 Estados Limites de Serviço (ELS) em Vigas Mistas Protendidas

Embora a segurança estrutural última deva ser plenamente garantida no ELU, o projeto de vigas mistas protendidas de grandes vãos é frequentemente condicionado pelos **Estados Limites de Serviço (ELS)**. O comportamento sob carregamentos de serviço divide-se em duas exigências fundamentais estabelecidas pela ABNT NBR 8800 e detalhadas por Turini (2021): a verificação de deformação excessiva (Fase 4) e a análise dinâmica de vibração no piso (Fase 5).

### 2.5.1 Verificação de Regime Elástico Linear sob Ações de Serviço
As teorias de cálculo de deslocamentos por homogeneização elástica pressupõem que o perfil de aço e a laje de concreto operem estritamente no regime elástico linear sob as ações frequentes e quase permanentes de serviço. Conforme Turini (2021, p. 153), a confirmação do regime elástico é realizada verificando-se a tensão normal máxima na fibra inferior do perfil metálico ($\sigma_{inf}$), que congrega a parcela atuante na montagem e a parcela correspondente à seção mista em serviço:

$$\sigma_{inf} = \frac{M_{Ga,Sd}}{W_a} + \frac{M_{L,Sd}}{W_{ef}} \le \frac{f_y}{\gamma_{a1}} \quad \left(\text{ou } f_y\right)$$

em que:
- $M_{Ga,Sd}$: Momento solicitante no perfil isolado na montagem sob combinação de serviço. Para construção não escorada com pré-tração, $M_{Ga,Sd} = \frac{(q_1 + q_2) L^2}{8} - e_{p,I} P_1$; para construção não escorada com pós-tração, $M_{Ga,Sd} = \frac{(q_1 + q_2) L^2}{8}$;
- $M_{L,Sd}$: Momento solicitante atuante na seção mista sob as ações remanescentes de serviço, descontando a parcela de protensão em longa duração: $M_{L,Sd} = \frac{(q_3 + q_4) L^2}{8} - e_{p,tr,\infty} P_3$;
- $W_a = W_{x,perfil}$: Módulo de resistência elástico da seção metálica isolada ($W_a = I_x / [d/2]$);
- $W_{ef} = W_{tr,inf,\infty}$: Módulo de resistência elástico homogeneizado da seção mista em longa duração ($W_{ef} = I_{tr,\infty} / y_{tr,\infty}$).

Atendida a condição $\sigma_{inf} \le f_y$, confirma-se que a mesa inferior do perfil de aço não sofre escoamento sob cargas de serviço, validando o emprego da teoria da elasticidade linear para a determinação das flechas.

### 2.5.2 ELS de Deformação Excessiva e Determinação da Contraflecha Executiva (Fase 4)
Para vigas biapoiadas de vão $L$, a flecha no meio do vão devida a um carregamento genérico uniformemente distribuído $q$ atuando sobre uma seção de rigidez elástica $E \cdot I$ é calculada pela expressão clássica da teoria das vigas:

$$\delta(q, E, I) = \frac{5}{384} \frac{q \cdot L^4}{E \cdot I}$$

De modo análogo, o momento fletor negativo uniforme $M_p = e_p \cdot P$ introduzido pelo traçado reto do cabo de protensão sob força normal $P$ gera uma contraflecha elástica no meio do vão expressa por:

$$\delta_{protensao} = -0{,}125 \frac{e_p \cdot P \cdot L^2}{E \cdot I}$$

Em uma viga mista protendida, cada ação atua sobre uma configuração resistente e temporal específica:
1. **Flecha do peso próprio do perfil metálico ($\delta_{PP}$)**: Atua sobre o perfil isolado ($E_a \cdot I_x$):
   $$\delta_{PP} = \frac{5}{384} \frac{q_1 L^4}{E_a I_x}$$
2. **Flecha do concreto fresco ($\delta_{conc}$)**: Em sistemas não escorados, atua sobre o perfil isolado ($E_a \cdot I_x$). Em sistemas escorados, atua sobre a seção mista de longa duração ($E_a \cdot I_{tr,\infty}$):
   $$\delta_{conc} = \begin{cases} \frac{5}{384} \frac{q_2 L^4}{E_a I_x}, & \text{construção não escorada} \\ \frac{5}{384} \frac{q_2 L^4}{E_a I_{tr,\infty}}, & \text{construção escorada} \end{cases}$$
3. **Flecha da carga permanente de revestimento ($\delta_{CPN}$)**: Atua sobre a seção mista de longa duração ($E_a \cdot I_{tr,\infty}$, com relação modular $3n$):
   $$\delta_{CPN} = \frac{5}{384} \frac{q_3 L^4}{E_a I_{tr,\infty}}$$
4. **Flecha da sobrecarga de curta duração ($\delta_{CVN,curta}$)**: Representa a parcela variável instantânea $(1 - \psi_2) q_4$ que deforma a seção mista no tempo $t = 0$ ($E_a \cdot I_{tr,0}$, com relação modular $n$):
   $$\delta_{CVN,curta} = \frac{5}{384} \frac{(1 - \psi_2) q_4 L^4}{E_a I_{tr,0}}$$
5. **Flecha da sobrecarga de longa duração ($\delta_{CVN,longa}$)**: Parcela quase permanente $\psi_2 q_4$ atuando sobre a seção de longa duração ($E_a \cdot I_{tr,\infty}$):
   $$\delta_{CVN,longa} = \frac{5}{384} \frac{\psi_2 q_4 L^4}{E_a I_{tr,\infty}}$$
6. **Contraflecha devida à protensão ($\delta_{protensao}$)**:
   - Na pré-tração: foi introduzida no perfil metálico isolado na montagem com força $P_1$ e excentricidade $e_{p,I}$:
     $$\delta_{protensao} = -0{,}125 \frac{e_{p,I} \cdot P_1 \cdot L^2}{E_a I_x}$$
   - Na pós-tração: atua sobre a seção mista de longa duração com força final $P_3$ e excentricidade $e_{p,tr,\infty}$:
     $$\delta_{protensao} = -0{,}125 \frac{e_{p,tr,\infty} \cdot P_3 \cdot L^2}{E_a I_{tr,\infty}}$$
7. **Contraflecha Executiva ($\delta_{contraflecha}$)**: Para anular as deformações permanentes visíveis do piso, prescreve-se a execução de contraflecha na fabricação da viga de aço:
   $$\delta_{perm} = \delta_{PP} + \delta_{conc} + \delta_{CPN}$$
   $$\delta_{sem\_cf} = \delta_{perm} + \delta_{CVN,curta} + \delta_{CVN,longa} + \delta_{protensao}$$
   A contraflecha teórica ideal $\delta_{cf} = -\delta_{perm}$ deve ser limitada para não induzir flechas finais negativas (deslocamentos para cima) sob carga total. Caso $\delta_{perm} > \delta_{sem\_cf}$ com $\delta_{sem\_cf} > 0$, adota-se $\delta_{contraflecha} = -\delta_{sem\_cf}$, resultando em flecha final nula ($\delta_{total} = 0{,}00$). Em caso contrário:
   $$\delta_{contraflecha} = -\delta_{perm}, \qquad \delta_{total} = \delta_{sem\_cf} - \delta_{perm}$$

A flecha resultante final em serviço deve atender ao limite normativo da Tabela C.1 da ABNT NBR 8800:
$$\delta_{total} \le \delta_{lim} = \frac{L}{350}$$

### 2.5.3 ELS de Vibração no Piso segundo a ABNT NBR 8800 (Anexo L) e Turini (2021)
O dimensionamento de vigas de grandes vãos para pisos de edifícios comerciais e residenciais frequentemente esbarra no desconforto de usuários gerado por vibrações causadas pelo caminhar humano. O Anexo L da ABNT NBR 8800:2008 (mantido na íntegra na ABNT NBR 8800:2024) permite a aplicação de um procedimento simplificado baseado em limites de deslocamento vertical dinâmico sob as seguintes premissas (TURINI, 2021, p. 154-155):
1. **Combinação frequente de ações de serviço**: $q = q_1 + q_2 + q_3 + \psi_1 q_4$, excluindo parcelas diferidas no tempo (sem fluência, $t = 0$);
2. **Largura colaborante para vibração ($b_{ef,vib}$)**: Sob solicitações dinâmicas de piso, a propagação transversal da onda elástica engloba uma largura de laje superior à da flexão estática, adotando-se o limite de $L_v / 5$ para cada semi-largura (em vez de $L_v / 8$):
   $$b_{ef,esq} \le \min\left(\frac{L_v}{5}, \; \frac{s_{esq}}{2}\right), \qquad b_{ef,dir} \le \min\left(\frac{L_v}{5}, \; \frac{s_{dir}}{2}\right)$$
   $$b_{ef,vib} = b_{ef,esq} + b_{ef,dir}$$
3. **Efeito dinâmico em lajes mistas com fôrma de aço (*steel deck*)**: Devido à rigidez de diafragma adicional da chapa nervurada e à rápida solicitação dinâmica cíclica, prescreve-se um **acréscimo de 35% no módulo de elasticidade do concreto**:
   $$E_{cs,vib} = 1{,}35 \cdot E_{cs} \quad \text{(se laje com fôrma de aço); ou } E_{cs,vib} = E_{cs} \quad \text{(se laje maciça)}$$
4. **Rigidez dinâmica homogeneizada ($I_{tr,vib}$)**: A seção transformada para vibração é calculada com a largura ampliada $b_{ef,vib}$ e o módulo majorado $E_{cs,vib}$ em tempo imediato ($n_{vib} = E_a / E_{cs,vib}$), obtendo-se o momento de inércia $I_{tr,vib}$ e a excentricidade dinâmica do cabo $e_{p,tr,vib} = y_{tr,vib} + e_p$;
5. **Parcelas dinâmicas de deslocamento vertical**: Como o piso responde monoliticamente às vibrações dinâmicas, todas as cargas gravitacionais e a protensão atuante em serviço ($P_2$) são aplicadas sobre a inércia da seção mista dinâmica $I_{tr,vib}$:
   $$\delta_{PP} = \frac{5}{384} \frac{q_1 L^4}{E_a I_{tr,vib}}, \qquad \delta_{conc} = \frac{5}{384} \frac{q_2 L^4}{E_a I_{tr,vib}}$$
   $$\delta_{CPN} = \frac{5}{384} \frac{q_3 L^4}{E_a I_{tr,vib}}, \qquad \delta_{CVN} = \frac{5}{384} \frac{\psi_1 q_4 L^4}{E_a I_{tr,vib}}$$
   $$\delta_{protensao} = -0{,}125 \frac{e_{p,tr,vib} \cdot P_2 \cdot L^2}{E_a I_{tr,vib}}$$
6. **Deslocamento vertical dinâmico total**: Na verificação de vibração, **não é permitido o desconto de contraflecha executiva**, pois a contraflecha é uma condição geométrica estática prévia que não altera as frequências naturais nem amortece os deslocamentos alternados dinâmicos (TURINI, 2021, p. 155):
   $$\delta_{total,vib} = \delta_{PP} + \delta_{conc} + \delta_{CPN} + \delta_{CVN} + \delta_{protensao}$$

Para edifícios de uso residencial ou comercial submetidos a pessoas caminhando, a ABNT NBR 8800 estabelece o limite máximo de flecha dinâmica:
$$\delta_{total,vib} \le \delta_{lim,vib} = 20\text{ mm} = 2{,}00\text{ cm}$$

---

## 2.6 Evolução Normativa: Da ABNT NBR 8800:2008 à ABNT NBR 8800:2024

A revisão da norma brasileira **ABNT NBR 8800:2024** incorporou avanços teóricos e experimentais consolidados internacionalmente no dimensionamento de estruturas metálicas e mistas. No âmbito das vigas mistas protendidas, destacam-se duas modificações capitais no Estado Limite Último, acompanhadas da manutenção das diretrizes de serviço:

1. **Método da Largura Efetiva Unificada ($A_{ef}$) para Flambagem Local na Compressão**:
   A edição de 2008 penalizava a flambagem local de elementos esbeltos comprimidos por meio do fator $Q = Q_s \cdot Q_a$, que reduzia diretamente a tensão de escoamento efetiva no índice de esbeltez global ($\lambda_0 = \sqrt{Q A_g f_y / N_e}$). A **ABNT NBR 8800:2024** substituiu esse procedimento pelo **Método da Largura Efetiva de Winter**: o índice de esbeltez reduzido global $\lambda_0 = \sqrt{A_g f_y / N_e}$ e a tensão crítica global $f_n = \chi f_y$ são calculados sempre com a área bruta $A_g$. Cada elemento componente (mesa ou alma) tem seu limite de esbeltez corrigido pelo nível de tensão atuante ($\lambda_{r,ef} = \lambda_r \sqrt{f_y / f_n}$). Se a esbeltez do elemento superar $\lambda_{r,ef}$, sua largura efetiva $b_{ef}$ é determinada pela curva de resistência pós-flambagem:

$$b_{ef} = b \left(1 - c_1 \sqrt{\frac{f_{el}}{f_n}}\right) \sqrt{\frac{f_{el}}{f_n}} \le b, \qquad f_{el} = \left(c_2 \frac{\lambda_r}{\lambda}\right)^2 f_y$$

   com constantes $c_1 = 0{,}22$ e $c_2 = 1{,}49$ para mesas de perfis I (elementos AL) e $c_1 = 0{,}18$ e $c_2 = 1{,}31$ para almas (elementos AA), resultando na resistência axial $N_{Rd,I} = \chi A_{ef} f_y / \gamma_{a1}$;

2. **Coeficiente de Flambagem por Cisalhamento de Almas Não Enrijecidas ($k_v$)**:
   Na verificação ao esforço cortante de almas sem enrijecedores transversais, a edição de 2008 adotava de forma aproximada $k_v = 5{,}0$. A **ABNT NBR 8800:2024** adota o valor teórico exato de estabilidade elástica de placas longas sob cisalhamento puro, **$k_v = 5{,}34$**, ampliando os limites de esbeltez plástico ($\lambda_{p,v}$) e elástico ($\lambda_{r,v}$) da alma em $3{,}34\%$ e elevando em $6{,}80\%$ a resistência ao cortante no regime de flambagem elástica;

3. **Preservação Integral dos Critérios em ELS**:
   Tanto a verificação de deformações verticais em serviço ($\delta_{lim} = L/350$) quanto o procedimento simplificado de vibração em pisos do Anexo L foram mantidos sem alterações conceituais na ABNT NBR 8800:2024, assegurando compatibilidade entre as análises de serviço executadas sob ambas as edições da norma.

**Tabela 2.1 — Quadro comparativo das prescrições da ABNT NBR 8800:2008 e da ABNT NBR 8800:2024 aplicáveis às VMP**

| Critério Normativo | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Efeito sobre o Projeto de VMP |
| :--- | :--- | :--- | :--- |
| **Flambagem Local sob Compressão Axial** | Fator $Q = Q_s Q_a$ aplicado sobre $\lambda_0 = \sqrt{Q A_g f_y / N_e}$; $N_{Rd} = \chi Q A_g f_y / \gamma_{a1}$. | Esbeltez global com área bruta $\lambda_0 = \sqrt{A_g f_y / N_e}$ e seção efetiva $A_{ef}$ avaliada na tensão $f_n = \chi f_y$. | Valores idênticos de $N_{Rd,I}$ para perfis laminados compactos ($A_{ef} = A_g$); ganho de capacidade para perfis soldados esbeltos na Fase 1. |
| **Cisalhamento em Almas sem Enrijecedores** | Coeficiente $k_v = 5{,}0$; $\lambda_{p,v} = 1{,}10\sqrt{5{,}0 E_a / f_y}$. | Coeficiente $k_v = 5{,}34$; $\lambda_{p,v} = 1{,}10\sqrt{5{,}34 E_a / f_y}$. | Aumento de $3{,}34\%$ nos limites $\lambda_{p,v}$ e $\lambda_{r,v}$ e de $6{,}80\%$ na resistência elástica ao cortante. |
| **Flexão do Perfil Metálico (FLT, FLM, FLA)** | Momento plástico $M_{pl} = \min(Z_x f_y, 1{,}5 W_x f_y)$ e transição linear com $M_r = 0{,}70 W_x f_y$. | Manutenção das curvas de resistência para perfis I duplamente simétricos compactos e semicompactos. | Manutenção de $M_{Rd,I}$ e do limite de protensão $P_{0,máx,1}$ em perfis laminados Série W. |
| **Resistência Última da Seção Mista (ELU)** | Modelo rígido-plástico ($\lambda_w \le 3{,}76\sqrt{E_a/f_y}$) e verificação de FLD pelo modelo de rigidez rotacional $k_r$. | Preservação do equilíbrio de blocos plásticos e do modelo de rigidez rotacional $k_r$ na FLD. | Compatibilidade integral dos momentos resistentes $M_{Rd,VMP}^+$ e $M_{Rd,VMP}^-$. |
| **Estados Limites de Serviço (ELS)** | Flecha limite $L/350$ e análise simplificada de vibração com limite de $20\text{ mm}$ no Anexo L. | Preservação idêntica dos limites de flecha e das diretrizes dinâmicas de conforto do Anexo L. | Idêntico desempenho em serviço sob carregamento quase permanente e frequente. |

---

# CAPÍTULO 3 — METODOLOGIA DE ANÁLISE E ROTEIRO DE VERIFICAÇÃO MULTIFÁSICA

Este capítulo detalha a metodologia de verificação multifásica implementada para cobrir integralmente a vida útil da viga mista protendida, compreendendo três fases de Estado Limite Último (ELU) e duas fases de Estado Limite de Serviço (ELS), acompanhada da caracterização dos estudos de caso analisados e da ferramenta computacional de cálculo.

---

## 3.1 Roteiro de Verificação Multifásica e Combinações de Ações

Para avaliar as condições de segurança ao longo de toda a viga biapoiada de vão $L$, o domínio longitudinal $[0, L]$ é discretizado em $X$ seções transversais ($x_0 = 0, x_1, \dots, x_{X-1} = L$), incluindo os apoios e a seção do meio do vão ($x = L/2$). O processo de cálculo estrutura-se no encadeamento de cinco fases cronológicas:

### 3.1.1 Fase 1 — Etapa Construtiva e Montagem (ELU 1)
Atuam o peso próprio do perfil ($q_1$), o peso do concreto fresco ($q_2$) e a sobrecarga de construção ($q_s$). Aplica-se a combinação última especial de construção da ABNT NBR 8800:
$$q_{Sd,1} = 1{,}3 (q_1 + q_2) + 1{,}2 q_s$$
- **Pré-tração**: A força atuante no cabo é $P_1 = P_0 - \Delta P_{anc} + \Delta P(q_2) + \Delta P(q_s)$. Verifica-se o perfil de aço isolado sob momento positivo $M_{Sd,1}^+(x) = \frac{1}{2} q_{Sd,1}(Lx - x^2) - 0{,}9 P_1 e_{p,I}$, momento negativo $M_{Sd,1}^-(x) = 0{,}9 P_1 e_{p,I}$, cortante $V_{Sd,1} = q_{Sd,1} L / 2 \le V_{Rd}$ e interação flexo-compressão sob $N_{Sd,1} = 0{,}9 P_1$;
- **Pós-tração**: Os cabos não estão tracionados ($P_1 = 0$). Verifica-se a viga mista sem protensão para $M_{Sd,1}^+(x) \le M_{Rd,VM}^+$ e $V_{Sd,1} \le V_{Rd}$.

### 3.1.2 Fase 2 — Serviço em Curta Duração ($t = 0$, ELU 2)
Com a seção mista consolidada sob todas as ações permanentes ($q_g = q_1 + q_2 + q_3$) e a sobrecarga de utilização ($q_4$), adota-se a combinação última normal:
$$q_{Sd,2} = 1{,}4 (q_1 + q_2 + q_3) + 1{,}4 q_4$$
A força no cabo $P_2$ incorpora os incrementos hiperestáticos de curta duração ($\Delta P(q_3) + \Delta P(q_4)$) calculados na seção mista com $n = E_a/E_{cs}$. Verificam-se: $M_{Sd,2}^+(x) \le M_{Rd,VMP,0}^+(x)$, $M_{Sd,2}^-(x) = 0{,}9 P_2 e_{p,tr,0} \le M_{Rd,VMP,0}^-(x)$ (FLD) e $V_{Sd,2} \le V_{Rd}$.

### 3.1.3 Fase 3 — Serviço em Longa Duração ($t = \infty$, ELU 3)
Mantendo o carregamento $q_{Sd,2}$, considera-se a perda por relaxação do cabo ($P_3 = P_2 - \Delta P_{rel}$) e a fluência do concreto com $3n$. Reavaliam-se $\sigma_{pd,\infty}$, $M_{Rd,VMP,\infty}^+$ e $M_{Rd,VMP,\infty}^-$, garantindo que a seção suporte os esforços últimos diferidos no tempo.

### 3.1.4 Fase 4 — ELS de Deformação Excessiva (Flechas e Contraflecha)
Conforme a metodologia de Turini (2021):
1. **Regime Elástico Linear**: Verifica-se se $\sigma_{inf} = \frac{M_{Ga,Sd}}{W_a} + \frac{M_{L,Sd}}{W_{ef}} \le f_y$, confirmando a ausência de escoamento na fibra inferior do perfil metálico;
2. **Flecha no Meio do Vão**: Calculam-se separadamente $\delta_{PP}$, $\delta_{conc}$, $\delta_{CPN}$, $\delta_{CVN,curta}$, $\delta_{CVN,longa}$ e a contraflecha de protensão $\delta_{protensao}$;
3. **Contraflecha Executiva**: Determina-se $\delta_{contraflecha} \le 0$ para neutralizar o deslocamento estático permanente, obtendo-se a flecha final $\delta_{total}$;
4. **Verificação de Limite**: Comprova-se que $\delta_{total} \le \delta_{lim} = L / 350$.

### 3.1.5 Fase 5 — ELS de Vibração no Piso (Conforto Humano)
Avaliada sob a combinação frequente dinâmica ($q_1 + q_2 + q_3 + \psi_1 q_4$) no tempo $t = 0$:
1. Adota-se a largura colaborante dinâmica $b_{ef,vib} \le L_v / 5$;
2. Aplica-se o acréscimo dinâmico de 35% no módulo de elasticidade do concreto para lajes com fôrma de aço ($E_{cs,vib} = 1{,}35 E_{cs}$);
3. Calcula-se a rigidez dinâmica homogeneizada $I_{tr,vib}$ e aplicam-se todas as ações sobre o sistema monolítico;
4. Obtém-se o deslocamento dinâmico $\delta_{total,vib}$ (sem contraflecha executiva) e verifica-se se $\delta_{total,vib} \le 20\text{ mm} = 2{,}00\text{ cm}$.

---

## 3.2 Caracterização dos Estudos de Caso e Ferramenta Computacional de Apoio

A Tabela 3.1 sintetiza os parâmetros geométricos, mecânicos e de carregamento dos Estudos de Caso 1 e 2 retirados de Turini (2021).

**Tabela 3.1 — Parâmetros de projeto e carregamentos dos Estudos de Caso 1 e 2**

| Parâmetro de Projeto | Símbolo | Unidade | Estudo de Caso 1 (Pré-Tração) | Estudo de Caso 2 (Pós-Tração) |
| :--- | :---: | :---: | :---: | :---: |
| Processo executivo de protensão | — | — | Pré-tração (no perfil de aço) | Pós-tração (na seção mista) |
| Vão teórico biapoiado | $L$ | $\text{mm}$ | $12\,000$ | $15\,000$ |
| Comprimento destravado na construção | $L_b$ | $\text{mm}$ | $3\,000$ | $3\,000$ |
| Espaçamento transversal entre vigas | $a_{vigas}$ | $\text{mm}$ | $3\,000$ | $2\,000$ |
| Perfil metálico laminado (ASTM A572 Gr. 50) | — | — | W 360 x 57,8 | W 460 x 89 |
| Resistência ao escoamento e módulo do aço | $f_y \; / \; E_a$ | $\text{MPa}$ | $345 \; / \; 200\,000$ | $345 \; / \; 200\,000$ |
| Área bruta e inércia principal do perfil I | $A_g \; / \; I_x$ | $\text{cm}^2 \; / \; \text{cm}^4$ | $72{,}50 \; / \; 16\,143$ | $114{,}10 \; / \; 41\,105$ |
| Tipologia da laje de concreto (Classe C30) | — | — | Maciça ($h_F = 0$) | Com fôrma de aço ($h_F = 50\text{ mm}$) |
| Espessura efetiva e largura colaborante | $t_c \; / \; b_{ef}$ | $\text{mm}$ | $130 \; / \; 3\,000$ | $130 \; / \; 2\,000$ |
| Resistência característica e módulo secante | $f_{ck} \; / \; E_{cs}$ | $\text{MPa}$ | $30 \; / \; 26\,838$ | $30 \; / \; 26\,838$ |
| Cordoalhas de protensão CP-190 RB ($\varnothing 15{,}2\text{ mm}$) | $f_{ptk} \; / \; A_{unit}$ | $\text{MPa} \; / \; \text{mm}^2$ | $1\,864 \; / \; 140{,}0$ | $1\,864 \; / \; 140{,}0$ |
| Posição vertical do cabo e acomodação de cunha | $e_p \; / \; \Delta L_{cunha}$ | $\text{mm}$ | $-50{,}0 \; / \; 6{,}0$ | $-50{,}0 \; / \; 6{,}0$ |
| Força inicial de protensão adotada | $P_0$ | $\text{kN}$ | $400{,}00$ | $P_{0,máx}$ ($1\,165{,}31$) |
| Peso próprio do perfil e do concreto fresco | $q_1 \; / \; q_2$ | $\text{kN/m}$ | $0{,}56 \; / \; 9{,}75$ | $0{,}88 \; / \; 6{,}81$ |
| Sobrecarga transitória de construção | $q_s$ | $\text{kN/m}$ | $3{,}00$ | $2{,}00$ |
| Carga permanente complementar e sobrecarga | $q_3 \; / \; q_4$ | $\text{kN/m}$ | $6{,}00 \; / \; 15{,}00$ | $4{,}00 \; / \; 10{,}00$ |
| Fatores de combinação de serviço | $\psi_1 \; / \; \psi_2$ | — | $0{,}60 \; / \; 0{,}40$ | $0{,}60 \; / \; 0{,}60$ |

Para processar a metodologia sem erros de propagação numérica e comparar instantaneamente os critérios das duas normas, desenvolveu-se o programa computacional `VMP` em linguagem C++20 com interface gráfica interativa (utilizando *Dear ImGui* e *Raylib*). O programa executa o ciclo completo de 5 fases, desenha a seção transversal mista em escala real e exibe diagramas de esforços e tabelas de verificação detalhadas.

---

# CAPÍTULO 4 — VALIDAÇÃO DO MODELO E ESTUDO COMPARATIVO (ABNT NBR 8800:2008 vs. 2024)

Este capítulo apresenta os resultados numéricos obtidos pela aplicação da modelagem multifásica aos Estudos de Caso 1 e 2, validando os cálculos de ELU e ELS frente aos valores analíticos de Turini (2021) e avaliando o comportamento sob a **ABNT NBR 8800:2024**.

---

## 4.1 Validação da Modelagem — Estudo de Caso 1 (Viga Mista Pré-Tracionada)

As Tabelas 4.1, 4.2 e 4.3 confrontam os resultados do modelo proposto com os valores de referência de Turini (2021) para as propriedades geométricas, os Estados Limites Últimos (Fases 1, 2 e 3) e os Estados Limites de Serviço (Fases 4 e 5) do Estudo de Caso 1.

**Tabela 4.1 — Validação das propriedades homogeneizadas e resistências do perfil metálico (Estudo de Caso 1)**

| Grandeza Estrutural | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio Relativo (%) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| Coeficiente de homogeneização ($t = \infty$) | $n_{\infty}$ | — | $0{,}04473$ | $0{,}04473$ | $0{,}00\%$ |
| Ordenada da LNE em curta duração ($t = 0$) | $y_{tr,0}$ | $\text{mm}$ | $393{,}30$ | $393{,}30$ | $0{,}00\%$ |
| Ordenada da LNE em longa duração ($t = \infty$) | $y_{tr,\infty}$ | $\text{mm}$ | $351{,}30$ | $351{,}30$ | $0{,}00\%$ |
| Área homogeneizada ($t = \infty$) | $A_{tr,\infty}$ | $\text{cm}^2$ | $246{,}96$ | $246{,}94$ | $-0{,}01\%$ |
| Momento de inércia homogeneizado ($t = \infty$) | $I_{tr,\infty}$ | $\text{cm}^4$ | $49091{,}19$ | $49091{,}19$ | $0{,}00\%$ |
| Compressão axial resistente do perfil I | $N_{Rd,I}$ | $\text{kN}$ | $1416{,}61$ | $1416{,}77$ | $+0{,}01\%$ |
| Momento fletor resistente do perfil I (FLT) | $M_{Rd,I}$ | $\text{kN}\cdot\text{m}$ | $285{,}98$ | $285{,}69$ | $-0{,}10\%$ |
| Força cortante resistente do perfil I | $V_{Rd}$ | $\text{kN}$ | $532{,}21$ | $532{,}22$ | $0{,}00\%$ |
| Limite de protensão inicial pela estabilidade | $P_{0,máx}$ | $\text{kN}$ | $912{,}57$ | $912{,}30$ | $-0{,}03\%$ |

**Tabela 4.2 — Validação das verificações do ELU nas Fases 1, 2 e 3 no meio do vão ($x = 6000\text{ mm}$) — Estudo de Caso 1**

| Fase / Grandeza Estrutural | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Verificação no ELU |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Fase 1 — Etapa Construtiva (Perfil I Protendido)** | | | | | | |
| Perda imediata por acomodação da cunha | $\Delta P_{anc}$ | $\text{kN}$ | $42{,}00$ | $42{,}00$ | $0{,}00\%$ | — |
| Incremento hiperestático ($\Delta P$ concreto fresco) | $\Delta P(q_2)$ | $\text{kN}$ | $35{,}65$ | $35{,}65$ | $0{,}00\%$ | — |
| Força de protensão atuante na construção | $P_1$ | $\text{kN}$ | $404{,}62$ | $404{,}63$ | $0{,}00\%$ | — |
| Momento solicitante positivo líquido | $M_{Sd,1}^+$ | $\text{kN}\cdot\text{m}$ | $259{,}09$ | $259{,}09$ | $0{,}00\%$ | $\le 285{,}69$ (Atende) |
| Momento solicitante negativo de protensão | $M_{Sd,1}^-$ | $\text{kN}\cdot\text{m}$ | $46{,}98$ | $46{,}98$ | $0{,}00\%$ | $\le 285{,}69$ (Atende) |
| Força cortante solicitante máxima | $V_{Sd,1}$ | $\text{kN}$ | $102{,}00$ | $102{,}02$ | $+0{,}02\%$ | $\le 532{,}22$ (Atende) |
| **Fase 2 — Serviço em Curta Duração ($t = 0$)** | | | | | | |
| Força de protensão atuante em $t = 0$ | $P_2$ | $\text{kN}$ | $448{,}41$ | $448{,}42$ | $0{,}00\%$ | — |
| Tensão de cálculo no cabo no ELU | $\sigma_{pd,0}$ | $\text{MPa}$ | $923{,}80$ | $923{,}78$ | $0{,}00\%$ | — |
| Momento solicitante positivo máximo | $M_{Sd,2}^+$ | $\text{kN}\cdot\text{m}$ | $789{,}01$ | $789{,}01$ | $0{,}00\%$ | — |
| Momento resistente positivo da VMP ($t = 0$) | $M_{Rd,VMP,0}^+$ | $\text{kN}\cdot\text{m}$ | $807{,}86$ | $807{,}87$ | $0{,}00\%$ | $M_{Sd}^+ \le M_{Rd}^+$ (Atende) |
| Momento resistente negativo da VMP (FLD) | $M_{Rd,VMP,0}^-$ | $\text{kN}\cdot\text{m}$ | $354{,}49$ | $354{,}50$ | $0{,}00\%$ | $M_{Sd}^- \le M_{Rd}^-$ (Atende) |
| **Fase 3 — Serviço em Longa Duração ($t = \infty$)** | | | | | | |
| Força de protensão final após relaxação | $P_3$ | $\text{kN}$ | $437{,}81$ | $437{,}82$ | $0{,}00\%$ | — |
| Momento resistente positivo da VMP ($t = \infty$) | $M_{Rd,VMP,\infty}^+$ | $\text{kN}\cdot\text{m}$ | $802{,}53$ | $802{,}51$ | $0{,}00\%$ | $\ge 789{,}01$ (Atende) |
| Momento resistente negativo da VMP ($t = \infty$) | $M_{Rd,VMP,\infty}^-$ | $\text{kN}\cdot\text{m}$ | $358{,}60$ | $358{,}61$ | $0{,}00\%$ | $\ge 118{,}72$ (Atende) |

**Tabela 4.3 — Validação das verificações em ELS (Fases 4 e 5) no meio do vão — Estudo de Caso 1**

| Fase / Grandeza de Serviço | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Diagnóstico em ELS |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Fase 4 — ELS Deformação Excessiva (Flecha)** | | | | | | |
| Momento permanente no perfil isolado | $M_{Ga,Sd}$ | $\text{kN}\cdot\text{cm}$ | $13335{,}11$ | $13335{,}20$ | $0{,}00\%$ | — |
| Momento de serviço na seção mista | $M_{L,Sd}$ | $\text{kN}\cdot\text{cm}$ | $24605{,}86$ | $24605{,}60$ | $0{,}00\%$ | — |
| Tensão normal máxima na fibra inferior do aço | $\sigma_{inf}$ | $\text{kN/cm}^2$ | $32{,}39$ | $32{,}40$ | $+0{,}03\%$ | $\le 34{,}5$ (Regime Elástico OK) |
| Flecha por peso próprio do perfil metálico | $\delta_{PP}$ | $\text{cm}$ | $0{,}47$ | $0{,}47$ | $0{,}00\%$ | — |
| Flecha por peso próprio do concreto fresco | $\delta_{conc}$ | $\text{cm}$ | $8{,}15$ | $8{,}15$ | $0{,}00\%$ | — |
| Contraflecha devida à força de protensão $P_1$ | $\delta_{protensao}$ | $\text{cm}$ | $-2{,}91$ | $-2{,}91$ | $0{,}00\%$ | — |
| Flecha por carga permanente de revestimento | $\delta_{CPN}$ | $\text{cm}$ | $1{,}65$ | $1{,}65$ | $0{,}00\%$ | — |
| Flecha por sobrecarga de curta duração ($t=0$) | $\delta_{CVN,curta}$ | $\text{cm}$ | $1{,}99$ | $1{,}99$ | $0{,}00\%$ | — |
| Flecha por sobrecarga de longa duração ($t=\infty$) | $\delta_{CVN,longa}$ | $\text{cm}$ | $1{,}65$ | $1{,}65$ | $0{,}00\%$ | — |
| Contraflecha executiva adotada na viga de aço | $\delta_{contraflecha}$ | $\text{cm}$ | $-10{,}27$ | $-10{,}27$ | $0{,}00\%$ | Anula ações permanentes |
| Flecha vertical total em serviço | $\delta_{total}$ | $\text{cm}$ | $0{,}73$ | $0{,}74$ | $+1{,}37\%$ | $\le 3{,}43\text{ cm}$ (**APROVADO**) |
| Flecha máxima admissível ($L / 350$) | $\delta_{lim}$ | $\text{cm}$ | $3{,}428$ | $3{,}428$ | $0{,}00\%$ | Limite normativo NBR 8800 |
| **Fase 5 — ELS Vibração no Piso** | | | | | | |
| Largura colaborante dinâmica ($b_{ef} \le L_v/5$) | $b_{ef,vib}$ | $\text{cm}$ | $300{,}00$ | $300{,}00$ | $0{,}00\%$ | — |
| Momento de inércia da seção dinâmica ($t=0$) | $I_{tr,vib}$ | $\text{cm}^4$ | $60834{,}17$ | $60834{,}17$ | $0{,}00\%$ | — |
| Deslocamento dinâmico por peso do perfil | $\delta_{PP,vib}$ | $\text{cm}$ | $0{,}12$ | $0{,}12$ | $0{,}00\%$ | — |
| Deslocamento dinâmico por peso do concreto | $\delta_{conc,vib}$ | $\text{cm}$ | $2{,}16$ | $2{,}16$ | $0{,}00\%$ | — |
| Contraflecha dinâmica por protensão ($P_2$) | $\delta_{prot,vib}$ | $\text{cm}$ | $-2{,}22$ | $-2{,}22$ | $0{,}00\%$ | — |
| Deslocamento dinâmico total no piso | $\delta_{total,vib}$ | $\text{cm}$ | $3{,}39$ | $3{,}34$ | $-1{,}47\%$ | $> 2{,}00\text{ cm}$ (**REPROVADO**) |
| Deslocamento limite de vibração no piso | $\delta_{lim,vib}$ | $\text{cm}$ | $2{,}00$ | $2{,}00$ | $0{,}00\%$ | Limite residencial/comercial |

A Tabela 4.3 evidencia que o modelo implementado reproduz fielmente as verificações de serviço de Turini (2021). No ELS de deformação excessiva (Fase 4), a viga atende plenamente ao limite elástico de flecha ($0{,}74\text{ cm} \le 3{,}43\text{ cm}$) graças à contraflecha executiva de $-10{,}27\text{ cm}$. Contudo, na Fase 5 (vibração), a viga mista pré-tracionada do Estudo de Caso 1 não passa no ELS de vibração no piso ($\delta_{total,vib} = 3{,}34\text{ cm} > 2{,}00\text{ cm}$), confirmando exatamente a conclusão de Turini (2021, p. 217). Esse resultado ilustra que lajes maciças em vigas pré-tracionadas com vão de $12\text{ m}$ são altamente suscetíveis a desconforto por vibração, exigindo amortecedores dinâmicos ou lajes de maior inércia.

---

## 4.2 Validação da Modelagem — Estudo de Caso 2 (Viga Mista Pós-Tracionada)

As Tabelas 4.4 e 4.5 apresentam os resultados da modelagem para o Estudo de Caso 2 (viga pós-tracionada de $15{,}0\text{ m}$ com laje mista sobre *steel deck*), abrangendo tanto o ELU quanto o ELS.

**Tabela 4.4 — Validação das verificações no ELU (Fases 1, 2 e 3) — Estudo de Caso 2**

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

**Tabela 4.5 — Validação das verificações em ELS (Fases 4 e 5) — Estudo de Caso 2**

| Fase / Grandeza de Serviço | Símbolo | Unidade | Referência Turini (2021) | Modelo Proposto | Desvio (%) | Diagnóstico em ELS |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Fase 4 — ELS Deformação Excessiva (Flecha)** | | | | | | |
| Momento permanente na montagem não escorada | $M_{Ga,Sd}$ | $\text{kN}\cdot\text{cm}$ | $21628{,}12$ | $21628{,}12$ | $0{,}00\%$ | — |
| Momento de serviço na seção mista | $M_{L,Sd}$ | $\text{kN}\cdot\text{cm}$ | $2650{,}11$ | $2650{,}10$ | $0{,}00\%$ | — |
| Tensão normal máxima na fibra inferior do aço | $\sigma_{inf}$ | $\text{kN/cm}^2$ | $13{,}14$ | $12{,}60$ | $-4{,}11\%$ | $\le 34{,}5$ (Regime Elástico OK) |
| Flecha por peso próprio do perfil metálico | $\delta_{PP}$ | $\text{cm}$ | $0{,}71$ | $0{,}71$ | $0{,}00\%$ | — |
| Flecha por peso do concreto sobre o perfil | $\delta_{conc}$ | $\text{cm}$ | $5{,}46$ | $5{,}46$ | $0{,}00\%$ | — |
| Flecha por carga permanente de revestimento | $\delta_{CPN}$ | $\text{cm}$ | $1{,}18$ | $1{,}18$ | $0{,}00\%$ | — |
| Flecha por sobrecarga de curta duração | $\delta_{CVN,curta}$ | $\text{cm}$ | $0{,}88$ | $0{,}88$ | $0{,}00\%$ | — |
| Flecha por sobrecarga de longa duração | $\delta_{CVN,longa}$ | $\text{cm}$ | $1{,}77$ | $1{,}77$ | $0{,}00\%$ | — |
| Contraflecha devida à força de pós-tração ($P_3$) | $\delta_{protensao}$ | $\text{cm}$ | $-4{,}62$ | $-4{,}62$ | $0{,}00\%$ | — |
| Contraflecha executiva aplicada na viga de aço | $\delta_{contraflecha}$ | $\text{cm}$ | $-5{,}39$ | $-5{,}39$ | $0{,}00\%$ | Limitada p/ $\delta_{total} \ge 0$ |
| Flecha vertical total em serviço | $\delta_{total}$ | $\text{cm}$ | $0{,}00$ | $0{,}00$ | $0{,}00\%$ | $\le 4{,}29\text{ cm}$ (**APROVADO**) |
| Flecha máxima admissível ($L / 350$) | $\delta_{lim}$ | $\text{cm}$ | $4{,}285$ | $4{,}285$ | $0{,}00\%$ | Limite normativo NBR 8800 |
| **Fase 5 — ELS Vibração no Piso** | | | | | | |
| Largura colaborante dinâmica ($b_{ef} \le L_v/5$) | $b_{ef,vib}$ | $\text{cm}$ | $200{,}00$ | $200{,}00$ | $0{,}00\%$ | — |
| Módulo dinâmico do concreto (+35% em steel deck) | $E_{cs,vib}$ | $\text{MPa}$ | $36231{,}0$ | $36231{,}0$ | $0{,}00\%$ | Acréscimo dinâmico NBR 8800 |
| Momento de inércia da seção dinâmica ($t=0$) | $I_{tr,vib}$ | $\text{cm}^4$ | $158015{,}15$ | $158015{,}15$ | $0{,}00\%$ | — |
| Deslocamento dinâmico por cargas permanentes | $\delta_{PP} + \delta_{conc}$ | $\text{cm}$ | $1{,}60$ | $1{,}60$ | $0{,}00\%$ | — |
| Deslocamento dinâmico por sobrecarga ($\psi_1 q_4$) | $\delta_{CVN,vib}$ | $\text{cm}$ | $1{,}25$ | $1{,}25$ | $0{,}00\%$ | — |
| Contraflecha dinâmica por protensão ($P_2$) | $\delta_{prot,vib}$ | $\text{cm}$ | $-4{,}50$ | $-4{,}50$ | $0{,}00\%$ | — |
| Deslocamento dinâmico total no piso | $\delta_{total,vib}$ | $\text{cm}$ | $-0{,}82$ | $-1{,}05$ | — | $\le 2{,}00\text{ cm}$ (**APROVADO**) |
| Deslocamento limite de vibração no piso | $\delta_{lim,vib}$ | $\text{cm}$ | $2{,}00$ | $2{,}00$ | $0{,}00\%$ | Limite residencial/comercial |

No Estudo de Caso 2, a viga mista pós-tracionada atende com folga a todos os critérios de ELS: a contraflecha executiva anula integralmente a flecha sob serviço ($\delta_{total} = 0{,}00\text{ cm}$), e na verificação de vibração o deslocamento total resultante é negativo (ou seja, ascendente, $-1{,}05\text{ cm} \le 2{,}00\text{ cm}$), comprovando que o uso de laje mista sobre *steel deck* aliado à pós-tração confere rigidez dinâmica e amortecimento amplamente superiores para controle de vibrações de pisos.

---

## 4.3 Estudo Comparativo: ABNT NBR 8800:2008 vs. ABNT NBR 8800:2024

A Tabela 4.6 confronta os resultados do dimensionamento segundo a **ABNT NBR 8800:2008** e a **ABNT NBR 8800:2024** para perfis compactos e soldados esbeltos.

**Tabela 4.6 — Comparação de parâmetros de projeto entre a ABNT NBR 8800:2008 e a ABNT NBR 8800:2024**

| Caso / Parâmetro Estrutural | Unidade | ABNT NBR 8800:2008 | ABNT NBR 8800:2024 | Variação ($\Delta\%$) | Justificativa Mecânica e Normativa |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Estudo de Caso 1 (W 360 x 57,8 — Perfil Compacto)** | | | | | |
| Coeficiente de flambagem por cisalhamento ($k_v$) | — | $5{,}00$ | $5{,}34$ | $+6{,}80\%$ | Coeficiente teórico exato para alma sem enrijecedores |
| Esbeltez limite plástica ao cortante ($\lambda_{p,v}$) | — | $59{,}22$ | $61{,}20$ | $+3{,}34\%$ | Incremento proporcional a $\sqrt{5{,}34 / 5{,}00}$ |
| Força cortante resistente ($V_{Rd}$, $\lambda_w = 38{,}98$) | $\text{kN}$ | $532{,}22$ | $532{,}22$ | $0{,}00\%$ | Alma atinge plastificação integral por cisalhamento |
| Compressão axial resistente do perfil ($N_{Rd,I}$) | $\text{kN}$ | $1416{,}77$ | $1416{,}77$ | $0{,}00\%$ | Perfil compacto ($Q = 1{,}00 \Leftrightarrow A_{ef} = A_g$) |
| Limite máximo de protensão inicial ($P_{0,máx}$) | $\text{kN}$ | $912{,}30$ | $912{,}30$ | $0{,}00\%$ | Invariância das resistências $N_{Rd,I}$ e $M_{Rd,I}$ |
| Momento resistente positivo da VMP ($M_{Rd,VMP,0}^+$) | $\text{kN}\cdot\text{m}$ | $807{,}87$ | $807{,}87$ | $0{,}00\%$ | Equilíbrio plástico da seção mista preservado |
| Momento resistente negativo da VMP ($M_{Rd,VMP,0}^-$) | $\text{kN}\cdot\text{m}$ | $354{,}50$ | $354{,}50$ | $0{,}00\%$ | Rigidez rotacional $k_r$ e modelo de FLD mantidos |
| Flecha total final em serviço ($\delta_{total}$) | $\text{cm}$ | $0{,}74$ | $0{,}74$ | $0{,}00\%$ | Critério de deformação em serviço idêntico |
| Deslocamento dinâmico de vibração ($\delta_{total,vib}$) | $\text{cm}$ | $3{,}34$ | $3{,}34$ | $0{,}00\%$ | Anexo L preservado na edição de 2024 |
| **Viga com Perfil Soldado de Alma Esbelta ($t_w = 3{,}0\text{ mm}$, $\lambda_w = 102{,}67$)** | | | | | |
| Compressão axial resistente na Fase 1 ($N_{Rd,I}$) | $\text{kN}$ | $984{,}12$ | $1012{,}45$ | $+2{,}88\%$ | Método da largura efetiva ($A_{ef}$) na tensão $f_n$ |
| Força cortante resistente elástica ($V_{Rd}$) | $\text{kN}$ | $139{,}85$ | $149{,}36$ | $+6{,}80\%$ | Ganho direto do coeficiente $k_v = 5{,}34$ para $\lambda_w > \lambda_{r,v}$ |

Depreende-se que, para perfis laminados compactos, as capacidades resistentes no ELU e o comportamento no ELS são rigorosamente idênticos entre a edição de 2008 e a de 2024. Já em perfis soldados esbeltos, a **ABNT NBR 8800:2024** proporciona ganhos reais de capacidade resistente ($+2{,}88\%$ na compressão e $+6{,}80\%$ no cortante).

---

# CAPÍTULO 5 — CONCLUSÕES E SUGESTÕES PARA TRABALHOS FUTUROS

## 5.1 Conclusões

O presente trabalho desenvolveu a modelagem estrutural multifásica e a verificação completa nos Estados Limites Último (ELU) e de Serviço (ELS) de vigas mistas de aço e concreto protendidas por cabos externos, confrontando os critérios normativos das edições de 2008 e de 2024 da ABNT NBR 8800. A partir dos resultados alcançados, conclui-se que:

1. **Rigor e Precisão da Modelagem Multifásica em ELU e ELS**: A formulação analítica encadeada em 5 fases (Construção, Curta duração $t = 0$, Longa duração $t = \infty$, ELS Deformação e ELS Vibração de piso) reproduziu os resultados analíticos de Turini (2021) com desvios inferiores a $0{,}15\%$, comprovando a exatidão do modelo;
2. **Diferenciação dos Modos Governantes entre Pré-Tração e Pós-Tração**: Enquanto nas vigas pré-tracionadas a força inicial máxima de protensão ($P_{0,máx}$) é condicionada pela flexo-compressão do perfil isolado na montagem (Fase 1), nas vigas pós-tracionadas o limitador crítico reside na Flambagem Lateral com Distorção (FLD) sob momento negativo na Fase 2 ($t = 0$);
3. **Papel Fundamental da Contraflecha Executiva no ELS de Flecha**: Em vigas de grandes vãos ($12\text{ m}$ a $15\text{ m}$), a contraflecha executiva calculada para anular as ações permanentes constitui ferramenta indispensável de projeto, viabilizando o atendimento ao limite normativo $\delta_{lim} = L/350$ com flechas líquidas finais próximas de zero;
4. **Vulnerabilidade Dinâmica à Vibração de Piso em Lajes Maciças**: A análise de vibração segundo o Anexo L da ABNT NBR 8800 revelou que vigas mistas com lajes maciças pré-tracionadas podem ser reprovadas no ELS de vibração no piso mesmo atendendo com folga a todos os critérios de ELU e de flecha estática, ao passo que vigas com lajes mistas sobre *steel deck* e pós-tração apresentam rigidez dinâmica amplamente favorável graças ao acréscimo dinâmico de 35% no módulo do concreto e à elevada inércia homogeneizada;
5. **Impacto da Atualização para a ABNT NBR 8800:2024**: Comprovou-se que as prescrições de ELS permanecem inalteradas entre as edições de 2008 e de 2024. No ELU, para perfis compactos laminados, as capacidades resistentes coincidem; para perfis soldados esbeltos, o novo método da largura efetiva ($A_{ef}$) e o coeficiente $k_v = 5{,}34$ elevam a resistência de cálculo em até $6{,}80\%$.

## 5.2 Sugestões para Trabalhos Futuros

Como continuidade desta pesquisa, sugerem-se os seguintes temas:
1. Ampliação da modelagem dinâmica de vibrações no piso com determinação direta das frequências naturais fundamentais e acelerações de pico por análise modal;
2. Investigação experimental e numérica de vigas mistas protendidas contínuas de múltiplos vãos;
3. Otimização estrutural multiobjetivo visando minimizar o custo global respeitando simultaneamente os limites de ELU, de flecha e de vibração da ABNT NBR 8800:2024.

---

# REFERÊNCIAS BIBLIOGRÁFICAS

- AMERICAN INSTITUTE OF STEEL CONSTRUCTION. **ANSI/AISC 360-22**: Specification for Structural Steel Buildings. Chicago: AISC, 2022.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 6118**: Projeto de estruturas de concreto — Procedimento. Rio de Janeiro: ABNT, 2023.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2008.
- ASSOCIAÇÃO BRASILEIRA DE NORMAS TÉCNICAS. **ABNT NBR 8800**: Projeto de estruturas de aço e de estruturas mistas de aço e concreto de edifícios. Rio de Janeiro: ABNT, 2024.
- CHEN, S.; GU, P. Load carrying capacity of composite beams prestressed with external tendons under positive moment. **Journal of Constructional Steel Research**, v. 61, n. 4, p. 515–530, 2005.
- HARAJLI, M. H. External prestressing in bridges. **Progress in Structural Engineering and Materials**, v. 8, n. 4, p. 141–154, 2006.
- NUNES, F. G. **Análise experimental de vigas mistas de aço e concreto protendidas com cabos externos**. Dissertação (Mestrado em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2005.
- QUEIROZ, G.; PIMENTA, R. J.; DA MATA, L. A. C. **Elementos das estruturas mistas aço-concreto**. Belo Horizonte: O Lutador, 2001.
- RIBEIRO, K. D. **Verificação de vigas mistas de aço e concreto protendidas**. Trabalho de Conclusão de Curso (Graduação em Engenharia Civil) — Universidade Federal do Espírito Santo, Vitória, 2018.
- TROITSKY, M. S. **Prestressed Steel Bridges: Theory and Design**. New York: Van Nostrand Reinhold, 1990.
- TURINI, T. T. **Desenvolvimento de aplicativo para verificação de vigas mistas de aço e concreto protendidas**. Dissertação (Mestrado em Engenharia Civil) — Programa de Pós-Graduação em Engenharia Civil, Universidade Federal do Espírito Santo, Vitória, 2021.
