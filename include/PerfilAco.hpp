#pragma once

#include "Material.hpp"
#include "SecaoTransversal.hpp"

struct PerfilAco
{
    enum class ProcessoFabricacao { Laminado, Soldado };

    Material Material;
    SecaoTransversal SecaoI;
    ProcessoFabricacao Fabricacao;

    
};