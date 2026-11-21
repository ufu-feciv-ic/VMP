import os
import re
import zipfile
from xml.sax.saxutils import escape

# ============================================================================
# CONVERSOR LATEX -> OMML NATIVO DO MICROSOFT WORD (<m:oMath> / <m:oMathPara>)
# ============================================================================

LATEX_SYMBOLS = {
    r"\infty": "∞",
    r"\le": "≤",
    r"\ge": "≥",
    r"\neq": "≠",
    r"\approx": "≈",
    r"\times": "×",
    r"\cdot": "⋅",
    r"\to": "→",
    r"\Rightarrow": "⇒",
    r"\Longrightarrow": "⟹",
    r"\Longleftrightarrow": "⟺",
    r"\Leftrightarrow": "⇔",
    r"\Delta": "Δ",
    r"\delta": "δ",
    r"\eta": "η",
    r"\lambda": "λ",
    r"\sigma": "σ",
    r"\varepsilon": "ε",
    r"\gamma": "γ",
    r"\alpha": "α",
    r"\beta": "β",
    r"\chi": "χ",
    r"\psi": "ψ",
    r"\rho": "ρ",
    r"\pi": "π",
    r"\varnothing": "⌀",
    r"\lceil": "⌈",
    r"\rceil": "⌉",
    r"\min": "mín",
    r"\max": "máx",
    r"\sum": "∑",
}

def m_r(text: str, normal_text: bool = False) -> str:
    """Cria um elemento <m:r> (run matemático) no padrão Office Math (OMML)."""
    if not text:
        return ""
    nor = "<m:nor/>" if normal_text else ""
    rpr = f"<m:rPr>{nor}</m:rPr>" if nor else ""
    w_rpr = '<w:rPr><w:rFonts w:ascii="Cambria Math" w:hAnsi="Cambria Math" w:cs="Cambria Math"/></w:rPr>'
    return f"<m:r>{rpr}{w_rpr}<m:t>{escape(text)}</m:t></m:r>"

def find_matching_brace(s: str, start_idx: int) -> int:
    """Dado s[start_idx] == '{', retorna o índice do '}' correspondente."""
    depth = 0
    for idx in range(start_idx, len(s)):
        if s[idx] == "{":
            depth += 1
        elif s[idx] == "}":
            depth -= 1
            if depth == 0:
                return idx
    return len(s) - 1

def preprocess_latex(s: str) -> str:
    s = s.replace("{,}", ",")
    s = s.replace(r"\%", "%")
    s = s.replace(r"\,", " ")
    s = s.replace(r"\;", " ")
    s = s.replace(r"\quad", "   ")
    s = s.replace(r"\qquad", "     ")
    s = s.replace(r"\left", "")
    s = s.replace(r"\right", "")
    # Ordena chaves maiores primeiro para evitar substituição parcial (\Longrightarrow antes de \Rightarrow)
    for k in sorted(LATEX_SYMBOLS.keys(), key=len, reverse=True):
        s = s.replace(k, LATEX_SYMBOLS[k])
    return s.strip()

