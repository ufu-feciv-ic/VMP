#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "modelos.h"
#include "catalogo.h"
#include "geometria.h"
#include "rotinas_nbr8800.h"
#include "rotinas_vmp.h"
#include "verificador.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

// Fontes globais (estilo NLtool: Segoe UI Regular + Bold no ImGui e fonte anti-aliased no Raylib)
ImFont* g_fontRegular = nullptr;
ImFont* g_fontBold    = nullptr;
Font    g_rlFont      = { 0 };
bool    g_rlFontLoaded = false;

void DrawTextClean(const char* text, float x, float y, float fontSize, Color tint) {
    if (g_rlFontLoaded) {
        DrawTextEx(g_rlFont, text, Vector2{ std::round(x), std::round(y) }, fontSize, 0.5f, tint);
    } else {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), tint);
    }
}

float MeasureTextClean(const char* text, float fontSize) {
    if (g_rlFontLoaded) {
        return MeasureTextEx(g_rlFont, text, fontSize, 0.5f).x;
    }
    return static_cast<float>(MeasureText(text, static_cast<int>(fontSize)));
}

void PushBoldFont() {
    if (g_fontBold) ImGui::PushFont(g_fontBold);
}

void PopBoldFont() {
    if (g_fontBold) ImGui::PopFont();
}

// ============================================================================
// BANCO DE PERFIS COMERCIAIS SÉRIE W (ABNT NBR 15980 / GERDAU)
// ============================================================================
struct ItemCatalogoPerfil {
    const char* nome;
    double d;
    double bf;
    double tf;
    double tw;
    double Ag;
    double Ix;
    double Wx;
    double rx;
    double Zx;
    double Iy;
    double Wy;
    double ry;
    double Zy;
    double It;
    double Cw;
};

constexpr ItemCatalogoPerfil CATALOGO_SERIE_W[] = {
    { "W 310 x 38,7", 310.0, 165.0,  9.7,  5.8,  4940.0,  8510e4,  549.0e3, 131.0,  610.0e3,  727e4,  88.1e3, 38.4, 134.0e3, 13.1e4,  162000e6 },
    { "W 360 x 44,0", 352.0, 171.0,  9.8,  6.9,  5730.0, 12258e4,  696.5e3, 146.0,  779.0e3,  818e4,  95.7e3, 37.8, 148.0e3, 16.4e4,  238000e6 },
    { "W 360 x 57,8", 358.0, 172.0, 13.1,  7.9,  7250.0, 16143e4,  901.8e3, 142.0, 1014.8e3, 1113e4, 129.4e3, 39.2, 199.8e3, 34.45e4, 330395.1e6 },
    { "W 410 x 60,0", 407.0, 178.0, 12.8,  7.7,  7600.0, 21600e4, 1061.0e3, 169.0, 1194.0e3, 1200e4, 135.0e3, 39.7, 209.0e3, 33.8e4,  467000e6 },
    { "W 460 x 74,0", 457.0, 190.0, 14.5,  9.0,  9460.0, 33300e4, 1457.0e3, 188.0, 1650.0e3, 1660e4, 175.0e3, 41.9, 271.0e3, 53.3e4,  817000e6 },
    { "W 460 x 89,0", 463.0, 192.0, 17.7, 10.5, 11410.0, 41105e4, 1775.6e3, 189.8, 2019.4e3, 2093e4, 218.0e3, 42.8, 339.0e3, 92.49e4, 1035073e6 },
    { "W 530 x 85,0", 535.0, 166.0, 16.5, 10.3, 10860.0, 48500e4, 1813.0e3, 211.0, 2100.0e3, 1260e4, 152.0e3, 34.1, 240.0e3, 75.8e4,  849000e6 },
    { "W 610 x 101,0", 603.0, 228.0, 14.9, 10.5, 12950.0, 76400e4, 2534.0e3, 243.0, 2900.0e3, 2950e4, 259.0e3, 47.7, 402.0e3, 79.5e4, 2520000e6 }
};

void aplicar_perfil_catalogo(PerfilI& p, int idx) {
    const auto& item = CATALOGO_SERIE_W[idx];
    p.nome = item.nome;
    p.tipo = LAMINADO;
    p.d = item.d;
    p.bf = item.bf;
    p.tf = item.tf;
    p.tw = item.tw;
    p.r = 0.0;
    p.h = item.d - 2.0 * item.tf;
    p.d1 = p.h - 24.0;
    p.Ag = item.Ag;
    p.Ix = item.Ix;
    p.Wx = item.Wx;
    p.rx = item.rx;
    p.Zx = item.Zx;
    p.Iy = item.Iy;
    p.Wy = item.Wy;
    p.ry = item.ry;
    p.Zy = item.Zy;
    p.It = item.It;
    p.Cw = item.Cw;
    p.lbf = p.bf / (2.0 * p.tf);
    p.lbw = p.d1 / p.tw;
}

// ============================================================================
// ESTADO DA INTERFACE (ARQUITETURA NLtool: TopToolbar + LeftPanel + Viewport)
// ============================================================================
struct EstadoInterfaceVMP {
    char nome_projeto[128] = "Viga Mista Protendida - Piso Industrial";
    char data_projeto[32]  = "21/11/2026";
    int edicao_norma_idx   = 1; // 0 = ABNT NBR 8800:2008, 1 = ABNT NBR 8800:2024

    // Protensão
    int tipo_tracado_idx   = 0; // 0 = Reto, 1 = Poligonal
    int tipo_tracao_idx    = 0; // 0 = Pré-tração, 1 = Pós-tração
    int aco_prot_idx       = 0; // 0 = CP-190 RB, 1 = CP-210 RB, 2 = CP-175 RB
    int cordoalha_idx      = 0; // 0 = 15.2 mm (140 mm²), 1 = 12.7 mm (98.7 mm²)
    int bainha_idx         = 0; // 0 = Polipropileno lubrificada, 1 = Metálica
    bool ativa_ativa       = false;
    bool informar_p0_manual = true;
    double P0_manual_kN    = 400.0;

    // Armaduras Passivas (Asl)
    double Asl_inf_cm2_m   = 5.0;
    double Asl_sup_cm2_m   = 5.0;
    double As_fis_cm2_m    = 1.96;
    double d_linha_mm      = 20.0;
    int aco_passivo_idx    = 1; // 0 = CA-25, 1 = CA-50, 2 = CA-60

    // Geometria do pavimento
    double Lesq_mm         = 3000.0;
    double Ldir_mm         = 3000.0;
    bool viga_borda_esq    = false;
    bool viga_borda_dir    = false;
    double a_enrij_mm      = 0.0;

    // Laje de concreto / Steel Deck
    int tipo_laje_idx      = 0; // 0 = Maciça, 1 = Mista (Steel Deck)
    int nervuras_idx       = 0; // 0 = Perpendicular, 1 = Paralela
    int forma_idx          = 0; // 0 = MF-50, 1 = MF-75, 2 = Polydeck 59S
    int esp_forma_idx      = 0; // 0 = 0,80 mm, 1 = 0,95 mm, 2 = 1,25 mm
    int agregado_idx       = 0; // 0 = Granito, 1 = Basalto, 2 = Calcário, 3 = Arenito
    int classe_conc_idx    = 2; // 0 = C20, 1 = C25, 2 = C30, 3 = C35, 4 = C40, 5 = C50
    int aco_forma_idx      = 0; // 0 = ZAR 280, 1 = ZAR 345

    // Perfil I de aço
    int modo_perfil_idx     = 0; // 0 = Laminado (Série W), 1 = Soldado, 2 = Customizado
    int perfil_catalogo_idx = 2; // W 360 x 57,8
    int aco_perfil_idx      = 0; // 0 = ASTM A572 Gr 50, 1 = ASTM A36, 2 = USI CIVIL 350

    // Carregamentos
    int modo_entrada_carga = 0; // 0 = Por área (kN/m²), 1 = Linear direta (kN/m)
    double carga_perm_kNm2 = 2.0;
    double carga_var_kNm2  = 5.0;
    double carga_cons_kNm2 = 1.0;
    int uso_estrutura_idx  = 0;
    int comb_servico_idx   = 0;

    // Auxílios Visuais e Opções de Viewport (Linguagem NLtool)
    bool exibir_grade         = true;
    bool exibir_cotas         = true;
    bool exibir_pontos_discr  = true;
    bool exibir_cargas_viga   = true;
    bool diag_preenchimento   = true;
    bool diag_valores         = true;
    float escala_cargas       = 1.0f;
    float escala_diagrama     = 1.0f;
    float escala_fonte_ui     = 1.0f;

    // Estado de Inspeção de Resultados (NLtool ResultsTab & LeftPanel)
    int fase_ativa_idx        = 1; // 0 = ELU 1 (Construção), 1 = ELU 2 (Serviço t=0), 2 = ELU 3 (Serviço t=inf), 3 = ELS
    int tipo_diagrama_idx     = 1; // 0 = Nenhum, 1 = Momento Fletor (MSd vs MRd), 2 = Cortante (VSd vs VRd), 3 = Protensão P(x), 4 = Interação Flexo-Compressão (Ia)
    int secao_monitorada_idx  = 2; // Índice do nó/seção selecionado na barra
    int aba_left_panel_idx    = 0; // 0 = Propriedades, 1 = Resultados da Seção

    // Controle de abertura de Modais (estilo NLtool: SectionDialog, MaterialDialog, etc.)
    bool abrir_modal_perfil      = false;
    bool abrir_modal_laje        = false;
    bool abrir_modal_armadura    = false;
    bool abrir_modal_comparativo = false;
    bool abrir_modal_sobre       = false;
};

void configurar_estilo_nltool() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding      = 3.0f;
    style.TabRounding       = 4.0f;
    style.FrameBorderSize   = 1.0f;
    style.WindowBorderSize  = 1.0f;
    style.WindowPadding     = ImVec2(8.0f, 6.0f);
    style.ItemSpacing       = ImVec2(7.0f, 5.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ImVec4(0.11f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.48f, 0.53f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.99f, 0.99f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.77f, 0.81f, 0.86f, 1.00f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.95f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.84f, 0.91f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.13f, 0.23f, 0.36f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.10f, 0.20f, 0.33f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.21f, 0.34f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.84f, 0.90f, 0.98f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.75f, 0.85f, 0.98f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.66f, 0.80f, 0.97f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.15f, 0.40f, 0.73f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.20f, 0.48f, 0.84f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.11f, 0.33f, 0.63f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.14f, 0.40f, 0.76f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.15f, 0.40f, 0.73f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.11f, 0.33f, 0.63f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.86f, 0.89f, 0.94f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.73f, 0.83f, 0.96f, 1.00f);
    colors[ImGuiCol_TabSelected]           = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TabDimmed]             = ImVec4(0.86f, 0.89f, 0.94f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.95f, 0.97f, 1.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.88f, 0.92f, 0.96f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.73f, 0.78f, 0.84f, 1.00f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.85f, 0.88f, 0.92f, 1.00f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.95f, 0.97f, 1.00f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.08f, 0.12f, 0.18f, 0.45f);
}

