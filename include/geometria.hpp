#pragma once
#include "modelos.hpp"

// Calcula e preenche automaticamente as propriedades geométricas de um Perfil I
// caso apenas as dimensões nominais (d, bf, tf, tw, r) tenham sido fornecidas.
void calcular_propriedades_secao_i(PerfilI& p);
