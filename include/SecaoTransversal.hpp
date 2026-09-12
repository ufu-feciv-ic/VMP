#pragma once

#include <string>

struct SecaoTransversal
{ 
    std::string NomePerfil;
    float MassaLinear;
    float d;
    float bf;
    float tw;
    float tf;
    float h;
    float dLinha;
    float Area;
    float Raio;
    float Ixx;
    float Wxx;
    float rxx;
    float Zxx;
    float Iyy;
    float Wyy;
    float ryy;
    float Zyy;
    float rt;
    float It;
    float Cw;
    float LambdaMesa;
    float LambdaAlma;

    SecaoTransversal(
        std::string NomePerfil,
        float MassaLinear,
        float AlturaExternaPerfil,
        float LarguraMesa,
        float EspessuraAlma,
        float EspessuraMesa,
        float AlturaAlma,
        float AlturaLivreAlma,
        float Area,
        float Raio,
        float Ixx,
        float Wxx,
        float rxx,
        float Zxx,
        float Iyy,
        float Wyy,
        float ryy,
        float Zyy,
        float rt,
        float It,
        float Cw
    ) 
    : 
    NomePerfil(NomePerfil),
    MassaLinear(MassaLinear),
    d(AlturaExternaPerfil), 
    bf(LarguraMesa),
    tw(EspessuraAlma),
    tf(EspessuraMesa),
    h(AlturaAlma),  
    dLinha(AlturaLivreAlma),
    Area(Area),
    Raio(Raio),
    Ixx(Ixx),
    Wxx(Wxx),
    rxx(rxx),
    Zxx(Zxx),
    Iyy(Iyy),
    Wyy(Wyy),
    ryy(ryy),
    Zyy(Zyy),
    rt(rt),
    It(It),
    Cw(Cw),
    LambdaMesa(bf / (2*tf)),
    LambdaAlma(dLinha / tw)
    {}
};
