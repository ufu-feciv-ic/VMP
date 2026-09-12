#pragma once

struct Material
{
    enum class Tipo { Aço, Concreto };

    Tipo TipoMaterial;
    float ModElasticidade;
    float Resistencia;

    Material(Tipo tipo, float modElas, float Resistencia) : 
    TipoMaterial(tipo), ModElasticidade(modElas), Resistencia(Resistencia) {}
};