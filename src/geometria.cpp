#include "geometria.hpp"
#include <cmath>

void calcular_propriedades_secao_i(PerfilI& p) {
    const double pi = 3.14159265358979323846;

    // Altura livre da alma
    if (p.r > 0.0 && p.tipo == LAMINADO) {
        p.d1 = p.d - 2.0 * (p.tf + p.r);
    } else {
        p.d1 = p.d - 2.0 * p.tf;
    }

    double h_alma = p.d - 2.0 * p.tf;
    double area_cantos = (p.r > 0.0) ? (4.0 - pi) * p.r * p.r : 0.0;

    // Área bruta
    if (p.Ag <= 0.0) {
        p.Ag = 2.0 * p.bf * p.tf + h_alma * p.tw + area_cantos;
    }

    // Momento de inércia x-x
    if (p.Ix <= 0.0) {
        double Ix_mesas = 2.0 * ((p.bf * std::pow(p.tf, 3.0)) / 12.0 + (p.bf * p.tf) * std::pow((p.d - p.tf) / 2.0, 2.0));
        double Ix_alma = (p.tw * std::pow(h_alma, 3.0)) / 12.0;
        p.Ix = Ix_mesas + Ix_alma;
    }

    // Momento de inércia y-y
    if (p.Iy <= 0.0) {
        p.Iy = 2.0 * ((p.tf * std::pow(p.bf, 3.0)) / 12.0) + (h_alma * std::pow(p.tw, 3.0)) / 12.0;
    }

    // Módulos elásticos
    if (p.Wx <= 0.0 && p.d > 0.0) p.Wx = p.Ix / (p.d / 2.0);
    if (p.Wy <= 0.0 && p.bf > 0.0) p.Wy = p.Iy / (p.bf / 2.0);

    // Módulos plásticos
    if (p.Zx <= 0.0) {
        p.Zx = p.bf * p.tf * (p.d - p.tf) + (p.tw * std::pow(h_alma, 2.0)) / 4.0;
    }
    if (p.Zy <= 0.0) {
        p.Zy = 2.0 * ((p.tf * std::pow(p.bf, 2.0)) / 4.0) + (h_alma * std::pow(p.tw, 2.0)) / 4.0;
    }

    // Raios de giração
    if (p.rx <= 0.0 && p.Ag > 0.0) p.rx = std::sqrt(p.Ix / p.Ag);
    if (p.ry <= 0.0 && p.Ag > 0.0) p.ry = std::sqrt(p.Iy / p.Ag);

    // Inércia de torção e constante de empenamento
    if (p.It <= 0.0) {
        p.It = (2.0 * p.bf * std::pow(p.tf, 3.0) + h_alma * std::pow(p.tw, 3.0)) / 3.0;
    }
    if (p.Cw <= 0.0) {
        p.Cw = (p.Iy * std::pow(p.d - p.tf, 2.0)) / 4.0;
    }
}
