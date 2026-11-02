#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "modelos.hpp"
#include "catalogo.hpp"
#include "geometria.hpp"
#include "verificador.hpp"

#include <string>
#include <vector>
#include <algorithm>

// Helper para Slider com double no Dear ImGui
inline bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.1f") {
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format);
}

// ============================================================================
// DESENHO GRÁFICO 2D DA SEÇÃO TRANSVERSAL COM RAYLIB
// ============================================================================
void desenhar_secao_transversal(
    const VigaVMP& viga,
    const RelatorioVMP& rel,
    int centro_x,
    int centro_y,
    float escala
) {
    // 1. Coordenadas de referência
    // y_base é a face inferior do perfil de aço
    float y_base = (float)centro_y + 150.0f;

    float d = (float)viga.perfil.d * escala;
    float bf = (float)viga.perfil.bf * escala;
    float tf = (float)viga.perfil.tf * escala;
    float tw = (float)viga.perfil.tw * escala;
    float tc = (float)viga.laje.tc * escala;

    // Largura visual da laje (representativa)
    float beff_vis = 400.0f * escala;

    // 2. Desenhar Laje de Concreto
    float y_topo_aco = y_base - d;
    float y_topo_laje = y_topo_aco - tc;

    DrawRectangle((int)(centro_x - beff_vis / 2), (int)y_topo_laje, (int)beff_vis, (int)tc, Fade(LIGHTGRAY, 0.8f));
    DrawRectangleLines((int)(centro_x - beff_vis / 2), (int)y_topo_laje, (int)beff_vis, (int)tc, DARKGRAY);

    // 3. Desenhar Perfil I de Aço
    // Mesa Inferior
    DrawRectangle((int)(centro_x - bf / 2), (int)(y_base - tf), (int)bf, (int)tf, DARKGRAY);
    // Alma
    DrawRectangle((int)(centro_x - tw / 2), (int)(y_base - d + tf), (int)tw, (int)(d - 2 * tf), DARKGRAY);
    // Mesa Superior
    DrawRectangle((int)(centro_x - bf / 2), (int)y_topo_aco, (int)bf, (int)tf, DARKGRAY);

    // Contorno do perfil
    DrawRectangleLines((int)(centro_x - bf / 2), (int)(y_base - tf), (int)bf, (int)tf, BLACK);
    DrawRectangleLines((int)(centro_x - tw / 2), (int)(y_base - d + tf), (int)tw, (int)(d - 2 * tf), BLACK);
    DrawRectangleLines((int)(centro_x - bf / 2), (int)y_topo_aco, (int)bf, (int)tf, BLACK);

    // 4. Desenhar Linha Neutra Elástica (LNE t=0)
    float y_lne = y_base - (float)rel.secao_t0.ytr_mm * escala;
    DrawLine((int)(centro_x - beff_vis / 2 - 20), (int)y_lne, (int)(centro_x + beff_vis / 2 + 20), (int)y_lne, BLUE);
    DrawText("LNE (t=0)", (int)(centro_x + beff_vis / 2 + 25), (int)(y_lne - 8), 12, BLUE);

    // 5. Desenhar Cabo de Protensão Externa
    // ep é negativo para baixo da face inferior
    float y_cabo = y_base - (float)viga.cabo.ep * escala;
    DrawCircle((int)centro_x, (int)y_cabo, 6.0f, RED);
    DrawCircleLines((int)centro_x, (int)y_cabo, 6.0f, MAROON);
    DrawText("Cabo", (int)(centro_x + 12), (int)(y_cabo - 6), 12, RED);

    // 6. Linha do solo / Cota da base
    DrawLine((int)(centro_x - 150), (int)y_base, (int)(centro_x + 150), (int)y_base, Fade(BLACK, 0.3f));
}

