#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "modelos.h"
#include "catalogo.h"
#include "geometria.h"
#include "rotinas_nbr8800.h"
#include "verificador.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>

// Helper para Slider com double no Dear ImGui
inline bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.1f") {
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format);
}

// ============================================================================
// DESENHO GRÁFICO 2D DA SEÇÃO TRANSVERSAL DO PERFIL I COM RAYLIB
// ============================================================================
void desenhar_secao_perfil_i(
    const PerfilI& perfil,
    int centro_x,
    int centro_y,
    float escala
) {
    float d = (float)perfil.d * escala;
    float bf = (float)perfil.bf * escala;
    float tf = (float)perfil.tf * escala;
    float tw = (float)perfil.tw * escala;

    if (d <= 0.0f || bf <= 0.0f) return;

    // Coordenadas das peças da seção I
    float x_esq_mesa = (float)centro_x - bf / 2.0f;
    float y_topo     = (float)centro_y - d / 2.0f;
    float y_base     = (float)centro_y + d / 2.0f;

    // 1. Eixos Baricêntricos (Linhas guia)
    DrawLine(centro_x - (int)(bf / 2.0f + 60.0f), centro_y, centro_x + (int)(bf / 2.0f + 60.0f), centro_y, Fade(RED, 0.4f));
    DrawText("X", centro_x + (int)(bf / 2.0f + 65.0f), centro_y - 6, 12, RED);

    DrawLine(centro_x, (int)(y_topo - 50.0f), centro_x, (int)(y_base + 50.0f), Fade(DARKGREEN, 0.4f));
    DrawText("Y", centro_x - 4, (int)(y_topo - 65.0f), 12, DARKGREEN);

    // 2. Preenchimento do Perfil I
    Color corAco = Fade(DARKGRAY, 0.9f);
    Color corBorda = BLACK;

    // Mesa Superior
    DrawRectangle((int)x_esq_mesa, (int)y_topo, (int)bf, (int)tf, corAco);
    // Alma
    float h_alma = d - 2.0f * tf;
    DrawRectangle((int)((float)centro_x - tw / 2.0f), (int)(y_topo + tf), (int)tw, (int)h_alma, corAco);
    // Mesa Inferior
    DrawRectangle((int)x_esq_mesa, (int)(y_base - tf), (int)bf, (int)tf, corAco);

    // 3. Contorno das faces
    DrawRectangleLines((int)x_esq_mesa, (int)y_topo, (int)bf, (int)tf, corBorda);
    DrawRectangleLines((int)((float)centro_x - tw / 2.0f), (int)(y_topo + tf), (int)tw, (int)h_alma, corBorda);
    DrawRectangleLines((int)x_esq_mesa, (int)(y_base - tf), (int)bf, (int)tf, corBorda);

    // 4. Centro de Gravidade (C.G.)
    DrawCircle(centro_x, centro_y, 4.0f, RED);
    DrawCircleLines(centro_x, centro_y, 4.0f, MAROON);
    DrawText("C.G.", centro_x + 8, centro_y - 8, 11, DARKGRAY);

    // 5. Linhas de Cota Geométricas
    // Cota Altura Total d (à esquerda)
    int cota_d_x = (int)(x_esq_mesa - 35.0f);
    DrawLine(cota_d_x, (int)y_topo, cota_d_x, (int)y_base, DARKBLUE);
    DrawLine(cota_d_x - 5, (int)y_topo, cota_d_x + 5, (int)y_topo, DARKBLUE);
    DrawLine(cota_d_x - 5, (int)y_base, cota_d_x + 5, (int)y_base, DARKBLUE);

    std::string txt_d = "d = " + std::to_string((int)perfil.d) + " mm";
    DrawText(txt_d.c_str(), cota_d_x - 85, centro_y - 6, 12, DARKBLUE);

    // Cota Largura Mesa bf (acima)
    int cota_bf_y = (int)(y_topo - 25.0f);
    DrawLine((int)x_esq_mesa, cota_bf_y, (int)(x_esq_mesa + bf), cota_bf_y, DARKBLUE);
    DrawLine((int)x_esq_mesa, cota_bf_y - 5, (int)x_esq_mesa, cota_bf_y + 5, DARKBLUE);
    DrawLine((int)(x_esq_mesa + bf), cota_bf_y - 5, (int)(x_esq_mesa + bf), cota_bf_y + 5, DARKBLUE);

    std::string txt_bf = "bf = " + std::to_string((int)perfil.bf) + " mm";
    DrawText(txt_bf.c_str(), centro_x - 35, cota_bf_y - 18, 12, DARKBLUE);
}

