#pragma once

namespace vmp::nucleo {

    struct EsforcosInternos {
        double momento_fletor; // kNm ou N.mm
        double forca_cortante; // kN ou N
        double forca_axial;    // kN ou N

        EsforcosInternos(double momento = 0.0, double cortante = 0.0, double axial = 0.0)
            : momento_fletor(momento), forca_cortante(cortante), forca_axial(axial) {}
    };

} // namespace vmp::nucleo
