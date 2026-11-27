#include "modelos.h"
#include "rotinas_nbr8800.h"
#include "verificador.h"
#include "catalogo.h"
#include "catalogo_completo.h"
#include "calculadora_cargas.h"
#include "geometria.h"
#include "vibracao.h"
#include "estudo_caso.h"

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// ============================================================================
// SISTEMA DE TIPOGRAFIA DE ALTA DEFINIÇÃO (SEGOE UI / VETORIAL)
// ============================================================================
void desenhar_texto_nitido(Font font, const char* texto, float x, float y, float tamanho, Color cor) {
    if (font.texture.id > 0) {
        float sz = (font.baseSize > 0) ? (float)font.baseSize : tamanho;
        DrawTextEx(font, texto, Vector2{ roundf(x), roundf(y) }, sz, 1.0f, cor);
    } else {
        DrawText(texto, (int)roundf(x), (int)roundf(y), (int)tamanho, cor);
    }
}

float medir_largura_texto(Font font, const char* texto, float tamanho) {
    if (font.texture.id > 0) {
        float sz = (font.baseSize > 0) ? (float)font.baseSize : tamanho;
        return MeasureTextEx(font, texto, sz, 1.0f).x;
    }
    return (float)MeasureText(texto, (int)tamanho);
}

void carregar_fontes_imgui() {
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.OversampleH = 3;
    cfg.OversampleV = 2;
    cfg.PixelSnapH = true;

    static const ImWchar glyph_ranges[] = {
        0x0020, 0x00FF,
        0x0100, 0x017F,
        0x0370, 0x03FF,
        0x2010, 0x205E,
        0x2070, 0x209F,
        0x2200, 0x22FF,
        0
    };

    const char* candidatos[] = {
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };

    bool carregada = false;
    for (const char* caminho : candidatos) {
        if (FileExists(caminho)) {
            io.Fonts->AddFontFromFileTTF(caminho, 17.5f, &cfg, glyph_ranges);
            carregada = true;
            break;
        }
    }

    if (!carregada) {
        io.Fonts->AddFontDefault();
    }
}

// ============================================================================
// CONFIGURAÇÃO DOS TEMAS VISUAIS: ESCURO (DARK SLATE) & CLARO (CLEAN WHITE)
// ============================================================================
void aplicar_tema_ui(bool escuro) {
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 5.0f;

    style.WindowPadding     = ImVec2(14.0f, 12.0f);
    style.FramePadding      = ImVec2(8.0f, 5.0f);
    style.ItemSpacing       = ImVec2(10.0f, 7.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 5.0f);
    style.IndentSpacing     = 18.0f;
    style.ScrollbarSize     = 12.0f;

    ImVec4* colors = style.Colors;
    if (escuro) {
        colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.55f, 0.58f, 0.65f, 1.00f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.12f, 0.14f, 0.18f, 0.98f);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.14f, 0.16f, 0.21f, 0.95f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.13f, 0.15f, 0.20f, 0.98f);
        colors[ImGuiCol_Border]                = ImVec4(0.24f, 0.28f, 0.36f, 0.70f);
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_FrameBg]               = ImVec4(0.18f, 0.21f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.24f, 0.28f, 0.38f, 1.00f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.28f, 0.34f, 0.46f, 1.00f);

        colors[ImGuiCol_TitleBg]               = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.15f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.10f, 0.12f, 0.16f, 0.75f);

        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.10f, 0.12f, 0.16f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.28f, 0.33f, 0.44f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.35f, 0.42f, 0.55f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.42f, 0.50f, 0.65f, 1.00f);

        colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.65f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.65f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.38f, 0.74f, 1.00f, 1.00f);

        colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.32f, 0.48f, 0.85f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.42f, 0.64f, 1.00f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.32f, 0.50f, 0.76f, 1.00f);

        colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.28f, 0.40f, 0.75f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.38f, 0.54f, 0.85f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.30f, 0.44f, 0.62f, 1.00f);

        colors[ImGuiCol_Separator]             = ImVec4(0.24f, 0.28f, 0.36f, 0.70f);
        colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.32f, 0.42f, 0.58f, 0.85f);
        colors[ImGuiCol_SeparatorActive]       = ImVec4(0.38f, 0.50f, 0.70f, 1.00f);

        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.65f, 0.98f, 0.30f);
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.65f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.65f, 0.98f, 0.95f);

        colors[ImGuiCol_Tab]                   = ImVec4(0.16f, 0.20f, 0.28f, 0.90f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.40f, 0.60f, 1.00f);
        colors[ImGuiCol_TabSelected]           = ImVec4(0.22f, 0.36f, 0.54f, 1.00f);
        colors[ImGuiCol_TabDimmed]             = ImVec4(0.12f, 0.15f, 0.20f, 0.90f);
        colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.18f, 0.24f, 0.34f, 1.00f);

        colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.20f, 0.75f, 0.45f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.16f, 0.20f, 0.27f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.24f, 0.30f, 0.40f, 1.00f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.20f, 0.24f, 0.32f, 0.70f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.13f, 0.15f, 0.20f, 0.70f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.15f, 0.18f, 0.24f, 0.70f);
    } else {
        colors[ImGuiCol_Text]                  = ImVec4(0.10f, 0.13f, 0.18f, 1.00f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.48f, 0.52f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.97f, 0.98f, 0.99f, 0.98f);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.93f, 0.95f, 0.97f, 0.95f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.99f, 0.99f, 1.00f, 0.98f);
        colors[ImGuiCol_Border]                = ImVec4(0.76f, 0.81f, 0.88f, 0.75f);
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_FrameBg]               = ImVec4(0.88f, 0.91f, 0.95f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.82f, 0.86f, 0.92f, 1.00f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.76f, 0.82f, 0.90f, 1.00f);

        colors[ImGuiCol_TitleBg]               = ImVec4(0.86f, 0.90f, 0.95f, 1.00f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.80f, 0.85f, 0.93f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.90f, 0.92f, 0.96f, 0.75f);

        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.90f, 0.93f, 0.97f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.92f, 0.94f, 0.97f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.68f, 0.73f, 0.80f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.55f, 0.62f, 0.72f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.42f, 0.50f, 0.62f, 1.00f);

        colors[ImGuiCol_CheckMark]             = ImVec4(0.12f, 0.45f, 0.85f, 1.00f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.16f, 0.48f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.10f, 0.38f, 0.78f, 1.00f);

        colors[ImGuiCol_Button]                = ImVec4(0.24f, 0.46f, 0.75f, 0.85f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.18f, 0.40f, 0.68f, 1.00f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.14f, 0.34f, 0.60f, 1.00f);

        colors[ImGuiCol_Header]                = ImVec4(0.82f, 0.88f, 0.96f, 0.80f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.74f, 0.82f, 0.92f, 0.90f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.68f, 0.78f, 0.88f, 1.00f);

        colors[ImGuiCol_Separator]             = ImVec4(0.74f, 0.80f, 0.88f, 0.75f);
        colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.55f, 0.65f, 0.78f, 0.85f);
        colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.52f, 0.68f, 1.00f);

        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.15f, 0.45f, 0.85f, 0.30f);
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.15f, 0.45f, 0.85f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.15f, 0.45f, 0.85f, 0.95f);

        colors[ImGuiCol_Tab]                   = ImVec4(0.85f, 0.89f, 0.95f, 0.90f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.76f, 0.83f, 0.92f, 1.00f);
        colors[ImGuiCol_TabSelected]           = ImVec4(0.68f, 0.78f, 0.90f, 1.00f);
        colors[ImGuiCol_TabDimmed]             = ImVec4(0.90f, 0.93f, 0.96f, 0.90f);
        colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.82f, 0.87f, 0.94f, 1.00f);

        colors[ImGuiCol_PlotLines]             = ImVec4(0.35f, 0.40f, 0.50f, 1.00f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.12f, 0.65f, 0.32f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.86f, 0.90f, 0.96f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.72f, 0.78f, 0.86f, 1.00f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.82f, 0.86f, 0.92f, 0.80f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.98f, 0.99f, 1.00f, 1.00f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.93f, 0.95f, 0.98f, 1.00f);
    }
}

