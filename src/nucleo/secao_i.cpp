#include "nucleo/secao_i.hpp"
#include <cmath>

namespace vmp::nucleo {

    const double PI = 3.14159265358979323846;

    SecaoI::SecaoI(
        std::string nome,
        TipoFabricacao tipo,
        double d,
        double bf,
        double tw,
        double tf,
        double r,
        double h,
        double d1,
        double Ag,
        double Ix,
        double Wx,
        double rx,
        double Zx, 
        double Iy,
        double Wy,
        double ry,
        double Zy,
        double rt,
        double It,
        double Cw
    ) : nome(nome),
        tipo(tipo),
        d(d),
        bf(bf),
        tf(tf),
        tw(tw),
        r(r)
    {
        // Altura entre as mesas
        this->h = (h > 0.0) ? h : (d - 2.0 * tf);

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

        // Eixo X-X
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

        // Módulo de resistencia elástico Wx (mm³)
        this->Wx = (Wx > 0.0) ? Wx : (2.0 * this-> Ix/d);

        // Raio de giração rx (mm)
        this->rx = (rx > 0.0) ? rx : std::sqrt(this->Ix / this->Ag);

        // Módulo de resistencia plástico Z (mm³)
        this->Zx = (Zx > 0.0) ? Zx : (bf * tf * (d - tf) + (tw * std::pow(this->h, 2.0)) / 4.0);
        

        // Eixo Y-Y
        // Se o momento de inercia Iy nao for informado, calcula pelas partes retangulares
        if (Iy > 0.0) {
            this->Iy = Iy;
        } else {
            double h_alma = d - 2.0 * tf;
            double iy_mesas = 2.0 * ((tf * std::pow(bf, 3)) / 12.0);
            double iy_alma = (h_alma * std::pow(tw, 3)) / 12.0;
            this->Iy = iy_mesas + iy_alma;
        }

        // Módulo de resistencia elástico Wx (mm³)
        this->Wy = (Wy > 0.0) ? Wy : (2.0 * this->Iy / bf);

        // Raio de giração rx (mm)
        this->ry = (ry > 0.0) ? ry : std::sqrt(this->Iy / this->Ag);

        // Módulo de resistencia plástico Z (mm³)
        this->Zy = (Zy > 0.0) ? Zy : (2.0 * (tf * std::pow(bf, 2.0) / 4.0) + (this->h * std::pow(tw, 2.0)) / 4.0);


        // TOrção e Empenamento
        // Raio de giração a FLT rt (mm)
        this->rt = (rt > 0.0) ? rt : (bf / std::sqrt(12.0 * (1.0 + (this->h * tw) / (6.0 * bf * tf))));

        // Constante de torcao de Saint-Venant It / J (mm⁴)
        this->It = (It > 0.0) ? It : ((2.0 * bf * std::pow(tf, 3.0) + this->h * std::pow(tw, 3.0)) / 3.0);

        // Constante de empenamento Cw (mm⁶)
        this->Cw = (Cw > 0.0) ? Cw : ((this->Iy * std::pow(d - tf, 2.0)) / 4.0);


        // Esbeltezes locais
        this->lambda_f = bf / (2.0 * tf); // Esbeltez da mesa
        this->lambda_w = this->d1 / tw; // Esbeltez da alma
    }

    SecaoI SecaoI::w360x57_8() {
        // Dados oficiais do perfil W 360 x 57.8 (NBR 15980 / Catalogo Gerdau)
        return SecaoI(
            "W 360 x 57.8",
            TipoFabricacao::Laminado,
            358.0,              // d (mm)
            172.0,              // bf (mm)
            7.9,                // tw (mm)
            13.1,               // tf (mm)
            10.2,               // r (mm) - raio de concordancia NBR 15980
            332.0,              // h (mm) - altura alma
            308.0,              // d' (mm) - altura alma livre
            72.50 * 1e2,        // Ag (mm²)
            16143 * 1e4,        // Ix (mm⁴)
            901.8 * 1e3,        // Wx (mm³)
            14.22 * 1e1,        // rx (mm)
            1014.8 * 1e3,       // Zx (mm²)
            1113 * 1e4,         // Iy (mm⁴)
            129.4 * 1e3,        // Wy (mm³)
            3.92 * 1e1,         // ry (mm)
            199.8 * 1e3,        // Zy (mm³)
            4.53 * 1e1,         // rt (mm)
            34.45 * 1e4,        // It (mm⁴)
            330.394 * 1e6       // Cw (mm⁴)
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
