#pragma once

struct EsforcosInternos
{
    float MomentoFletor;
    float ForcaCortante;
    float ForcaAxial;

    EsforcosInternos(float m, float v, float n) :
    MomentoFletor(m), ForcaCortante(v), ForcaAxial(n) {}
};