# Dados numéricos e tabelas do TCC

> Arquivo gerado automaticamente por `vmp_tabelas` (src/gerar_tabelas_tcc.cpp). Não edite à mão: altere o código ou o estudo de caso e gere novamente.

## 1. Estudo de caso V1 — ABNT NBR 8800:2024

Dados: W460×52,0 (A572 Gr. 50), steel deck MF-75 (h = 140 mm, t = 0,80 mm) com nervuras perpendiculares, C30, vão de 9,0 m, vigas a cada 2,8 m, pinos Ø19 × 125 mm (fu = 415 MPa) em todas as nervuras, construção não escorada, contraflecha de 15 mm.

Ações lineares: g1 = 7,942 kN/m; q1 = 2,80 kN/m; g2 = 4,34 kN/m; q2 = 9,10 kN/m.

### 1.1 Grandezas intermediárias

| Grandeza | Símbolo | Valor | Unidade |
| :--- | :---: | ---: | :---: |
| Largura efetiva | $b_{ef}$ | 2250 | mm |
| Momento solicitante, construção | $M_{Sd,c}$ | 136,60 | kN·m |
| Momento resistente do perfil isolado | $M_{Rd,a}$ | 343,71 | kN·m |
| Resistência de um conector | $Q_{Rd}$ | 70,60 | kN |
| Coeficientes de grupo e posição | $R_g \cdot R_p$ | 0,75 | — |
| Força de cisalhamento de cálculo | $F_{hd}$ | 2088,82 | kN |
| Espaçamento adotado | $s$ | 274 | mm |
| Conectores por meio vão (adotados) | $n$ | 16 | un |
| Conectores por meio vão p/ interação completa | $n_{compl}$ | 30 | un |
| Grau de interação | $\eta$ | 0,541 | — |
| Grau de interação mínimo | $\eta_i$ | 0,280 | — |
| Conectores mínimos por meio vão | $n_{min}$ | 9 | un |
| Momento solicitante de cálculo | $M_{Sd}$ | 311,91 | kN·m |
| Compressão no perfil | $C_{ad}$ | 479,62 | kN |
| Posição da LNP a partir do topo do perfil | $y_p$ | 10,06 | mm |
| Momento resistente da viga mista | $M_{Rd}$ | 607,73 | kN·m |
| Força cortante solicitante | $V_{Sd}$ | 138,63 | kN |
| Força cortante resistente | $V_{Rd}$ | 643,58 | kN |
| Inércia homogeneizada, curta duração | $I_{tr}$ | 76609 | cm⁴ |
| Inércia homogeneizada, longa duração | $I_{tr,\infty}$ | 57546 | cm⁴ |
| Inércia efetiva, curta duração | $I_{ef}$ | 61991 | cm⁴ |
| Inércia efetiva, longa duração | $I_{ef,\infty}$ | 47973 | cm⁴ |
| Flecha de $g_1$ no perfil isolado | $\delta_{g1}$ | 15,88 | mm |
| Flecha das permanentes (curta duração) | $\delta_1$ | 18,87 | mm |
| Acréscimo por fluência | $\delta_{fl}$ | 0,87 | mm |
| Flecha por retração | $\delta_{sh}$ | 4,56 | mm |
| Flecha das variáveis (quase permanente) | $\delta_3$ | 3,24 | mm |
| Contraflecha considerada | $\delta_0$ | 15,00 | mm |
| Flecha máxima | $\delta_{max}$ | 12,55 | mm |
| Limite $L/350$ | $\delta_{lim}$ | 25,71 | mm |
| Flecha pós-cura (combinação rara) | $\delta_2+\delta_3$ | 12,44 | mm |
| Tensão na face inferior em serviço | $\sigma$ | 181,1 | MPa |
| Inércia dinâmica da viga mista | $I_b$ | 81743 | cm⁴ |
| Massa distribuída do piso | $m$ | 480,3 | kg/m² |
| Deslocamento do piso (modo A) | $\delta$ | 7,18 | mm |
| Frequência fundamental | $f_0$ | 6,72 | Hz |
| Largura colaborante do piso | $B_b$ | 8,30 | m |
| Massa modal | $M$ | 17929 | kg |
| Razão de amortecimento | $\beta$ | 0,030 | — |
| Aceleração de pico | $a_p$ | 0,0365 | m/s² |
| Valor eficaz ponderado | $a_{w,rms}$ | 0,0493 | m/s² |
| Fator de resposta | $R$ | 9,86 | — |

### 1.2 Quadro de verificações

| Verificação | Solicitante | Resistente / limite | Utilização | Situação |
| :--- | ---: | ---: | ---: | :---: |
| Flexão Fase 1 (Perfil Isolado) | 136,600 kN.m | 343,714 kN.m | 39,7% | Atende |
| Cortante Fase 1 (Perfil Isolado) | 60,711 kN | 643,582 kN | 9,4% | Atende |
| Conectores - Grau de Interação | 0,280 - | 0,541 - | 51,8% | Atende |
| Conectores - Espaçamento Longitudinal | 274,000 mm | 915,000 mm | 29,9% | Atende |
| Conectores - Altura e Cobrimento | 115,000 mm | 125,000 mm | 92,0% | Atende |
| Conectores - Geometria Transversal | 50,000 mm | 152,000 mm | 32,9% | Atende |
| Flexão Fase 2 (Viga Mista) | 311,907 kN.m | 607,733 kN.m | 51,3% | Atende |
| Cortante Fase 2 (Viga Mista) | 138,625 kN | 643,582 kN | 21,5% | Atende |
| ELS Tensão Mesa Inferior | 181,070 MPa | 345,000 MPa | 52,5% | Atende |
| ELS Flecha Máxima | 12,545 mm | 25,714 mm | 48,8% | Atende |
| ELS Flecha Pós-Cura | 12,441 mm | 25,714 mm | 48,4% | Atende |
| ELS Vibração do Piso | 0,037 m/s² | 0,050 m/s² | 73,1% | Atende |