def parse_atom(s: str, pos: int) -> tuple[str, int]:
    """Lê um único átomo LaTeX a partir de pos e retorna (omml_xml, next_pos)."""
    while pos < len(s) and s[pos] == " ":
        pos += 1
    if pos >= len(s):
        return "", pos

    # 1. Grupo entre chaves { ... }
    if s[pos] == "{":
        end_b = find_matching_brace(s, pos)
        inner = s[pos + 1 : end_b]
        return latex_to_omml_nodes(inner), end_b + 1

    # 2. Comandos LaTeX iniciados por \
    if s[pos] == "\\":
        # \frac ou \dfrac
        for cmd in (r"\dfrac", r"\frac"):
            if s.startswith(cmd, pos):
                p = pos + len(cmd)
                while p < len(s) and s[p] == " ":
                    p += 1
                if p < len(s) and s[p] == "{":
                    end_num = find_matching_brace(s, p)
                    num_str = s[p + 1 : end_num]
                    p2 = end_num + 1
                    while p2 < len(s) and s[p2] == " ":
                        p2 += 1
                    if p2 < len(s) and s[p2] == "{":
                        end_den = find_matching_brace(s, p2)
                        den_str = s[p2 + 1 : end_den]
                        num_xml = latex_to_omml_nodes(num_str)
                        den_xml = latex_to_omml_nodes(den_str)
                        frac_xml = f"<m:f><m:num>{num_xml}</m:num><m:den>{den_xml}</m:den></m:f>"
                        return frac_xml, end_den + 1

        # \sqrt{...}
        if s.startswith(r"\sqrt", pos):
            p = pos + len(r"\sqrt")
            while p < len(s) and s[p] == " ":
                p += 1
            if p < len(s) and s[p] == "{":
                end_rad = find_matching_brace(s, p)
                rad_str = s[p + 1 : end_rad]
                rad_xml = latex_to_omml_nodes(rad_str)
                return f"<m:rad><m:radPr><m:degHide m:val=\"1\"/></m:radPr><m:deg/><m:e>{rad_xml}</m:e></m:rad>", end_rad + 1

        # \text{...} ou \mathrm{...}
        for cmd in (r"\text", r"\mathrm"):
            if s.startswith(cmd, pos):
                p = pos + len(cmd)
                while p < len(s) and s[p] == " ":
                    p += 1
                if p < len(s) and s[p] == "{":
                    end_t = find_matching_brace(s, p)
                    txt_str = s[p + 1 : end_t]
                    return m_r(txt_str, normal_text=True), end_t + 1

        # \begin{cases} ... \end{cases}
        if s.startswith(r"\begin{cases}", pos):
            end_cases = s.find(r"\end{cases}", pos)
            if end_cases != -1:
                inner_cases = s[pos + len(r"\begin{cases}") : end_cases].strip()
                rows = [r.strip() for r in inner_cases.split(r"\\") if r.strip()]
                eq_rows_xml = []
                for r_str in rows:
                    parts = [p.strip() for p in r_str.split("&")]
                    if len(parts) == 2:
                        row_omml = latex_to_omml_nodes(parts[0]) + m_r("   ") + latex_to_omml_nodes(parts[1])
                    else:
                        row_omml = latex_to_omml_nodes(r_str)
                    eq_rows_xml.append(f"<m:e>{row_omml}</m:e>")
                cases_xml = (
                    '<m:d><m:dPr><m:begChr m:val="{"/><m:endChr m:val=""/></m:dPr>'
                    f'<m:e><m:eqArr>{"".join(eq_rows_xml)}</m:eqArr></m:e></m:d>'
                )
                return cases_xml, end_cases + len(r"\end{cases}")

        # Comando desconhecido: lê letras
        m_cmd = re.match(r"\\[a-zA-Z]+", s[pos:])
        if m_cmd:
            cmd_name = m_cmd.group(0)
            return m_r(cmd_name[1:]), pos + len(cmd_name)
        return m_r(s[pos]), pos + 1

    # 3. Parênteses ( ... ) ou Colchetes [ ... ]
    if s[pos] in "([":
        open_ch = s[pos]
        close_ch = ")" if open_ch == "(" else "]"
        depth = 0
        end_p = -1
        for idx in range(pos, len(s)):
            if s[idx] == open_ch:
                depth += 1
            elif s[idx] == close_ch:
                depth -= 1
                if depth == 0:
                    end_p = idx
                    break
        if end_p != -1:
            inner = s[pos + 1 : end_p]
            inner_xml = latex_to_omml_nodes(inner)
            return (
                f'<m:d><m:dPr><m:begChr m:val="{open_ch}"/><m:endChr m:val="{close_ch}"/></m:dPr>'
                f"<m:e>{inner_xml}</m:e></m:d>"
            ), end_p + 1

    # 4. Número contínuo (ex: 0,85 ou 1416,77) ou variável isolada
    m_num = re.match(r"^[0-9]+(,[0-9]+|\.[0-9]+)?", s[pos:])
    if m_num:
        val = m_num.group(0)
        return m_r(val), pos + len(val)

    # 5. Palavra reservada como mín ou máx
    for kw in ("mín", "máx"):
        if s.startswith(kw, pos):
            return m_r(kw, normal_text=True), pos + len(kw)

    # 6. Caractere individual (variável, letra grega ou operador)
    ch = s[pos]
    return m_r(ch), pos + 1

def read_script_arg(s: str, pos: int) -> tuple[str, int]:
    """Lê o argumento de um subscrito (_) ou sobrescrito (^)."""
    if pos >= len(s):
        return "", pos
    if s[pos] == "{":
        end_b = find_matching_brace(s, pos)
        return latex_to_omml_nodes(s[pos + 1 : end_b]), end_b + 1
    # Caso contrário, lê 1 caractere ou número
    m_tok = re.match(r"^[a-zA-Z0-9∞+\-]+", s[pos:])
    if m_tok:
        tok = m_tok.group(0)
        return m_r(tok), pos + len(tok)
    return m_r(s[pos]), pos + 1

