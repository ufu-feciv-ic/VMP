#include "nucleo/secao_i.hpp"
#include <cmath>

namespace vmp::nucleo {

    const double PI = 3.14159265358979323846;

    SecaoI::SecaoI(
        std::string nome,
        TipoFabricacao tipo,
        double d,
        double bf,
        double tf,
        double tw,
        double r,
        double d1,
        double Ag,
        double Ix,
        double Iy
    ) : nome(nome),
        tipo(tipo),
        d(d),
        bf(bf),
        tf(tf),
        tw(tw),
        r(r)
    {
        // Se d1 nao for informado manualmente:
        // Para laminados, desconta os raios de concordancia (d - 2*(tf + r))
        // Para soldados, a distancia livre reta e de mesa a mesa (d - 2*tf)
        if (d1 > 0.0) {
            this->d1 = d1;
        } else {
            if (r > 0.0) {
                this->d1 = d - 2.0 * (tf + r);
            } else {
                this->d1 = d - 2.0 * tf;
            }
        }

        // Se a area bruta nao for informada, calcula pelos retangulos + cantos curvos
        if (Ag > 0.0) {
            this->Ag = Ag;
        } else {
            double h_entre_mesas = d - 2.0 * tf;
            double area_retangulos = 2.0 * bf * tf + h_entre_mesas * tw;
            double area_raios = area_cantos_raio();
            this->Ag = area_retangulos + area_raios;
        }

        // Se o momento de inercia Ix nao for informado, calcula pelas partes retangulares
        if (Ix > 0.0) {
            this->Ix = Ix;
        } else {
            double h_alma = d - 2.0 * tf;
            double y_mesa = (d - tf) / 2.0;
            double ix_alma = (tw * std::pow(h_alma, 3)) / 12.0;
            double ix_mesas = 2.0 * ((bf * std::pow(tf, 3)) / 12.0 + (bf * tf) * std::pow(y_mesa, 2));
            this->Ix = ix_alma + ix_mesas;
        }

        // Se o momento de inercia Iy nao for informado, calcula pelas partes retangulares
        if (Iy > 0.0) {
            this->Iy = Iy;
        } else {
            double h_alma = d - 2.0 * tf;
            double iy_mesas = 2.0 * ((tf * std::pow(bf, 3)) / 12.0);
            double iy_alma = (h_alma * std::pow(tw, 3)) / 12.0;
            this->Iy = iy_mesas + iy_alma;
        }
    }

    SecaoI SecaoI::w360x57_8() {
        // Dados oficiais do perfil W 360 x 57.8 (NBR 15980 / Catalogo Gerdau)
        return SecaoI(
            "W 360 x 57.8",
            TipoFabricacao::Laminado,
            358.0,              // d (mm)
            172.0,              // bf (mm)
            13.1,               // tf (mm)
            7.9,                // tw (mm)
            10.2,               // r (mm) - raio de concordancia NBR 15980
            331.8,              // d1 (mm) - conforme Tabela 10 da dissertacao
            7320.0,             // Ag (mm²)
            16067.0 * 1e4,      // Ix (mm⁴)
            1118.0 * 1e4        // Iy (mm⁴)
        );
    }

    bool SecaoI::eh_laminado() const {
        return tipo == TipoFabricacao::Laminado;
    }

    bool SecaoI::eh_soldado() const {
        return tipo == TipoFabricacao::Soldado;
    }

    double SecaoI::esbeltez_mesa() const {
        return bf / (2.0 * tf);
    }

    double SecaoI::esbeltez_alma() const {
        return d1 / tw;
    }

    double SecaoI::altura_entre_mesas() const {
        return d - 2.0 * tf;
    }

    double SecaoI::cota_k() const {
        return tf + r;
    }

    double SecaoI::area_cantos_raio() const {
        // Area dos 4 cantos de concordancia: 4 * (1 - pi/4) * r^2 = (4 - pi) * r^2
        return (4.0 - PI) * r * r;
    }

} // namespace vmp::nucleo