Resultado global: **aprovado**.

## 2. Mesmo caso nas duas edições da norma

| Grandeza | NBR 8800:2008 | NBR 8800:2024 | Variação |
| :--- | ---: | ---: | ---: |
| Módulo do concreto $E_c$ (MPa) | 26072 | 26072 | 0,0% |
| $Q_{Rd}$ (kN) | 70,60 | 70,60 | 0,0% |
| Grau de interação $\eta$ | 0,541 | 0,541 | 0,0% |
| Grau mínimo $\eta_i$ | 0,519 | 0,280 | -46,0% |
| Conectores mínimos por meio vão | 16 | 9 | -43,8% |
| $M_{Rd}$ da viga mista (kN·m) | 607,73 | 607,73 | 0,0% |
| $V_{Rd}$ (kN) | 643,58 | 643,58 | 0,0% |
| $I_{ef}$ curta duração (cm⁴) | 61991 | 61991 | 0,0% |
| $\delta_{max}$ (mm) | 12,55 | 12,55 | 0,0% |

Vibração: na edição 2008 (L.3.2) o critério é indireto — deslocamento frequente sem a parcela diferida = 22,63 mm ≤ 20 mm (**Não atende**). Na edição 2024 (Anexo I): f0 = 6,72 Hz, a_p = 0,0365 m/s² ≤ 0,050 m/s² (Atende).

## 3. Estudo paramétrico — variação do vão (V1, conectores em todas as nervuras)

| L (m) | $\eta$ | $\eta_i$ 2008 | $\eta_i$ 2024 | $M_{Sd}/M_{Rd}$ 2024 | $\delta_{max}/\delta_{lim}$ | $f_0$ (Hz) | $a_p$ (m/s²) | Global 2024 |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | :---: |
| 6,0 | 0,398 | 0,428 | 0,280 | 25,8% | 17,3% | 13,62 | 0,0072 | Atende |
| 7,0 | 0,409 | 0,458 | 0,280 | 33,4% | 22,7% | 10,54 | 0,0157 | Atende |
| 8,0 | 0,473 | 0,488 | 0,280 | 41,9% | 28,0% | 8,33 | 0,0262 | Atende |
| 9,0 | 0,541 | 0,519 | 0,280 | 51,3% | 48,8% | 6,72 | 0,0365 | Atende |
| 10,0 | 0,608 | 0,549 | 0,280 | 61,4% | 87,3% | 5,52 | 0,0452 | Atende |
| 11,0 | 0,676 | 0,579 | 0,289 | 72,2% | 130,8% | 4,61 | 0,0515 | **Não atende** |
| 12,0 | 0,710 | 0,609 | 0,373 | 84,8% | 182,3% | 3,89 | 0,0557 | **Não atende** |

## 4. Estudo paramétrico — conector a cada k nervuras (V1, L = 9 m)

| k | s (mm) | n por meio vão | $\eta$ | $\eta_i$ 2008 | $\eta_i$ 2024 | Atende 2008 | Atende 2024 | $M_{Rd}$ 2024 (kN·m) | $\delta_{max}$ (mm) |
| ---: | ---: | ---: | ---: | ---: | ---: | :---: | :---: | ---: | ---: |
| 1 | 274 | 16 | 0,541 | 0,519 | 0,280 | Atende | Atende | 607,7 | 12,55 |
| 2 | 548 | 8 | 0,270 | 0,519 | 0,280 | **Não atende** | **Não atende** | 508,6 | 14,81 |
| 3 | 822 | 5 | 0,169 | 0,519 | 0,280 | **Não atende** | **Não atende** | 452,4 | 16,32 |
| 4 | 1096 | 4 | 0,135 | 0,519 | 0,289 | **Não atende** | **Não atende** | 431,3 | 17,02 |

## 5. Estudo paramétrico — resistência do concreto (V1, interação completa)

| $f_{ck}$ (MPa) | $\eta_c$ 2024 | $Q_{Rd}$ 2008 (kN) | $Q_{Rd}$ 2024 (kN) | $n_{compl}$ 2008 | $n_{compl}$ 2024 | $M_{Rd}$ 2008 (kN·m) | $M_{Rd}$ 2024 (kN·m) |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 25 | 1,000 | 87,48 | 70,60 | 24 | 30 | 656,8 | 656,8 |
| 30 | 1,000 | 94,13 | 70,60 | 23 | 30 | 667,4 | 667,4 |
| 35 | 1,000 | 94,13 | 70,60 | 23 | 30 | 675,0 | 675,0 |
| 40 | 1,000 | 94,13 | 70,60 | 23 | 30 | 680,7 | 680,7 |
| 45 | 0,961 | 94,13 | 70,60 | 23 | 30 | 685,2 | 683,7 |
| 50 | 0,928 | 94,13 | 70,60 | 23 | 30 | 688,7 | 686,2 |

Nos casos da seção 5 a laje é maciça (tc = 120 mm), o que torna visível o efeito de $R_p$ (1,00 em 2008 e 0,75 em 2024 para pinos soldados diretamente no perfil).