def latex_to_omml_nodes(s: str) -> str:
    """Converte uma expressão LaTeX em uma sequência de nós OMML (<m:r>, <m:f>, <m:sSub>, etc.)."""
    s = preprocess_latex(s)
    pos = 0
    nodes = []

    while pos < len(s):
        if s[pos] == " ":
            pos += 1
            continue

        base_xml, next_pos = parse_atom(s, pos)
        pos = next_pos

        # Verifica se há subscrito (_) e/ou sobrescrito (^) acoplado a este átomo
        sub_xml = None
        sup_xml = None

        while pos < len(s) and s[pos] in "_^":
            op = s[pos]
            pos += 1
            arg_xml, pos = read_script_arg(s, pos)
            if op == "_":
                sub_xml = arg_xml
            else:
                sup_xml = arg_xml

        if sub_xml is not None and sup_xml is not None:
            nodes.append(f"<m:sSubSup><m:e>{base_xml}</m:e><m:sub>{sub_xml}</m:sub><m:sup>{sup_xml}</m:sup></m:sSubSup>")
        elif sub_xml is not None:
            nodes.append(f"<m:sSub><m:e>{base_xml}</m:e><m:sub>{sub_xml}</m:sub></m:sSub>")
        elif sup_xml is not None:
            nodes.append(f"<m:sSup><m:e>{base_xml}</m:e><m:sup>{sup_xml}</m:sup></m:sSup>")
        else:
            nodes.append(base_xml)

    return "".join(nodes)

def make_omath_inline(latex_expr: str) -> str:
    """Cria um objeto de equação inline <m:oMath> nativo do Word."""
    omml_body = latex_to_omml_nodes(latex_expr)
    return f"<m:oMath>{omml_body}</m:oMath>"

def make_omath_block_paragraph(latex_expr: str, eq_number_str: str) -> str:
    """Cria um parágrafo com equação em bloco centralizada (<m:oMathPara>) e numeração (X.Y)."""
    # Se houver múltiplas equações separadas por \qquad ou \quad\Longrightarrow\quad, mantém no mesmo oMath
    omml_body = latex_to_omml_nodes(latex_expr)
    num_run = m_r(f"     ({eq_number_str})", normal_text=True)
    ppr = '<w:pPr><w:spacing w:before="160" w:after="160" w:line="360" w:lineRule="auto"/><w:jc w:val="center"/></w:pPr>'
    return f"<w:p>{ppr}<m:oMathPara><m:oMath>{omml_body}{num_run}</m:oMath></m:oMathPara></w:p>"

# ============================================================================
# GERADOR DE PARÁGRAFOS E TABELAS OPENXML (WORD .DOCX)
# ============================================================================

def make_run_xml(text: str, bold=False, italic=False, size_half_pt=24, color="000000", font="Times New Roman") -> str:
    if not text:
        return ""
    rpr = f'<w:rFonts w:ascii="{font}" w:hAnsi="{font}" w:cs="{font}"/>'
    if bold:
        rpr += "<w:b/>"
    if italic:
        rpr += "<w:i/>"
    rpr += f'<w:color w:val="{color}"/><w:sz w:val="{size_half_pt}"/><w:szCs w:val="{size_half_pt}"/>'
    return f'<w:r><w:rPr>{rpr}</w:rPr><w:t xml:space="preserve">{escape(text)}</w:t></w:r>'

def parse_text_and_bold_italic(segment: str, base_bold=False, base_italic=False, size_half_pt=24, color="000000", font="Times New Roman") -> str:
    segment = segment.replace("`", "")
    tokens = re.split(r"(\*\*[^*]+\*\*|\*[^*]+\*)", segment)
    xml_runs = []
    for tok in tokens:
        if not tok:
            continue
        if tok.startswith("**") and tok.endswith("**"):
            xml_runs.append(make_run_xml(tok[2:-2], bold=True, italic=base_italic, size_half_pt=size_half_pt, color=color, font=font))
        elif tok.startswith("*") and tok.endswith("*"):
            xml_runs.append(make_run_xml(tok[1:-1], bold=base_bold, italic=True, size_half_pt=size_half_pt, color=color, font=font))
        else:
            xml_runs.append(make_run_xml(tok, bold=base_bold, italic=base_italic, size_half_pt=size_half_pt, color=color, font=font))
    return "".join(xml_runs)