// ============================================================================
// DESENHO GRÁFICO 2D DA SEÇÃO MISTA DE AÇO E CONCRETO (RAYLIB)
// ============================================================================
void desenhar_secao_viga_mista(
    const VigaMistaConfig& cfg,
    const RelatorioVigaMista& rel,
    int centro_x,
    float canvas_topo_y,
    float canvas_base_y,
    float canvas_largura,
    Font font,
    bool tema_escuro
) {
    const auto& p = cfg.perfil;
    if (p.d <= 0.0 || p.bf <= 0.0) return;

    Color corAco        = tema_escuro ? GetColor(0x2D3442FF) : GetColor(0x334155EE);
    Color corBordaAco   = tema_escuro ? GetColor(0x707A8EFF) : GetColor(0x0F172AFF);
    Color corConcreto   = tema_escuro ? GetColor(0x3B4A62CC) : GetColor(0x93C5FD44);
    Color corBordaConc  = tema_escuro ? GetColor(0x5B9DEEFF) : GetColor(0x2563EBFF);
    Color corFormaAco   = tema_escuro ? GetColor(0x50E3C2FF) : GetColor(0x0F766EFF);
    Color corStud       = tema_escuro ? GetColor(0xF5A623FF) : GetColor(0xD97706FF);
    Color corStudCabeca = tema_escuro ? GetColor(0xD08010FF) : GetColor(0xB45309FF);
    Color corEixo       = tema_escuro ? Fade(RED, 0.25f) : Fade(DARKBLUE, 0.22f);

    float painel_dir_x_inicio = (float)centro_x - canvas_largura / 2.0f;
    float painel_dir_x_fim    = (float)centro_x + canvas_largura / 2.0f;

    float y_min_desenho = canvas_topo_y + 38.0f + 32.0f;
    float y_max_desenho = canvas_base_y - 36.0f;
    float h_disp_h = y_max_desenho - y_min_desenho;
    if (h_disp_h < 120.0f) h_disp_h = 120.0f;

    float hF_real = (cfg.laje.tipo == LAJE_STEEL_DECK) ? (float)cfg.laje.hF_mm : 0.0f;
    float tc_real = (float)cfg.laje.tc_mm;
    float H_fisica = (float)p.d + hF_real + tc_real;
    if (H_fisica <= 0.0f) H_fisica = 500.0f;

    float max_w_livre = canvas_largura - 240.0f;
    if (max_w_livre < 160.0f) max_w_livre = 160.0f;

    float W_fisica = (float)rel.fase2.bef_mm;
    if (W_fisica <= 0.0f) W_fisica = 1500.0f;

    float W_visual_fisica = std::min(W_fisica, (float)p.d * 3.8f);
    if (W_visual_fisica < (float)p.bf * 2.4f) W_visual_fisica = (float)p.bf * 2.4f;

    float escala_h = h_disp_h / H_fisica;
    float escala_w = max_w_livre / W_visual_fisica;
    float escala = std::min(escala_h, escala_w);
    if (escala > 0.40f) escala = 0.40f;
    if (escala < 0.16f) escala = 0.16f;

    float d = (float)p.d * escala;
    float bf = (float)p.bf * escala;
    float tf = std::max(7.0f, (float)p.tf * escala);
    float tw = std::max(4.5f, (float)p.tw * escala);
    float tc = tc_real * escala;
    float hF = hF_real * escala;
    float passo = (cfg.laje.eF_mm > 0.0) ? (float)cfg.laje.eF_mm * escala : bf * 1.30f;
    if (passo < 35.0f) passo = 35.0f;
    float bF = (cfg.laje.bF_mm > 0.0) ? (float)cfg.laje.bF_mm * escala : passo * 0.55f;
    if (bF > passo * 0.80f) bF = passo * 0.80f;
    if (bF < 15.0f) bF = 15.0f;

    float w_rampa = std::min(hF * 0.45f, (passo - bF) * 0.40f);
    if (w_rampa < 4.0f) w_rampa = 4.0f;
    float w_fundo = bF - w_rampa;
    if (w_fundo < 12.0f) w_fundo = 12.0f;
    float half_fundo = w_fundo * 0.5f;

    float largura_laje = W_visual_fisica * escala;
    float x_esq_laje = (float)centro_x - largura_laje / 2.0f;
    float x_dir_laje = x_esq_laje + largura_laje;

    float altura_desenho = d + hF + tc;
    float y_topo_laje = y_min_desenho + (h_disp_h - altura_desenho) / 2.0f;
    float y_crista_deck = y_topo_laje + tc;
    float y_topo_aco = y_crista_deck + hF;
    float y_base_aco = y_topo_aco + d;
    float x_esq_mesa = (float)centro_x - bf / 2.0f;

    // 1. Eixos baricêntricos discretos
    float y_cg_perfil = y_base_aco - d / 2.0f;
    DrawLine((int)(centro_x - bf / 2.0f - 80.0f), (int)y_cg_perfil, (int)(centro_x + bf / 2.0f + 80.0f), (int)y_cg_perfil, corEixo);
    DrawLine(centro_x, (int)(y_topo_laje - 12.0f), centro_x, (int)(y_base_aco + 12.0f), Fade(DARKGREEN, 0.25f));

    // 2. Desenho do Perfil I de Aço
    DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, corAco);
    float h_alma = d - 2.0f * tf;
    DrawRectangle((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)h_alma, corAco);
    DrawRectangle((int)x_esq_mesa, (int)(y_base_aco - tf), (int)bf, (int)tf, corAco);

    DrawRectangleLines((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, corBordaAco);
    DrawRectangleLines((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)h_alma, corBordaAco);
    DrawRectangleLines((int)x_esq_mesa, (int)(y_base_aco - tf), (int)bf, (int)tf, corBordaAco);

    // 3. Desenho da Laje de Concreto & Steel Deck
    if (cfg.laje.tipo == LAJE_MACICA || hF <= 0.0f) {
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, corConcreto);
        DrawRectangleLines((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, corBordaConc);

        float w_badge_m = medir_largura_texto(font, "Laje Maciça", 16.0f);
        Color corBadgeConcBg = tema_escuro ? Fade(DARKGREEN, 0.80f) : GetColor(0x15803DFF);
        DrawRectangle((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_m + 16, 22, corBadgeConcBg);
        desenhar_texto_nitido(font, "Laje Maciça", x_esq_laje + 18.0f, y_topo_laje + 11.0f, 16.0f, WHITE);
    }
    else if (!cfg.laje.nervura_paralela) {
        float h_total_laje = tc + hF;
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)h_total_laje, corConcreto);

        DrawLineEx(Vector2{ x_esq_laje, y_topo_aco }, Vector2{ x_dir_laje, y_topo_aco }, 3.0f, corFormaAco);

        for (float x = x_esq_laje; x < x_dir_laje; x += 14.0f) {
            float x_fim = std::min(x + 8.0f, x_dir_laje);
            DrawLineEx(Vector2{ x, y_crista_deck }, Vector2{ x_fim, y_crista_deck }, 1.2f, Fade(corFormaAco, 0.45f));
        }

        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_dir_laje, y_topo_laje }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_esq_laje, y_topo_aco }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_dir_laje, y_topo_laje }, Vector2{ x_dir_laje, y_topo_aco }, 1.8f, corBordaConc);

        float w_badge_p = medir_largura_texto(font, "Steel Deck: Nervuras Transversais (Perpendiculares)", 16.0f);
        Color corBadgeDeckBg = tema_escuro ? GetColor(0x0E5A52FF) : GetColor(0xCCFBF1FF);
        Color corBadgeDeckTxt= tema_escuro ? corFormaAco : GetColor(0x0F766EFF);
        DrawRectangle((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_p + 16, 22, corBadgeDeckBg);
        if (!tema_escuro) {
            DrawRectangleLines((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_p + 16, 22, GetColor(0x14B8A6FF));
        }
        desenhar_texto_nitido(font, "Steel Deck: Nervuras Transversais (Perpendiculares)", x_esq_laje + 18.0f, y_topo_laje + 11.0f, 16.0f, corBadgeDeckTxt);
    }
    else {
        auto avaliar_y_chapa_paralela = [&](float x) -> float {
            float dx = x - (float)centro_x;
            float u = fmodf(dx + passo * 0.5f, passo);
            if (u < 0.0f) u += passo;
            u -= passo * 0.5f;
            float abs_u = fabsf(u);

            if (abs_u <= half_fundo) {
                return y_topo_aco;
            } else if (abs_u <= half_fundo + w_rampa) {
                float t = (abs_u - half_fundo) / w_rampa;
                return y_topo_aco - t * hF;
            } else {
                return y_crista_deck;
            }
        };

        float dx_step = 1.5f;
        float x_curr = x_esq_laje;
        float y_curr = avaliar_y_chapa_paralela(x_curr);

        while (x_curr < x_dir_laje) {
            float x_next = std::min(x_curr + dx_step, x_dir_laje);
            float y_next = avaliar_y_chapa_paralela(x_next);

            Vector2 p1 = { x_curr, y_topo_laje };
            Vector2 p2 = { x_next, y_topo_laje };
            Vector2 p3 = { x_next, y_next };
            Vector2 p4 = { x_curr, y_curr };

            DrawTriangle(p1, p4, p2, corConcreto);
            DrawTriangle(p2, p4, p3, corConcreto);

            DrawLineEx(Vector2{ x_curr, y_curr }, Vector2{ x_next, y_next }, 2.5f, corFormaAco);

            x_curr = x_next;
            y_curr = y_next;
        }

        float y_borda_esq = avaliar_y_chapa_paralela(x_esq_laje);
        float y_borda_dir = avaliar_y_chapa_paralela(x_dir_laje);

        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_dir_laje, y_topo_laje }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_esq_laje, y_borda_esq }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_dir_laje, y_topo_laje }, Vector2{ x_dir_laje, y_borda_dir }, 1.8f, corBordaConc);

        float w_badge_par = medir_largura_texto(font, "Steel Deck: Nervuras Longitudinais (Paralelas)", 16.0f);
        Color corBadgeDeckBg = tema_escuro ? GetColor(0x0E5A52FF) : GetColor(0xCCFBF1FF);
        Color corBadgeDeckTxt= tema_escuro ? corFormaAco : GetColor(0x0F766EFF);
        DrawRectangle((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_par + 16, 22, corBadgeDeckBg);
        if (!tema_escuro) {
            DrawRectangleLines((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_par + 16, 22, GetColor(0x14B8A6FF));
        }
        desenhar_texto_nitido(font, "Steel Deck: Nervuras Longitudinais (Paralelas)", x_esq_laje + 18.0f, y_topo_laje + 11.0f, 16.0f, corBadgeDeckTxt);
    }

    // 4. Conectores Stud Bolt
    float h_stud = (float)cfg.conector.h * escala;
    float d_stud = (float)cfg.conector.d * escala;
    if (d_stud < 4.0f) d_stud = 4.0f;
    if (h_stud < 10.0f) h_stud = 10.0f;

    int n_pinos = std::max(1, cfg.conector.nt);
    float st_escala = (float)rel.conectores.s_transv_min_mm * escala;
    if (st_escala < d_stud * 1.5f && n_pinos > 1) st_escala = d_stud * 1.5f;

    float espacamento_transv = (n_pinos > 1) ? st_escala : 0.0f;
    float x_inicio_pinos = (float)centro_x - (n_pinos - 1) * espacamento_transv / 2.0f;

    Color corStudAtual = rel.conectores.geometria_transversal_atende ? corStud : GetColor(0xEF4444FF);
    Color corStudCabecaAtual = rel.conectores.geometria_transversal_atende ? corStudCabeca : GetColor(0xDC2626FF);

    for (int i = 0; i < n_pinos; ++i) {
        float px = x_inicio_pinos + i * espacamento_transv;
        DrawRectangle((int)(px - d_stud / 2.0f), (int)(y_topo_aco - h_stud), (int)d_stud, (int)h_stud, corStudAtual);
        DrawRectangleLines((int)(px - d_stud / 2.0f), (int)(y_topo_aco - h_stud), (int)d_stud, (int)h_stud, DARKGRAY);
        float d_cabeca = d_stud * 1.6f;
        float h_cabeca = d_stud * 0.5f;
        DrawRectangle((int)(px - d_cabeca / 2.0f), (int)(y_topo_aco - h_stud - h_cabeca), (int)d_cabeca, (int)h_cabeca, corStudCabecaAtual);
        DrawRectangleLines((int)(px - d_cabeca / 2.0f), (int)(y_topo_aco - h_stud - h_cabeca), (int)d_cabeca, (int)h_cabeca, BLACK);
    }

    if (!rel.conectores.geometria_transversal_atende) {
        const char* msg_alerta = (!rel.conectores.largura_mesa_atende)
            ? "MESA INSUFICIENTE P/ PINOS"
            : "COLISAO COM FORMA DECK";
        float w_alerta = medir_largura_texto(font, msg_alerta, 14.0f);
        DrawRectangle((int)((float)centro_x - w_alerta / 2.0f - 6.0f), (int)(y_topo_aco - h_stud - 24.0f), (int)w_alerta + 12, 18, GetColor(0xDC2626EE));
        desenhar_texto_nitido(font, msg_alerta, (float)centro_x - w_alerta / 2.0f, y_topo_aco - h_stud - 22.0f, 14.0f, WHITE);
    }

    // 5. Linha Neutra Plástica (LNP) e Bloco de Compressão (usando ResultadoMomentoPositivoVM de rotinas_nbr8800)
    const auto& mom = rel.fase2.momento_misto;
    if (mom.lnp_na_laje && mom.a_linha_neutra_mm > 0.0) {
        float a_comp = (float)mom.a_linha_neutra_mm * escala;
        if (a_comp > tc) a_comp = tc;
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)a_comp, Fade(RED, 0.25f));
        DrawLineEx(Vector2{ x_esq_laje - 18.0f, y_topo_laje + a_comp }, Vector2{ x_dir_laje + 18.0f, y_topo_laje + a_comp }, 1.5f, RED);

        std::string txt_lnp = TextFormat("LNP (Laje): a = %.1f mm", mom.a_linha_neutra_mm);
        float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
        float x_badge_lnp = x_dir_laje + 8.0f;
        float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
        if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

        DrawRectangle((int)x_badge_lnp, (int)(y_topo_laje + a_comp - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
        desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_topo_laje + a_comp - 8.0f, 16.0f, WHITE);
    } else if (!mom.lnp_na_laje) {
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, Fade(RED, 0.25f));

        if (mom.y_lnp_mm <= cfg.perfil.tf) {
            float ratio = (float)(mom.y_lnp_mm / std::max(0.1, cfg.perfil.tf));
            float y_lnp_mesa = y_topo_aco + ratio * tf;

            DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)(ratio * tf), Fade(RED, 0.38f));
            DrawLineEx(Vector2{ x_esq_mesa - 18.0f, y_lnp_mesa }, Vector2{ x_esq_mesa + bf + 18.0f, y_lnp_mesa }, 1.8f, RED);

            std::string txt_lnp = TextFormat("LNP (Mesa): yp = %.1f mm (tf = %.1f mm)", mom.y_lnp_mm, cfg.perfil.tf);
            float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
            float x_badge_lnp = x_esq_mesa + bf + 8.0f;
            float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
            if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

            DrawRectangle((int)x_badge_lnp, (int)(y_lnp_mesa - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
            desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_lnp_mesa - 8.0f, 16.0f, WHITE);
        } else {
            float h_wc_real = (float)(mom.y_lnp_mm - cfg.perfil.tf);
            float h_alma_total_real = (float)(cfg.perfil.d - 2.0 * cfg.perfil.tf);
            float ratio_alma = (h_alma_total_real > 0.1f) ? (h_wc_real / h_alma_total_real) : 0.0f;
            float y_lnp_alma = y_topo_aco + tf + ratio_alma * h_alma;

            DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, Fade(RED, 0.38f));
            DrawRectangle((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)(ratio_alma * h_alma), Fade(RED, 0.45f));
            DrawLineEx(Vector2{ (float)centro_x - bf * 0.45f, y_lnp_alma }, Vector2{ (float)centro_x + bf * 0.45f, y_lnp_alma }, 1.8f, RED);

            std::string txt_lnp = TextFormat("LNP (Alma): yp = %.1f mm (tf = %.1f mm)", mom.y_lnp_mm, cfg.perfil.tf);
            float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
            float x_badge_lnp = (float)centro_x + tw / 2.0f + 10.0f;
            float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
            if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

            DrawRectangle((int)x_badge_lnp, (int)(y_lnp_alma - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
            desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_lnp_alma - 8.0f, 16.0f, WHITE);
        }
    }

    // 6. Legendas
    std::string txt_laje = "Laje: beff = " + std::to_string((int)rel.fase2.bef_mm) + " mm  |  tc = " + std::to_string((int)cfg.laje.tc_mm) + " mm";
    if (cfg.laje.tipo == LAJE_STEEL_DECK) {
        txt_laje = "Laje Mista: beff = " + std::to_string((int)rel.fase2.bef_mm) + " mm  |  Capa tc = " + std::to_string((int)cfg.laje.tc_mm) + " mm  |  Fôrma hF = " + std::to_string((int)cfg.laje.hF_mm) + " mm";
    }
    float w_txt_laje = medir_largura_texto(font, txt_laje.c_str(), 16.0f);
    float x_txt_laje = x_esq_laje;
    if (x_txt_laje < painel_dir_x_inicio + 12.0f) x_txt_laje = painel_dir_x_inicio + 12.0f;
    if (x_txt_laje + w_txt_laje + 16.0f > painel_dir_x_fim - 12.0f) {
        x_txt_laje = painel_dir_x_fim - w_txt_laje - 24.0f;
    }
    float y_txt_laje = y_topo_laje - 25.0f;

    Color corPillLajeBg  = tema_escuro ? Fade(BLACK, 0.70f) : GetColor(0xF1F5F9FF);
    Color corPillLajeTxt = tema_escuro ? GetColor(0x4ADE80FF) : GetColor(0x15803DFF);
    DrawRectangle((int)x_txt_laje, (int)y_txt_laje, (int)w_txt_laje + 14, 22, corPillLajeBg);
    if (!tema_escuro) {
        DrawRectangleLines((int)x_txt_laje, (int)y_txt_laje, (int)w_txt_laje + 14, 22, GetColor(0xCBD5E1FF));
    }
    desenhar_texto_nitido(font, txt_laje.c_str(), x_txt_laje + 7.0f, y_txt_laje + 3.0f, 16.0f, corPillLajeTxt);

    std::string txt_perfil = p.nome + "  (d = " + std::to_string((int)p.d) + " mm, bf = " + std::to_string((int)p.bf) + " mm, tf = " + TextFormat("%.1f", p.tf) + " mm)";
    float w_txt_p = medir_largura_texto(font, txt_perfil.c_str(), 16.0f);
    float x_txt_p = (float)centro_x - w_txt_p / 2.0f;
    if (x_txt_p < painel_dir_x_inicio + 12.0f) x_txt_p = painel_dir_x_inicio + 12.0f;
    if (x_txt_p + w_txt_p + 16.0f > painel_dir_x_fim - 12.0f) {
        x_txt_p = painel_dir_x_fim - w_txt_p - 24.0f;
    }
    float y_txt_p = std::min(canvas_base_y - 28.0f, y_base_aco + 8.0f);

    Color corPillPerfilBg  = tema_escuro ? Fade(BLACK, 0.70f) : GetColor(0x1E293BFF);
    Color corPillPerfilTxt = WHITE;
    DrawRectangle((int)x_txt_p, (int)y_txt_p, (int)w_txt_p + 14, 22, corPillPerfilBg);
    desenhar_texto_nitido(font, txt_perfil.c_str(), x_txt_p + 7.0f, y_txt_p + 3.0f, 16.0f, corPillPerfilTxt);
}

