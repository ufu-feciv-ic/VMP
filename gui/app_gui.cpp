#include "modelos_viga_mista.h"
#include "rotinas_viga_mista.h"
#include "catalogo.h"
#include "geometria.h"

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace viga_mista;

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

// Callback para carregamento da fonte Dear ImGui com nitidez máxima, acentos e algarismos limpos
void carregar_fontes_imgui() {
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.OversampleH = 3;
    cfg.OversampleV = 2;
    cfg.PixelSnapH = true;

    // Range completo cobrindo Latin Básico + Latin Suplementar (acentos da língua portuguesa: ç, á, é, í, ó, ú, ã, õ, etc.)
    // + Latin Extendido-A + Letras Gregas (η, δ) + Operadores Matemáticos (≤, ≥, ±, ², ³)
    static const ImWchar glyph_ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement (Todos os acentos do português)
        0x0100, 0x017F, // Latin Extended-A
        0x0370, 0x03FF, // Greek (η, δ, etc.)
        0x2010, 0x205E, // General Punctuation
        0x2070, 0x209F, // Superscripts and Subscripts (², ³, etc.)
        0x2200, 0x22FF, // Mathematical Operators (≤, ≥, ±, etc.)
        0
    };

    // Fontes corporativas e de engenharia do Windows com excelente distinção numérica
    const char* candidatos[] = {
        "C:\\Windows\\Fonts\\segoeui.ttf",     // Segoe UI (Padrão moderno, números límpidos e abertos)
        "C:\\Windows\\Fonts\\calibri.ttf",     // Calibri (Alternativa limpa e suave)
        "C:\\Windows\\Fonts\\arial.ttf"        // Arial (Fallback universal)
    };

    bool carregada = false;
    for (const char* caminho : candidatos) {
        if (FileExists(caminho)) {
            // Tamanho 17.5f com escala nativa 1.0f e ranges de acentuação completos
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
    
    // Geometria e cantos arredondados modernos
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 5.0f;

    // Espaçamentos e respiração visual otimizados
    style.WindowPadding     = ImVec2(14.0f, 12.0f);
    style.FramePadding      = ImVec2(8.0f, 5.0f);
    style.ItemSpacing       = ImVec2(10.0f, 7.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 5.0f);
    style.IndentSpacing     = 18.0f;
    style.ScrollbarSize     = 12.0f;

    ImVec4* colors = style.Colors;
    if (escuro) {
        // TEMA ESCURO (Modern Engineering Slate / Dark Blue)
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
        // TEMA CLARO (Modern Clean Engineering White / Slate)
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

    // Paleta de cores dinâmica sensível ao tema (Escuro vs Claro)
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

    // 1. ZONA LIVRE VERTICAL SEGURA (Sem invadir a barra de título superior de 38px)
    float y_min_desenho = canvas_topo_y + 38.0f + 32.0f; // 38px título + 32px gap seguro para a anotação da laje
    float y_max_desenho = canvas_base_y - 36.0f;         // 36px gap seguro para a cota inferior do perfil
    float h_disp_h = y_max_desenho - y_min_desenho;
    if (h_disp_h < 120.0f) h_disp_h = 120.0f;

    // Altura física real do pacote estrutural completo (Aço + Laje)
    float hF_real = (cfg.laje.tipo == TipoLajeMista::STEEL_DECK) ? (float)cfg.laje.hF_deck_mm : 0.0f;
    float tc_real = (float)cfg.laje.tc_mm;
    float H_fisica = (float)p.d + hF_real + tc_real;
    if (H_fisica <= 0.0f) H_fisica = 500.0f;

    // Largura representativa da laje de concreto (deixamos 240px de margem lateral para badges da LNP)
    float max_w_livre = canvas_largura - 240.0f;
    if (max_w_livre < 160.0f) max_w_livre = 160.0f;

    float W_fisica = (float)rel.fase2.bef_mm;
    if (W_fisica <= 0.0f) W_fisica = 1500.0f;

    float W_visual_fisica = std::min(W_fisica, (float)p.d * 3.8f);
    if (W_visual_fisica < (float)p.bf * 2.4f) W_visual_fisica = (float)p.bf * 2.4f;

    // Escala geométrica unificada mantendo proporção de aspecto real
    float escala_h = h_disp_h / H_fisica;
    float escala_w = max_w_livre / W_visual_fisica;
    float escala = std::min(escala_h, escala_w);
    if (escala > 0.40f) escala = 0.40f;
    if (escala < 0.16f) escala = 0.16f;

    // Dimensões escaladas para pixels com espessuras visuais nítidas
    float d = (float)p.d * escala;
    float bf = (float)p.bf * escala;
    float tf = std::max(7.0f, (float)p.tf * escala);
    float tw = std::max(4.5f, (float)p.tw * escala);
    float tc = tc_real * escala;
    float hF = hF_real * escala;
    float passo = (cfg.laje.passo_deck_mm > 0.0) ? (float)cfg.laje.passo_deck_mm * escala : bf * 1.30f;
    if (passo < 35.0f) passo = 35.0f;
    float bF = (cfg.laje.bF_deck_mm > 0.0) ? (float)cfg.laje.bF_deck_mm * escala : passo * 0.55f;
    if (bF > passo * 0.80f) bF = passo * 0.80f;
    if (bF < 15.0f) bF = 15.0f;

    // Rampa inclinada e fundo plano da nervura do steel deck
    float w_rampa = std::min(hF * 0.45f, (passo - bF) * 0.40f);
    if (w_rampa < 4.0f) w_rampa = 4.0f;
    float w_fundo = bF - w_rampa;
    if (w_fundo < 12.0f) w_fundo = 12.0f;
    float half_fundo = w_fundo * 0.5f;

    float largura_laje = W_visual_fisica * escala;
    float x_esq_laje = (float)centro_x - largura_laje / 2.0f;
    float x_dir_laje = x_esq_laje + largura_laje;

    // Centralização vertical estrita dentro da zona segura
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
    // Mesa Superior
    DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, corAco);
    // Alma
    float h_alma = d - 2.0f * tf;
    DrawRectangle((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)h_alma, corAco);
    // Mesa Inferior
    DrawRectangle((int)x_esq_mesa, (int)(y_base_aco - tf), (int)bf, (int)tf, corAco);

    // Contornos nítidos do perfil
    DrawRectangleLines((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, corBordaAco);
    DrawRectangleLines((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)h_alma, corBordaAco);
    DrawRectangleLines((int)x_esq_mesa, (int)(y_base_aco - tf), (int)bf, (int)tf, corBordaAco);

    // 3. Desenho da Laje de Concreto & Steel Deck com MESMO COMPRIMENTO RIGOROSO [x_esq_laje, x_dir_laje]
    if (cfg.laje.tipo == TipoLajeMista::MACICA || hF <= 0.0f) {
        // ================= CASO 1: LAJE MACIÇA DE CONCRETO =================
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, corConcreto);
        DrawRectangleLines((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, corBordaConc);

        // Badge estilizado com tipografia nítida
        float w_badge_m = medir_largura_texto(font, "Laje Maciça", 16.0f);
        Color corBadgeConcBg = tema_escuro ? Fade(DARKGREEN, 0.80f) : GetColor(0x15803DFF);
        DrawRectangle((int)x_esq_laje + 10, (int)y_topo_laje + 8, (int)w_badge_m + 16, 22, corBadgeConcBg);
        desenhar_texto_nitido(font, "Laje Maciça", x_esq_laje + 18.0f, y_topo_laje + 11.0f, 16.0f, WHITE);
    }
    else if (cfg.laje.orientacao == OrientacaoDeck::PERPENDICULAR_A_VIGA) {
        // ================= CASO 2: STEEL DECK PERPENDICULAR À VIGA =================
        float h_total_laje = tc + hF;
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)h_total_laje, corConcreto);

        // Chapa de aço do Steel Deck na face inferior contínua sobre a mesa de aço
        DrawLineEx(Vector2{ x_esq_laje, y_topo_aco }, Vector2{ x_dir_laje, y_topo_aco }, 3.0f, corFormaAco);

        // Linha demarcatória da crista da fôrma
        for (float x = x_esq_laje; x < x_dir_laje; x += 14.0f) {
            float x_fim = std::min(x + 8.0f, x_dir_laje);
            DrawLineEx(Vector2{ x, y_crista_deck }, Vector2{ x_fim, y_crista_deck }, 1.2f, Fade(corFormaAco, 0.45f));
        }

        // Contorno externo do concreto
        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_dir_laje, y_topo_laje }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_esq_laje, y_topo_laje }, Vector2{ x_esq_laje, y_topo_aco }, 1.8f, corBordaConc);
        DrawLineEx(Vector2{ x_dir_laje, y_topo_laje }, Vector2{ x_dir_laje, y_topo_aco }, 1.8f, corBordaConc);

        // Badge informativo com tipografia nítida
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
        // ================= CASO 3: STEEL DECK PARALELO À VIGA =================
        // Alinhamento rigoroso: um fundo de nervura tem seu centro exatamente no eixo da mesa (centro_x).
        // A partir dele, o padrão trapezoidal propaga-se simetricamente para os dois lados.
        auto avaliar_y_chapa_paralela = [&](float x) -> float {
            float dx = x - (float)centro_x;
            float u = fmodf(dx + passo * 0.5f, passo);
            if (u < 0.0f) u += passo;
            u -= passo * 0.5f;
            float abs_u = fabsf(u);

            if (abs_u <= half_fundo) {
                // Fundo da nervura assentado diretamente no nível da mesa
                return y_topo_aco;
            } else if (abs_u <= half_fundo + w_rampa) {
                // Rampa inclinada da chapa
                float t = (abs_u - half_fundo) / w_rampa;
                return y_topo_aco - t * hF;
            } else {
                // Crista superior da fôrma
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

    // 4. Conectores Stud Bolt (Desenhados SOBRE a laje para ficarem 100% visíveis em toda altura h)
    float h_stud = (float)cfg.conector.h_cs_mm * escala;
    float d_stud = (float)cfg.conector.d_cs_mm * escala;
    if (d_stud < 4.0f) d_stud = 4.0f;
    if (h_stud < 10.0f) h_stud = 10.0f;

    int n_pinos = cfg.conector.n_por_linha;
    float max_disp_pinos = (cfg.laje.tipo == TipoLajeMista::STEEL_DECK && cfg.laje.orientacao == OrientacaoDeck::PARALELA_A_VIGA)
                           ? std::min(w_fundo * 0.70f, bf * 0.42f) : (bf * 0.42f);
    float espacamento_transv = (n_pinos > 1) ? (max_disp_pinos / (n_pinos - 1)) : 0.0f;
    float x_inicio_pinos = (float)centro_x - (n_pinos - 1) * espacamento_transv / 2.0f;

    for (int i = 0; i < n_pinos; ++i) {
        float px = x_inicio_pinos + i * espacamento_transv;
        // Haste cilíndrica com cor metálica e contorno escuro nítido
        DrawRectangle((int)(px - d_stud / 2.0f), (int)(y_topo_aco - h_stud), (int)d_stud, (int)h_stud, corStud);
        DrawRectangleLines((int)(px - d_stud / 2.0f), (int)(y_topo_aco - h_stud), (int)d_stud, (int)h_stud, DARKGRAY);
        // Cabeça forjada do conector stud bolt
        float d_cabeca = d_stud * 1.6f;
        float h_cabeca = d_stud * 0.5f;
        DrawRectangle((int)(px - d_cabeca / 2.0f), (int)(y_topo_aco - h_stud - h_cabeca), (int)d_cabeca, (int)h_cabeca, corStudCabeca);
        DrawRectangleLines((int)(px - d_cabeca / 2.0f), (int)(y_topo_aco - h_stud - h_cabeca), (int)d_cabeca, (int)h_cabeca, BLACK);
    }

    // 5. Linha Neutra Plástica (LNP) e Bloco de Compressão
    if (rel.fase2.lnp_na_laje && rel.fase2.a_bloco_concreto_mm > 0.0) {
        float a_comp = (float)rel.fase2.a_bloco_concreto_mm * escala;
        if (a_comp > tc) a_comp = tc;
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)a_comp, Fade(RED, 0.25f));
        DrawLineEx(Vector2{ x_esq_laje - 18.0f, y_topo_laje + a_comp }, Vector2{ x_dir_laje + 18.0f, y_topo_laje + a_comp }, 1.5f, RED);
        
        std::string txt_lnp = TextFormat("LNP (Laje): a = %.1f mm", rel.fase2.a_bloco_concreto_mm);
        float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
        float x_badge_lnp = x_dir_laje + 8.0f;
        float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
        if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

        DrawRectangle((int)x_badge_lnp, (int)(y_topo_laje + a_comp - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
        desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_topo_laje + a_comp - 8.0f, 16.0f, WHITE);
    } else if (!rel.fase2.lnp_na_laje) {
        DrawRectangle((int)x_esq_laje, (int)y_topo_laje, (int)largura_laje, (int)tc, Fade(RED, 0.25f));

        if (rel.fase2.yp_lnp_aco_mm <= cfg.perfil.tf) {
            float ratio = (float)(rel.fase2.yp_lnp_aco_mm / std::max(0.1, cfg.perfil.tf));
            float y_lnp_mesa = y_topo_aco + ratio * tf;

            DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)(ratio * tf), Fade(RED, 0.38f));
            DrawLineEx(Vector2{ x_esq_mesa - 18.0f, y_lnp_mesa }, Vector2{ x_esq_mesa + bf + 18.0f, y_lnp_mesa }, 1.8f, RED);

            std::string txt_lnp = TextFormat("LNP (Mesa): yp = %.1f mm (tf = %.1f mm)", rel.fase2.yp_lnp_aco_mm, cfg.perfil.tf);
            float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
            float x_badge_lnp = x_esq_mesa + bf + 8.0f;
            float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
            if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

            DrawRectangle((int)x_badge_lnp, (int)(y_lnp_mesa - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
            desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_lnp_mesa - 8.0f, 16.0f, WHITE);
        } else {
            float h_wc_real = (float)(rel.fase2.yp_lnp_aco_mm - cfg.perfil.tf);
            float h_alma_total_real = (float)(cfg.perfil.d - 2.0 * cfg.perfil.tf);
            float ratio_alma = (h_alma_total_real > 0.1f) ? (h_wc_real / h_alma_total_real) : 0.0f;
            float y_lnp_alma = y_topo_aco + tf + ratio_alma * h_alma;

            DrawRectangle((int)x_esq_mesa, (int)y_topo_aco, (int)bf, (int)tf, Fade(RED, 0.38f));
            DrawRectangle((int)((float)centro_x - tw / 2.0f), (int)(y_topo_aco + tf), (int)tw, (int)(ratio_alma * h_alma), Fade(RED, 0.45f));
            DrawLineEx(Vector2{ (float)centro_x - bf * 0.45f, y_lnp_alma }, Vector2{ (float)centro_x + bf * 0.45f, y_lnp_alma }, 1.8f, RED);

            std::string txt_lnp = TextFormat("LNP (Alma): yp = %.1f mm (tf = %.1f mm)", rel.fase2.yp_lnp_aco_mm, cfg.perfil.tf);
            float w_b_lnp = medir_largura_texto(font, txt_lnp.c_str(), 16.0f);
            float x_badge_lnp = (float)centro_x + tw / 2.0f + 10.0f;
            float x_max_badge = painel_dir_x_fim - w_b_lnp - 14.0f;
            if (x_badge_lnp > x_max_badge) x_badge_lnp = x_max_badge;

            DrawRectangle((int)x_badge_lnp, (int)(y_lnp_alma - 11), (int)w_b_lnp + 14, 22, GetColor(0xDC2626FF));
            desenhar_texto_nitido(font, txt_lnp.c_str(), x_badge_lnp + 7.0f, y_lnp_alma - 8.0f, 16.0f, WHITE);
        }
    }

    // 6. Legendas Grandes, Claras e Seguras (Posicionadas com folga, sem invadir o cabeçalho)
    std::string txt_laje = "Laje: beff = " + std::to_string((int)rel.fase2.bef_mm) + " mm  |  tc = " + std::to_string((int)cfg.laje.tc_mm) + " mm";
    if (cfg.laje.tipo == TipoLajeMista::STEEL_DECK) {
        txt_laje = "Laje Mista: beff = " + std::to_string((int)rel.fase2.bef_mm) + " mm  |  Capa tc = " + std::to_string((int)cfg.laje.tc_mm) + " mm  |  Fôrma hF = " + std::to_string((int)cfg.laje.hF_deck_mm) + " mm";
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
    InitWindow(screenWidth, screenHeight, "VMP - Dimensionamento de Vigas Mistas de Aço e Concreto | NBR 8800");
    SetTargetFPS(60);

    // Configuração de tipografia nítida TrueType para a UI (Segoe UI / Calibri)
    rlImGuiSetLoadFontsCallback(carregar_fontes_imgui);
    rlImGuiSetup(true);

    bool tema_escuro = true;
    aplicar_tema_ui(tema_escuro);

    // Fontes vetoriais TrueType para a tela gráfica Raylib em resoluções nativas (16px e 18px)
    // Carregamos explicitamente todos os caracteres acentuados da língua portuguesa (ASCII + Latin-1 Supplement + Latin Extended)
    std::vector<int> codepointsRaylib;
    codepointsRaylib.reserve(512);
    for (int i = 32; i <= 255; ++i) codepointsRaylib.push_back(i); // Cobre ç, á, é, í, ó, ú, ã, õ, â, ê, ô, à, ü, etc.
    for (int i = 0x0100; i <= 0x017F; ++i) codepointsRaylib.push_back(i); // Latin Extended-A
    codepointsRaylib.push_back(0x2264); // ≤
    codepointsRaylib.push_back(0x2265); // ≥
    codepointsRaylib.push_back(0x03B7); // η

    const char* fontRaylibPath = "C:\\Windows\\Fonts\\seguisb.ttf";
    if (!FileExists(fontRaylibPath)) fontRaylibPath = "C:\\Windows\\Fonts\\segoeuib.ttf";
    if (!FileExists(fontRaylibPath)) fontRaylibPath = "C:\\Windows\\Fonts\\segoeui.ttf";

    Font fontRaylib = LoadFontEx(fontRaylibPath, 16, codepointsRaylib.data(), (int)codepointsRaylib.size());
    Font fontRaylibTitulo = LoadFontEx(fontRaylibPath, 18, codepointsRaylib.data(), (int)codepointsRaylib.size());

    // Estado da Viga Mista (Inicializada com o Caso de Referência da NBR 8800:2024)
    VigaMistaConfig cfg;
    cfg.norma = NormaEdicao::NBR8800_2024;
    cfg.perfil = perfil_w360x51();
    cfg.aco = aco_a572_gr50();
    cfg.concreto = concreto_c30();

    cfg.laje.tipo = TipoLajeMista::MACICA;
    cfg.laje.tc_mm = 120.0;
    cfg.laje.hF_deck_mm = 0.0;
    cfg.laje.bF_deck_mm = 120.0;
    cfg.laje.passo_deck_mm = 200.0;

    cfg.conector.d_cs_mm = 19.0;
    cfg.conector.h_cs_mm = 100.0;
    cfg.conector.fu_MPa = 420.0;
    cfg.conector.n_por_linha = 1;
    cfg.conector.espacamento_adotado_mm = 150.0;

    cfg.geometria.L_vao_mm = 7000.0;
    cfg.geometria.Lb_mm = 3500.0;
    cfg.geometria.dist_esq_mm = 1750.0;
    cfg.geometria.dist_dir_mm = 1750.0;
    cfg.geometria.eh_viga_borda = false;
    cfg.geometria.escorada = false;

    cfg.acoes.peso_proprio_perfil_kN_m = 0.499;
    cfg.acoes.q_laje_fresco_kN_m2 = 3.0;
    cfg.acoes.q_sobrecarga_const_kN_m2 = 1.0;
    cfg.acoes.g_revestimento_kN_m2 = 4.0;
    cfg.acoes.q_uso_kN_m2 = 3.0;
    cfg.acoes.psi1 = 0.70;
    cfg.acoes.psi2 = 0.40;

    int norma_idx = 0;       // 0 = 2024, 1 = 2008
    int perfil_idx = 0;      // 0 = W360x51, 1 = W360x57.8, 2 = Customizado
    int laje_idx = 0;        // 0 = Maciça, 1 = Steel Deck
    int orient_deck_idx = 0; // 0 = Perpendicular, 1 = Paralela
    int modo_conexao_idx = 0; // 0 = Interação Completa Automática, 1 = Interação Parcial por Grau Alvo, 2 = Espaçamento Manual
    float grau_alvo_slider = 0.65f;

    while (!WindowShouldClose()) {
        int cur_w = GetScreenWidth();
        int cur_h = GetScreenHeight();

        cfg.norma = (norma_idx == 0) ? NormaEdicao::NBR8800_2024 : NormaEdicao::NBR8800_2008;
        cfg.laje.tipo = (laje_idx == 0) ? TipoLajeMista::MACICA : TipoLajeMista::STEEL_DECK;
        cfg.laje.orientacao = (orient_deck_idx == 0) ? OrientacaoDeck::PERPENDICULAR_A_VIGA : OrientacaoDeck::PARALELA_A_VIGA;

        if (modo_conexao_idx == 0) {
            cfg.conector.espacamento_adotado_mm = 0.0; // 100% Interação completa automática
        } else if (modo_conexao_idx == 1) {
            // Calcula pinos para atingir o grau alvo de interação parcial
            double bef_est = 1750.0;
            auto conn_pre = calcularConectoresStud(cfg, bef_est);
            int n_semi_alvo = static_cast<int>(std::ceil(grau_alvo_slider * conn_pre.Fhd_interface_kN / std::max(1.0, conn_pre.QRd_adotado_kN)));
            int n_linhas = std::max(2, (2 * n_semi_alvo) / std::max(1, cfg.conector.n_por_linha));
            cfg.conector.espacamento_adotado_mm = cfg.geometria.L_vao_mm / (n_linhas - 1);
        }

        if (perfil_idx == 2) {
            calcular_propriedades_secao_i(cfg.perfil);
        }

        // Executa a verificação completa em tempo real
        RelatorioVigaMista rel = verificar_viga_mista(cfg);

        // Layout Responsivo Otimizado sem transbordamentos
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
        
        // Cabeçalho da Área Gráfica (Barra de título destacada de 38px)
        DrawRectangle((int)painel_dir_x, (int)canvas_y, (int)painel_dir_w, 38, corTituloBarra);
        desenhar_texto_nitido(fontRaylibTitulo, "SEÇÃO TRANSVERSAL MISTA (AÇO + CONCRETO)", painel_dir_x + 16.0f, canvas_y + 9.0f, 18.0f, corTituloTxt);

        // Badges informativos no canto direito da barra de título com fonte nítida
        std::string badge_norma = (norma_idx == 0) ? "NBR 8800:2024" : "NBR 8800:2008";
        float w_b_norma = medir_largura_texto(fontRaylib, badge_norma.c_str(), 16.0f);
        float x_badge = painel_dir_x + painel_dir_w - w_b_norma - 20.0f;
        DrawRectangle((int)x_badge - 6, (int)canvas_y + 8, (int)w_b_norma + 12, 22, corBadgeNormaBg);
        desenhar_texto_nitido(fontRaylib, badge_norma.c_str(), x_badge, canvas_y + 11.0f, 16.0f, corBadgeNormaTxt);

        int canvas_centro_x = (int)(painel_dir_x + painel_dir_w / 2.0f);

        // Desenho geométrico com proporções e ancoragem rigorosa
        desenhar_secao_viga_mista(cfg, rel, canvas_centro_x, canvas_y, canvas_y + canvas_h, painel_dir_w, fontRaylib, tema_escuro);

        // 2. INTERFACE DEAR IMGUI
        rlImGuiBegin();

        // Barra Superior de Menus (Mais discreta, acessível e profissional)
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
            if (ImGui::BeginMenu("Norma")) {
                if (ImGui::MenuItem("ABNT NBR 8800:2024 (Vigente)", nullptr, norma_idx == 0)) norma_idx = 0;
                if (ImGui::MenuItem("ABNT NBR 8800:2008 (Anterior)", nullptr, norma_idx == 1)) norma_idx = 1;
                ImGui::EndMenu();
            }
            float w_total = ImGui::GetWindowWidth();
            std::string status_bar = std::string(norma_idx == 0 ? "NBR 8800:2024" : "NBR 8800:2008") + "  |  " + (tema_escuro ? "Tema: Escuro" : "Tema: Claro");
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

        // Largura compacta para as caixas numéricas (145px) deixa todo o restante para os rótulos
        float input_w = 145.0f;

        if (ImGui::BeginTabBar("AbasVigaMista", ImGuiTabBarFlags_None)) {

            // ABA 1: PERFIL I DE AÇO
            if (ImGui::BeginTabItem("Perfil de Aço")) {
                ImGui::Spacing();
                const char* perfis[] = { "W 360 x 51,0 (Caso Estudo)", "W 360 x 57,8", "Perfil Customizado / Soldado" };
                ImGui::Text("Catálogo de Perfis:");
                ImGui::SetNextItemWidth(-1.0f);
                if (ImGui::Combo("##PerfilCombo", &perfil_idx, perfis, 3)) {
                    if (perfil_idx == 0) cfg.perfil = perfil_w360x51();
                    else if (perfil_idx == 1) cfg.perfil = perfil_w360x57_8();
                }

                ImGui::Spacing();
                if (perfil_idx == 2) {
                    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Edição Paramétrica:");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Altura d (mm)", &cfg.perfil.d, 5.0, 20.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Largura mesa bf (mm)", &cfg.perfil.bf, 5.0, 20.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Espessura mesa tf (mm)", &cfg.perfil.tf, 0.5, 2.0, "%.2f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Espessura alma tw (mm)", &cfg.perfil.tw, 0.5, 1.0, "%.2f");
                } else {
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Propriedades Nominais:");
                    ImGui::BulletText("d = %.1f mm  |  bf = %.1f mm", cfg.perfil.d, cfg.perfil.bf);
                    ImGui::BulletText("tf = %.2f mm  |  tw = %.2f mm", cfg.perfil.tf, cfg.perfil.tw);
                    ImGui::BulletText("Área Ag = %.2f cm²  |  Inércia Ix = %.0f cm⁴", cfg.perfil.Ag / 100.0, cfg.perfil.Ix / 1e4);
                    ImGui::BulletText("Módulo Elástico Wx = %.1f cm³  |  Plástico Zx = %.1f cm³", cfg.perfil.Wx / 1e3, cfg.perfil.Zx / 1e3);
                }

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
                    if (laje_idx == 1 && cfg.laje.hF_deck_mm <= 0.0) {
                        cfg.laje.hF_deck_mm = 50.0;
                        cfg.laje.bF_deck_mm = 120.0;
                        cfg.laje.passo_deck_mm = 200.0;
                        if (cfg.laje.tc_mm > 100.0) cfg.laje.tc_mm = 80.0; // Capa típica sobre deck de 50mm
                    }
                }

                ImGui::Spacing();
                ImGui::SetNextItemWidth(input_w);
                if (laje_idx == 0) {
                    ImGui::InputDouble("Espessura da Laje tc (mm)", &cfg.laje.tc_mm, 5.0, 10.0, "%.1f");
                } else {
                    ImGui::InputDouble("Capa de Concreto tc (mm)", &cfg.laje.tc_mm, 5.0, 10.0, "%.1f");
                }

                if (laje_idx == 1) {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.8f, 1.0f), "Fôrma Nervurada de Aço (Steel Deck):");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Altura da Fôrma hF (mm)", &cfg.laje.hF_deck_mm, 5.0, 10.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Largura da Nervura bF (mm)", &cfg.laje.bF_deck_mm, 5.0, 10.0, "%.1f");
                    ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Passo entre Nervuras eF (mm)", &cfg.laje.passo_deck_mm, 10.0, 50.0, "%.1f");

                    const char* orientacoes[] = { 
                        "Nervuras Transversais (Perpendiculares à Viga)", 
                        "Nervuras Longitudinais (Paralelas à Viga)" 
                    };
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("##OrientacaoCombo", &orient_deck_idx, orientacoes, 2);

                    ImGui::Spacing();
                    ImGui::BulletText("Altura total da laje: %.1f mm (Capa tc = %.1f mm + Fôrma hF = %.1f mm)", 
                        cfg.laje.tc_mm + cfg.laje.hF_deck_mm, cfg.laje.tc_mm, cfg.laje.hF_deck_mm);
                    
                    if (orient_deck_idx == 0) {
                        ImGui::TextDisabled("Transversais (Perpendiculares): cristas interrompem o concreto no vão;");
                        ImGui::TextDisabled("conectores confinados em cada nervura (rege apenas capa tc, NBR 8800 L.1.2).");
                    } else {
                        ImGui::TextDisabled("Longitudinais (Paralelas): calha contínua de concreto sobre a mesa (caminho livre);");
                        ImGui::TextDisabled("adota-se a capa tc como simplificação a favor da segurança.");
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Concreto da Laje:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Resistência fck (MPa)", &cfg.concreto.fck_MPa, 5.0, 10.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Módulo Secante Ecs (MPa)", &cfg.concreto.Ecs_MPa, 500.0, 1000.0, "%.0f");
                ImGui::EndTabItem();
            }

            // ABA 3: CONECTORES STUD BOLT
            if (ImGui::BeginTabItem("Stud Bolts")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Parâmetros do Stud Bolt:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Diâmetro do Pino d (mm)", &cfg.conector.d_cs_mm, 1.0, 3.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Altura do Pino h (mm)", &cfg.conector.h_cs_mm, 5.0, 10.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Resistência fu (MPa)", &cfg.conector.fu_MPa, 10.0, 50.0, "%.1f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputInt("Pinos por Linha/Nervura", &cfg.conector.n_por_linha);
                if (cfg.conector.n_por_linha < 1) cfg.conector.n_por_linha = 1;

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Seletor de Modo de Conexão: Interação Completa vs Parcial
                const char* modos_conexao[] = {
                    "Interação Completa Automática (eta = 100%)",
                    "Interação Parcial por Grau Alvo (eta desejado)",
                    "Espaçamento Manual Adotado (mm)"
                };
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Modo de Dimensionamento da Conexão:");
                ImGui::SetNextItemWidth(-1.0f);
                ImGui::Combo("##ModoConexaoCombo", &modo_conexao_idx, modos_conexao, 3);

                if (modo_conexao_idx == 1) {
                    ImGui::Spacing();
                    ImGui::Text("Grau de Interação Alvo (eta):");
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::SliderFloat("##GrauAlvoSlider", &grau_alvo_slider, (float)rel.conectores.grau_interacao_minimo, 1.00f, "%.2f");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "(Mínimo normativo: %.2f)", rel.conectores.grau_interacao_minimo);
                } else if (modo_conexao_idx == 2) {
                    ImGui::Spacing();
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::InputDouble("Espaçamento Adotado (mm)", &cfg.conector.espacamento_adotado_mm, 10.0, 50.0, "%.1f");
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Diagnóstico da Conexão & Grau de Interação:");
                
                // Tipo de interação
                if (rel.conectores.grau_interacao_eta >= 0.999) {
                    ImGui::BulletText("Tipo de Interação: INTERAÇÃO COMPLETA (eta = 100 %%)");
                } else {
                    ImGui::BulletText("Tipo de Interação: INTERAÇÃO PARCIAL (eta = %.1f %%)", rel.conectores.grau_interacao_eta * 100.0f);
                }
                ImGui::BulletText("Grau Mínimo: eta_min = %.2f %s", rel.conectores.grau_interacao_minimo, rel.conectores.grau_interacao_atende ? "[OK]" : "[NÃO ATENDE]");
                ImGui::BulletText("Capacidade Unitária QRd: %.2f kN  |  Força Rasante Fhd: %.2f kN", rel.conectores.QRd_adotado_kN, rel.conectores.Fhd_interface_kN);
                ImGui::BulletText("Total de Pinos: %d adotados (mínimo p/ 100%%: %d)", rel.conectores.n_total_adotado, rel.conectores.n_total_calculado);
                ImGui::BulletText("Espaçamento: %.0f mm (Limites: %.0f a %.0f mm) %s", rel.conectores.espacamento_adotado_mm, rel.conectores.s_min_long_mm, rel.conectores.s_max_long_mm, rel.conectores.espacamento_atende ? "[OK]" : "[FORA DOS LIMITES]");

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Verificações Geométricas do Pino (NBR 8800 L.8.1/L.6.6):");
                ImGui::BulletText("Altura do Pino: h = %.0f mm (Mínimo exigido: %.0f mm) %s", cfg.conector.h_cs_mm, rel.conectores.h_min_normativo_mm, rel.conectores.altura_atende ? "[OK]" : "[INSUFICIENTE]");
                ImGui::BulletText("Cobrimento Superior: c = %.1f mm (Mínimo da norma: %.1f mm) %s", rel.conectores.cobrimento_topo_mm, rel.conectores.cobrimento_min_mm, rel.conectores.cobrimento_atende ? "[OK]" : "[INSUFICIENTE]");
                ImGui::BulletText("Diâmetro do Pino: d = %.1f mm (Máximo p/ mesa tf: %.1f mm) %s", cfg.conector.d_cs_mm, rel.conectores.d_max_mesa_mm, rel.conectores.diametro_mesa_atende ? "[OK]" : "[EXCESSIVO]");
                ImGui::EndTabItem();
            }

            // ABA 4: VÃO E CARGAS
            if (ImGui::BeginTabItem("Vão & Cargas")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "Geometria do Piso:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Vão da Viga L (mm)", &cfg.geometria.L_vao_mm, 500.0, 1000.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Comprimento Destravado Lb (mm)", &cfg.geometria.Lb_mm, 250.0, 500.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Distância viga esquerda (mm)", &cfg.geometria.dist_esq_mm, 100.0, 500.0, "%.0f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Distância viga direita (mm)", &cfg.geometria.dist_dir_mm, 100.0, 500.0, "%.0f");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Ações Superficiais:");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Laje Fresca (kN/m²)", &cfg.acoes.q_laje_fresco_kN_m2, 0.5, 1.0, "%.2f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Construção q1 (kN/m²)", &cfg.acoes.q_sobrecarga_const_kN_m2, 0.5, 1.0, "%.2f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Revestimento/Paredes g2 (kN/m²)", &cfg.acoes.g_revestimento_kN_m2, 0.5, 1.0, "%.2f");
                ImGui::SetNextItemWidth(input_w); ImGui::InputDouble("Sobrecarga de Uso q2 (kN/m²)", &cfg.acoes.q_uso_kN_m2, 0.5, 1.0, "%.2f");
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
        ImGui::Begin("Diagnóstico Consolidado ELU & ELS", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // KPI CARDS TOPO (Tabela de 4 colunas proporcionais)
        if (ImGui::BeginTable("KpiTable", 4, ImGuiTableFlags_None)) {
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Momento", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Conexao", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Flecha", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            
            // Col 1: Status Geral
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

            // Col 2: Flexão Fase 2
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Momento Misto:");
            ImGui::TextColored(corMomento, "%.1f / %.1f kNm", rel.fase2.MSd2_kNm, rel.fase2.MRd_VM_kNm);

            // Col 3: Conexão Studs
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Conexão Studs:");
            ImVec4 corConn = (rel.conectores.grau_interacao_atende) ? corAprovado : corReprovado;
            ImGui::TextColored(corConn, "eta = %.0f %% (%d pinos)", rel.conectores.grau_interacao_eta * 100.0f, rel.conectores.n_total_adotado);

            // Col 4: Flecha Total
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Flecha Serviço:");
            ImVec4 corFlecha = (rel.flechas.aprovado_flecha) ? corAprovado : corReprovado;
            double razao_L = (rel.flechas.flecha_total_mm > 0.05) ? (cfg.geometria.L_vao_mm / rel.flechas.flecha_total_mm) : 9999.0;
            ImGui::TextColored(corFlecha, "%.1f mm (L/%.0f)", rel.flechas.flecha_total_mm, razao_L);

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // TABELA DE SÍNTESE COM LARGURAS OTIMIZADAS PARA NÃO TRUNCAR TEXTO
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
                if (item.aprovado) {
                    ImGui::TextColored(corStatus, "OK");
                } else {
                    ImGui::TextColored(corStatus, "FALHA");
                }
            }
            ImGui::EndTable();
        }

        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    if (fontRaylib.texture.id > 0) {
        UnloadFont(fontRaylib);
    }
    if (fontRaylibTitulo.texture.id > 0) {
        UnloadFont(fontRaylibTitulo);
    }
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