def parse_inline_runs_with_omath(line: str, base_bold=False, base_italic=False, size_half_pt=24, color="000000", font="Times New Roman") -> str:
    """Separa trechos de texto comum e expressões matemáticas inline $...$, convertendo $...$ em <m:oMath> nativo."""
    parts = re.split(r"(\$[^$]+\$)", line)
    out_xml = []
    for part in parts:
        if not part:
            continue
        if part.startswith("$") and part.endswith("$") and len(part) > 2:
            latex_inner = part[1:-1]
            out_xml.append(make_omath_inline(latex_inner))
        else:
            out_xml.append(parse_text_and_bold_italic(part, base_bold=base_bold, base_italic=base_italic, size_half_pt=size_half_pt, color=color, font=font))
    return "".join(out_xml)

def make_paragraph_xml(text: str, align="both", first_line_twips=709, space_before=0, space_after=120, line_spacing=360,
                       bold=False, italic=False, size_half_pt=24, color="000000", font="Times New Roman") -> str:
    ppr = f'<w:spacing w:before="{space_before}" w:after="{space_after}" w:line="{line_spacing}" w:lineRule="auto"/>'
    if first_line_twips > 0:
        ppr += f'<w:ind w:firstLine="{first_line_twips}"/>'
    ppr += f'<w:jc w:val="{align}"/>'
    runs = parse_inline_runs_with_omath(text, base_bold=bold, base_italic=italic, size_half_pt=size_half_pt, color=color, font=font)
    return f"<w:p><w:pPr>{ppr}</w:pPr>{runs}</w:p>"

def make_table_xml(rows: list[list[str]]) -> str:
    tbl_pr = """
    <w:tblPr>
      <w:tblW w:w="5000" w:type="pct"/>
      <w:jc w:val="center"/>
      <w:tblBorders>
        <w:top w:val="single" w:sz="8" w:space="0" w:color="000000"/>
        <w:left w:val="none"/>
        <w:bottom w:val="single" w:sz="8" w:space="0" w:color="000000"/>
        <w:right w:val="none"/>
        <w:insideH w:val="single" w:sz="4" w:space="0" w:color="BFBFBF"/>
        <w:insideV w:val="none"/>
      </w:tblBorders>
    </w:tblPr>
    """
    tr_xml_list = []
    for r_idx, row in enumerate(rows):
        is_header = (r_idx == 0)
        tc_list = []
        for c_idx, cell_text in enumerate(row):
            shd = '<w:shd w:val="clear" w:color="auto" w:fill="EAEAEA"/>' if is_header else ''
            align = "left" if c_idx == 0 else "center"
            p_xml = make_paragraph_xml(
                cell_text.strip(),
                align=align,
                first_line_twips=0,
                space_before=60,
                space_after=60,
                line_spacing=240,
                bold=is_header,
                size_half_pt=20
            )
            tc_list.append(
                f'<w:tc><w:tcPr><w:tcMar><w:top w:w="60" w:type="dxa"/><w:bottom w:w="60" w:type="dxa"/>'
                f'<w:left w:w="100" w:type="dxa"/><w:right w:w="100" w:type="dxa"/></w:tcMar>{shd}</w:tcPr>{p_xml}</w:tc>'
            )
        tr_xml_list.append(f"<w:tr>{''.join(tc_list)}</w:tr>")
    return f"<w:tbl>{tbl_pr}{''.join(tr_xml_list)}</w:tbl>" + make_paragraph_xml("", first_line_twips=0, space_after=120)