// ============================================================================
// PONTO DE ENTRADA PRINCIPAL DA INTERFACE GRÁFICA
// ============================================================================
int main() {
    const int screenWidth = 1400;
    const int screenHeight = 820;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "VMP - Vigas Mistas Protendidas | Fase Atual: Geometria & Rotina A");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    // Estado da estrutura (Perfil W360x57.8 + A572 Gr. 50 por padrão)
    PerfilI perfil = perfil_w360x57_8();
    Aco aco = aco_a572_gr50();

    // Parâmetros de Instabilidade / Compressão (Rotina A)
    double Lv_mm = 12000.0;    // Vão da viga (mm)
    double Lb_mm = 3000.0;     // Comprimento destravado (mm)
    double kx = 1.0;           // Coeficiente flambagem x-x
    double ky = 1.0;           // Coeficiente flambagem y-y
    double gamma_a1 = 1.10;    // Coeficiente ponderação da resistência do aço

    int perfil_selecionado = 0; // 0 = W360x57.8, 1 = Customizado
    int material_selecionado = 0; // 0 = A572 Gr.50, 1 = A36, 2 = Customizado
    int tipo_fabricacao_idx = (perfil.tipo == LAMINADO) ? 0 : 1;

    while (!WindowShouldClose()) {
        // Se perfil for customizado, recalcula propriedades em tempo real
        if (perfil_selecionado == 1) {
            perfil.tipo = (tipo_fabricacao_idx == 0) ? LAMINADO : SOLDADO;
            calcular_propriedades_secao_i(perfil);
        }

        // Executa a Rotina A (NBR 8800:2008) instantaneamente
        nbr8800::ResultadoNormalResistentePerfil rotA = nbr8800::calcularNormalResistentePerfil(
            perfil, aco, Lb_mm, Lv_mm, kx, ky, gamma_a1
        );

        // Renderização
        BeginDrawing();
        ClearBackground(GetColor(0xF4F6F9FF));

        // 1. ÁREA GRÁFICA DE DESENHO (RAYLIB)
        int canvas_x = 1050;
        int canvas_y = 280;

        DrawRectangle(720, 20, 660, 500, WHITE);
        DrawRectangleLines(720, 20, 660, 500, GetColor(0xDFE3E8FF));
        DrawText("SEÇÃO TRANSVERSAL DO PERFIL DE AÇO", 740, 35, 16, DARKGRAY);

        // Escala visual adaptativa com base na altura d do perfil
        float escala = (perfil.d > 0.0) ? (320.0f / (float)perfil.d) : 0.8f;
        if (escala > 1.2f) escala = 1.2f;

        desenhar_secao_perfil_i(perfil, canvas_x, canvas_y, escala);

        // 2. INTERFACE DEAR IMGUI
        rlImGuiBegin();

        // --------------------------------------------------------------------
        // PAINEL ESQUERDO: ENTRADA DE DADOS & PARÂMETROS
        // --------------------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(680, 780), ImGuiCond_Once);
        ImGui::Begin("Entrada de Dados - Parâmetros do Perfil & Carregamento", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Módulo de Cálculo: NBR 8800:2008 (Edição Vigente)");
        ImGui::Separator();

        if (ImGui::BeginTabBar("AbasEntrada")) {
            
            // ABA 1: GEOMETRIA DO PERFIL I
            if (ImGui::BeginTabItem("Perfil I de Aço")) {
                const char* opcoes_perfis[] = { "W 360 x 57.8 (Catálogo Gerdau)", "Perfil Customizado / Soldado" };
                if (ImGui::Combo("Seleção do Perfil", &perfil_selecionado, opcoes_perfis, 2)) {
                    if (perfil_selecionado == 0) {
                        perfil = perfil_w360x57_8();
                        tipo_fabricacao_idx = 0;
                    }
                }

                const char* tipos_fab[] = { "Laminado", "Soldado" };
                if (ImGui::Combo("Tipo de Fabricação", &tipo_fabricacao_idx, tipos_fab, 2)) {
                    perfil.tipo = (tipo_fabricacao_idx == 0) ? LAMINADO : SOLDADO;
                    if (perfil_selecionado == 1) calcular_propriedades_secao_i(perfil);
                }

                ImGui::Separator();

                if (perfil_selecionado == 1) {
                    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Edição das Dimensões Nominais:");
                    ImGui::InputDouble("Altura total d (mm)", &perfil.d, 5.0, 20.0, "%.1f");
                    ImGui::InputDouble("Largura da mesa bf (mm)", &perfil.bf, 5.0, 20.0, "%.1f");
                    ImGui::InputDouble("Espessura da mesa tf (mm)", &perfil.tf, 0.5, 2.0, "%.2f");
                    ImGui::InputDouble("Espessura da alma tw (mm)", &perfil.tw, 0.5, 1.0, "%.2f");
                    if (perfil.tipo == LAMINADO) {
                        ImGui::InputDouble("Raio de adoçamento r (mm)", &perfil.r, 0.5, 1.0, "%.1f");
                    }
                } else {
                    ImGui::TextColored(ImVec4(0.1f, 0.7f, 0.3f, 1.0f), "Dimensões Padronizadas (Série W):");
                    ImGui::BulletText("d = %.1f mm  |  bf = %.1f mm", perfil.d, perfil.bf);
                    ImGui::BulletText("tf = %.2f mm  |  tw = %.2f mm", perfil.tf, perfil.tw);
                    ImGui::BulletText("d1 (livre) = %.1f mm  |  h = %.1f mm", perfil.d1, perfil.h);
                }

                ImGui::Separator();
                ImGui::Text("Esbeltezes Locais:");
                ImGui::BulletText("Mesa: lambda_f = bf / (2*tf) = %.2f", perfil.lbf);
                ImGui::BulletText("Alma: lambda_w = d1 / tw = %.2f", perfil.lbw);

                ImGui::EndTabItem();
            }

            // ABA 2: PROPRIEDADES DO AÇO
            if (ImGui::BeginTabItem("Aço Estrutural")) {
                const char* opcoes_materiais[] = { "ASTM A572 Grau 50 (fy = 345 MPa)", "ASTM A36 (fy = 250 MPa)", "Personalizado" };
                if (ImGui::Combo("Material do Aço", &material_selecionado, opcoes_materiais, 3)) {
                    if (material_selecionado == 0) {
                        aco = aco_a572_gr50();
                    } else if (material_selecionado == 1) {
                        aco.nome = "ASTM A36";
                        aco.fy = 250.0;
                        aco.fu = 400.0;
                        aco.E  = 200000.0;
                    }
                }

                ImGui::Separator();
                if (material_selecionado == 2) {
                    ImGui::InputDouble("Escoamento fy (MPa)", &aco.fy, 10.0, 50.0, "%.1f");
                    ImGui::InputDouble("Ruptura fu (MPa)", &aco.fu, 10.0, 50.0, "%.1f");
                    ImGui::InputDouble("Módulo Elasticidade E (MPa)", &aco.E, 5000.0, 10000.0, "%.0f");
                } else {
                    ImGui::BulletText("Tensão de escoamento (fy) : %.1f MPa", aco.fy);
                    ImGui::BulletText("Tensão de ruptura (fu)    : %.1f MPa", aco.fu);
                    ImGui::BulletText("Módulo longitudinal (E)   : %.0f MPa", aco.E);
                }

                ImGui::EndTabItem();
            }

            // ABA 3: COMPRESSÃO AXIAL E CONDIÇÕES DE APOIO (ROTINA A)
            if (ImGui::BeginTabItem("Condições de Flambagem")) {
                ImGui::TextColored(ImVec4(0.2f, 0.7f, 0.9f, 1.0f), "Comprimentos e Fatores de Flambagem:");
                ImGui::InputDouble("Vão Total Lv (Flambagem x-x) [mm]", &Lv_mm, 500.0, 1000.0, "%.0f");
                ImGui::InputDouble("Comprimento Lb (Flambagem y-y) [mm]", &Lb_mm, 250.0, 500.0, "%.0f");

                ImGui::Separator();
                ImGui::InputDouble("Coeficiente Kx (Eixo Forte X)", &kx, 0.1, 0.5, "%.2f");
                ImGui::InputDouble("Coeficiente Ky (Eixo Fraco Y)", &ky, 0.1, 0.5, "%.2f");
                ImGui::InputDouble("Gama a1 (Coef. de ponderação)", &gamma_a1, 0.05, 0.10, "%.2f");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Separator();

        // --------------------------------------------------------------------
        // PAINEL DE RESULTADOS DA ROTINA A & GEOMETRIA
        // --------------------------------------------------------------------
        ImGui::TextColored(ImVec4(0.1f, 0.8f, 0.3f, 1.0f), "MEMÓRIA DE CÁLCULO - RESULTADOS DA ROTINA A");

        ImGui::Columns(2, "ColunasResultados", true);

        // COLUNA 1: PROPRIEDADES DA SEÇÃO TRANSVERSAL
        ImGui::Text("1. Propriedades Geométricas");
        ImGui::Separator();
        ImGui::BulletText("Área bruta Ag: %.2f cm²", perfil.Ag / 100.0);
        ImGui::BulletText("Inércia Ix: %.0f cm⁴", perfil.Ix / 10000.0);
        ImGui::BulletText("Inércia Iy: %.0f cm⁴", perfil.Iy / 10000.0);
        ImGui::BulletText("Módulo Wx: %.1f cm³", perfil.Wx / 1000.0);
        ImGui::BulletText("Módulo Wy: %.1f cm³", perfil.Wy / 1000.0);
        ImGui::BulletText("Módulo Plástico Zx: %.1f cm³", perfil.Zx / 1000.0);
        ImGui::BulletText("Módulo Plástico Zy: %.1f cm³", perfil.Zy / 1000.0);
        ImGui::BulletText("Raio rx: %.2f cm", perfil.rx / 10.0);
        ImGui::BulletText("Raio ry: %.2f cm", perfil.ry / 10.0);
        ImGui::BulletText("Torção It: %.2f cm⁴", perfil.It / 10000.0);
        ImGui::BulletText("Empenamento Cw: %.1f cm⁶", perfil.Cw / 1e6);

        ImGui::NextColumn();

        // COLUNA 2: COMPRESSÃO AXIAL (NBR 8800:2008)
        ImGui::Text("2. Flambagem e Resistência NRd");
        ImGui::Separator();
        ImGui::BulletText("Euler Nex (Eixo X): %.2f kN", rotA.Nex_kN);
        ImGui::BulletText("Euler Ney (Eixo Y): %.2f kN", rotA.Ney_kN);

        bool governa_y = (rotA.Ney_kN <= rotA.Nex_kN);
        ImGui::BulletText("Carga crítica Ne : %.2f kN (%s)", rotA.Ne_kN, governa_y ? "Governa Eixo Y" : "Governa Eixo X");

        ImGui::Separator();
        ImGui::BulletText("Fator da mesa Qs: %.3f", rotA.Qs);
        ImGui::BulletText("Fator da alma Qa: %.3f", rotA.Qa);
        ImGui::BulletText("Fator total Q    : %.3f (%s)", rotA.Q, (rotA.Q >= 0.999) ? "Totalmente Efetiva" : "Flambagem Local");

        ImGui::Separator();
        ImGui::BulletText("Esbeltez reduzida lb0 : %.3f", rotA.lb0);
        ImGui::BulletText("Fator de redução chi  : %.3f", rotA.chi);

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.4f, 1.0f));
        ImGui::Text("FORÇA RESISTENTE NRd: %.2f kN (%.2f tf)", rotA.NRd_kN, rotA.NRd_kN / 9.80665);
        ImGui::PopStyleColor();

        ImGui::Columns(1);
        ImGui::End();

        // --------------------------------------------------------------------
        // PAINEL INFERIOR DIREITO: INFORMATIVO DO TCC E STATUS
        // --------------------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(720, 540), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(660, 260), ImGuiCond_Once);
        ImGui::Begin("Status de Implantação do Motor VMP", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "Fase de Desenvolvimento: Motor Básico (Geometria + Rotina A)");
        ImGui::Separator();
        ImGui::TextWrapped(
            "Este ambiente interativo demonstra o cálculo instantâneo da geometria da seção transversal do perfil I "
            "e a verificação completa à compressão axial pela ABNT NBR 8800:2008 (Rotina A).\n\n"
            "As próximas fases incorporarão:\n"
            "• Rotina B: Momento Fletor Resistente do Aço (FLT, FLM, FLA)\n"
            "• Rotina D: Força Cortante Resistente (VRd)\n"
            "• Rotina E: Conectores de Cisalhamento (Perfil U e Stud Bolt)\n"
            "• Rotina F: Armadura de Costura contra Fendilhamento\n"
            "• Rotinas VMP: Seção Transformada, Protensão e Momento Resistente da Viga Mista."
        );

        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
