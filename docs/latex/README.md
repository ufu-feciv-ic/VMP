# TCC em LaTeX (abnTeX2)

Conversão fiel de `docs/TCC_VIGA_MISTA_NBR8800_2024.md` para a classe abnTeX2 (ABNT). O conteúdo técnico não foi alterado.

**Compilar:** no Overleaf, envie a pasta inteira e use o compilador **pdfLaTeX** (BibTeX roda automaticamente). Localmente: `latexmk -pdf main.tex` (TeX Live/MiKTeX com abntex2, listings, tikz, booktabs, tabularx e longtable).

**Metadados a preencher em `main.tex`:** `\autor{[NOME DO AUTOR]}`, `\orientador{[NOME DO ORIENTADOR]}`; na folha de aprovação: `[DIA] de [MÊS]`, `[NOME DO MEMBRO DA BANCA 1]`, `[NOME DO MEMBRO DA BANCA 2]` e `[INSTITUIÇÃO]`. Em `referencias.bib`: `urldate`/`urlaccessdate` da entrada `carvalho2024` (`[DATA DE ACESSO]`).

**Estrutura:** `main.tex` (preâmbulo e pré-textuais), `pretextuais/` (resumo, abstract, símbolos), `capitulos/01..05_*.tex`, `referencias.bib`.

**Marcadores `\conferir{...}` (texto em vermelho) a resolver antes da entrega:**
1. `capitulos/02_fundamentacao.tex`, seção 2.9.1 (nota sobre a expressão de $a_p$): `\conferir{Conferir na versão impressa da norma e remover esta nota.}`

**Outras pendências (fora do texto):**
- `referencias.bib`, entrada `turini2021`: campo `note = {[Conferir título e instituição]}`, que aparece na lista de referências.