def convert_md_to_docx(md_path: str, docx_path: str):
    with open(md_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    body_elements = []
    i = 0
    n = len(lines)
    current_chapter = 0
    eq_counter = 0

    while i < n:
        line = lines[i].rstrip("\n")
        stripped = line.strip()

        if stripped == "---" or stripped == "":
            i += 1
            continue

        # Bloco de equação $$ ... $$ -> Objeto <m:oMathPara> nativo do Word
        if stripped.startswith("$$"):
            eq_lines = [stripped[2:]]
            while i < n and not lines[i].strip().endswith("$$"):
                i += 1
                if i < n:
                    eq_lines.append(lines[i].strip())
            full_eq = " ".join(eq_lines)
            if full_eq.endswith("$$"):
                full_eq = full_eq[:-2]
            eq_counter += 1
            chap_num = current_chapter if current_chapter > 0 else 2
            eq_label = f"{chap_num}.{eq_counter}"
            body_elements.append(make_omath_block_paragraph(full_eq, eq_label))
            i += 1
            continue

        # Tabela Markdown
        if stripped.startswith("|") and stripped.endswith("|"):
            table_rows = []
            while i < n and lines[i].strip().startswith("|"):
                row_line = lines[i].strip()[1:-1]
                cells = [c.strip() for c in row_line.split("|")]
                if not all(re.match(r"^:?-+:?$", c) for c in cells if c):
                    table_rows.append(cells)
                i += 1
            if table_rows:
                body_elements.append(make_table_xml(table_rows))
            continue

        # Títulos H1, H2, H3
        if stripped.startswith("# "):
            title_text = stripped[2:].strip()
            m_chap = re.search(r"CAPÍTULO\s+(\d+)", title_text, re.IGNORECASE)
            if m_chap:
                current_chapter = int(m_chap.group(1))
                eq_counter = 0
            body_elements.append(make_paragraph_xml(
                title_text.upper(),
                align="left",
                first_line_twips=0,
                space_before=360,
                space_after=240,
                line_spacing=360,
                bold=True,
                size_half_pt=28
            ))
            i += 1
            continue

        if stripped.startswith("## "):
            title_text = stripped[3:].strip()
            body_elements.append(make_paragraph_xml(
                title_text,
                align="left",
                first_line_twips=0,
                space_before=280,
                space_after=160,
                line_spacing=360,
                bold=True,
                size_half_pt=24
            ))
            i += 1
            continue

        if stripped.startswith("### "):
            title_text = stripped[4:].strip()
            body_elements.append(make_paragraph_xml(
                title_text,
                align="left",
                first_line_twips=0,
                space_before=200,
                space_after=120,
                line_spacing=360,
                bold=True,
                italic=True,
                size_half_pt=24
            ))
            i += 1
            continue

        if stripped.startswith("> "):
            quote_text = stripped[2:].strip()
            body_elements.append(make_paragraph_xml(
                quote_text,
                align="center",
                first_line_twips=0,
                space_before=200,
                space_after=240,
                line_spacing=360,
                bold=True,
                size_half_pt=26
            ))
            i += 1
            continue

        if stripped.startswith("- ") or re.match(r"^\d+\.\s", stripped):
            bullet_text = "• " + stripped[2:].strip() if stripped.startswith("- ") else stripped
            body_elements.append(make_paragraph_xml(
                bullet_text,
                align="both",
                first_line_twips=360,
                space_before=40,
                space_after=80,
                line_spacing=360,
                size_half_pt=24
            ))
            i += 1
            continue

        if stripped.startswith("**Tabela "):
            body_elements.append(make_paragraph_xml(
                stripped,
                align="center",
                first_line_twips=0,
                space_before=240,
                space_after=80,
                line_spacing=240,
                bold=True,
                size_half_pt=22
            ))
            i += 1
            continue

        body_elements.append(make_paragraph_xml(
            stripped,
            align="both",
            first_line_twips=709,
            space_before=0,
            space_after=120,
            line_spacing=360,
            size_half_pt=24
        ))
        i += 1

    sect_pr = """
    <w:sectPr>
      <w:pgSz w:w="11906" w:h="16838"/>
      <w:pgMar w:top="1701" w:right="1134" w:bottom="1134" w:left="1701" w:header="709" w:footer="709" w:gutter="0"/>
    </w:sectPr>
    """

    document_xml = f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:m="http://schemas.openxmlformats.org/officeDocument/2006/math">
  <w:body>
    {''.join(body_elements)}
    {sect_pr}
  </w:body>
</w:document>"""

    content_types_xml = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>"""

    rels_xml = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>"""

    os.makedirs(os.path.dirname(docx_path), exist_ok=True)
    with zipfile.ZipFile(docx_path, "w", zipfile.ZIP_DEFLATED) as zf:
        zf.writestr("[Content_Types].xml", content_types_xml)
        zf.writestr("_rels/.rels", rels_xml)
        zf.writestr("word/document.xml", document_xml)

    print(f"Arquivo Word com equacoes nativas OMML gerado com sucesso: {docx_path}")

if __name__ == "__main__":
    import sys
    base_dir = os.path.dirname(os.path.abspath(__file__))
    md_file = sys.argv[1] if len(sys.argv) > 1 else os.path.join(base_dir, "tcc_completo_base.md")
    docx_file = sys.argv[2] if len(sys.argv) > 2 else os.path.join(base_dir, "TCC_Vigas_Mistas_Protendidas_NBR8800_2024.docx")
    convert_md_to_docx(md_file, docx_file)