void sincronizar_modelo_viga(VigaVMP& viga, EstadoInterfaceVMP& ui) {
    viga.norma = (ui.edicao_norma_idx == 0) ? NBR8800_2008 : NBR8800_2024;
    viga.cabo.tracado = (ui.tipo_tracado_idx == 0) ? CABO_RETO : CABO_POLIGONAL;
    viga.cabo.tipo = (ui.tipo_tracao_idx == 0) ? PRE_TRACAO : POS_TRACAO;
    viga.cabo.ancoragem = ui.ativa_ativa ? ATIVA_ATIVA : ATIVA_PASSIVA;

    if (ui.aco_prot_idx == 0) {
        viga.cabo.cabo = CP_190_RB;
        viga.cabo.fpk = 1864.0;
    } else if (ui.aco_prot_idx == 1) {
        viga.cabo.cabo = CP_210_RB;
        viga.cabo.fpk = 2060.0;
    } else {
        viga.cabo.cabo = CP_175_RB;
        viga.cabo.fpk = 1750.0;
    }
    viga.cabo.fpyk = 0.90 * viga.cabo.fpk;
    viga.cabo.Ep = 200000.0;

    if (ui.cordoalha_idx == 0) {
        viga.cabo.cordoalha = CORDOALHA_15_2;
        viga.cabo.diametro = 15.2;
        viga.cabo.area_unit = 140.0;
    } else {
        viga.cabo.cordoalha = CORDOALHA_12_7;
        viga.cabo.diametro = 12.7;
        viga.cabo.area_unit = 98.7;
    }

    viga.P0_adotado = ui.informar_p0_manual ? ui.P0_manual_kN : 0.0;
    viga.cabo.Ap = 0.0;

    viga.a_vigas_mm = 0.5 * (ui.Lesq_mm + ui.Ldir_mm);
    bool eh_borda = (ui.viga_borda_esq || ui.viga_borda_dir);
    viga.alpha_viga = eh_borda ? 2.0 : 3.0;

    double bef_esq = ui.viga_borda_esq ? 0.0 : std::min(viga.L / 8.0, ui.Lesq_mm / 2.0);
    double bef_dir = ui.viga_borda_dir ? 0.0 : std::min(viga.L / 8.0, ui.Ldir_mm / 2.0);
    if (bef_esq + bef_dir > 0.0) {
        viga.laje.bef_mm = bef_esq + bef_dir;
    }

    viga.laje.tipo = (ui.tipo_laje_idx == 0) ? LAJE_MACICA : LAJE_STEEL_DECK;
    viga.laje.nervura_paralela = (ui.nervuras_idx == 1);
    if (viga.laje.tipo == LAJE_MACICA) {
        viga.laje.hF_mm = 0.0;
        viga.laje.bF_mm = 0.0;
        viga.laje.eF_mm = 0.0;
    } else {
        if (ui.forma_idx == 0) { // MF-50
            viga.laje.hF_mm = 50.0;
            viga.laje.bF_mm = 130.0;
            viga.laje.eF_mm = 305.0;
        } else if (ui.forma_idx == 1) { // MF-75
            viga.laje.hF_mm = 75.0;
            viga.laje.bF_mm = 132.0;
            viga.laje.eF_mm = 280.0;
        } else { // Polydeck 59S
            viga.laje.hF_mm = 59.0;
            viga.laje.bF_mm = 115.0;
            viga.laje.eF_mm = 212.0;
        }
    }

    constexpr double FCK_VALS[] = { 20.0, 25.0, 30.0, 35.0, 40.0, 50.0 };
    constexpr const char* FCK_NAMES[] = { "C20", "C25", "C30", "C35", "C40", "C50" };
    constexpr double ALPHA_E_AGREG[] = { 1.0, 1.2, 0.9, 0.7 };
    viga.concreto.fck_MPa = FCK_VALS[ui.classe_conc_idx];
    viga.concreto.classe = FCK_NAMES[ui.classe_conc_idx];
    double alpha_e_ag = ALPHA_E_AGREG[ui.agregado_idx];
    double Eci = alpha_e_ag * 5600.0 * std::sqrt(viga.concreto.fck_MPa);
    double alpha_i = std::min(0.8 + 0.2 * (viga.concreto.fck_MPa / 80.0), 1.0);
    viga.concreto.Ecs_MPa = std::round(alpha_i * Eci);
    viga.concreto.fctm_MPa = 0.30 * std::pow(viga.concreto.fck_MPa, 2.0 / 3.0);

    constexpr double FYS_VALS[] = { 250.0, 500.0, 600.0 };
    viga.armacao_laje.As = ui.Asl_sup_cm2_m * 100.0;
    viga.armacao_laje.Ads = viga.armacao_laje.As * (viga.laje.bef_mm / 1000.0);
    viga.armacao_laje.fsd = FYS_VALS[ui.aco_passivo_idx] / 1.15;
    viga.armacao_laje.d_linha_mm = ui.d_linha_mm;

    if (ui.modo_perfil_idx == 0) {
        aplicar_perfil_catalogo(viga.perfil, ui.perfil_catalogo_idx);
    } else {
        viga.perfil.tipo = (ui.modo_perfil_idx == 1) ? SOLDADO : LAMINADO;
        calcular_propriedades_secao_i(viga.perfil);
    }

    if (ui.aco_perfil_idx == 0) {
        viga.aco.nome = "ASTM A572 Gr. 50";
        viga.aco.fy_MPa = 345.0;
        viga.aco.fu_MPa = 450.0;
    } else if (ui.aco_perfil_idx == 1) {
        viga.aco.nome = "ASTM A36";
        viga.aco.fy_MPa = 250.0;
        viga.aco.fu_MPa = 400.0;
    } else {
        viga.aco.nome = "USI CIVIL 350";
        viga.aco.fy_MPa = 350.0;
        viga.aco.fu_MPa = 500.0;
    }
    viga.aco.E_MPa = 200000.0;
    viga.aco.G_MPa = 7700.0;

    if (ui.modo_entrada_carga == 0) {
        double larg_infl_m = 0.5 * (ui.Lesq_mm + ui.Ldir_mm) / 1000.0;
        viga.cargas.q1_perfil = (viga.perfil.Ag / 1e6) * 77.0;
        double esp_media_conc_m = (viga.laje.tc_mm + 0.5 * viga.laje.hF_mm * (viga.laje.bF_mm > 0.0 ? (viga.laje.bF_mm / viga.laje.eF_mm) : 0.0)) / 1000.0;
        if (viga.laje.tipo == LAJE_MACICA) {
            esp_media_conc_m = viga.laje.tc_mm / 1000.0;
        }
        viga.cargas.q2_concreto = larg_infl_m * esp_media_conc_m * 25.0;
        viga.cargas.qs_construcao = ui.carga_cons_kNm2 * larg_infl_m;
        viga.cargas.q3_revestimento = ui.carga_perm_kNm2 * larg_infl_m;
        viga.cargas.q4_uso = ui.carga_var_kNm2 * larg_infl_m;
    }
}

// ============================================================================
// AUXILIARES DA TOP TOOLBAR (ESTILO RIBBON DO NLtool)
// ============================================================================
void desenhar_divisor_ribbon(float height = 86.0f) {
    ImGui::SameLine(0.0f, 10.0f);
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(p.x, p.y + 2.0f),
        ImVec2(p.x, p.y + height),
        IM_COL32(188, 198, 210, 255),
        1.0f
    );
    ImGui::Dummy(ImVec2(8.0f, height));
    ImGui::SameLine(0.0f, 4.0f);
}

void cabecalho_grupo_ribbon(const char* titulo) {
    PushBoldFont();
    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "%s", titulo);
    PopBoldFont();
}

// ============================================================================
// DESENHO TÉCNICO 2D NO VIEWPORT CAD (ESTILO NLtool + VMPCalc)
// ============================================================================
void desenhar_grade_viewport(Rectangle area) {
    Color corGradeFina = GetColor(0xE8EEF5FF);
    Color corGradeGrossa = GetColor(0xD8E2EDFF);
    int passo = 24;
    int idx = 0;
    for (int x = static_cast<int>(area.x); x < static_cast<int>(area.x + area.width); x += passo, ++idx) {
        DrawLine(x, static_cast<int>(area.y), x, static_cast<int>(area.y + area.height), (idx % 4 == 0) ? corGradeGrossa : corGradeFina);
    }
    idx = 0;
    for (int y = static_cast<int>(area.y); y < static_cast<int>(area.y + area.height); y += passo, ++idx) {
        DrawLine(static_cast<int>(area.x), y, static_cast<int>(area.x + area.width), y, (idx % 4 == 0) ? corGradeGrossa : corGradeFina);
    }
}

void desenhar_cota_vertical(int x, int y1, int y2, const char* texto, Color cor) {
    DrawLine(x, y1, x, y2, cor);
    DrawLine(x - 4, y1, x + 4, y1, cor);
    DrawLine(x - 4, y2, x + 4, y2, cor);
    int ym = (y1 + y2) / 2;
    float tw = MeasureTextClean(texto, 16.0f);
    DrawTextClean(texto, static_cast<float>(x) - tw - 7.0f, static_cast<float>(ym - 8), 16.0f, cor);
}

void desenhar_cota_horizontal(int x1, int x2, int y, const char* texto, Color cor) {
    DrawLine(x1, y, x2, y, cor);
    DrawLine(x1, y - 4, x1, y + 4, cor);
    DrawLine(x2, y - 4, x2, y + 4, cor);
    int xm = (x1 + x2) / 2;
    float tw = MeasureTextClean(texto, 16.0f);
    DrawTextClean(texto, static_cast<float>(xm) - tw * 0.5f, static_cast<float>(y - 19), 16.0f, cor);
}

void desenhar_textura_concreto(int rx, int ry, int rw, int rh) {
    Color corBrita = GetColor(0x788492AA);
    int passo_x = std::max(14, rw / 18);
    int passo_y = std::max(10, rh / 4);
    for (int y = ry + 6; y < ry + rh - 5; y += passo_y) {
        for (int x = rx + 8; x < rx + rw - 8; x += passo_x) {
            int ox = ((x * 17 + y * 31) % 7) - 3;
            int oy = ((x * 23 + y * 13) % 5) - 2;
            DrawCircle(x + ox, y + oy, 1.3f, corBrita);
            if ((x + y) % 3 == 0) {
                DrawTriangle(
                    Vector2{ static_cast<float>(x + ox + 4), static_cast<float>(y + oy - 2) },
                    Vector2{ static_cast<float>(x + ox + 2), static_cast<float>(y + oy + 2) },
                    Vector2{ static_cast<float>(x + ox + 6), static_cast<float>(y + oy + 2) },
                    corBrita
                );
            }
        }
    }
}

void desenhar_secao_transversal_tecnica(
    const VigaVMP& viga,
    const RelatorioVMP& rel,
    bool exibir_cotas,
    Rectangle area
) {
    const PerfilI& p = viga.perfil;
    const Laje& l = viga.laje;

    double h_total_mm = p.d + l.hF_mm + l.tc_mm + std::max(0.0, viga.cabo.ep);
    if (h_total_mm <= 0.0) return;

    float escala = (area.height * 0.62f) / static_cast<float>(h_total_mm);
    float d = static_cast<float>(p.d) * escala;
    float bf = static_cast<float>(p.bf) * escala;
    float tf = std::max(3.5f, static_cast<float>(p.tf) * escala);
    float tw = std::max(2.5f, static_cast<float>(p.tw) * escala);
    float tc = static_cast<float>(l.tc_mm) * escala;
    float hF = (l.tipo == LAJE_STEEL_DECK) ? (static_cast<float>(l.hF_mm) * escala) : 0.0f;

    int cx = static_cast<int>(area.x + area.width * 0.50f);
    int y_topo_laje = static_cast<int>(area.y + 52.0f);
    int y_base_capa_laje = y_topo_laje + static_cast<int>(tc);
    int y_topo_perfil = y_base_capa_laje + static_cast<int>(hF);
    int y_base_perfil = y_topo_perfil + static_cast<int>(d);

    float larg_laje_vis = std::clamp(bf * 2.7f, 200.0f, area.width * 0.64f);
    int x_laje_esq = cx - static_cast<int>(larg_laje_vis / 2.0f);
    int x_laje_dir = cx + static_cast<int>(larg_laje_vis / 2.0f);
    int w_laje = x_laje_dir - x_laje_esq;

    Color corConcretoFundo = GetColor(0xEAEEF3FF);
    Color corBorda = GetColor(0x2B3440FF);
    Color corPerfilAco = GetColor(0xB71C1CFF);
    Color corPerfilBorda = GetColor(0x5F0909FF);

    // 1. Laje Maciça ou Mista (Steel Deck com nervuras trapezoidais)
    DrawRectangle(x_laje_esq, y_topo_laje, w_laje, static_cast<int>(tc), corConcretoFundo);
    desenhar_textura_concreto(x_laje_esq, y_topo_laje, w_laje, static_cast<int>(tc));

    if (l.tipo == LAJE_MACICA || hF <= 1.0f) {
        DrawRectangleLines(x_laje_esq, y_topo_laje, w_laje, static_cast<int>(tc), corBorda);
    } else {
        DrawLine(x_laje_esq, y_topo_laje, x_laje_dir, y_topo_laje, corBorda);
        DrawLine(x_laje_esq, y_topo_laje, x_laje_esq, y_base_capa_laje, corBorda);
        DrawLine(x_laje_dir, y_topo_laje, x_laje_dir, y_base_capa_laje, corBorda);

        int num_ondas = 5;
        float passo_onda = static_cast<float>(w_laje) / static_cast<float>(num_ondas);
        for (int k = 0; k < num_ondas; ++k) {
            float x0 = static_cast<float>(x_laje_esq) + k * passo_onda;
            float x1 = x0 + passo_onda * 0.18f;
            float x2 = x0 + passo_onda * 0.52f;
            float x3 = x0 + passo_onda * 0.70f;
            float x4 = x0 + passo_onda;

            Vector2 v_top_l = { x0, static_cast<float>(y_base_capa_laje) };
            Vector2 v_bot_l = { x1, static_cast<float>(y_topo_perfil) };
            Vector2 v_bot_r = { x2, static_cast<float>(y_topo_perfil) };
            Vector2 v_top_r = { x3, static_cast<float>(y_base_capa_laje) };

            DrawTriangle(v_top_l, v_bot_l, v_bot_r, corConcretoFundo);
            DrawTriangle(v_top_l, v_bot_r, v_top_r, corConcretoFundo);
            DrawCircle(static_cast<int>((x1 + x2) * 0.5f), static_cast<int>((y_base_capa_laje + y_topo_perfil) * 0.5f), 1.3f, GetColor(0x788492AA));

            Color corChapaDeck = GetColor(0x1E3A5FFF);
            DrawLineEx(v_top_l, v_bot_l, 2.2f, corChapaDeck);
            DrawLineEx(v_bot_l, v_bot_r, 2.5f, corChapaDeck);
            DrawLineEx(v_bot_r, v_top_r, 2.2f, corChapaDeck);
            DrawLineEx(v_top_r, Vector2{ x4, static_cast<float>(y_base_capa_laje) }, 2.2f, corChapaDeck);
        }
    }

    // 2. Armaduras passivas longitudinais
    int y_arm_sup = y_topo_laje + std::max(6, static_cast<int>(viga.armacao_laje.d_linha_mm * escala));
    int y_arm_inf = y_base_capa_laje - 6;
    DrawLine(x_laje_esq + 10, y_arm_sup, x_laje_dir - 10, y_arm_sup, GetColor(0x212B36CC));
    for (int x_bar = x_laje_esq + 16; x_bar <= x_laje_dir - 16; x_bar += 22) {
        DrawCircle(x_bar, y_arm_sup, 2.5f, BLACK);
        DrawCircle(x_bar, y_arm_inf, 2.0f, GetColor(0x454F5BFF));
    }

    // 3. Perfil I de aço
    int x_mesa_esq = cx - static_cast<int>(bf / 2.0f);
    int x_alma_esq = cx - static_cast<int>(tw / 2.0f);
    int h_alma = static_cast<int>(d - 2.0f * tf);

    DrawRectangle(x_mesa_esq, y_topo_perfil, static_cast<int>(bf), static_cast<int>(tf), corPerfilAco);
    DrawRectangle(x_alma_esq, y_topo_perfil + static_cast<int>(tf), static_cast<int>(tw), h_alma, corPerfilAco);
    DrawRectangle(x_mesa_esq, y_base_perfil - static_cast<int>(tf), static_cast<int>(bf), static_cast<int>(tf), corPerfilAco);

    DrawRectangleLines(x_mesa_esq, y_topo_perfil, static_cast<int>(bf), static_cast<int>(tf), corPerfilBorda);
    DrawRectangleLines(x_alma_esq, y_topo_perfil + static_cast<int>(tf), static_cast<int>(tw), h_alma, corPerfilBorda);
    DrawRectangleLines(x_mesa_esq, y_base_perfil - static_cast<int>(tf), static_cast<int>(bf), static_cast<int>(tf), corPerfilBorda);

    // 4. Linha Neutra Elástica (LNE t=0)
    int y_lne = y_base_perfil - static_cast<int>(rel.secao_t0.ytr_mm * escala);
    for (int x_tr = x_laje_esq - 12; x_tr < x_laje_dir + 12; x_tr += 14) {
        DrawLine(x_tr, y_lne, x_tr + 8, y_lne, GetColor(0x00796BFF));
    }
    char txt_lne[64];
    std::snprintf(txt_lne, sizeof(txt_lne), "LNE (ytr=%.0f mm)", rel.secao_t0.ytr_mm);
    DrawTextClean(txt_lne, static_cast<float>(x_laje_dir + 8), static_cast<float>(y_lne - 8), 15.5f, GetColor(0x00695CFF));

    // 5. Cabos de protensão externos
    int y_cabo = y_base_perfil + static_cast<int>(viga.cabo.ep * escala);
    int dist_cabo_x = static_cast<int>(bf * 0.28f);
    DrawCircle(cx - dist_cabo_x, y_cabo, 4.5f, BLACK);
    DrawCircle(cx + dist_cabo_x, y_cabo, 4.5f, BLACK);
    DrawCircleLines(cx - dist_cabo_x, y_cabo, 5.5f, GOLD);
    DrawCircleLines(cx + dist_cabo_x, y_cabo, 5.5f, GOLD);

    // 6. Cotas técnicas
    if (exibir_cotas) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "bef = %.0f mm", l.bef_mm);
        desenhar_cota_horizontal(x_laje_esq, x_laje_dir, y_topo_laje - 12, buf, corBorda);

        std::snprintf(buf, sizeof(buf), "tc = %.0f", l.tc_mm);
        desenhar_cota_vertical(x_laje_dir + 14, y_topo_laje, y_base_capa_laje, buf, corBorda);

        if (l.tipo == LAJE_STEEL_DECK && hF > 1.0f) {
            std::snprintf(buf, sizeof(buf), "hF = %.0f", l.hF_mm);
            desenhar_cota_vertical(x_laje_dir + 14, y_base_capa_laje, y_topo_perfil, buf, GetColor(0x1E3A5FFF));
        }

        std::snprintf(buf, sizeof(buf), "d = %.0f mm", p.d);
        desenhar_cota_vertical(x_laje_esq - 12, y_topo_perfil, y_base_perfil, buf, corPerfilBorda);

        std::snprintf(buf, sizeof(buf), "bf = %.0f mm", p.bf);
        desenhar_cota_horizontal(x_mesa_esq, x_mesa_esq + static_cast<int>(bf), y_base_perfil + 24, buf, corPerfilBorda);

        std::snprintf(buf, sizeof(buf), "ep = %.0f", std::abs(viga.cabo.ep));
        int y_cota_ep_1 = std::min(y_base_perfil, y_cabo);
        int y_cota_ep_2 = std::max(y_base_perfil, y_cabo);
        if (std::abs(y_cota_ep_2 - y_cota_ep_1) >= 4) {
            desenhar_cota_vertical(x_mesa_esq - 10, y_cota_ep_1, y_cota_ep_2, buf, BLACK);
        }

        std::snprintf(buf, sizeof(buf), "tw=%.1f | tf=%.1f mm", p.tw, p.tf);
        DrawTextClean(buf, static_cast<float>(cx + static_cast<int>(bf / 2.0f) + 10), static_cast<float>((y_topo_perfil + y_base_perfil) / 2 - 8), 15.5f, DARKGRAY);
    }
}

