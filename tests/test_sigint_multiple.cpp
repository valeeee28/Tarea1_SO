#include "../src/procesos.h"

#include <iostream>
#include <vector>
#include <unistd.h>

int main() {

    
    configurarSIGINT();

    std::vector<ProcesoActividad> procesos;

    
    Actividad a1;
    a1.id = "1";
    a1.nombre = "actividad_larga_1";
    a1.tiempo = 10000;
    a1.estado = Estado::PENDIENTE;

    
    Actividad a2;
    a2.id = "2";
    a2.nombre = "actividad_larga_2";
    a2.tiempo = 10000;
    a2.estado = Estado::PENDIENTE;

    
    Actividad a3;
    a3.id = "3";
    a3.nombre = "actividad_larga_3";
    a3.tiempo = 10000;
    a3.estado = Estado::PENDIENTE;

    
    lanzarActividad(a1, procesos);
    lanzarActividad(a2, procesos);
    lanzarActividad(a3, procesos);

    std::cout << std::endl;
    std::cout << "Hay 3 actividades ejecutandose." << std::endl;
    std::cout << "Presiona Ctrl+C antes de que pasen 10 segundos."
              << std::endl;

    
    while (true) {
        pause();
    }

    return 0;
}