// ============================================================================
// PONTO DE ENTRADA PRINCIPAL DA INTERFACE GRÁFICA
// ============================================================================
int main() {
    const int screenWidth = 1480;
    const int screenHeight = 880;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "VMP - Dimensionamento de Vigas Mistas de Aço e Concreto | NBR 8800 (v2008 / v2024)");
    SetTargetFPS(60);

    rlImGuiSetLoadFontsCallback(carregar_fontes_imgui);
    rlImGuiSetup(true);

    bool tema_escuro = true;
    aplicar_tema_ui(tema_escuro);

    std::vector<int> codepointsRaylib;
    codepointsRaylib.reserve(512);
    for (int i = 32; i <= 255; ++i) codepointsRaylib.push_back(i);
    for (int i = 0x0100; i <= 0x017F; ++i) codepointsRaylib.push_back(i);
    codepointsRaylib.push_back(0x2264);
    codepointsRaylib.push_back(0x2265);
    codepointsRaylib.push_back(0x03B7);

    const char* fontRaylibPath = "C:\\Windows\\Fonts\\seguisb.ttf";
    if (!FileExists(fontRaylibPath)) fontRaylibPath = "C:\\Windows\\Fonts\\segoeuib.ttf";
    if (!FileExists(fontRaylibPath)) fontRaylibPath = "C:\\Windows\\Fonts\\segoeui.ttf";

    Font fontRaylib = LoadFontEx(fontRaylibPath, 16, codepointsRaylib.data(), (int)codepointsRaylib.size());
    Font fontRaylibTitulo = LoadFontEx(fontRaylibPath, 18, codepointsRaylib.data(), (int)codepointsRaylib.size());

    // Inicialização usando as estruturas unificadas de modelos.h
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = LAJE_MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.laje.hF_mm = 0.0;
    cfg.laje.bF_mm = 120.0;
    cfg.laje.eF_mm = 200.0;
    cfg.laje.nervura_paralela = false;

    cfg.conector.tipo = CONECTOR_STUD_BOLT;
    cfg.conector.d = 19.0;
    cfg.conector.h = 100.0;
    cfg.conector.fu = 420.0;
    cfg.conector.nt = 1;
    cfg.conector.k_nervuras = 1;
    cfg.conector.espacamento_adotado_mm = 200.0;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;
    cfg.geometria.eh_viga_borda = false;
    cfg.geometria.escorada = false;

    cfg.acoes.g1_kN_m = 5.749;
    cfg.acoes.q1_kN_m = 1.750;
    cfg.acoes.g2_kN_m = 7.000;
    cfg.acoes.q2_kN_m = 5.250;
    cfg.acoes.psi1 = 0.70;
    cfg.acoes.psi2 = 0.40;

    int norma_idx = 0;        // 0 = 2024 (nbr8800::v2024), 1 = 2008 (nbr8800::v2008)
    int perfil_modo_idx = 0;  // 0 = Perfis Rápidos (Caso de Estudo), 1 = Catálogo Completo Gerdau (W) / Soldado (VS), 2 = Personalizado
    int perfil_idx = 0;       // 0 = W360x51, 1 = W360x57.8, 2 = W460x89
    int perfil_cat_idx = 0;   // Índice na lista completa de 157 perfis
    int deck_cat_idx = 0;     // Índice na lista de fôrmas Steel Deck Metform
    int laje_idx = 0;         // 0 = Maciça, 1 = Steel Deck
    int orient_deck_idx = 0;  // 0 = Perpendicular, 1 = Paralela

    // Estado do Assistente de Cargas (NBR 6120:2019)
    bool usar_assistente_cargas = true;

    // Concreto: Ecs calculado pela norma selecionada (4.6.3.1) ou informado manualmente
    bool ecs_automatico = true;
    int agregado_idx = 1;     // 0 basalto/diabásio, 1 granito/gnaisse, 2 calcário, 3 arenito
    int ocup_vib_idx = 0;     // Ocupação para vibração (Tabelas I.2 e I.3)
    nbr6120::AssistenteCargasConfig assist_cfg;

    while (!WindowShouldClose()) {
        int cur_w = GetScreenWidth();
        int cur_h = GetScreenHeight();

        cfg.norma = (norma_idx == 0) ? NormaEdicao::NBR8800_2024 : NormaEdicao::NBR8800_2008;
        cfg.laje.tipo = (laje_idx == 0) ? LAJE_MACICA : LAJE_STEEL_DECK;
        cfg.laje.nervura_paralela = (orient_deck_idx == 1);

        if (perfil_modo_idx == 2) {
            calcular_propriedades_secao_i(cfg.perfil);
        }

        if (ecs_automatico) {
            cfg.concreto.Ecs_MPa = (cfg.norma == NormaEdicao::NBR8800_2024)
                ? nbr8800::v2024::calcularEcsConcreto(cfg.concreto.fck_MPa, cfg.concreto.alpha_ag)
                : nbr8800::v2008::calcularEcsConcreto(cfg.concreto.fck_MPa, cfg.concreto.alpha_ag);
        }

        {
            // Vibração: inércia da laje por unidade de largura. Steel deck com nervuras perpendiculares
            // usa a inércia da laje mista do catálogo Metform; nos demais casos, tc³/(12·alpha_din).
            const auto& decks_vib = obter_catalogo_steel_deck();
            bool deck_perp = (cfg.laje.tipo == LAJE_STEEL_DECK && !cfg.laje.nervura_paralela);
            cfg.vibracao.Is_laje_mm4_por_mm = (deck_perp && deck_cat_idx >= 0 && deck_cat_idx < (int)decks_vib.size())
                ? decks_vib[deck_cat_idx].inercia_laje_mista_1e6_mm4_m * 1000.0
                : 0.0;
        }

        // Executa o verificador que reutiliza rotinas_nbr8800 (v2008 / v2024) e rotinas_vmp
        RelatorioVigaMista rel = verificar_viga_mista(cfg);

        float margem = 14.0f;
        float menu_bar_h = 24.0f;
        float margem_y = margem + menu_bar_h;
        float painel_esq_w = std::min(640.0f, cur_w * 0.43f);
        float painel_dir_x = painel_esq_w + margem * 2.0f;
        float painel_dir_w = cur_w - painel_dir_x - margem;
        float canvas_h = std::max(340.0f, cur_h * 0.45f);
        float canvas_y = margem_y;
        float tabela_y = canvas_y + canvas_h + margem;
        float tabela_h = cur_h - tabela_y - margem;

        BeginDrawing();
        Color corFundoJanela   = tema_escuro ? GetColor(0x181B22FF) : GetColor(0xEDF1F7FF);
        Color corCanvasBg      = tema_escuro ? GetColor(0x1F242EFF) : GetColor(0xFFFFFFFF);
        Color corCanvasBorda   = tema_escuro ? GetColor(0x353E50FF) : GetColor(0xCBD5E1FF);
        Color corTituloBarra   = tema_escuro ? GetColor(0x141820FF) : GetColor(0xF1F5F9FF);
        Color corTituloTxt     = tema_escuro ? GetColor(0x60A5FAFF) : GetColor(0x1E40AFFF);
        Color corBadgeNormaBg  = tema_escuro ? GetColor(0x283244FF) : GetColor(0xE2E8F0FF);
        Color corBadgeNormaTxt = tema_escuro ? GetColor(0x93C5FDFF) : GetColor(0x1D4ED8FF);

        ClearBackground(corFundoJanela);

        // 1. ÁREA GRÁFICA DE DESENHO (RAYLIB)
        DrawRectangle((int)painel_dir_x, (int)canvas_y, (int)painel_dir_w, (int)canvas_h, corCanvasBg);
        DrawRectangleLines((int)painel_dir_x, (int)canvas_y, (int)painel_dir_w, (int)canvas_h, corCanvasBorda);

        DrawRectangle((int)painel_dir_x, (int)canvas_y, (int)painel_dir_w, 38, corTituloBarra);
        desenhar_texto_nitido(fontRaylibTitulo, "SEÇÃO TRANSVERSAL MISTA (AÇO + CONCRETO)", painel_dir_x + 16.0f, canvas_y + 9.0f, 18.0f, corTituloTxt);

        std::string badge_norma = (norma_idx == 0) ? "nbr8800::v2024" : "nbr8800::v2008";
        float w_b_norma = medir_largura_texto(fontRaylib, badge_norma.c_str(), 16.0f);
        float x_badge = painel_dir_x + painel_dir_w - w_b_norma - 20.0f;
        DrawRectangle((int)x_badge - 6, (int)canvas_y + 8, (int)w_b_norma + 12, 22, corBadgeNormaBg);
        desenhar_texto_nitido(fontRaylib, badge_norma.c_str(), x_badge, canvas_y + 11.0f, 16.0f, corBadgeNormaTxt);

        int canvas_centro_x = (int)(painel_dir_x + painel_dir_w / 2.0f);
        desenhar_secao_viga_mista(cfg, rel, canvas_centro_x, canvas_y, canvas_y + canvas_h, painel_dir_w, fontRaylib, tema_escuro);

        // 2. INTERFACE DEAR IMGUI
        rlImGuiBegin();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Exibição")) {
                if (ImGui::MenuItem("Tema Escuro", nullptr, tema_escuro)) {
                    if (!tema_escuro) {
                        tema_escuro = true;
                        aplicar_tema_ui(true);
                    }
                }
                if (ImGui::MenuItem("Tema Claro", nullptr, !tema_escuro)) {
                    if (tema_escuro) {
                        tema_escuro = false;
                        aplicar_tema_ui(false);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Exemplos")) {
                if (ImGui::MenuItem("Estudo de caso V1 (TCC) - W460x52, MF-75, L = 9 m")) {
                    cfg = estudo_caso_v1::configuracao();
                    norma_idx = 0;
                    perfil_modo_idx = 0;
                    perfil_idx = 3;
                    laje_idx = 1;
                    orient_deck_idx = 0;
                    usar_assistente_cargas = false;
                    ecs_automatico = true;
                    agregado_idx = 1;
                    ocup_vib_idx = 0;
                    const auto& decks = obter_catalogo_steel_deck();
                    for (int n = 0; n < (int)decks.size(); ++n) {
                        if (std::string(decks[n].chave) == "MF-75|140|80") deck_cat_idx = n;
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Norma")) {
                if (ImGui::MenuItem("ABNT NBR 8800:2024 (nbr8800::v2024)", nullptr, norma_idx == 0)) norma_idx = 0;
                if (ImGui::MenuItem("ABNT NBR 8800:2008 (nbr8800::v2008)", nullptr, norma_idx == 1)) norma_idx = 1;
                ImGui::EndMenu();
            }
            float w_total = ImGui::GetWindowWidth();
            std::string status_bar = std::string(norma_idx == 0 ? "NBR 8800:2024 (v2024)" : "NBR 8800:2008 (v2008)") + "  |  " + (tema_escuro ? "Tema: Escuro" : "Tema: Claro");
            float w_t = ImGui::CalcTextSize(status_bar.c_str()).x;
            ImGui::SameLine(w_total - w_t - 24.0f);
            ImGui::TextDisabled("%s", status_bar.c_str());
            ImGui::EndMainMenuBar();
        }

        // --------------------------------------------------------------------
        // PAINEL ESQUERDO: ENTRADA DE DADOS & PARÂMETROS
        // --------------------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(margem, margem_y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(painel_esq_w, cur_h - margem - margem_y), ImGuiCond_Always);
        ImGui::Begin("Parâmetros da Estrutura", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        float input_w = 145.0f;

        if (ImGui::BeginTabBar("AbasVigaMista", ImGuiTabBarFlags_None)) {

            // ABA 1: PERFIL I DE AÇO
            if (ImGui::BeginTabItem("Perfil de Aço")) {
                ImGui::Spacing();
                const char* modos_perfil[] = {
                    "Perfis Rápidos (Estudo / Padrão)",
                    "Catálogo Completo (Gerdau W & Soldados VS)",
                    "Perfil Paramétrico Customizado"
                };
                ImGui::Text("Seleção do Perfil:");
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::Combo("##ModoPerfilCombo", &perfil_modo_idx, modos_perfil, 3);
                ImGui::Spacing();

                if (perfil_modo_idx == 0) {
                    const char* perfis[] = { "W 360 x 51,0", "W 360 x 57,8", "W 460 x 89,0", "W 460 x 52,0 (Estudo de caso V1)" };
                    ImGui::Text("Perfil Rápido:");
                    ImGui::SetNextItemWidth(-1.0f);
                    if (ImGui::Combo("##PerfilComboRapido", &perfil_idx, perfis, 4)) {
                        if (perfil_idx == 0) cfg.perfil = perfil_w360x51();
                        else if (perfil_idx == 1) cfg.perfil = perfil_w360x57_8();
                        else if (perfil_idx == 2) cfg.perfil = perfil_w460x89();
                        else if (perfil_idx == 3) cfg.perfil = perfil_w460x52();
                    }
                } else if (perfil_modo_idx == 1) {
                    const auto& lista_cat = obter_catalogo_perfis_completo();
                    static std::vector<std::string> nomes_cat;
                    if (nomes_cat.empty()) {
                        nomes_cat.reserve(lista_cat.size());
                        for (const auto& item : lista_cat) {
                            nomes_cat.push_back(std::string(item.nome) + " (" + (item.tipo == LAMINADO ? "Gerdau W" : "Soldado VS") + ")");
                        }
                    }

                    ImGui::Text("Catálogo Normativo (%d perfis disponíveis):", (int)lista_cat.size());
                    ImGui::SetNextItemWidth(-1.0f);
                    if (ImGui::BeginCombo("##CatalogoCompletoCombo", nomes_cat[perfil_cat_idx].c_str(), ImGuiComboFlags_HeightLarge)) {
                        for (int n = 0; n < (int)nomes_cat.size(); n++) {
                            const bool is_selected = (perfil_cat_idx == n);
                            if (ImGui::Selectable(nomes_cat[n].c_str(), is_selected)) {
                                perfil_cat_idx = n;
                                cfg.perfil = criar_perfil_do_catalogo(lista_cat[n]);
                            }
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                } else {
                    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Edição Paramétrica:");
                    bool dimensoes_mudaram = false;
                    ImGui::SetNextItemWidth(input_w); dimensoes_mudaram |= ImGui::InputDouble("Altura d (mm)", &cfg.perfil.d, 5.0, 20.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); dimensoes_mudaram |= ImGui::InputDouble("Largura mesa bf (mm)", &cfg.perfil.bf, 5.0, 20.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); dimensoes_mudaram |= ImGui::InputDouble("Espessura mesa tf (mm)", &cfg.perfil.tf, 0.5, 2.0, "%.2f");
                    ImGui::SetNextItemWidth(input_w); dimensoes_mudaram |= ImGui::InputDouble("Espessura alma tw (mm)", &cfg.perfil.tw, 0.5, 1.0, "%.2f");
                    if (dimensoes_mudaram) {
                        // Zera as propriedades derivadas e recalcula imediatamente, antes de exibi-las
                        cfg.perfil.nome = "Perfil Customizado";
                        cfg.perfil.r = 0.0; cfg.perfil.d1 = 0.0;
                        cfg.perfil.Ag = 0.0; cfg.perfil.Ix = 0.0; cfg.perfil.Iy = 0.0;
                        cfg.perfil.Wx = 0.0; cfg.perfil.Wy = 0.0; cfg.perfil.Zx = 0.0; cfg.perfil.Zy = 0.0;
                        cfg.perfil.rx = 0.0; cfg.perfil.ry = 0.0; cfg.perfil.rt = 0.0;
                        cfg.perfil.lbf = 0.0; cfg.perfil.lbw = 0.0;
                        cfg.perfil.It = 0.0; cfg.perfil.Cw = 0.0;
                        calcular_propriedades_secao_i(cfg.perfil);
                    }
                }

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Propriedades Nominais da Seção:");
                ImGui::BulletText("Nome: %s", cfg.perfil.nome.c_str());
                ImGui::BulletText("d = %.1f mm  |  bf = %.1f mm", cfg.perfil.d, cfg.perfil.bf);
                ImGui::BulletText("tf = %.2f mm  |  tw = %.2f mm", cfg.perfil.tf, cfg.perfil.tw);
                ImGui::BulletText("Área Ag = %.2f cm²  |  Inércia Ix = %.0f cm⁴", cfg.perfil.Ag / 100.0, cfg.perfil.Ix / 1e4);
                ImGui::BulletText("Módulo Elástico Wx = %.1f cm³  |  Plástico Zx = %.1f cm³", cfg.perfil.Wx / 1e3, cfg.perfil.Zx / 1e3);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Aço Estrutural:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Escoamento fy (MPa)", &cfg.aco.fy_MPa, 10.0, 50.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Módulo Elasticidade E (MPa)", &cfg.aco.E_MPa, 5000.0, 10000.0, "%.0f");
                ImGui::EndTabItem();
            }

            // ABA 2: LAJE DE CONCRETO & STEEL DECK
            if (ImGui::BeginTabItem("Laje & Deck")) {
                ImGui::Spacing();
                const char* tipos_laje[] = { "Laje Maciça de Concreto", "Laje Mista com Steel Deck" };
                ImGui::Text("Tipo da Solução:");
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::Combo("##TipoLajeCombo", &laje_idx, tipos_laje, 2)) {
                    if (laje_idx == 1 && cfg.laje.hF_mm <= 0.0) {
                        cfg.laje.hF_mm = 75.0;
                        cfg.laje.bF_mm = 137.0;
                        cfg.laje.eF_mm = 274.0;
                        if (cfg.laje.tc_mm > 100.0) cfg.laje.tc_mm = 65.0;
                    } else if (laje_idx == 0) {
                        cfg.laje.hF_mm = 0.0;
                    }
                }

                ImGui::Spacing();
                if (laje_idx == 1) {
                    const auto& lista_decks = obter_catalogo_steel_deck();
                    static std::vector<std::string> nomes_deck;
                    if (nomes_deck.empty()) {
                        nomes_deck.reserve(lista_decks.size());
                        for (const auto& item : lista_decks) {
                            nomes_deck.push_back(std::string(item.chave) + " (h=" + std::to_string((int)item.h_total_mm) + "mm, t=" + std::to_string(item.espessura_mm).substr(0, 4) + "mm)");
                        }
                    }

                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.0f), "Catálogo Metform (%d opções de fôrma):", (int)lista_decks.size());
                    ImGui::SetNextItemWidth(-1.0f);
                    if (ImGui::BeginCombo("##DeckCatalogoCombo", nomes_deck[deck_cat_idx].c_str(), ImGuiComboFlags_HeightLarge)) {
                        for (int n = 0; n < (int)nomes_deck.size(); n++) {
                            const bool is_selected = (deck_cat_idx == n);
                            if (ImGui::Selectable(nomes_deck[n].c_str(), is_selected)) {
                                deck_cat_idx = n;
                                const auto& d = lista_decks[n];
                                cfg.laje.hF_mm = d.hF_mm;
                                cfg.laje.eF_mm = d.passo_eF_mm;
                                cfg.laje.bF_mm = d.bF_medio_mm;
                                cfg.laje.tf_deck_mm = d.espessura_mm;
                                cfg.laje.tc_mm = d.h_total_mm - d.hF_mm;
                            }
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Spacing();
                }

                ImGui::SetNextItemWidth(input_w);
                if (laje_idx == 0) {
                    ImGui::InputDouble("Espessura da Laje tc (mm)", &cfg.laje.tc_mm, 5.0, 10.0, "%.1f");
                } else {
                    ImGui::InputDouble("Capa de Concreto tc (mm)", &cfg.laje.tc_mm, 5.0, 10.0, "%.1f");
                }

                if (laje_idx == 1) {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.0f), "Geometria da Fôrma Nervurada de Aço:");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Altura da Fôrma hF (mm)", &cfg.laje.hF_mm, 5.0, 10.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Largura da Nervura bF (mm)", &cfg.laje.bF_mm, 5.0, 10.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Passo entre Nervuras eF (mm)", &cfg.laje.eF_mm, 10.0, 50.0, "%.1f");

                    const char* orientacoes[] = {
                        "Nervuras Transversais (Perpendiculares à Viga)",
                        "Nervuras Longitudinais (Paralelas à Viga)"
                    };
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##OrientacaoCombo", &orient_deck_idx, orientacoes, 2);

                    ImGui::Spacing();
                    ImGui::BulletText("Altura total da laje: %.1f mm (Capa tc = %.1f mm + Fôrma hF = %.1f mm)",
                        cfg.laje.tc_mm + cfg.laje.hF_mm, cfg.laje.tc_mm, cfg.laje.hF_mm);
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Concreto da Laje:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Resistência fck (MPa)", &cfg.concreto.fck_MPa, 5.0, 10.0, "%.1f");
                const char* agregados[] = { "Basalto e diabásio (1,2)", "Granito e gnaisse (1,0)", "Calcário (0,9)", "Arenito (0,7)" };
                const double alphas_ag[] = { 1.2, 1.0, 0.9, 0.7 };
                ImGui::SetNextItemWidth(input_w * 1.6f);
                if (ImGui::Combo("Agregado graúdo (αE)", &agregado_idx, agregados, 4)) {
                    cfg.concreto.alpha_ag = alphas_ag[agregado_idx];
                }
                ImGui::Checkbox("Calcular Ec pela norma selecionada (4.6.3.1)", &ecs_automatico);
                if (ecs_automatico) {
                    ImGui::BulletText("Ec = %.0f MPa (%s)", cfg.concreto.Ecs_MPa,
                        norma_idx == 0 ? "0,85·αE·5600·√fck" : "4760·√fck");
                } else {
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Módulo Secante Ecs (MPa)", &cfg.concreto.Ecs_MPa, 500.0, 1000.0, "%.0f");
                }
                ImGui::BulletText("Fator eta_c (bloco compressão): %.3f", rel.fase2.momento_misto.eta_c);
                ImGui::EndTabItem();
            }

            // ABA 3: CONECTORES STUD BOLT
            if (ImGui::BeginTabItem("Stud Bolts")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Propriedades do Stud Bolt (Pino com Cabeça):");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Diâmetro do Pino d (mm)", &cfg.conector.d, 1.0, 3.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Altura do Pino h (mm)", &cfg.conector.h, 5.0, 10.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Resistência fu (MPa)", &cfg.conector.fu, 10.0, 50.0, "%.1f");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                bool eh_deck_perp = (cfg.laje.tipo == LAJE_STEEL_DECK && !cfg.laje.nervura_paralela);

                if (eh_deck_perp) {
                    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Disposição em Steel Deck com Nervuras Perpendiculares:");
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::InputInt("Pinos por Nervura (nt)", &cfg.conector.nt);
                    if (cfg.conector.nt < 1) cfg.conector.nt = 1;

                    ImGui::TextDisabled("Capacidade transversal da mesa bf: até %d pinos lado a lado", rel.conectores.nt_max_mesa);
                    if (cfg.conector.nt > rel.conectores.nt_max_mesa) {
                        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "ATENÇÃO: nt = %d excede a largura da mesa bf (máximo %d pinos)!", cfg.conector.nt, rel.conectores.nt_max_mesa);
                    }

                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::InputInt("Conector a cada k nervuras", &cfg.conector.k_nervuras);
                    if (cfg.conector.k_nervuras < 1) cfg.conector.k_nervuras = 1;

                    double passo_onda = (cfg.laje.eF_mm > 0.0) ? cfg.laje.eF_mm : 200.0;
                    ImGui::BulletText("Passo da onda da fôrma: eF = %.1f mm", passo_onda);
                    ImGui::BulletText("Espaçamento longitudinal resultante: s = k * eF = %.1f mm", rel.conectores.el_espacamento_mm);
                } else {
                    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Disposição dos Conectores (Laje Maciça ou Nervuras Paralelas):");
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::InputInt("Pinos por Linha Transversal (nt)", &cfg.conector.nt);
                    if (cfg.conector.nt < 1) cfg.conector.nt = 1;

                    ImGui::TextDisabled("Capacidade transversal da mesa bf: até %d pinos lado a lado", rel.conectores.nt_max_mesa);
                    if (cfg.conector.nt > rel.conectores.nt_max_mesa) {
                        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "ATENÇÃO: nt = %d excede a largura da mesa bf (máximo %d pinos)!", cfg.conector.nt, rel.conectores.nt_max_mesa);
                    }

                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::InputDouble("Espaçamento Longitudinal s (mm)", &cfg.conector.espacamento_adotado_mm, 10.0, 50.0, "%.1f");
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Diagnóstico da Conexão e Grau de Interação (Rotina E):");

                // Tipo e Grau de Interação
                if (rel.conectores.grau_interacao_eta >= 0.999) {
                    ImGui::BulletText("Tipo de Interação: INTERAÇÃO COMPLETA (eta = 100 %%)");
                } else {
                    ImGui::BulletText("Tipo de Interação: INTERAÇÃO PARCIAL (eta = %.1f %%)", rel.conectores.grau_interacao_eta * 100.0f);
                }

                // Grau mínimo
                if (rel.conectores.grau_interacao_atende) {
                    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "  -> Grau Mínimo Normativo: eta_min = %.2f [ATENDE: eta >= eta_min]", rel.conectores.grau_interacao_minimo);
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "  -> Grau Mínimo Normativo: eta_min = %.2f [NÃO ATENDE: eta < eta_min]", rel.conectores.grau_interacao_minimo);
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "     Reduza o espaçamento k ou aumente pinos/nervura para atingir o mínimo!");
                }

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Quantitativo de Conectores (Meio Vão / Vão Total):");
                ImGui::BulletText("Linhas no meio vão: %d linhas [nlin = floor((L/2)/s)]", rel.conectores.n_linhas_meio_vao);
                ImGui::BulletText("Conectores Adotados: %d por meio vão (%d no total da viga)", rel.conectores.n_semi_vao_adotado, rel.conectores.n_total_adotado);
                ImGui::BulletText("Necessários p/ Interação Completa (100%%): %d por meio vão (%d no total da viga)", rel.conectores.n_semi_vao_calculado, rel.conectores.nl_conectores);
                ImGui::BulletText("Necessários p/ Grau Mínimo (eta_min = %.2f): %d por meio vão (%d no total da viga)", rel.conectores.grau_interacao_minimo, rel.conectores.n_semi_vao_minimo, rel.conectores.n_total_minimo);

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Esforços e Capacidade Resistente:");
                ImGui::BulletText("Capacidade Unitária QRd: %.2f kN (Concreto: %.2f kN | Aço: %.2f kN)", rel.conectores.QRd_kN, rel.conectores.QRd1_concreto_kN, rel.conectores.QRd2_aco_kN);
                ImGui::BulletText("Força Rasante Fhd (100%%): %.2f kN  |  Força Transmitida: %.2f kN", rel.conectores.Fhd_kN, rel.conectores.n_semi_vao_adotado * rel.conectores.QRd_kN);
                ImGui::BulletText("Coeficientes Normativos: Rg = %.2f  |  Rp = %.2f", rel.conectores.Rg, rel.conectores.Rp);

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Verificações Geométricas e Limites Normativos:");
                ImGui::BulletText("Espaçamento Longitudinal: s = %.0f mm (Limites: %.0f a %.0f mm) %s", rel.conectores.el_espacamento_mm, rel.conectores.s_min_long_mm, rel.conectores.s_max_long_mm, rel.conectores.espacamento_atende ? "[OK]" : "[FORA DOS LIMITES]");
                if (rel.conectores.alerta_ancoragem_deck) {
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "  [AVISO L.2.6.2-b] Espaçamento s > 450 mm: Prever fixação auxiliar da fôrma (soldas tampão) a cada 450 mm.");
                }
                ImGui::BulletText("Altura do Pino: h = %.0f mm (Mínimo exigido: %.0f mm) %s", cfg.conector.h, rel.conectores.h_min_normativo_mm, rel.conectores.altura_atende ? "[OK]" : "[INSUFICIENTE]");
                ImGui::BulletText("Cobrimento Superior: c = %.1f mm (Mínimo da norma: %.1f mm) %s", rel.conectores.cobrimento_topo_mm, rel.conectores.cobrimento_min_mm, rel.conectores.cobrimento_atende ? "[OK]" : "[INSUFICIENTE]");
                ImGui::BulletText("Diâmetro do Pino: d = %.1f mm (Máximo p/ mesa tf: %.1f mm) %s", cfg.conector.d, rel.conectores.d_max_mesa_mm, rel.conectores.diametro_mesa_atende ? "[OK]" : "[EXCESSIVO]");
                ImGui::BulletText("Espaçamento Transversal: st_min = %.1f mm (4*d)", rel.conectores.s_transv_min_mm);
                ImGui::BulletText("Largura da Mesa: bf = %.1f mm (Necessário p/ %d pinos: %.1f mm) %s", cfg.perfil.bf, cfg.conector.nt, rel.conectores.bf_min_necessario_mm, rel.conectores.largura_mesa_atende ? "[OK]" : "[INSUFICIENTE]");
                if (cfg.laje.tipo == LAJE_STEEL_DECK && cfg.laje.nervura_paralela) {
                    ImGui::BulletText("Calha Steel Deck: w_fundo = %.1f mm (Necessário: %.1f mm) %s", rel.conectores.largura_deck_disp_mm, rel.conectores.largura_deck_nec_mm, rel.conectores.nervura_deck_atende ? "[OK]" : "[COLISÃO]");
                }
                ImGui::EndTabItem();
            }

            // ABA 4: VÃO E CARGAS
            if (ImGui::BeginTabItem("Vão & Cargas")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Geometria do Piso:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Vão da Viga L (mm)", &cfg.geometria.L_vao_mm, 500.0, 1000.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Comprimento Destravado Lb (mm)", &cfg.geometria.Lb_mm, 250.0, 500.0, "%.0f");
                if (laje_idx == 1) {
                    ImGui::TextDisabled("Steel deck fixado ao perfil contém a mesa: use Lb = 0 na concretagem.");
                }
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Distância viga esquerda (mm)", &cfg.geometria.dist_esq_mm, 100.0, 500.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Distância viga direita (mm)", &cfg.geometria.dist_dir_mm, 100.0, 500.0, "%.0f");
                ImGui::Checkbox("Construção Escorada", &cfg.geometria.escorada);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Checkbox("Usar Assistente / Calculadora de Cargas (NBR 6120:2019)", &usar_assistente_cargas);
                if (usar_assistente_cargas) {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Configuração de Uso e Sobrecargas (NBR 6120):");

                    // 1. Ocupação
                    const auto& ocups = nbr6120::obter_tabela_ocupacoes();
                    static std::vector<const char*> nomes_ocups;
                    if (nomes_ocups.empty()) {
                        nomes_ocups.reserve(ocups.size());
                        for (const auto& item : ocups) nomes_ocups.push_back(item.descricao);
                    }
                    ImGui::Text("Uso e Ocupação (Tabela 10):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##OcupacaoCombo", &assist_cfg.idx_ocupacao, nomes_ocups.data(), (int)nomes_ocups.size());

                    if (assist_cfg.idx_ocupacao == (int)ocups.size() - 1) {
                        ImGui::SetNextItemWidth(input_w);
                        ImGui::InputDouble("Carga de Uso Custom (kN/m²)", &assist_cfg.q_custom_kN_m2, 0.5, 1.0, "%.2f");
                    }

                    // 2. Divisórias
                    const auto& divs = nbr6120::obter_tabela_divisorias();
                    static std::vector<const char*> nomes_divs;
                    if (nomes_divs.empty()) {
                        nomes_divs.reserve(divs.size());
                        for (const auto& item : divs) nomes_divs.push_back(item.descricao);
                    }
                    ImGui::Text("Divisórias sem posição definida (Tabela 11):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##DivisoriaCombo", &assist_cfg.idx_divisoria, nomes_divs.data(), (int)nomes_divs.size());

                    // 3. Revestimento
                    const auto& revs = nbr6120::obter_tabela_revestimentos();
                    static std::vector<const char*> nomes_revs;
                    if (nomes_revs.empty()) {
                        nomes_revs.reserve(revs.size());
                        for (const auto& item : revs) nomes_revs.push_back(item.descricao);
                    }
                    ImGui::Text("Revestimento do Piso (Tabela 4):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##RevestimentoCombo", &assist_cfg.idx_revestimento, nomes_revs.data(), (int)nomes_revs.size());

                    // 4. Forro
                    const auto& forros = nbr6120::obter_tabela_forros();
                    static std::vector<const char*> nomes_forros;
                    if (nomes_forros.empty()) {
                        nomes_forros.reserve(forros.size());
                        for (const auto& item : forros) nomes_forros.push_back(item.descricao);
                    }
                    ImGui::Text("Forro do Teto (Tabela 8):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##ForroCombo", &assist_cfg.idx_forro, nomes_forros.data(), (int)nomes_forros.size());

                    // 5. Instalações
                    const auto& insts = nbr6120::obter_tabela_instalacoes();
                    static std::vector<const char*> nomes_insts;
                    if (nomes_insts.empty()) {
                        nomes_insts.reserve(insts.size());
                        for (const auto& item : insts) nomes_insts.push_back(item.descricao);
                    }
                    ImGui::Text("Instalações Suspensas / Dutos (Tabela 8):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##InstalacaoCombo", &assist_cfg.idx_instalacao, nomes_insts.data(), (int)nomes_insts.size());

                    // 6. Paredes
                    const auto& paredes = nbr6120::obter_tabela_paredes();
                    static std::vector<const char*> nomes_paredes;
                    if (nomes_paredes.empty()) {
                        nomes_paredes.reserve(paredes.size());
                        for (const auto& item : paredes) nomes_paredes.push_back(item.descricao);
                    }
                    ImGui::Text("Parede Apoiada sobre a Viga (Tabelas 2 e 3):");
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##ParedeCombo", &assist_cfg.idx_parede, nomes_paredes.data(), (int)nomes_paredes.size());

                    if (assist_cfg.idx_parede > 0) {
                        ImGui::SetNextItemWidth(input_w);
                        ImGui::InputDouble("Altura da Parede (m)", &assist_cfg.altura_parede_m, 0.1, 0.5, "%.2f");
                    }

                    // Calcula automaticamente com base na largura de influência
                    double binf_calc = (cfg.geometria.dist_esq_mm + cfg.geometria.dist_dir_mm) / 2000.0;
                    if (binf_calc <= 0.0) binf_calc = 2.0;

                    const auto& decks = obter_catalogo_steel_deck();
                    double peso_deck_laje = (deck_cat_idx >= 0 && deck_cat_idx < (int)decks.size()) ? decks[deck_cat_idx].peso_proprio_laje_kN_m2 : 2.50;
                    double consumo_c = (deck_cat_idx >= 0 && deck_cat_idx < (int)decks.size()) ? decks[deck_cat_idx].consumo_concreto_m3_m2 : 0.10;

                    // Estima massa do perfil
                    double massa_p = (cfg.perfil.Ag > 0.0) ? (cfg.perfil.Ag * 1e-6 * 7850.0) : 52.0;

                    auto res_cargas = nbr6120::calcular_cargas_assistente(
                        assist_cfg,
                        binf_calc,
                        massa_p,
                        cfg.laje.tipo == LAJE_STEEL_DECK,
                        peso_deck_laje,
                        consumo_c,
                        cfg.laje.tc_mm
                    );

                    // Atualiza cargas da viga
                    cfg.acoes.g1_kN_m = res_cargas.g1_linear_kN_m;
                    cfg.acoes.q1_kN_m = res_cargas.q1_linear_kN_m;
                    cfg.acoes.g2_kN_m = res_cargas.g2_linear_kN_m;
                    cfg.acoes.q2_kN_m = res_cargas.q2_linear_kN_m;
                    cfg.acoes.psi1 = res_cargas.psi1;
                    cfg.acoes.psi2 = res_cargas.psi2;

                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Resultados Calculados pela NBR 6120 (binf = %.2f m):", binf_calc);
                    ImGui::BulletText("g1 (Fase 1 - Laje + Perfil): %.3f kN/m", cfg.acoes.g1_kN_m);
                    ImGui::BulletText("q1 (Fase 1 - Sobrecarga Construtiva): %.3f kN/m", cfg.acoes.q1_kN_m);
                    ImGui::BulletText("g2 (Fase 2 - Permanentes Pós-Cura): %.3f kN/m", cfg.acoes.g2_kN_m);
                    ImGui::BulletText("q2 (Fase 2 - Uso e Ocupação Total): %.3f kN/m", cfg.acoes.q2_kN_m);
                    ImGui::BulletText("Fatores: psi0 = %.2f | psi1 = %.2f | psi2 = %.2f (Cat. %c)",
                        res_cargas.psi0, res_cargas.psi1, res_cargas.psi2, res_cargas.categoria_psi);
                } else {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Ações Lineares Diretas (kN/m):");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Permanente Fase 1 g1 (kN/m)", &cfg.acoes.g1_kN_m, 0.5, 1.0, "%.3f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Variável Construção q1 (kN/m)", &cfg.acoes.q1_kN_m, 0.5, 1.0, "%.3f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Permanente Fase 2 g2 (kN/m)", &cfg.acoes.g2_kN_m, 0.5, 1.0, "%.3f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Variável de Uso q2 (kN/m)", &cfg.acoes.q2_kN_m, 0.5, 1.0, "%.3f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Fator psi1", &cfg.acoes.psi1, 0.05, 0.1, "%.2f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Fator psi2", &cfg.acoes.psi2, 0.05, 0.1, "%.2f");
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Coeficientes de Ponderação (Tabela 1):");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("γg de g1 na construção", &cfg.acoes.gamma_g1, 0.05, 0.1, "%.3f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("γq da sobrecarga de construção", &cfg.acoes.gamma_q1, 0.05, 0.1, "%.3f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("γg de g1 (combinação normal)", &cfg.acoes.gamma_g1_fase2, 0.05, 0.1, "%.3f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("γg de g2 (combinação normal)", &cfg.acoes.gamma_g2, 0.05, 0.1, "%.3f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("γq de q2 (combinação normal)", &cfg.acoes.gamma_q2, 0.05, 0.1, "%.3f");
                ImGui::EndTabItem();
            }

            // ABA 5: ESTADOS-LIMITES DE SERVIÇO (FLECHAS E VIBRAÇÃO)
            if (ImGui::BeginTabItem("ELS & Vibração")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Deslocamentos (Anexo B e L.1.2):");
                const char* combos[] = { "Quase permanente (ψ2)", "Frequente (ψ1)", "Rara (1,0)" };
                int combo_max = static_cast<int>(cfg.acoes.combinacao_flecha_max);
                int combo_dano = static_cast<int>(cfg.acoes.combinacao_flecha_dano);
                ImGui::SetNextItemWidth(input_w * 1.6f);
                if (ImGui::Combo("Combinação de δmax", &combo_max, combos, 3)) cfg.acoes.combinacao_flecha_max = static_cast<CombinacaoServico>(combo_max);
                ImGui::SetNextItemWidth(input_w * 1.6f);
                if (ImGui::Combo("Combinação de δ2+δ3", &combo_dano, combos, 3)) cfg.acoes.combinacao_flecha_dano = static_cast<CombinacaoServico>(combo_dano);
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Limite L / ...", &cfg.acoes.denominador_limite_flecha, 10.0, 50.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Contraflecha (mm)", &cfg.acoes.contraflecha_mm, 1.0, 5.0, "%.1f");
                ImGui::Checkbox("Alvenaria solidarizada à viga (δ2+δ3 ≤ 15 mm)", &cfg.acoes.alvenaria_solidarizada);
                ImGui::Checkbox("Considerar retração do concreto", &cfg.acoes.considerar_retracao);
                if (cfg.acoes.considerar_retracao) {
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("εsh", &cfg.acoes.eps_sh, 0.00005, 0.0001, "%.5f");
                }
                ImGui::BulletText("δg1 = %.1f mm | δ1 = %.1f | δ2 = %.1f | δ3 = %.1f mm",
                    rel.flechas.flecha_g1_mm, rel.flechas.flecha_perm_curta_mm,
                    rel.flechas.flecha_longa_duracao_mm, rel.flechas.flecha_var_max_mm);
                ImGui::BulletText("δmax = %.1f mm (lim. %.1f) | δ2+δ3 = %.1f mm (lim. %.1f)",
                    rel.flechas.flecha_total_mm, rel.flechas.limite_total_mm,
                    rel.flechas.flecha_dano_mm, rel.flechas.limite_dano_mm);
                ImGui::BulletText("Contraflecha sugerida: %.0f mm", rel.flechas.contraflecha_sugerida_mm);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Vibração do Piso (Anexo I, modo A):");
                ImGui::Checkbox("Verificar vibração", &cfg.vibracao.verificar);
                if (cfg.vibracao.verificar) {
                    const auto& ocups_vib = nbr8800::obter_tabela_ocupacoes_vibracao();
                    static std::vector<const char*> nomes_vib;
                    if (nomes_vib.empty()) {
                        for (const auto& o : ocups_vib) nomes_vib.push_back(o.descricao);
                    }
                    ImGui::SetNextItemWidth(-1.0f);
                    if (ImGui::Combo("##OcupVib", &ocup_vib_idx, nomes_vib.data(), (int)nomes_vib.size())) {
                        nbr8800::aplicar_ocupacao_vibracao(cfg.vibracao, ocups_vib[ocup_vib_idx]);
                    }
                    ImGui::Checkbox("Forros e instalações suspensos (+0,010)", &cfg.vibracao.forro_instalacoes_suspensos);
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("β divisórias piso-teto", &cfg.vibracao.beta_divisorias, 0.005, 0.01, "%.3f");
                    ImGui::Checkbox("Viga interna (kb = 2,0)", &cfg.vibracao.viga_interna);
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Largura do piso np·Lp (m)", &cfg.vibracao.largura_piso_m, 1.0, 5.0, "%.1f");
                    const char* criterios[] = { "Aceleração de pico a_p", "Fator de resposta R" };
                    int crit = static_cast<int>(cfg.vibracao.criterio);
                    ImGui::SetNextItemWidth(input_w * 1.6f);
                    if (ImGui::Combo("Critério", &crit, criterios, 2)) cfg.vibracao.criterio = static_cast<CriterioVibracao>(crit);

                    const auto& v = rel.vibracao;
                    if (norma_idx == 0) {
                        ImGui::BulletText("Is = %.0f mm⁴/mm %s", v.Is_mm4_por_mm,
                            cfg.vibracao.Is_laje_mm4_por_mm > 0.0 ? "(catálogo Metform)" : "(tc³/12α)");
                        ImGui::BulletText("δ = %.2f mm  ->  f0 = %.2f Hz%s", v.delta_mm, v.f0_Hz,
                            v.faixa_metodo_valida ? "" : "  (fora de 3-9 Hz)");
                        ImGui::BulletText("Bb = %.2f m | M = %.0f kg | β = %.3f", v.Bb_m, v.M_kg, v.beta);
                        ImGui::BulletText("a_p = %.4f m/s² (lim. %.3f) | R = %.2f (máx. %.0f)",
                            v.ap_m_s2, v.ap_lim_m_s2, v.R, v.R_max);
                    } else {
                        ImGui::BulletText("NBR 8800:2008 L.3.2: δ frequente = %.1f mm ≤ 20 mm",
                            rel.flechas.flecha_vibracao_2008_mm);
                    }
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // --------------------------------------------------------------------
        // PAINEL INFERIOR DIREITO: RESULTADOS CONSOLIDADOS (DASHBOARD)
        // --------------------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(painel_dir_x, tabela_y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(painel_dir_w, tabela_h), ImGuiCond_Always);
        ImGui::Begin("Diagnóstico Consolidado ELU & ELS (Verificador)", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (ImGui::BeginTable("KpiTable", 5, ImGuiTableFlags_None)) {
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Momento", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Conexao", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Flecha", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Vibracao", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImVec4 corAprovado  = tema_escuro ? ImVec4(0.20f, 0.85f, 0.35f, 1.0f) : ImVec4(0.08f, 0.58f, 0.20f, 1.0f);
            ImVec4 corReprovado = tema_escuro ? ImVec4(1.00f, 0.25f, 0.25f, 1.0f) : ImVec4(0.85f, 0.15f, 0.15f, 1.0f);
            ImVec4 corMomento   = tema_escuro ? ImVec4(0.30f, 0.70f, 1.00f, 1.0f) : ImVec4(0.10f, 0.40f, 0.85f, 1.0f);

            if (rel.aprovado_global) {
                ImGui::TextColored(corAprovado, "STATUS GERAL:");
                ImGui::TextColored(corAprovado, "APROVADO");
            } else {
                ImGui::TextColored(corReprovado, "STATUS GERAL:");
                ImGui::TextColored(corReprovado, "NÃO ATENDE");
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Momento Misto:");
            ImGui::TextColored(corMomento, "%.1f / %.1f kNm", rel.fase2.MSd2_kNm, rel.fase2.momento_misto.MRd_VM_kNm);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Conexão Studs:");
            ImVec4 corConn = (rel.conectores.grau_interacao_atende) ? corAprovado : corReprovado;
            ImGui::TextColored(corConn, "eta = %.0f %% (%d pinos)", rel.conectores.grau_interacao_eta * 100.0f, rel.conectores.n_total_adotado);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Flecha Serviço:");
            ImVec4 corFlecha = (rel.flechas.aprovado_flecha) ? corAprovado : corReprovado;
            double razao_L = (rel.flechas.flecha_total_mm > 0.05) ? (cfg.geometria.L_vao_mm / rel.flechas.flecha_total_mm) : 9999.0;
            ImGui::TextColored(corFlecha, "%.1f mm (L/%.0f)", rel.flechas.flecha_total_mm, razao_L);

            ImGui::TableSetColumnIndex(4);
            ImGui::Text("Vibração:");
            if (!cfg.vibracao.verificar) {
                ImGui::TextDisabled("não verificada");
            } else if (norma_idx == 0) {
                ImVec4 corVib = rel.vibracao.aprovado ? corAprovado : corReprovado;
                ImGui::TextColored(corVib, "f0 = %.2f Hz", rel.vibracao.f0_Hz);
            } else {
                ImVec4 corVib = (rel.flechas.flecha_vibracao_2008_mm <= 20.0) ? corAprovado : corReprovado;
                ImGui::TextColored(corVib, "δ = %.1f mm", rel.flechas.flecha_vibracao_2008_mm);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTable("TabelaSintese", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Item Verificado", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Solicitante", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Resistente / Limite", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Utilização", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableHeadersRow();

            for (const auto& item : rel.itens) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", item.item.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f %s", item.solicitante, item.unidade.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f %s", item.resistente, item.unidade.c_str());

                ImGui::TableSetColumnIndex(3);
                float prog = (float)item.taxa_utilizacao;
                ImVec4 corBarra;
                if (tema_escuro) {
                    corBarra = (prog <= 0.75f) ? ImVec4(0.20f, 0.85f, 0.35f, 1.0f)
                             : ((prog <= 1.00f) ? ImVec4(0.95f, 0.70f, 0.15f, 1.0f)
                                               : ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
                } else {
                    corBarra = (prog <= 0.75f) ? ImVec4(0.10f, 0.60f, 0.22f, 1.0f)
                             : ((prog <= 1.00f) ? ImVec4(0.85f, 0.50f, 0.05f, 1.0f)
                                               : ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
                }
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, corBarra);
                ImGui::ProgressBar(std::min(prog, 1.0f), ImVec2(110, 18), TextFormat("%.1f %%", prog * 100.0f));
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex(4);
                ImVec4 corStatus = item.aprovado ? (tema_escuro ? ImVec4(0.20f, 0.85f, 0.35f, 1.0f) : ImVec4(0.08f, 0.58f, 0.20f, 1.0f))
                                                 : (tema_escuro ? ImVec4(0.95f, 0.25f, 0.25f, 1.0f) : ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
                ImGui::TextColored(corStatus, item.aprovado ? "OK" : "FALHA");
            }

            ImGui::EndTable();
        }

        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    if (fontRaylib.texture.id > 0) UnloadFont(fontRaylib);
    if (fontRaylibTitulo.texture.id > 0) UnloadFont(fontRaylibTitulo);
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