// Retorna o índice do nó sob o cursor do mouse (-1 se nenhum) e atualiza a seleção quando clicado
int desenhar_vista_longitudinal_nltool(
    const VigaVMP& viga,
    const RelatorioVMP& rel,
    EstadoInterfaceVMP& ui,
    Rectangle area,
    Vector2& out_hovered_node_pos
) {
    int x_esq = static_cast<int>(area.x + 64.0f);
    int x_dir = static_cast<int>(area.x + area.width - 64.0f);
    int larg_viga = x_dir - x_esq;
    if (larg_viga <= 60) return -1;

    int y_topo_laje = static_cast<int>(area.y + 82.0f);
    int h_laje_vis = 20;
    int h_perfil_vis = 52;
    int y_topo_perfil = y_topo_laje + h_laje_vis;
    int y_base_perfil = y_topo_perfil + h_perfil_vis;

    const ResultadoFaseELU& fase = (ui.fase_ativa_idx == 0) ? rel.fase1_elu :
                                   ((ui.fase_ativa_idx == 2) ? rel.fase3_elu : rel.fase2_elu);

    // 1. CARREGAMENTOS DISTRIBUÍDOS SOBRE A BARRA (ESTILO NLtool / Ftool)
    if (ui.exibir_cargas_viga) {
        double q_perm = viga.cargas.q1_perfil + viga.cargas.q2_concreto + ((ui.fase_ativa_idx == 0) ? 0.0 : viga.cargas.q3_revestimento);
        double q_var  = (ui.fase_ativa_idx == 0) ? viga.cargas.qs_construcao : viga.cargas.q4_uso;
        int h_seta = static_cast<int>(std::clamp(28.0f * ui.escala_cargas, 14.0f, 46.0f));
        int y_barra_carga = y_topo_laje - 6 - h_seta;
        Color corCarga = GetColor(0x1565C0FF);

        DrawLineEx(Vector2{ static_cast<float>(x_esq), static_cast<float>(y_barra_carga) },
                   Vector2{ static_cast<float>(x_dir), static_cast<float>(y_barra_carga) }, 2.0f, corCarga);

        int num_setas = std::max(8, larg_viga / 36);
        for (int i = 0; i <= num_setas; ++i) {
            int xs = x_esq + (larg_viga * i) / num_setas;
            int y_ponta = y_topo_laje - 4;
            DrawLine(xs, y_barra_carga, xs, y_ponta, corCarga);
            DrawTriangle(
                Vector2{ static_cast<float>(xs), static_cast<float>(y_ponta) },
                Vector2{ static_cast<float>(xs - 3), static_cast<float>(y_ponta - 6) },
                Vector2{ static_cast<float>(xs + 3), static_cast<float>(y_ponta - 6) },
                corCarga
            );
        }

        char txt_q[96];
        std::snprintf(txt_q, sizeof(txt_q), "qG = %.2f kN/m  +  qQ = %.2f kN/m  (Total nominal = %.2f kN/m)",
            q_perm, q_var, q_perm + q_var);
        float tw = MeasureTextClean(txt_q, 16.5f);
        DrawTextClean(txt_q, static_cast<float>(x_esq + larg_viga / 2) - tw * 0.5f, static_cast<float>(y_barra_carga - 21), 16.5f, corCarga);
    }

    // 2. LAJE LONGITUDINAL E CONECTORES DE CISALHAMENTO
    DrawRectangle(x_esq, y_topo_laje, larg_viga, h_laje_vis, GetColor(0xEAEEF3FF));
    desenhar_textura_concreto(x_esq, y_topo_laje, larg_viga, h_laje_vis);
    DrawRectangleLines(x_esq, y_topo_laje, larg_viga, h_laje_vis, GetColor(0x2B3440FF));

    for (int xc = x_esq + 16; xc < x_dir - 12; xc += 26) {
        DrawLine(xc, y_topo_perfil, xc, y_topo_laje + 5, GetColor(0x55606EFF));
        DrawLine(xc - 3, y_topo_laje + 5, xc + 3, y_topo_laje + 5, GetColor(0x55606EFF));
    }

    // 3. PERFIL LONGITUDINAL DE AÇO
    Color corPerfil = GetColor(0xB71C1CFF);
    DrawRectangle(x_esq, y_topo_perfil, larg_viga, 5, corPerfil);
    DrawRectangle(x_esq, y_topo_perfil + 5, larg_viga, h_perfil_vis - 10, GetColor(0xFAFBFDFF));
    DrawRectangle(x_esq, y_base_perfil - 5, larg_viga, 5, corPerfil);
    DrawRectangleLines(x_esq, y_topo_perfil, larg_viga, h_perfil_vis, GetColor(0x5F0909FF));

    // 4. APOIO FIXO (ESQUERDO) E APOIO MÓVEL (DIREITO)
    Vector2 ap_esq_1 = { static_cast<float>(x_esq), static_cast<float>(y_base_perfil) };
    Vector2 ap_esq_2 = { static_cast<float>(x_esq - 11), static_cast<float>(y_base_perfil + 17) };
    Vector2 ap_esq_3 = { static_cast<float>(x_esq + 11), static_cast<float>(y_base_perfil + 17) };
    DrawTriangleLines(ap_esq_1, ap_esq_2, ap_esq_3, BLACK);
    DrawLine(x_esq - 15, y_base_perfil + 17, x_esq + 15, y_base_perfil + 17, BLACK);

    Vector2 ap_dir_1 = { static_cast<float>(x_dir), static_cast<float>(y_base_perfil) };
    Vector2 ap_dir_2 = { static_cast<float>(x_dir - 11), static_cast<float>(y_base_perfil + 14) };
    Vector2 ap_dir_3 = { static_cast<float>(x_dir + 11), static_cast<float>(y_base_perfil + 14) };
    DrawTriangleLines(ap_dir_1, ap_dir_2, ap_dir_3, BLACK);
    DrawLine(x_dir - 15, y_base_perfil + 18, x_dir + 15, y_base_perfil + 18, BLACK);

    // 5. TRAÇADO DO CABO DE PROTENSÃO
    float frac_ep = std::clamp(static_cast<float>(viga.cabo.ep / viga.perfil.d), -0.85f, 0.45f);
    int y_cabo_meio = y_base_perfil + static_cast<int>(frac_ep * h_perfil_vis);
    double X_ep = (viga.cabo.X_ep_mm > 0.0) ? viga.cabo.X_ep_mm : (viga.L / 3.0);

    auto calcular_y_cabo_em_x = [&](double x_mm) -> int {
        if (viga.cabo.tracado == CABO_RETO || X_ep <= 1.0) return y_cabo_meio;
        int y_apoio = y_topo_perfil + 8;
        if (x_mm < X_ep) {
            double t = x_mm / X_ep;
            return static_cast<int>(y_apoio + t * (y_cabo_meio - y_apoio));
        } else if (x_mm > viga.L - X_ep) {
            double t = (viga.L - x_mm) / X_ep;
            return static_cast<int>(y_apoio + t * (y_cabo_meio - y_apoio));
        }
        return y_cabo_meio;
    };

    if (viga.cabo.tracado == CABO_RETO) {
        DrawLineEx(
            Vector2{ static_cast<float>(x_esq), static_cast<float>(y_cabo_meio) },
            Vector2{ static_cast<float>(x_dir), static_cast<float>(y_cabo_meio) },
            2.6f, BLACK
        );
    } else {
        float frac_x = std::clamp(static_cast<float>(X_ep / viga.L), 0.10f, 0.45f);
        int x_desv_1 = x_esq + static_cast<int>(frac_x * larg_viga);
        int x_desv_2 = x_dir - static_cast<int>(frac_x * larg_viga);
        int y_cabo_apoio = y_topo_perfil + 8;

        DrawLineEx(Vector2{ static_cast<float>(x_esq), static_cast<float>(y_cabo_apoio) }, Vector2{ static_cast<float>(x_desv_1), static_cast<float>(y_cabo_meio) }, 2.6f, BLACK);
        DrawLineEx(Vector2{ static_cast<float>(x_desv_1), static_cast<float>(y_cabo_meio) }, Vector2{ static_cast<float>(x_desv_2), static_cast<float>(y_cabo_meio) }, 2.6f, BLACK);
        DrawLineEx(Vector2{ static_cast<float>(x_desv_2), static_cast<float>(y_cabo_meio) }, Vector2{ static_cast<float>(x_dir), static_cast<float>(y_cabo_apoio) }, 2.6f, BLACK);

        if (ui.exibir_cotas) {
            char txt_xep[48];
            std::snprintf(txt_xep, sizeof(txt_xep), "Xep = %.0f mm", X_ep);
            desenhar_cota_horizontal(x_esq, x_desv_1, y_base_perfil + 28, txt_xep, DARKBLUE);
        }
    }

    // 6. DIAGRAMA DE ESFORÇOS SOBREPOSTO / ABAIXO DA VIGA (ESTILO NLtool ResultsDiagrams)
    int y_eixo_diag = y_base_perfil + 68;
    int h_max_diag  = static_cast<int>(std::clamp((area.y + area.height - y_eixo_diag - 34.0f) * ui.escala_diagrama, 45.0f, 135.0f));

    if (ui.tipo_diagrama_idx > 0 && !fase.secoes.empty()) {
        DrawLine(x_esq, y_eixo_diag, x_dir, y_eixo_diag, GetColor(0x55606EFF));

        double max_val = 1.0;
        for (const auto& s : fase.secoes) {
            if (ui.tipo_diagrama_idx == 1) {
                max_val = std::max({ max_val, std::abs(s.MSd_pos_kNm), std::abs(s.MRd_pos_kNm) });
            } else if (ui.tipo_diagrama_idx == 2) {
                max_val = std::max({ max_val, std::abs(s.VSd_max_kN), std::abs(s.VRd_kN) });
            } else if (ui.tipo_diagrama_idx == 3) {
                max_val = std::max(max_val, std::abs(s.P_atuante_kN));
            } else if (ui.tipo_diagrama_idx == 4) {
                max_val = std::max({ max_val, 1.0, s.indice_flexo_compressao_pos, s.indice_flexo_compressao_neg });
            }
        }

        auto obter_val_secao = [&](const ResultadoSecaoELU& s) -> double {
            if (ui.tipo_diagrama_idx == 1) return s.MSd_pos_kNm;
            if (ui.tipo_diagrama_idx == 2) return s.VSd_max_kN;
            if (ui.tipo_diagrama_idx == 3) return s.P_atuante_kN;
            return std::max(s.indice_flexo_compressao_pos, s.indice_flexo_compressao_neg);
        };

        auto obter_lim_secao = [&](const ResultadoSecaoELU& s) -> double {
            if (ui.tipo_diagrama_idx == 1) return s.MRd_pos_kNm;
            if (ui.tipo_diagrama_idx == 2) return s.VRd_kN;
            if (ui.tipo_diagrama_idx == 3) return rel.estimativa_p0.P0_max_kN;
            return 1.00;
        };

        Color corCurva = (ui.tipo_diagrama_idx == 1) ? GetColor(0x1565C0FF) :
                         (ui.tipo_diagrama_idx == 2) ? GetColor(0x6A1B9AFF) :
                         (ui.tipo_diagrama_idx == 3) ? GetColor(0x00838FFF) : GetColor(0xE65100FF);
        Color corFill  = ColorAlpha(corCurva, 0.18f);
        Color corLim   = GetColor(0x2E7D32FF);

        // Desenha patamar resistente/limite tracejado
        double lim0 = obter_lim_secao(fase.secoes[fase.secoes.size() / 2]);
        int y_lim = y_eixo_diag + static_cast<int>((lim0 / max_val) * h_max_diag);
        for (int xd = x_esq; xd < x_dir; xd += 16) {
            DrawLine(xd, y_lim, std::min(xd + 9, x_dir), y_lim, corLim);
        }
        char txt_lim[64];
        if (ui.tipo_diagrama_idx == 1) std::snprintf(txt_lim, sizeof(txt_lim), "MRd+ = %.1f kN.m", lim0);
        else if (ui.tipo_diagrama_idx == 2) std::snprintf(txt_lim, sizeof(txt_lim), "VRd = %.1f kN", lim0);
        else if (ui.tipo_diagrama_idx == 3) std::snprintf(txt_lim, sizeof(txt_lim), "P0,max = %.1f kN", lim0);
        else std::snprintf(txt_lim, sizeof(txt_lim), "Limite NBR 8800 (Ia = 1,00)");
        DrawTextClean(txt_lim, static_cast<float>(x_dir - MeasureTextClean(txt_lim, 15.5f) - 4.0f), static_cast<float>(y_lim + 4), 15.5f, corLim);

        // Preenchimento e curva solicitante ao longo das seções
        for (size_t i = 0; i + 1 < fase.secoes.size(); ++i) {
            double f1 = (viga.L > 0.0) ? (fase.secoes[i].x_mm / viga.L) : 0.0;
            double f2 = (viga.L > 0.0) ? (fase.secoes[i + 1].x_mm / viga.L) : 0.0;
            int x1 = x_esq + static_cast<int>(f1 * larg_viga);
            int x2 = x_esq + static_cast<int>(f2 * larg_viga);

            double v1 = obter_val_secao(fase.secoes[i]);
            double v2 = obter_val_secao(fase.secoes[i + 1]);
            int y1 = y_eixo_diag + static_cast<int>((v1 / max_val) * h_max_diag);
            int y2 = y_eixo_diag + static_cast<int>((v2 / max_val) * h_max_diag);

            if (ui.diag_preenchimento) {
                int n_sub = std::max(4, (x2 - x1) / 4);
                for (int k = 0; k <= n_sub; ++k) {
                    float t = static_cast<float>(k) / static_cast<float>(n_sub);
                    int xs = static_cast<int>(x1 + t * (x2 - x1));
                    int ys = static_cast<int>(y1 + t * (y2 - y1));
                    DrawLine(xs, y_eixo_diag, xs, ys, corFill);
                }
            }
            DrawLineEx(Vector2{ static_cast<float>(x1), static_cast<float>(y1) },
                       Vector2{ static_cast<float>(x2), static_cast<float>(y2) }, 2.4f, corCurva);
        }

        // Valores nos pontos discretizados do diagrama
        for (size_t i = 0; i < fase.secoes.size(); ++i) {
            double f = (viga.L > 0.0) ? (fase.secoes[i].x_mm / viga.L) : 0.0;
            int xs = x_esq + static_cast<int>(f * larg_viga);
            double v = obter_val_secao(fase.secoes[i]);
            int ys = y_eixo_diag + static_cast<int>((v / max_val) * h_max_diag);

            DrawCircle(xs, ys, 4.0f, corCurva);
            if (ui.diag_valores) {
                char txt_v[32];
                if (ui.tipo_diagrama_idx == 4) std::snprintf(txt_v, sizeof(txt_v), "%.3f", v);
                else std::snprintf(txt_v, sizeof(txt_v), "%.1f", v);
                float tw = MeasureTextClean(txt_v, 15.5f);
                DrawTextClean(txt_v, static_cast<float>(xs) - tw * 0.5f, static_cast<float>(ys + 6), 15.5f, corCurva);
            }
        }
    }

    // 7. SEÇÕES TRANSVERSAIS DISCRETIZADAS E INTERAÇÃO COM O MOUSE (ESTILO NLtool)
    Vector2 mouse = GetMousePosition();
    int hovered_idx = -1;
    bool mouse_em_modal = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId);

    for (size_t i = 0; i < rel.malha_secoes.size(); ++i) {
        double frac = (viga.L > 0.0) ? (rel.malha_secoes[i].x_mm / viga.L) : 0.0;
        int xs = x_esq + static_cast<int>(frac * larg_viga);
        int yc = calcular_y_cabo_em_x(rel.malha_secoes[i].x_mm);
        int yn = (y_topo_perfil + y_base_perfil) / 2;

        float dist_mouse = std::hypot(mouse.x - static_cast<float>(xs), mouse.y - static_cast<float>(yn));
        if (!mouse_em_modal && CheckCollisionPointRec(mouse, area) && std::abs(mouse.x - static_cast<float>(xs)) <= 16.0f &&
            mouse.y >= static_cast<float>(y_topo_laje - 15) && mouse.y <= static_cast<float>(y_base_perfil + 25)) {
            hovered_idx = static_cast<int>(i);
            out_hovered_node_pos = Vector2{ static_cast<float>(xs), static_cast<float>(y_topo_laje - 10) };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ui.secao_monitorada_idx = static_cast<int>(i);
                ui.aba_left_panel_idx = 1; // Foca automaticamente nos Resultados da Seção clicada
            }
        }
        (void)dist_mouse;

        bool selecionada = (static_cast<int>(i) == ui.secao_monitorada_idx);
        bool is_hover    = (static_cast<int>(i) == hovered_idx);

        if (selecionada || is_hover) {
            Color corDest = selecionada ? GetColor(0x00897BFF) : GetColor(0x1E88E5FF);
            DrawLineEx(Vector2{ static_cast<float>(xs), static_cast<float>(y_topo_laje - 8) },
                       Vector2{ static_cast<float>(xs), static_cast<float>(y_base_perfil + 8) }, 2.6f, corDest);
            DrawCircle(xs, yn, 6.0f, corDest);
            DrawCircleLines(xs, yn, 8.0f, WHITE);
            DrawCircle(xs, yc, 4.5f, GOLD);
        } else if (ui.exibir_pontos_discr) {
            DrawLine(xs, y_topo_perfil + 5, xs, y_base_perfil - 5, GetColor(0x919EABAA));
            DrawCircle(xs, yn, 4.2f, GetColor(0x37474FFF));
        }

        if (ui.exibir_pontos_discr) {
            char lbl_no[16];
            std::snprintf(lbl_no, sizeof(lbl_no), "S%zu", i);
            float tw = MeasureTextClean(lbl_no, 15.0f);
            DrawTextClean(lbl_no, static_cast<float>(xs) - tw * 0.5f, static_cast<float>(y_topo_laje - 23), 15.0f,
                selecionada ? GetColor(0x00695CFF) : GetColor(0x455A64FF));
        }
    }

    // 8. Cota do Vão Total Lv
    if (ui.exibir_cotas) {
        char txt_lv[80];
        std::snprintf(txt_lv, sizeof(txt_lv), "Vao Total Lv = %.0f mm (%d secoes discretizadas)", viga.L, static_cast<int>(rel.malha_secoes.size()));
        desenhar_cota_horizontal(x_esq, x_dir, y_base_perfil + 45, txt_lv, GetColor(0x2B3440FF));
    }

    return hovered_idx;
}

} // namespace

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1520, 900, "VMPCalc | Verificacao de Vigas Mistas Protendidas (ABNT NBR 8800:2024)");
    SetTargetFPS(60);

    // 1. Inicialização de Fontes Multi-Peso estilo NLtool (Segoe UI Regular + Bold com suporte UTF-8 completo)
    rlImGuiBeginInitImGui();
    ImGuiIO& io = ImGui::GetIO();
    static const ImWchar ranges_pt_br[] = {
        0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement (acentos PT-BR)
        0x0370, 0x03FF, // Alfabeto Grego (λ, Δ, σ, α, ψ)
        0x2000, 0x206F,
        0x2200, 0x22FF, // Operadores matemáticos (∞, ≤, ≥)
        0,
    };
    if (FileExists("C:/Windows/Fonts/segoeui.ttf")) {
        g_fontRegular = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 19.5f, nullptr, ranges_pt_br);
    } else {
        g_fontRegular = io.Fonts->AddFontDefault();
    }
    if (FileExists("C:/Windows/Fonts/segoeuib.ttf")) {
        g_fontBold = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeuib.ttf", 19.5f, nullptr, ranges_pt_br);
    } else {
        g_fontBold = g_fontRegular;
    }
    rlImGuiEndInitImGui();
    configurar_estilo_nltool();

    if (FileExists("C:/Windows/Fonts/segoeui.ttf")) {
        g_rlFont = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 32, nullptr, 255);
        SetTextureFilter(g_rlFont.texture, TEXTURE_FILTER_BILINEAR);
        g_rlFontLoaded = true;
    }

    EstadoInterfaceVMP ui;
    VigaVMP viga;
    aplicar_perfil_catalogo(viga.perfil, ui.perfil_catalogo_idx);
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje = laje_macica_130();
    viga.armacao_laje = laje_5cm_m();
    viga.cabo = cabo_190RB();
    viga.L = 12000.0;
    viga.Lb = 3000.0;
    viga.num_secoes = 5;

    while (!WindowShouldClose()) {
        int winW = GetScreenWidth();
        int winH = GetScreenHeight();

        sincronizar_modelo_viga(viga, ui);
        RelatorioVMP rel = verificar_viga_vmp(viga);

        if (ui.secao_monitorada_idx < 0 || ui.secao_monitorada_idx >= static_cast<int>(rel.malha_secoes.size())) {
            ui.secao_monitorada_idx = static_cast<int>(rel.malha_secoes.size()) / 2;
        }

        // Métricas de Layout Padrão NLtool:
        // - MainMenuBar no topo (~24 px)
        // - TopToolbar (Ribbon) abaixo da MainMenuBar (altura = 112 px)
        // - LeftPanel à esquerda (largura = 410 px)
        // - Viewport CAD central/direito ocupando todo o restante
        // - Barra de Status inferior (altura = 26 px)
        io.FontGlobalScale = ui.escala_fonte_ui;

        const float menu_bar_h   = 28.0f * ui.escala_fonte_ui;
        const float ribbon_h     = 132.0f * ui.escala_fonte_ui;
        const float top_offset_y = menu_bar_h + ribbon_h;
        const float status_bar_h = 31.0f * ui.escala_fonte_ui;
        const float left_panel_w = std::clamp(winW * 0.32f, 450.0f, 560.0f);
        const float work_h       = std::max(200.0f, static_cast<float>(winH) - top_offset_y - status_bar_h);

        Rectangle rect_viewport = {
            left_panel_w,
            top_offset_y,
            std::max(300.0f, static_cast<float>(winW) - left_panel_w),
            work_h
        };

        BeginDrawing();
        ClearBackground(GetColor(0xF3F6FAFF));

        // ====================================================================
        // RENDERIZAÇÃO DO VIEWPORT CAD 2D (ESTILO NLtool)
        // ====================================================================
        DrawRectangleRec(rect_viewport, WHITE);
        if (ui.exibir_grade) {
            desenhar_grade_viewport(rect_viewport);
        }
        DrawRectangleLinesEx(rect_viewport, 1.0f, GetColor(0xC3CCD6FF));

        // Divisão do Viewport: Vista Longitudinal Interativa + Diagramas (60% esq) e Corte Transversal (40% dir)
        float w_long  = rect_viewport.width * 0.60f;
        float w_trans = rect_viewport.width - w_long;
        Rectangle area_long  = { rect_viewport.x + 4.0f, rect_viewport.y + 30.0f, w_long - 8.0f, rect_viewport.height - 36.0f };
        Rectangle area_trans = { rect_viewport.x + w_long + 4.0f, rect_viewport.y + 30.0f, w_trans - 8.0f, rect_viewport.height - 36.0f };

        DrawLine(static_cast<int>(rect_viewport.x + w_long), static_cast<int>(rect_viewport.y),
                 static_cast<int>(rect_viewport.x + w_long), static_cast<int>(rect_viewport.y + rect_viewport.height),
                 GetColor(0xCFD8E3FF));

        // Cabeçalhos discretos do Viewport CAD
        const char* tit_long = (viga.cabo.tracado == CABO_RETO)
            ? "VISTA LONGITUDINAL DA BARRA & DIAGRAMAS (TRACADO RETILINEO)"
            : "VISTA LONGITUDINAL DA BARRA & DIAGRAMAS (TRACADO POLIGONAL)";
        DrawTextClean(tit_long, area_long.x + 12.0f, rect_viewport.y + 7.0f, 17.5f, GetColor(0x183454FF));

        const char* tit_trans = (viga.laje.tipo == LAJE_MACICA)
            ? "SECAO TRANSVERSAL (LAJE MACICA)"
            : "SECAO TRANSVERSAL (LAJE MISTA - STEEL DECK)";
        DrawTextClean(tit_trans, area_trans.x + 12.0f, rect_viewport.y + 7.0f, 17.5f, GetColor(0x183454FF));

        Vector2 hovered_node_pos = { 0.0f, 0.0f };
        int hovered_node_idx = desenhar_vista_longitudinal_nltool(viga, rel, ui, area_long, hovered_node_pos);
        desenhar_secao_transversal_tecnica(viga, rel, ui.exibir_cotas, area_trans);

        // ====================================================================
        // CAMADA DEAR IMGUI (MainMenuBar + TopToolbar + LeftPanel + Modals)
        // ====================================================================
        rlImGuiBegin();

        // 1. BARRA DE MENU PRINCIPAL (##MainMenuBar)
        if (ImGui::BeginMainMenuBar()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            bool open_arq = ImGui::BeginMenu("Arquivo");
            ImGui::PopStyleColor();
            if (open_arq) {
                if (ImGui::MenuItem("Novo Projeto")) {
                    ui = EstadoInterfaceVMP{};
                    viga.L = 12000.0;
                    viga.Lb = 3000.0;
                    viga.laje.tc_mm = 130.0;
                    viga.cabo.ep = -50.0;
                    viga.cabo.delta_cunha = 6.0;
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Modelos de Referência (Aferição)")) {
                    if (ImGui::MenuItem("Viga Pré-Tracionada com Laje Maciça (W360x57,8 | L = 12 m)")) {
                        ui = EstadoInterfaceVMP{};
                        std::snprintf(ui.nome_projeto, sizeof(ui.nome_projeto), "Viga Mista Pre-Tracionada - Laje Macica (L = 12 m)");
                        ui.tipo_tracao_idx = 0;
                        ui.tipo_tracado_idx = 0;
                        ui.tipo_laje_idx = 0;
                        ui.perfil_catalogo_idx = 2;
                        ui.Lesq_mm = 3000.0;
                        ui.Ldir_mm = 3000.0;
                        ui.informar_p0_manual = true;
                        ui.P0_manual_kN = 400.0;
                        viga.L = 12000.0;
                        viga.Lb = 3000.0;
                        viga.laje.tc_mm = 130.0;
                        viga.cabo.ep = -50.0;
                        viga.cabo.delta_cunha = 6.0;
                    }
                    if (ImGui::MenuItem("Viga Pós-Tracionada com Steel Deck MF-50 (W460x89,0 | L = 15 m)")) {
                        ui = EstadoInterfaceVMP{};
                        std::snprintf(ui.nome_projeto, sizeof(ui.nome_projeto), "Viga Mista Pos-Tracionada - Steel Deck MF-50 (L = 15 m)");
                        ui.tipo_tracao_idx = 1;
                        ui.tipo_tracado_idx = 0;
                        ui.tipo_laje_idx = 1;
                        ui.forma_idx = 0;
                        ui.perfil_catalogo_idx = 5;
                        ui.Lesq_mm = 2000.0;
                        ui.Ldir_mm = 2000.0;
                        ui.informar_p0_manual = false;
                        viga.L = 15000.0;
                        viga.Lb = 3000.0;
                        viga.laje.tc_mm = 130.0;
                        viga.cabo.ep = -50.0;
                        viga.cabo.delta_cunha = 6.0;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            bool open_mat = ImGui::BeginMenu("Materiais e Seções");
            ImGui::PopStyleColor();
            if (open_mat) {
                if (ImGui::MenuItem("Perfil I e Aço Estrutural...")) ui.abrir_modal_perfil = true;
                if (ImGui::MenuItem("Laje de Concreto e Fôrma Steel Deck...")) ui.abrir_modal_laje = true;
                if (ImGui::MenuItem("Armaduras Passivas da Laje (Asl)...")) ui.abrir_modal_armadura = true;
                ImGui::EndMenu();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            bool open_vis = ImGui::BeginMenu("Visualizar");
            ImGui::PopStyleColor();
            if (open_vis) {
                ImGui::MenuItem("Grade no Viewport", nullptr, &ui.exibir_grade);
                ImGui::MenuItem("Cotas Dimensionais", nullptr, &ui.exibir_cotas);
                ImGui::MenuItem("Pontos de Discretização", nullptr, &ui.exibir_pontos_discr);
                ImGui::MenuItem("Carregamentos Distribuídos", nullptr, &ui.exibir_cargas_viga);
                ImGui::Separator();
                ImGui::MenuItem("Preenchimento de Diagramas", nullptr, &ui.diag_preenchimento);
                ImGui::MenuItem("Valores nos Diagramas", nullptr, &ui.diag_valores);
                ImGui::Separator();
                if (ImGui::BeginMenu("Tamanho da Fonte (Escala UI)")) {
                    if (ImGui::MenuItem("Padrão Grande (100%)", nullptr, std::abs(ui.escala_fonte_ui - 1.0f) < 0.02f)) ui.escala_fonte_ui = 1.0f;
                    if (ImGui::MenuItem("Extra Grande (115%)", nullptr, std::abs(ui.escala_fonte_ui - 1.15f) < 0.02f)) ui.escala_fonte_ui = 1.15f;
                    if (ImGui::MenuItem("Máximo (130%)", nullptr, std::abs(ui.escala_fonte_ui - 1.30f) < 0.02f)) ui.escala_fonte_ui = 1.30f;
                    if (ImGui::MenuItem("Compacto (90%)", nullptr, std::abs(ui.escala_fonte_ui - 0.90f) < 0.02f)) ui.escala_fonte_ui = 0.90f;
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            bool open_nbr = ImGui::BeginMenu("Norma");
            ImGui::PopStyleColor();
            if (open_nbr) {
                if (ImGui::MenuItem("ABNT NBR 8800:2024 (Edição Vigente)", nullptr, ui.edicao_norma_idx == 1)) ui.edicao_norma_idx = 1;
                if (ImGui::MenuItem("ABNT NBR 8800:2008 (Edição Anterior)", nullptr, ui.edicao_norma_idx == 0)) ui.edicao_norma_idx = 0;
                ImGui::Separator();
                if (ImGui::MenuItem("Quadro Comparativo NBR 8800:2008 vs 2024...")) ui.abrir_modal_comparativo = true;
                ImGui::EndMenu();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            bool open_sobre = ImGui::BeginMenu("Sobre");
            ImGui::PopStyleColor();
            if (open_sobre) {
                if (ImGui::MenuItem("Sobre o Aplicativo e Metodologia...")) ui.abrir_modal_sobre = true;
                ImGui::EndMenu();
            }

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.82f, 0.91f, 1.0f, 1.0f));
            ImGui::Text("   |   Projeto: %s   |   Norma Ativa: %s",
                ui.nome_projeto, (ui.edicao_norma_idx == 1) ? "ABNT NBR 8800:2024" : "ABNT NBR 8800:2008");
            ImGui::PopStyleColor();

            ImGui::EndMainMenuBar();
        }

        // ====================================================================
        // 2. TOP TOOLBAR - RIBBON POR ABAS DE FLUXO DE TRABALHO (ESTILO NLtool)
        // ====================================================================
        ImGui::SetNextWindowPos(ImVec2(0.0f, menu_bar_h), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(winW), ribbon_h), ImGuiCond_Always);
        ImGui::Begin("TopToolbar", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("TopToolbarTabs")) {

            // ----------------------------------------------------------------
            // ABA 1: PÁGINA INICIAL (TopToolbar::DrawHomeTab)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Página Inicial  ")) {
                // Grupo 1: Projeto e Norma
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Projeto e Norma");
                ImGui::SetNextItemWidth(205.0f);
                ImGui::InputText("##NomeProjRibbon", ui.nome_projeto, sizeof(ui.nome_projeto));
                const char* normas_op[] = { "ABNT NBR 8800:2008", "ABNT NBR 8800:2024" };
                ImGui::SetNextItemWidth(205.0f);
                ImGui::Combo("##NormaRibbon", &ui.edicao_norma_idx, normas_op, 2);
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                // Grupo 2: Materiais e Seções (abre Modais como no NLtool)
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Materiais e Seções");
                if (ImGui::Button("Perfil I e Aço...", ImVec2(128.0f, 26.0f))) ui.abrir_modal_perfil = true;
                ImGui::SameLine();
                if (ImGui::Button("Laje / Steel Deck...", ImVec2(142.0f, 26.0f))) ui.abrir_modal_laje = true;
                ImGui::SameLine();
                if (ImGui::Button("Armadura Asl...", ImVec2(120.0f, 26.0f))) ui.abrir_modal_armadura = true;
                ImGui::TextDisabled("Ativo: %s (%s) | Laje %s (%s, tc=%.0f mm)",
                    viga.perfil.nome.c_str(), viga.aco.nome.c_str(),
                    (viga.laje.tipo == LAJE_MACICA) ? "Maciça" : "Steel Deck",
                    viga.concreto.classe.c_str(), viga.laje.tc_mm);
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                // Grupo 3: Geometria da Barra
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Geometria da Barra");
                ImGui::SetNextItemWidth(110.0f);
                ImGui::InputDouble("Vão Lv (mm)", &viga.L, 500.0, 1000.0, "%.0f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputDouble("Lb (mm)", &viga.Lb, 250.0, 500.0, "%.0f");
                ImGui::TextDisabled("Largura colaborante bef = %.0f mm (%s)",
                    viga.laje.bef_mm, (ui.viga_borda_esq || ui.viga_borda_dir) ? "Viga de Borda" : "Viga Interna");
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                // Grupo 4: Auxílios de Visualização (igual ao grupo Auxílios do NLtool)
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Auxílios");
                ImGui::Checkbox("Grade", &ui.exibir_grade);
                ImGui::SameLine();
                ImGui::Checkbox("Cotas", &ui.exibir_cotas);
                ImGui::Checkbox("Ver pontos", &ui.exibir_pontos_discr);
                ImGui::SameLine();
                ImGui::Checkbox("Cargas", &ui.exibir_cargas_viga);
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA 2: PROTENSÃO
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Protensão  ")) {
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Traçado e Posição do Cabo");
                ImGui::RadioButton("Retilíneo", &ui.tipo_tracado_idx, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Poligonal", &ui.tipo_tracado_idx, 1);
                ImGui::SetNextItemWidth(95.0f);
                ImGui::InputDouble("ep (mm)", &viga.cabo.ep, 5.0, 10.0, "%.1f");
                if (ui.tipo_tracado_idx == 1) {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::InputDouble("Xep (mm)", &viga.cabo.X_ep_mm, 250.0, 500.0, "%.0f");
                }
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Sistema e Ancoragem");
                ImGui::RadioButton("Pré-tração", &ui.tipo_tracao_idx, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Pós-tração", &ui.tipo_tracao_idx, 1);
                ImGui::SameLine();
                ImGui::Checkbox("Ativa-Ativa", &ui.ativa_ativa);
                ImGui::SetNextItemWidth(95.0f);
                ImGui::InputDouble("Cunha Δp (mm)", &viga.cabo.delta_cunha, 0.5, 1.0, "%.1f");
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Aço Ativo e Cordoalhas");
                const char* acos_prot[] = { "CP-190 RB", "CP-210 RB", "CP-175 RB" };
                ImGui::SetNextItemWidth(115.0f);
                ImGui::Combo("Aço", &ui.aco_prot_idx, acos_prot, 3);
                ImGui::SameLine();
                const char* cords_op[] = { "d=15,2 mm (140 mm²)", "d=12,7 mm (98,7 mm²)" };
                ImGui::SetNextItemWidth(165.0f);
                ImGui::Combo("Cordoalha", &ui.cordoalha_idx, cords_op, 2);
                ImGui::Checkbox("Informar P0:", &ui.informar_p0_manual);
                if (ui.informar_p0_manual) {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(105.0f);
                    ImGui::InputDouble("kN##P0Rib", &ui.P0_manual_kN, 25.0, 50.0, "%.1f");
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(P0,máx = %.1f kN | %.0f cord.)",
                    rel.estimativa_p0.P0_max_kN, rel.armadura_protensao.n_cordoalhas);
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA 3: CARREGAMENTOS (TopToolbar::DrawLoadsTab)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Carregamentos  ")) {
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Modo de Entrada");
                ImGui::RadioButton("Por Área (kN/m²)", &ui.modo_entrada_carga, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Linear na Barra (kN/m)", &ui.modo_entrada_carga, 1);
                ImGui::Checkbox("Construção Escorada?", &viga.cargas.escorada);
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Distribuída na Barra");
                if (ui.modo_entrada_carga == 0) {
                    ImGui::SetNextItemWidth(90.0f);
                    ImGui::InputDouble("Perm. (kN/m²)", &ui.carga_perm_kNm2, 0.5, 1.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(90.0f);
                    ImGui::InputDouble("Var. (kN/m²)", &ui.carga_var_kNm2, 0.5, 1.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(90.0f);
                    ImGui::InputDouble("Const. (kN/m²)", &ui.carga_cons_kNm2, 0.2, 0.5, "%.2f");
                    ImGui::TextDisabled("Resultante linear: q1=%.2f | q2=%.2f | qs=%.2f | q3=%.2f | q4=%.2f kN/m",
                        viga.cargas.q1_perfil, viga.cargas.q2_concreto, viga.cargas.qs_construcao,
                        viga.cargas.q3_revestimento, viga.cargas.q4_uso);
                } else {
                    ImGui::SetNextItemWidth(78.0f);
                    ImGui::InputDouble("q1", &viga.cargas.q1_perfil, 0.0, 0.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(78.0f);
                    ImGui::InputDouble("q2", &viga.cargas.q2_concreto, 0.0, 0.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(78.0f);
                    ImGui::InputDouble("qs", &viga.cargas.qs_construcao, 0.0, 0.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(78.0f);
                    ImGui::InputDouble("q3", &viga.cargas.q3_revestimento, 0.0, 0.0, "%.2f");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(78.0f);
                    ImGui::InputDouble("q4 (kN/m)", &viga.cargas.q4_uso, 0.0, 0.0, "%.2f");
                }
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Visualização");
                ImGui::Checkbox("Exibir na barra", &ui.exibir_cargas_viga);
                ImGui::SetNextItemWidth(120.0f);
                ImGui::SliderFloat("Escala Q", &ui.escala_cargas, 0.4f, 2.0f, "%.2fx");
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA 4: DISCRETIZAÇÃO (TopToolbar::DrawDiscretizationTab)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Discretização  ")) {
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Divisões da Barra");
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderInt("Partes / Seções (X)", &viga.num_secoes, 3, 11);
                ImGui::Checkbox("Ver pontos na viga", &ui.exibir_pontos_discr);
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Largura de Influência Transversal");
                ImGui::SetNextItemWidth(105.0f);
                ImGui::InputDouble("Vão Esq. (mm)", &ui.Lesq_mm, 250.0, 500.0, "%.0f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(105.0f);
                ImGui::InputDouble("Vão Dir. (mm)", &ui.Ldir_mm, 250.0, 500.0, "%.0f");
                ImGui::Checkbox("Borda à esquerda", &ui.viga_borda_esq);
                ImGui::SameLine();
                ImGui::Checkbox("Borda à direita", &ui.viga_borda_dir);
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Seção Monitorada");
                if (!rel.malha_secoes.empty()) {
                    ImGui::SetNextItemWidth(220.0f);
                    if (ImGui::BeginCombo("##ComboSecRibbon", rel.malha_secoes[ui.secao_monitorada_idx].nome.c_str())) {
                        for (int i = 0; i < static_cast<int>(rel.malha_secoes.size()); ++i) {
                            bool sel = (ui.secao_monitorada_idx == i);
                            if (ImGui::Selectable(rel.malha_secoes[i].nome.c_str(), sel)) {
                                ui.secao_monitorada_idx = i;
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TextDisabled("Dica: clique em qualquer nó S0..S%zu na viga", rel.malha_secoes.size() - 1);
                }
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA 5: ANÁLISE (TopToolbar::DrawAnalysisTab)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Análise  ")) {
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Parâmetros Normativos");
                const char* normas_op[] = { "ABNT NBR 8800:2008", "ABNT NBR 8800:2024" };
                ImGui::SetNextItemWidth(175.0f);
                ImGui::Combo("Edição", &ui.edicao_norma_idx, normas_op, 2);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75.0f);
                ImGui::InputDouble("ψ1", &viga.cargas.psi1, 0.05, 0.1, "%.2f");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(75.0f);
                ImGui::InputDouble("ψ2", &viga.cargas.psi2, 0.05, 0.1, "%.2f");
                ImGui::TextDisabled("Verificação de 3 Fases Construtivas (ELU 1, ELU 2, ELU 3) + ELS");
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Execução");
                if (ImGui::Button("Executar / Atualizar Análise", ImVec2(200.0f, 30.0f))) {
                    sincronizar_modelo_viga(viga, ui);
                }
                ImGui::SameLine();
                if (ImGui::Button("Comparativo 2008 vs 2024...", ImVec2(195.0f, 30.0f))) {
                    ui.abrir_modal_comparativo = true;
                }
                if (rel.aprovado_global) {
                    ImGui::TextColored(ImVec4(0.08f, 0.56f, 0.24f, 1.0f), "Análise Concluída: Viga APROVADA em todos os Estados Limites.");
                } else {
                    ImGui::TextColored(ImVec4(0.82f, 0.14f, 0.14f, 1.0f), "Análise Concluída: Há verificações reprovadas (inspecione na aba Resultados).");
                }
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA 6: RESULTADOS (TopToolbar::DrawResultsTab)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("  Resultados  ")) {
                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Fase de Verificação");
                ImGui::RadioButton("ELU 1 (Construção)", &ui.fase_ativa_idx, 0);
                ImGui::SameLine();
                ImGui::RadioButton("ELU 2 (Serviço t=0)", &ui.fase_ativa_idx, 1);
                ImGui::SameLine();
                ImGui::RadioButton("ELU 3 (Serviço t=∞)", &ui.fase_ativa_idx, 2);
                ImGui::TextDisabled("Selecione a fase para atualizar os diagramas e o inspetor lateral");
                ImGui::EndGroup();

                desenhar_divisor_ribbon();

                ImGui::BeginGroup();
                cabecalho_grupo_ribbon("Diagramas na Barra");
                const char* diags_op[] = {
                    "Nenhum (Somente Geometria e Cargas)",
                    "Momento Fletor: MSd+(x) vs MRd+",
                    "Esforço Cortante: VSd(x) vs VRd",
                    "Força de Protensão Efetiva: P(x)",
                    "Índice de Flexo-Compressão: Ia(x) <= 1,00"
                };
                ImGui::SetNextItemWidth(275.0f);
                ImGui::Combo("##TipoDiagRibbon", &ui.tipo_diagrama_idx, diags_op, 5);
                ImGui::Checkbox("Preenchimento", &ui.diag_preenchimento);
                ImGui::SameLine();
                ImGui::Checkbox("Valores", &ui.diag_valores);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100.0f);
                ImGui::SliderFloat("Escala", &ui.escala_diagrama, 0.5f, 1.8f, "%.1fx");
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        // ====================================================================
        // 3. LEFT PANEL - INSPETOR LATERAL DE PROPRIEDADES E RESULTADOS (NLtool)
        // ====================================================================
        ImGui::SetNextWindowPos(ImVec2(0.0f, top_offset_y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(left_panel_w, work_h), ImGuiCond_Always);
        ImGui::Begin("LeftPanel", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("LeftPanelTabs")) {

            // ----------------------------------------------------------------
            // ABA INSPETOR 1: PROPRIEDADES DO MODELO (SectionPropertyTable, etc.)
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("Propriedades")) {
                ui.aba_left_panel_idx = 0;

                // Seção 1: Perfil I e Aço (SectionPropertyTable)
                PushBoldFont();
                ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Perfil I e Aço Estrutural");
                PopBoldFont();
                ImGui::SameLine(left_panel_w - 86.0f);
                if (ImGui::SmallButton("Editar...##BtnEditPerfil")) ui.abrir_modal_perfil = true;

                if (ImGui::BeginTable("SectionPropertyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("Propriedade", ImGuiTableColumnFlags_WidthStretch, 1.5f);
                    ImGui::TableSetupColumn("Valor", ImGuiTableColumnFlags_WidthStretch, 1.1f);
                    ImGui::TableSetupColumn("Unid.", ImGuiTableColumnFlags_WidthFixed, 48.0f);
                    ImGui::TableHeadersRow();

                    auto row_str = [](const char* k, const char* v, const char* u) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", k);
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%s", v);
                        ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("%s", u);
                    };
                    auto row_num = [](const char* k, double v, const char* fmt, const char* u) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", k);
                        ImGui::TableSetColumnIndex(1); ImGui::Text(fmt, v);
                        ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("%s", u);
                    };

                    row_str("Seção adotada", viga.perfil.nome.c_str(), (viga.perfil.tipo == LAMINADO) ? "Lam." : "Sold.");
                    row_str("Aço estrutural", viga.aco.nome.c_str(), "-");
                    row_num("Resistência ao escoamento (fy)", viga.aco.fy_MPa, "%.0f", "MPa");
                    row_num("Dimensões (d x bf)", viga.perfil.d, "%.0f", "mm");
                    row_num("Espessuras (tf / tw)", viga.perfil.tf, "%.1f", "mm");
                    row_num("Área bruta (Ag)", viga.perfil.Ag / 100.0, "%.2f", "cm²");
                    row_num("Inércia principal (Ix)", viga.perfil.Ix / 1e4, "%.0f", "cm⁴");
                    row_num("Esbeltez mesa / alma", viga.perfil.lbw, "%.2f", "λw");
                    row_num("Normal resistente (NRd,I)", rel.compressao_aco.NRd_kN, "%.2f", "kN");
                    row_num("Momento resistente (MRd,I)", rel.flexao_aco.MRd_kNm, "%.2f", "kN.m");
                    ImGui::EndTable();
                }

                ImGui::Spacing();

                // Seção 2: Laje e Seção Transformada (MaterialPropertyTable)
                PushBoldFont();
                ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Laje de Concreto e Seção Mista");
                PopBoldFont();
                ImGui::SameLine(left_panel_w - 86.0f);
                if (ImGui::SmallButton("Editar...##BtnEditLaje")) ui.abrir_modal_laje = true;

                if (ImGui::BeginTable("MaterialPropertyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("Propriedade", ImGuiTableColumnFlags_WidthStretch, 1.5f);
                    ImGui::TableSetupColumn("Valor", ImGuiTableColumnFlags_WidthStretch, 1.1f);
                    ImGui::TableSetupColumn("Unid.", ImGuiTableColumnFlags_WidthFixed, 48.0f);
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Tipologia da laje");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", (viga.laje.tipo == LAJE_MACICA) ? "Maciça" : "Mista (Steel Deck)");
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("%s", viga.concreto.classe.c_str());

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Largura efetiva (bef)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", viga.laje.bef_mm);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("mm");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Espessura tc / nervura hF");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f / %.0f", viga.laje.tc_mm, viga.laje.hF_mm);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("mm");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Módulo secante (Ecs)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", viga.concreto.Ecs_MPa);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("MPa");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Inércia transformada (Itr, t=0)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", rel.secao_t0.Itr_mm4 / 1e4);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("cm⁴");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Linha Neutra Elástica (ytr, t=0)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", rel.secao_t0.ytr_mm);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("mm");
                    ImGui::EndTable();
                }

                ImGui::Spacing();

                // Seção 3: Dimensionamento da Protensão (ProtensaoTable)
                PushBoldFont();
                ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Dimensionamento da Protensão");
                PopBoldFont();

                if (ImGui::BeginTable("ProtensaoPropertyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("Grandeza", ImGuiTableColumnFlags_WidthStretch, 1.5f);
                    ImGui::TableSetupColumn("Valor", ImGuiTableColumnFlags_WidthStretch, 1.1f);
                    ImGui::TableSetupColumn("Unid.", ImGuiTableColumnFlags_WidthFixed, 48.0f);
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Sistema / Traçado");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s / %s",
                        (viga.cabo.tipo == PRE_TRACAO) ? "Pré-tração" : "Pós-tração",
                        (viga.cabo.tracado == CABO_RETO) ? "Reto" : "Poligonal");
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("-");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Força máxima (P0,máx)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", rel.estimativa_p0.P0_max_kN);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("kN");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Força inicial adotada (P0)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", rel.P0_efetivo_kN);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("kN");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Armadura ativa (Ap,real)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f (%.0f cord.)", rel.armadura_protensao.Ap_real / 100.0, rel.armadura_protensao.n_cordoalhas);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("cm²");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Força pós-perda imediata (P2)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", rel.P2_fase2_kN);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("kN");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Força pós-perda progressiva (P3)");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", rel.P3_fase3_kN);
                    ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("kN");
                    ImGui::EndTable();
                }

                ImGui::EndTabItem();
            }

            // ----------------------------------------------------------------
            // ABA INSPETOR 2: RESULTADOS DA SEÇÃO E FASES ELU/ELS
            // ----------------------------------------------------------------
            if (ImGui::BeginTabItem("Resultados da Seção")) {
                ui.aba_left_panel_idx = 1;

                const ResultadoFaseELU& fase = (ui.fase_ativa_idx == 0) ? rel.fase1_elu :
                                               ((ui.fase_ativa_idx == 2) ? rel.fase3_elu : rel.fase2_elu);

                // Seletor de Fase e Seção Monitorada
                const char* fases_nomes[] = {
                    "ELU 1 - Fase Construtiva (Perfil Isolado)",
                    "ELU 2 - Serviço Inicial (t = 0)",
                    "ELU 3 - Longa Duração (t = infinito)"
                };
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::Combo("##ComboFaseLeft", &ui.fase_ativa_idx, fases_nomes, 3);

                if (!fase.secoes.empty()) {
                    ImGui::SetNextItemWidth(-1.0f);
                    if (ImGui::BeginCombo("##ComboSecLeft", fase.secoes[ui.secao_monitorada_idx].nome_secao.c_str())) {
                        for (int i = 0; i < static_cast<int>(fase.secoes.size()); ++i) {
                            bool sel = (ui.secao_monitorada_idx == i);
                            if (ImGui::Selectable(fase.secoes[i].nome_secao.c_str(), sel)) {
                                ui.secao_monitorada_idx = i;
                            }
                        }
                        ImGui::EndCombo();
                    }

                    const ResultadoSecaoELU& sec = fase.secoes[ui.secao_monitorada_idx];

                    ImGui::Spacing();
                    PushBoldFont();
                    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Verificação Detalhada — Nó S%d (x = %.0f mm)",
                        ui.secao_monitorada_idx, sec.x_mm);
                    PopBoldFont();

                    if (ImGui::BeginTable("ResultsLimitTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                        ImGui::TableSetupColumn("Parâmetro / Verificação", ImGuiTableColumnFlags_WidthStretch, 1.55f);
                        ImGui::TableSetupColumn("Valor", ImGuiTableColumnFlags_WidthStretch, 1.05f);
                        ImGui::TableSetupColumn("Unid.", ImGuiTableColumnFlags_WidthFixed, 48.0f);
                        ImGui::TableHeadersRow();

                        auto r_val = [](const char* k, double v, const char* fmt, const char* u) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", k);
                            ImGui::TableSetColumnIndex(1); ImGui::Text(fmt, v);
                            ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("%s", u);
                        };

                        r_val("Coordenada longitudinal (X)", sec.x_mm, "%.0f", "mm");
                        r_val("Força de protensão atuante (P)", sec.P_atuante_kN, "%.2f", "kN");
                        r_val("Excentricidade relativa à LNE (ep,X)", sec.ep_LNE_mm, "%.2f", "mm");
                        if (fase.numero_fase >= 2) {
                            r_val("Acréscimo de tensão no cabo (Δσp)", sec.tensao_cabo.delta_sigma_MPa, "%.1f", "MPa");
                            r_val("Tensão no cabo no ELU (σpd)", sec.tensao_cabo.sigma_pd_MPa, "%.1f", "MPa");
                        }
                        r_val("Momento solicitante positivo (MSd+)", sec.MSd_pos_kNm, "%.2f", "kN.m");
                        r_val("Momento resistente positivo (MRd+)", sec.MRd_pos_kNm, "%.2f", "kN.m");
                        r_val("Momento solicitante negativo (MSd-)", -std::abs(sec.MSd_neg_kNm), "%.2f", "kN.m");
                        r_val("Momento resistente à FLD (MRd-)", sec.MRd_neg_kNm, "%.2f", "kN.m");
                        r_val("Cortante solicitante (VSd)", sec.VSd_max_kN, "%.2f", "kN");
                        r_val("Cortante resistente (VRd)", sec.VRd_kN, "%.2f", "kN");
                        r_val("Índice Flexo-Compressão (Ia <= 1,0)", std::max(sec.indice_flexo_compressao_pos, sec.indice_flexo_compressao_neg), "%.3f", "-");

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Estado Limite da Seção");
                        ImGui::TableSetColumnIndex(1);
                        if (sec.aprovado) {
                            ImGui::TextColored(ImVec4(0.08f, 0.58f, 0.25f, 1.0f), "APROVADO (OK)");
                        } else {
                            ImGui::TextColored(ImVec4(0.82f, 0.15f, 0.15f, 1.0f), "REPROVADO");
                        }
                        ImGui::TableSetColumnIndex(2); ImGui::TextDisabled("ELU %d", fase.numero_fase);

                        ImGui::EndTable();
                    }

                    ImGui::Spacing();
                    PushBoldFont();
                    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Todas as Seções da Barra (Clique para Selecionar)");
                    PopBoldFont();

                    if (ImGui::BeginTable("AllSectionsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 150.0f))) {
                        ImGui::TableSetupColumn("Nó / Seção", ImGuiTableColumnFlags_WidthStretch, 1.3f);
                        ImGui::TableSetupColumn("MSd+", ImGuiTableColumnFlags_WidthStretch, 0.8f);
                        ImGui::TableSetupColumn("MRd+", ImGuiTableColumnFlags_WidthStretch, 0.8f);
                        ImGui::TableSetupColumn("Ia", ImGuiTableColumnFlags_WidthStretch, 0.65f);
                        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 46.0f);
                        ImGui::TableHeadersRow();

                        for (int i = 0; i < static_cast<int>(fase.secoes.size()); ++i) {
                            const auto& s = fase.secoes[i];
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            bool is_sel = (ui.secao_monitorada_idx == i);
                            char lbl[64];
                            std::snprintf(lbl, sizeof(lbl), "S%d (x=%.0f)", i, s.x_mm);
                            if (ImGui::Selectable(lbl, is_sel, ImGuiSelectableFlags_SpanAllColumns)) {
                                ui.secao_monitorada_idx = i;
                            }
                            ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", s.MSd_pos_kNm);
                            ImGui::TableSetColumnIndex(2); ImGui::Text("%.1f", s.MRd_pos_kNm);
                            ImGui::TableSetColumnIndex(3); ImGui::Text("%.3f", std::max(s.indice_flexo_compressao_pos, s.indice_flexo_compressao_neg));
                            ImGui::TableSetColumnIndex(4);
                            if (s.aprovado) ImGui::TextColored(ImVec4(0.08f, 0.58f, 0.25f, 1.0f), "OK");
                            else ImGui::TextColored(ImVec4(0.82f, 0.15f, 0.15f, 1.0f), "FALHA");
                        }
                        ImGui::EndTable();
                    }

                    // Resumo rápido das Verificações Globais e Conectores
                    ImGui::Spacing();
                    PushBoldFont();
                    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Verificações Complementares (ELS e Conectores)");
                    PopBoldFont();
                    if (ImGui::BeginTable("ELSSummaryTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        ImGui::TableSetupColumn("Verificação");
                        ImGui::TableSetupColumn("Solicitante / Limite");
                        ImGui::TableSetupColumn("Status");
                        ImGui::TableHeadersRow();

                        for (const auto& item : rel.verificacoes) {
                            if (item.rotina.find("ELS") != std::string::npos || item.rotina.find("Rotina E") != std::string::npos) {
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", item.descricao.c_str());
                                ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f / %.2f %s", item.solicitante, item.resistente, item.unidade.c_str());
                                ImGui::TableSetColumnIndex(2);
                                if (item.aprovado) ImGui::TextColored(ImVec4(0.08f, 0.58f, 0.25f, 1.0f), "OK");
                                else ImGui::TextColored(ImVec4(0.82f, 0.15f, 0.15f, 1.0f), "FALHA");
                            }
                        }
                        ImGui::EndTable();
                    }

                    // Detalhamento das Flechas ELS (Fase 4 - Turini 2021)
                    ImGui::Spacing();
                    PushBoldFont();
                    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Fase 4: Detalhamento de Flechas no Meio do Vão (ELS)");
                    PopBoldFont();
                    if (ImGui::BeginTable("ELSFlechasTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        ImGui::TableSetupColumn("Parcela de Flecha");
                        ImGui::TableSetupColumn("Valor (cm)");
                        ImGui::TableSetupColumn("Descrição");
                        ImGui::TableHeadersRow();

                        auto add_linha = [](const char* p, double val, const char* desc) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", p);
                            ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f cm", val);
                            ImGui::TableSetColumnIndex(2); ImGui::Text("%s", desc);
                        };

                        add_linha("δ_PP", rel.els_deformacao.delta_PP_cm, "Peso próprio do perfil de aço");
                        add_linha("δ_conc", rel.els_deformacao.delta_conc_cm, "Peso próprio do concreto fresco");
                        add_linha("δ_CPN", rel.els_deformacao.delta_CPN_cm, "Carga permanente longa duração (Itr,inf)");
                        add_linha("δ_CVN (curta)", rel.els_deformacao.delta_CVN_curta_cm, "(1 - ψ2)*q4 na seção de curta duração (Itr,0)");
                        add_linha("δ_CVN (longa)", rel.els_deformacao.delta_CVN_longa_cm, "ψ2*q4 na seção de longa duração (Itr,inf)");
                        add_linha("δ_prot", rel.els_deformacao.delta_protensao_cm, "Contraflecha da força de protensão");
                        add_linha("δ_contraflecha", rel.els_deformacao.delta_contraflecha_cm, "Contraflecha executiva aplicada");
                        add_linha("δ_total", rel.els_deformacao.delta_total_cm, "Flecha resultante final em serviço");
                        add_linha("δ_lim", rel.els_deformacao.delta_lim_cm, "Limite normativo (L / 350)");

                        ImGui::EndTable();
                    }

                    // Detalhamento de Vibração no Piso (Fase 5 - Turini 2021)
                    ImGui::Spacing();
                    PushBoldFont();
                    ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Fase 5: Detalhamento de Vibração no Piso (NBR 8800 Anexo L)");
                    PopBoldFont();
                    if (ImGui::BeginTable("ELSVibTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        ImGui::TableSetupColumn("Parâmetro Dinâmico");
                        ImGui::TableSetupColumn("Valor");
                        ImGui::TableSetupColumn("Critério");
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Largura Colaborante (bef,vib)");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f mm", rel.els_vibracao.bef_vib_mm);
                        ImGui::TableSetColumnIndex(2); ImGui::Text("<= Lv / 5 (NBR 8800 Anexo L)");

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Inércia Transf. Dinâmica (Itr,vib)");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f cm4", rel.els_vibracao.secao_vib.Itr_mm4 / 1e4);
                        ImGui::TableSetColumnIndex(2); ImGui::Text("Ecs,vib (+35%% em steel deck)");

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("Deslocamento Dinâmico Total");
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f cm", rel.els_vibracao.delta_total_cm);
                        ImGui::TableSetColumnIndex(2); ImGui::Text("Limite = 2,00 cm (20 mm)");

                        ImGui::EndTable();
                    }
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        // ====================================================================
        // 4. TOOLTIP INTERATIVO SOBRE O NÓ DA VIGA NO VIEWPORT (ESTILO NLtool)
        // ====================================================================
        if (hovered_node_idx >= 0) {
            const ResultadoFaseELU& fase = (ui.fase_ativa_idx == 0) ? rel.fase1_elu :
                                           ((ui.fase_ativa_idx == 2) ? rel.fase3_elu : rel.fase2_elu);
            if (hovered_node_idx < static_cast<int>(fase.secoes.size())) {
                const auto& s = fase.secoes[hovered_node_idx];
                ImGui::BeginTooltip();
                PushBoldFont();
                ImGui::TextColored(ImVec4(0.11f, 0.32f, 0.62f, 1.0f), "Resultados do Nó S%d — %s (ELU %d)",
                    hovered_node_idx, s.nome_secao.c_str(), fase.numero_fase);
                PopBoldFont();
                ImGui::Separator();
                if (ImGui::BeginTable("BeamDispTooltipTable", 2, ImGuiTableFlags_BordersInnerV)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Coordenada X:");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f mm (ep,X = %.1f mm)", s.x_mm, s.ep_LNE_mm);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Força Protensão P:");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f kN", s.P_atuante_kN);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Momento MSd+ / MRd+:");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f / %.2f kN.m", s.MSd_pos_kNm, s.MRd_pos_kNm);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Cortante VSd / VRd:");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f / %.2f kN", s.VSd_max_kN, s.VRd_kN);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Flexo-Compressão Ia:");
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f (<= 1,00)", std::max(s.indice_flexo_compressao_pos, s.indice_flexo_compressao_neg));
                    ImGui::EndTable();
                }
                ImGui::TextDisabled("Clique para fixar esta seção no Inspetor Lateral");
                ImGui::EndTooltip();
            }
        }

        // ====================================================================
        // 5. BARRA DE STATUS INFERIOR (ESTILO NLtool)
        // ====================================================================
        ImGui::SetNextWindowPos(ImVec2(0.0f, static_cast<float>(winH) - status_bar_h), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(winW), status_bar_h), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 4.0f));
        ImGui::Begin("StatusBarNLtool", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleVar();

        const ResultadoFaseELU& fase_st = (ui.fase_ativa_idx == 0) ? rel.fase1_elu :
                                          ((ui.fase_ativa_idx == 2) ? rel.fase3_elu : rel.fase2_elu);
        const ResultadoSecaoELU& sec_st = fase_st.secoes[ui.secao_monitorada_idx];

        ImGui::Text("Fase Ativa: ELU %d  |  Monitorando Nó ID: S%d (x = %.0f mm)  |  MSd+ = %.1f kN.m (MRd+ = %.1f kN.m)  |  Ia = %.3f  |  ",
            fase_st.numero_fase, ui.secao_monitorada_idx, sec_st.x_mm, sec_st.MSd_pos_kNm, sec_st.MRd_pos_kNm,
            std::max(sec_st.indice_flexo_compressao_pos, sec_st.indice_flexo_compressao_neg));
        ImGui::SameLine(0.0f, 0.0f);
        if (rel.aprovado_global) {
            ImGui::TextColored(ImVec4(0.08f, 0.56f, 0.24f, 1.0f), "Resultados prontos: VIGA APROVADA");
        } else {
            ImGui::TextColored(ImVec4(0.82f, 0.14f, 0.14f, 1.0f), "Atenção: Verificação com pendências");
        }
        ImGui::End();

        // ====================================================================
        // 6. DIÁLOGOS MODAIS (ESTILO NLtool: SectionDialog, SlabDialog, etc.)
        // ====================================================================
        if (ui.abrir_modal_perfil) {
            ImGui::OpenPopup("Perfil I e Aço Estrutural###SectionDialog");
            ui.abrir_modal_perfil = false;
        }
        if (ui.abrir_modal_laje) {
            ImGui::OpenPopup("Laje de Concreto e Fôrma Steel Deck###SlabDialog");
            ui.abrir_modal_laje = false;
        }
        if (ui.abrir_modal_armadura) {
            ImGui::OpenPopup("Armaduras Passivas da Laje (Asl)###RebarDialog");
            ui.abrir_modal_armadura = false;
        }
        if (ui.abrir_modal_comparativo) {
            ImGui::OpenPopup("Comparativo ABNT NBR 8800:2008 vs 2024###CompareDialog");
            ui.abrir_modal_comparativo = false;
        }
        if (ui.abrir_modal_sobre) {
            ImGui::OpenPopup("Sobre o Aplicativo###AboutDialog");
            ui.abrir_modal_sobre = false;
        }

        // MODAL 1: PERFIL I E AÇO ESTRUTURAL
        ImGui::SetNextWindowSize(ImVec2(540.0f, 360.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Perfil I e Aço Estrutural###SectionDialog", nullptr, ImGuiWindowFlags_NoResize)) {
            PushBoldFont();
            ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Configuração do Perfil Metálico Duplo T");
            PopBoldFont();
            ImGui::RadioButton("Laminado (Catálogo Série W)", &ui.modo_perfil_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("Soldado (Série VS)", &ui.modo_perfil_idx, 1); ImGui::SameLine();
            ImGui::RadioButton("Customizado", &ui.modo_perfil_idx, 2);
            ImGui::Separator();

            if (ui.modo_perfil_idx == 0) {
                const char* nomes_w[8];
                for (int k = 0; k < 8; ++k) nomes_w[k] = CATALOGO_SERIE_W[k].nome;
                ImGui::Combo("Seção Série W", &ui.perfil_catalogo_idx, nomes_w, 8);
            } else {
                ImGui::Columns(2, "ColsModalPerfil", false);
                ImGui::InputDouble("Altura d (mm)", &viga.perfil.d, 5.0, 20.0, "%.1f");
                ImGui::InputDouble("Largura bf (mm)", &viga.perfil.bf, 5.0, 20.0, "%.1f");
                ImGui::NextColumn();
                ImGui::InputDouble("Espessura tf (mm)", &viga.perfil.tf, 0.5, 2.0, "%.2f");
                ImGui::InputDouble("Espessura tw (mm)", &viga.perfil.tw, 0.5, 2.0, "%.2f");
                ImGui::Columns(1);
            }

            const char* acos_perf[] = { "ASTM A572 Gr 50 (fy = 345 MPa)", "ASTM A36 (fy = 250 MPa)", "USI CIVIL 350 (fy = 350 MPa)" };
            ImGui::Combo("Aço do Perfil", &ui.aco_perfil_idx, acos_perf, 3);
            ImGui::Separator();

            ImGui::BulletText("Dimensões: d = %.1f mm | bf = %.1f mm | tf = %.1f mm | tw = %.1f mm",
                viga.perfil.d, viga.perfil.bf, viga.perfil.tf, viga.perfil.tw);
            ImGui::BulletText("Propriedades: Ag = %.2f cm² | Ix = %.0f cm⁴ | Iy = %.0f cm⁴ | Zx = %.1f cm³",
                viga.perfil.Ag / 100.0, viga.perfil.Ix / 1e4, viga.perfil.Iy / 1e4, viga.perfil.Zx / 1e3);
            ImGui::BulletText("Esbeltezes locais: λf = %.2f | λw = %.2f", viga.perfil.lbf, viga.perfil.lbw);

            ImGui::Spacing();
            if (ImGui::Button("Confirmar e Fechar", ImVec2(160.0f, 30.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // MODAL 2: LAJE DE CONCRETO E FÔRMA STEEL DECK
        ImGui::SetNextWindowSize(ImVec2(560.0f, 340.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Laje de Concreto e Fôrma Steel Deck###SlabDialog", nullptr, ImGuiWindowFlags_NoResize)) {
            PushBoldFont();
            ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Tipologia da Laje e Propriedades do Concreto");
            PopBoldFont();
            ImGui::RadioButton("Laje Maciça", &ui.tipo_laje_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("Laje Mista (Fôrma de Aço Incorporada - Steel Deck)", &ui.tipo_laje_idx, 1);
            ImGui::Separator();

            ImGui::Columns(2, "ColsModalLaje", false);
            ImGui::InputDouble("Espessura tc (mm)", &viga.laje.tc_mm, 5.0, 10.0, "%.1f");
            const char* agregados[] = { "Granito / Gnaisse", "Basalto / Diabásio", "Calcário", "Arenito" };
            ImGui::Combo("Agregado", &ui.agregado_idx, agregados, 4);
            const char* classes_c[] = { "C20", "C25", "C30", "C35", "C40", "C50" };
            ImGui::Combo("Classe Concreto", &ui.classe_conc_idx, classes_c, 6);

            ImGui::NextColumn();
            if (ui.tipo_laje_idx == 1) {
                const char* orient_nerv[] = { "Perpendicular ao Perfil I", "Paralela ao Perfil I" };
                ImGui::Combo("Nervuras", &ui.nervuras_idx, orient_nerv, 2);
                const char* formas_op[] = { "Metform MF-50 (hF = 50 mm)", "Metform MF-75 (hF = 75 mm)", "Arcelor Polydeck 59S (hF = 59 mm)" };
                ImGui::Combo("Fôrma", &ui.forma_idx, formas_op, 3);
                const char* esp_op[] = { "0,80 mm", "0,95 mm", "1,25 mm" };
                ImGui::Combo("Esp. Fôrma", &ui.esp_forma_idx, esp_op, 3);
                const char* aco_f[] = { "ZAR 280 (fy = 280 MPa)", "ZAR 345 (fy = 345 MPa)" };
                ImGui::Combo("Aço da Fôrma", &ui.aco_forma_idx, aco_f, 2);
            } else {
                ImGui::TextDisabled("Fôrma metálica desativada (Laje Maciça).");
            }
            ImGui::Columns(1);

            ImGui::Separator();
            ImGui::BulletText("Concreto %s: fck = %.0f MPa | Ecs = %.0f MPa | bef = %.0f mm (Interação Completa)",
                viga.concreto.classe.c_str(), viga.concreto.fck_MPa, viga.concreto.Ecs_MPa, viga.laje.bef_mm);

            ImGui::Spacing();
            if (ImGui::Button("Confirmar e Fechar", ImVec2(160.0f, 30.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // MODAL 3: ARMADURAS PASSIVAS DA LAJE (Asl)
        ImGui::SetNextWindowSize(ImVec2(500.0f, 300.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Armaduras Passivas da Laje (Asl)###RebarDialog", nullptr, ImGuiWindowFlags_NoResize)) {
            PushBoldFont();
            ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f), "Armaduras Passivas Longitudinais na Laje");
            PopBoldFont();
            ImGui::Separator();

            ImGui::InputDouble("Armadura Inferior Asl,inf (cm²/m)", &ui.Asl_inf_cm2_m, 0.5, 1.0, "%.2f");
            ImGui::InputDouble("Armadura Superior Asl,sup (cm²/m)", &ui.Asl_sup_cm2_m, 0.5, 1.0, "%.2f");
            ImGui::InputDouble("Armadura de Fissuração As,fis (cm²/m)", &ui.As_fis_cm2_m, 0.2, 0.5, "%.2f");
            ImGui::InputDouble("Cobrimento d' (mm)", &ui.d_linha_mm, 2.0, 5.0, "%.1f");
            const char* acos_passivos[] = { "CA-25 (fyk = 250 MPa)", "CA-50 (fyk = 500 MPa)", "CA-60 (fyk = 600 MPa)" };
            ImGui::Combo("Aço Passivo", &ui.aco_passivo_idx, acos_passivos, 3);
            ImGui::Separator();
            ImGui::BulletText("Área total colaborante na largura bef (Ads): %.2f cm²", viga.armacao_laje.Ads / 100.0);

            ImGui::Spacing();
            if (ImGui::Button("Confirmar e Fechar", ImVec2(160.0f, 30.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // MODAL 4: COMPARATIVO ABNT NBR 8800:2008 vs 2024
        ImGui::SetNextWindowSize(ImVec2(820.0f, 540.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Comparativo ABNT NBR 8800:2008 vs 2024###CompareDialog", nullptr, ImGuiWindowFlags_NoResize)) {
            VigaVMP v08 = viga; v08.norma = NBR8800_2008;
            RelatorioVMP rel_2008 = verificar_viga_vmp(v08);
            VigaVMP v24 = viga; v24.norma = NBR8800_2024;
            RelatorioVMP rel_2024 = verificar_viga_vmp(v24);

            PushBoldFont();
            ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f),
                "Análise Comparativa Direta — ABNT NBR 8800:2008 (Anexo O) vs ABNT NBR 8800:2024 (Anexo L)");
            PopBoldFont();
            ImGui::Separator();

            if (ImGui::BeginTable("TabCompModal", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Grandeza Estrutural", ImGuiTableColumnFlags_WidthStretch, 2.3f);
                ImGui::TableSetupColumn("NBR 8800:2008", ImGuiTableColumnFlags_WidthStretch, 0.9f);
                ImGui::TableSetupColumn("NBR 8800:2024", ImGuiTableColumnFlags_WidthStretch, 0.9f);
                ImGui::TableSetupColumn("Unid.", ImGuiTableColumnFlags_WidthFixed, 55.0f);
                ImGui::TableSetupColumn("Diferença (%)", ImGuiTableColumnFlags_WidthStretch, 0.85f);
                ImGui::TableHeadersRow();

                auto row_cmp = [](const char* label, double val08, double val24, const char* unid) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", label);
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", val08);
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", val24);
                    ImGui::TableSetColumnIndex(3); ImGui::TextDisabled("%s", unid);
                    ImGui::TableSetColumnIndex(4);
                    double d = (std::abs(val08) > 1e-6) ? ((val24 - val08) / val08 * 100.0) : 0.0;
                    ImGui::Text("%+.2f%%", d);
                };

                row_cmp("Força Normal Resistente do Perfil (NRd,I)", rel_2008.compressao_aco.NRd_kN, rel_2024.compressao_aco.NRd_kN, "kN");
                row_cmp("Fator Efetivo Local na Compressão (Q vs Aef/Ag)", rel_2008.compressao_aco.Q, rel_2024.compressao_aco.Q, "-");
                row_cmp("Momento Resistente do Perfil Isolado (MRd,I)", rel_2008.flexao_aco.MRd_kNm, rel_2024.flexao_aco.MRd_kNm, "kN.m");
                row_cmp("Coeficiente de Flambagem por Cisalhamento (kv)", rel_2008.cortante_aco.kv, rel_2024.cortante_aco.kv, "-");
                row_cmp("Esbeltez Limite Plástica da Alma ao Cortante (λp,v)", rel_2008.cortante_aco.lbw_p, rel_2024.cortante_aco.lbw_p, "-");
                row_cmp("Força Cortante Resistente da Alma (VRd)", rel_2008.cortante_aco.VRd_kN, rel_2024.cortante_aco.VRd_kN, "kN");
                row_cmp("Força Máxima de Compressão na Laje (Ccd,máx)", rel_2008.momento_vmp.Cc_max_kN, rel_2024.momento_vmp.Cc_max_kN, "kN");
                row_cmp("Força Máxima de Protensão Inicial (P0,máx)", rel_2008.estimativa_p0.P0_max_kN, rel_2024.estimativa_p0.P0_max_kN, "kN");
                if (const auto* s08 = rel_2008.fase2_elu.secaoMeioVao()) {
                    const auto* s24 = rel_2024.fase2_elu.secaoMeioVao();
                    row_cmp("Momento Positivo Resistente da VMP (t = 0)", s08->MRd_pos_kNm, s24->MRd_pos_kNm, "kN.m");
                    row_cmp("Momento Resistente à FLD (Mrd,dist)", s08->res_momento_neg.Mrd_dist_kNm, s24->res_momento_neg.Mrd_dist_kNm, "kN.m");
                    row_cmp("Momento Negativo Resistente da VMP (t = 0)", s08->MRd_neg_kNm, s24->MRd_neg_kNm, "kN.m");
                }
                ImGui::EndTable();
            }

            ImGui::Spacing();
            if (ImGui::Button("Fechar", ImVec2(140.0f, 30.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // MODAL 5: SOBRE O APLICATIVO (AboutAppSection / AuthorsTable estilo NLtool)
        ImGui::SetNextWindowSize(ImVec2(540.0f, 310.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Sobre o Aplicativo###AboutDialog", nullptr, ImGuiWindowFlags_NoResize)) {
            PushBoldFont();
            ImGui::TextColored(ImVec4(0.12f, 0.30f, 0.56f, 1.0f),
                "VMPCalc — Dimensionamento e Verificação de Vigas Mistas Protendidas");
            PopBoldFont();
            ImGui::Separator();
            ImGui::TextWrapped(
                "Ferramenta computacional para modelagem e verificação estrutural de Vigas Mistas de Aço e Concreto "
                "Protendidas (VMP) com cabos externos retilíneos ou poligonais, em pré-tração e pós-tração, "
                "atualizada para os critérios da ABNT NBR 8800:2024 e ABNT NBR 6118:2023.");
            ImGui::Spacing();
            ImGui::BulletText("Interface gráfica inspirada na linguagem visual do NLtool (FECIV/UFU) e VMPCalc.");
            ImGui::BulletText("Verificação completa nas 3 fases construtivas (ELU 1, ELU 2, ELU 3) e em Serviço (ELS).");
            ImGui::BulletText("Suporte a Lajes Maciças e Lajes Mistas com Fôrma de Aço Incorporada (Steel Deck).");
            ImGui::Separator();
            if (ImGui::Button("Fechar", ImVec2(130.0f, 28.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        rlImGuiEnd();
        EndDrawing();
    }

    if (g_rlFontLoaded) {
        UnloadFont(g_rlFont);
    }
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
