#include "../src/procesos.h"

#include <iostream>
#include <vector>

const char* nombreEstado(Estado estado) {

    switch (estado) {
        case Estado::PENDIENTE:
            return "PENDIENTE";

        case Estado::EJECUTANDO:
            return "EJECUTANDO";

        case Estado::TERMINADA:
            return "TERMINADA";

        case Estado::FALLIDA:
            return "FALLIDA";

        case Estado::CANCELADA:
            return "CANCELADA";
    }

    return "DESCONOCIDO";
}

int main() {

    std::vector<Actividad> actividades;

    Actividad a1;
    a1.id = "1";
    a1.nombre = "actividad_1";
    a1.tiempo = 100;
    a1.dependientes = {"3"};
    a1.dependenciasRestantes = 0;
    a1.estado = Estado::PENDIENTE;

    Actividad a2;
    a2.id = "2";
    a2.nombre = "actividad_2";
    a2.tiempo = 100;
    a2.dependientes = {"4"};
    a2.dependenciasRestantes = 0;
    a2.estado = Estado::PENDIENTE;

    Actividad a3;
    a3.id = "3";
    a3.nombre = "actividad_3";
    a3.tiempo = 100;
    a3.dependencias = {"1"};
    a3.dependientes = {"5"};
    a3.dependenciasRestantes = 1;
    a3.estado = Estado::PENDIENTE;

    Actividad a4;
    a4.id = "4";
    a4.nombre = "actividad_4";
    a4.tiempo = 100;
    a4.dependencias = {"2"};
    a4.dependientes = {"6"};
    a4.dependenciasRestantes = 1;
    a4.estado = Estado::PENDIENTE;

    Actividad a5;
    a5.id = "5";
    a5.nombre = "actividad_5";
    a5.tiempo = 100;
    a5.dependencias = {"3"};
    a5.dependenciasRestantes = 1;
    a5.estado = Estado::PENDIENTE;

    Actividad a6;
    a6.id = "6";
    a6.nombre = "actividad_6";
    a6.tiempo = 100;
    a6.dependencias = {"4"};
    a6.dependenciasRestantes = 1;
    a6.estado = Estado::PENDIENTE;

    actividades.push_back(a1);
    actividades.push_back(a2);
    actividades.push_back(a3);
    actividades.push_back(a4);
    actividades.push_back(a5);
    actividades.push_back(a6);

    
    cancelarRamaPorFallo(actividades, "3");

    std::cout << "Estados despues del fallo:" << std::endl;

    for (const auto& actividad : actividades) {

        std::cout << "Actividad "
                  << actividad.id
                  << ": "
                  << nombreEstado(actividad.estado)
                  << std::endl;
    }

    return 0;
}