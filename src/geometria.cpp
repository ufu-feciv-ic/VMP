#include "geometria.h"
#include <cmath>
#include <numbers>

void calcular_propriedades_secao_i(PerfilI& p) {
    const double pi = std::numbers::pi;

    // 1. Altura entre faces internas das mesas (h) e altura livre da alma (d1)
    p.h = p.d - 2.0 * p.tf;

    if (p.tipo == LAMINADO) {
        if (p.r <= 0.0 && p.d1 > 0.0 && p.d1 < p.h) {
            p.r = (p.h - p.d1) / 2.0;
        }
        if (p.r > 0.0) {
            p.d1 = p.d - 2.0 * (p.tf + p.r);
        } else {
            p.d1 = p.h;
        }
    } else {
        p.d1 = p.h;
    }

    // 2. Contribuição geométrica dos 4 raios de concordância (fillets) em perfis laminados
    double r = (p.tipo == LAMINADO && p.r > 0.0) ? p.r : 0.0;
    double area_cantos = (r > 0.0) ? (4.0 - pi) * r * r : 0.0;
    // Distância do centroide de 1 canto à face interna da mesa / face da alma:
    // d_canto = ((10 - 3*pi) / (3*(4 - pi))) * r ~= 0.223402 * r
    double d_canto = (r > 0.0) ? ((10.0 - 3.0 * pi) / (3.0 * (4.0 - pi))) * r : 0.0;
    double y_canto = (p.h / 2.0) - d_canto;
    double x_canto = (p.tw / 2.0) + d_canto;
    double I_canto_proprio = (r > 0.0)
        ? (1.0 / 3.0 - pi / 16.0 - std::pow(10.0 - 3.0 * pi, 2.0) / (9.0 * (4.0 - pi))) * std::pow(r, 4.0)
        : 0.0;

    // 3. Área bruta (Ag)
    if (p.Ag <= 0.0) {
        p.Ag = 2.0 * p.bf * p.tf + p.h * p.tw + area_cantos;
    }

    // 4. Momento de inércia x-x (Ix)
    if (p.Ix <= 0.0) {
        double Ix_mesas = 2.0 * ((p.bf * std::pow(p.tf, 3.0)) / 12.0 + (p.bf * p.tf) * std::pow((p.d - p.tf) / 2.0, 2.0));
        double Ix_alma = (p.tw * std::pow(p.h, 3.0)) / 12.0;
        double Ix_cantos = 4.0 * I_canto_proprio + area_cantos * std::pow(y_canto, 2.0);
        p.Ix = Ix_mesas + Ix_alma + Ix_cantos;
    }

    // 5. Momento de inércia y-y (Iy)
    if (p.Iy <= 0.0) {
        double Iy_mesas = 2.0 * ((p.tf * std::pow(p.bf, 3.0)) / 12.0);
        double Iy_alma = (p.h * std::pow(p.tw, 3.0)) / 12.0;
        double Iy_cantos = 4.0 * I_canto_proprio + area_cantos * std::pow(x_canto, 2.0);
        p.Iy = Iy_mesas + Iy_alma + Iy_cantos;
    }

    // 6. Módulos de resistência elásticos (Wx e Wy)
    if (p.Wx <= 0.0 && p.d > 0.0) p.Wx = p.Ix / (p.d / 2.0);
    if (p.Wy <= 0.0 && p.bf > 0.0) p.Wy = p.Iy / (p.bf / 2.0);

    // 7. Módulos de resistência plásticos (Zx e Zy)
    if (p.Zx <= 0.0) {
        p.Zx = p.bf * p.tf * (p.d - p.tf) + (p.tw * std::pow(p.h, 2.0)) / 4.0 + area_cantos * y_canto;
    }
    if (p.Zy <= 0.0) {
        p.Zy = (p.tf * std::pow(p.bf, 2.0)) / 2.0 + (p.h * std::pow(p.tw, 2.0)) / 4.0 + area_cantos * x_canto;
    }

    // 8. Raios de giração (rx, ry e rt)
    if (p.rx <= 0.0 && p.Ag > 0.0) p.rx = std::sqrt(p.Ix / p.Ag);
    if (p.ry <= 0.0 && p.Ag > 0.0) p.ry = std::sqrt(p.Iy / p.Ag);
    if (p.rt <= 0.0 && p.bf > 0.0 && p.tf > 0.0) {
        p.rt = p.bf / std::sqrt(12.0 * (1.0 + (p.h * p.tw) / (6.0 * p.bf * p.tf)));
    }

    // 9. Esbeltezes locais (lbf e lbw)
    if (p.lbf <= 0.0 && p.tf > 0.0) p.lbf = p.bf / (2.0 * p.tf);
    if (p.lbw <= 0.0 && p.tw > 0.0) p.lbw = p.d1 / p.tw;

    // 10. Constante de torção St. Venant (It) com correção de concordância mesa-alma (El-Darwish & Johnston)
    if (p.It <= 0.0) {
        double It_retangulos = (2.0 * p.bf * std::pow(p.tf, 3.0) + p.h * std::pow(p.tw, 3.0)) / 3.0;
        if (r > 0.0 && (2.0 * r + p.tf) > 0.0) {
            double D = (std::pow(p.tf + r, 2.0) + p.tw * (r + p.tw / 4.0)) / (2.0 * r + p.tf);
            double alpha = 0.094 + 0.070 * (p.tw / p.tf);
            double delta_It = 2.0 * alpha * std::pow(D, 4.0) - 0.420 * std::pow(p.tf, 4.0);
            p.It = It_retangulos + delta_It;
        } else {
            p.It = It_retangulos;
        }
    }

    // 11. Constante de empenamento (Cw)
    if (p.Cw <= 0.0) {
        p.Cw = (p.tf * std::pow(p.bf, 3.0) * std::pow(p.d - p.tf, 2.0)) / 24.0;
    }
}
