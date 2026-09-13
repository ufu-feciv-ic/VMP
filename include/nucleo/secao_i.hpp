#pragma once

#include <string>

namespace vmp::nucleo {

    enum class TipoFabricacao {
        Laminado, // Perfil laminado (ex: serie W, HP)
        Soldado   // Perfil soldado (ex: serie VS)
    };

    class SecaoI {
    public:
        std::string nome;
        TipoFabricacao tipo;

        // Dimensoes em milimetros (mm)
        double d;   // Altura total do perfil
        double bf;  // Largura da mesa
        double tf;  // Espessura da mesa
        double tw;  // Espessura da alma
        double r;   // Raio de concordancia entre mesa e alma (0.0 para soldados)
        double d1;  // Altura livre da alma

        // Propriedades geometricas (mm² e mm⁴)
        double Ag;  // Area bruta da secao
        double Ix;  // Momento de inercia no eixo de maior inercia (x-x)
        double Iy;  // Momento de inercia no eixo de menor inercia (y-y)

        SecaoI(
            std::string nome,
            TipoFabricacao tipo,
            double d,
            double bf,
            double tf,
            double tw,
            double r = 0.0,
            double d1 = 0.0,
            double Ag = 0.0,
            double Ix = 0.0,
            double Iy = 0.0
        );

        // Perfil de catalogo de exemplo (Dissertacao Tabela 10)
        static SecaoI w360x57_8();

        // Metodos utilitarios simples
        bool eh_laminado() const;
        bool eh_soldado() const;

        // Razoes de esbeltez locais
        double esbeltez_mesa() const; // bf / (2 * tf)
        double esbeltez_alma() const; // d1 / tw

        // Geometria complementar considerando o raio de concordancia
        double altura_entre_mesas() const; // h = d - 2 * tf (distancia entre faces internas)
        double cota_k() const;             // k = tf + r (distancia da face externa ao inicio da alma reta)
        double area_cantos_raio() const;   // Area de aco adicional dos 4 cantos curvos
    };

} // namespace vmp::nucleo