// ============================================================================
// PONTO DE ENTRADA DA INTERFACE GRÁFICA (RAYLIB + DEAR IMGUI)
// ============================================================================
int main() {
    // Configuração inicial da janela Raylib
    const int screenWidth = 1360;
    const int screenHeight = 768;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "VMP - Vigas Mistas Protendidas (Raylib + Dear ImGui)");
    SetTargetFPS(60);

    // Inicializa integração rlImGui
    rlImGuiSetup(true);

    // Estado da Viga (inicializado com Exemplo 1 de Turini 2021)
    VigaVMP viga;
    viga.perfil = perfil_w360x57_8();
    viga.aco = aco_a572_gr50();
    viga.concreto = concreto_c30();
    viga.laje.tc = 130.0;
    viga.laje.largura_efetiva = 3000.0;
    viga.cabo.ep = -50.0;
    viga.cabo.Ap = 420.0;
    viga.cabo.n_cordoalhas = 3;
    viga.cabo.delta_cunha = 6.0;
    viga.P0_adotado = 400.0;
    viga.L = 12000.0;
    viga.Lb = 3000.0;

    int perfil_selecionado = 0; // 0 = W360x57.8, 1 = Customizado

    // Loop principal da aplicação gráfica
    while (!WindowShouldClose()) {
        // Atualiza propriedades geométricas automaticamente se customizado
        if (perfil_selecionado == 1) {
            calcular_propriedades_secao_i(viga.perfil);
        }

        // CÁLCULO ESTRUTURAL INSTANTÂNEO EM TEMPO REAL (< 0.05 ms)
        RelatorioVMP rel = verificar_viga_vmp(viga);

        // Início da renderização
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 1. DESENHO GRÁFICO RAYLIB (Lado direito)
        int canvas_x = 920;
        int canvas_y = 300;
        DrawText("SEÇÃO TRANSVERSAL INTERATIVA", canvas_x - 120, 40, 16, DARKGRAY);
        desenhar_secao_transversal(viga, rel, canvas_x, canvas_y, 0.65f);

        // 2. INTERFACE DEAR IMGUI (Lado esquerdo)
        rlImGuiBegin();

        // Janela de Parâmetros de Entrada
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(520, 748), ImGuiCond_Once);
        ImGui::Begin("Parâmetros Estruturais - VMP", nullptr, ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("AbasDados")) {
            // ABA 1: PERFIL DE AÇO
            if (ImGui::BeginTabItem("Perfil de Aço")) {
                const char* opcoes_perfis[] = { "W 360 x 57.8 (Catálogo Gerdau)", "Perfil Customizado" };
                if (ImGui::Combo("Perfil Selecionado", &perfil_selecionado, opcoes_perfis, 2)) {
                    if (perfil_selecionado == 0) {
                        viga.perfil = perfil_w360x57_8();
                    }
                }

                ImGui::Separator();
                if (perfil_selecionado == 1) {
                    ImGui::InputDouble("Altura d (mm)", &viga.perfil.d, 1.0, 10.0, "%.1f");
                    ImGui::InputDouble("Largura da Mesa bf (mm)", &viga.perfil.bf, 1.0, 10.0, "%.1f");
                    ImGui::InputDouble("Espessura da Mesa tf (mm)", &viga.perfil.tf, 0.5, 2.0, "%.2f");
                    ImGui::InputDouble("Espessura da Alma tw (mm)", &viga.perfil.tw, 0.5, 2.0, "%.2f");
                    ImGui::InputDouble("Raio de adoçamento r (mm)", &viga.perfil.r, 0.5, 2.0, "%.1f");
                } else {
                    ImGui::Text("Dimensões do Perfil W 360 x 57.8:");
                    ImGui::BulletText("d = %.1f mm | bf = %.1f mm", viga.perfil.d, viga.perfil.bf);
                    ImGui::BulletText("tf = %.1f mm | tw = %.1f mm | r = %.1f mm", viga.perfil.tf, viga.perfil.tw, viga.perfil.r);
                    ImGui::BulletText("Ag = %.1f cm² | Ix = %.0f cm⁴", viga.perfil.Ag / 100.0, viga.perfil.Ix / 10000.0);
                }

                ImGui::Separator();
                ImGui::Text("Material:");
                ImGui::InputDouble("fy (MPa)", &viga.aco.fy, 5.0, 20.0, "%.1f");
                ImGui::InputDouble("E (MPa)", &viga.aco.E, 1000.0, 10000.0, "%.0f");

                ImGui::Separator();
                ImGui::InputDouble("Vão Total Lv (mm)", &viga.L, 500.0, 1000.0, "%.0f");
                ImGui::InputDouble("Comprimento Destravado Lb (mm)", &viga.Lb, 250.0, 500.0, "%.0f");

                ImGui::EndTabItem();
            }

            // ABA 2: LAJE E CONCRETO
            if (ImGui::BeginTabItem("Laje e Concreto")) {
                SliderDouble("Espessura da Laje tc (mm)", &viga.laje.tc, 80.0, 250.0, "%.0f mm");
                ImGui::InputDouble("Largura Colaborante beff (mm)", &viga.laje.largura_efetiva, 100.0, 500.0, "%.0f");
                ImGui::InputDouble("fck do Concreto (MPa)", &viga.concreto.fck, 5.0, 10.0, "%.1f");
                ImGui::InputDouble("Módulo Secante Ecs (MPa)", &viga.concreto.Ecs, 500.0, 2000.0, "%.0f");
                ImGui::EndTabItem();
            }

            // ABA 3: PROTENSÃO
            if (ImGui::BeginTabItem("Protensão")) {
                SliderDouble("Excentricidade ep (mm)", &viga.cabo.ep, -150.0, 0.0, "%.1f mm");
                ImGui::InputDouble("Força Inicial P0 (kN)", &viga.P0_adotado, 25.0, 50.0, "%.1f");
                
                int n_cord = viga.cabo.n_cordoalhas;
                if (ImGui::SliderInt("Nº de Cordoalhas", &n_cord, 1, 10)) {
                    viga.cabo.n_cordoalhas = n_cord;
                    viga.cabo.Ap = n_cord * 140.0; // 140 mm² cada (15.2 mm)
                }
                ImGui::Text("Área total Ap: %.1f mm²", viga.cabo.Ap);
                ImGui::InputDouble("Acomodação Cunha (mm)", &viga.cabo.delta_cunha, 1.0, 2.0, "%.1f");
                ImGui::EndTabItem();
            }

            // ABA 4: CARGAS
            if (ImGui::BeginTabItem("Carregamentos")) {
                ImGui::InputDouble("q1 - Peso Perfil (kN/m)", &viga.cargas.q1_perfil, 0.1, 0.5, "%.2f");
                ImGui::InputDouble("q2 - Concreto Fresco (kN/m)", &viga.cargas.q2_concreto, 0.5, 1.0, "%.2f");
                ImGui::InputDouble("qs - Montagem (kN/m)", &viga.cargas.qs_construcao, 0.5, 1.0, "%.2f");
                ImGui::InputDouble("q3 - Revestimento (kN/m)", &viga.cargas.q3_revestimento, 0.5, 1.0, "%.2f");
                ImGui::InputDouble("q4 - Sobrecarga Uso (kN/m)", &viga.cargas.q4_uso, 1.0, 2.0, "%.2f");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Separator();
        // STATUS GERAL DA ESTRUTURA
        if (rel.aprovado_global) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
            ImGui::Text("STATUS GERAL: VIGA APROVADA EM TODAS AS FASES");
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::Text("STATUS GERAL: REPROVADA (Verifique os itens abaixo)");
            ImGui::PopStyleColor();
        }

        ImGui::End();

        // Janela de Resultados / Memória de Cálculo (Inferior Direito)
        ImGui::SetNextWindowPos(ImVec2(540, 500), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(810, 258), ImGuiCond_Once);
        ImGui::Begin("Memória de Cálculo e Verificações Normativas", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::Columns(4, "ColsResultados", true);
        ImGui::Text("FASE 1 (Construção)");
        ImGui::NextColumn();
        ImGui::Text("FASE 2 (VMP t=0)");
        ImGui::NextColumn();
        ImGui::Text("SEÇÃO TRANSFORMADA");
        ImGui::NextColumn();
        ImGui::Text("CONECTORES");
        ImGui::NextColumn();
        ImGui::Separator();

        // Coluna 1: Fase 1
        ImGui::Text("P1: %.2f kN", rel.P1_fase1_kN);
        ImGui::Text("MSd+: %.2f kN.m", rel.MSd_fase1_kNm);
        ImGui::Text("MRd: %.2f kN.m", rel.flexao_aco.MRd_kNm);
        float taxa_fase1 = (float)(rel.MSd_fase1_kNm / rel.flexao_aco.MRd_kNm);
        ImGui::ProgressBar(taxa_fase1, ImVec2(-1, 0), (std::to_string((int)(taxa_fase1 * 100)) + "%").c_str());
        ImGui::NextColumn();

        // Coluna 2: Fase 2
        ImGui::Text("P2: %.2f kN", rel.P2_fase2_kN);
        ImGui::Text("MSd+: %.2f kN.m", rel.MSd_fase2_kNm);
        ImGui::Text("MRd,VMP: %.2f kN.m", rel.momento_vmp.MRd_VMP_kNm);
        float taxa_fase2 = (float)(rel.MSd_fase2_kNm / rel.momento_vmp.MRd_VMP_kNm);
        ImGui::ProgressBar(taxa_fase2, ImVec2(-1, 0), (std::to_string((int)(taxa_fase2 * 100)) + "%").c_str());
        ImGui::NextColumn();

        // Coluna 3: Seção Transf
        ImGui::Text("ytr (t=0): %.2f cm", rel.secao_t0.ytr_mm / 10.0);
        ImGui::Text("Itr (t=0): %.0f cm⁴", rel.secao_t0.Itr_mm4 / 10000.0);
        ImGui::Text("Wtr,inf: %.0f cm³", rel.secao_t0.Wtr_inf_mm3 / 1000.0);
        ImGui::NextColumn();

        // Coluna 4: Conectores
        ImGui::Text("QRd: %.2f kN", rel.conectores.QRd_kN);
        ImGui::Text("Total: %d un", rel.conectores.nl_conectores);
        ImGui::Text("Espaçam.: %.1f mm", rel.conectores.el_espacamento_mm);
        ImGui::NextColumn();

        ImGui::Columns(1);
        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    // Finalização limpa
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
