#include "../src/procesos.h"

#include <iostream>
#include <string>

int main() {

    
    configurarSIGINT();

    
    Actividad actividad;

    actividad.id = "99";
    actividad.nombre = "actividad_larga";
    actividad.tiempo = 10000; // 10 segundos
    actividad.estado = Estado::PENDIENTE;

    
    int tuberia[2];

    if (crearPipe(tuberia) == -1) {
        return 1;
    }

    
    pid_t pid = crearProceso(actividad, tuberia);

    if (pid == -1) {
        return 1;
    }

    std::cout << "Actividad ejecutandose." << std::endl;
    std::cout << "Presiona Ctrl+C antes de que pasen 10 segundos."
              << std::endl;

    std::string mensaje;

    esperarYRecibir(
        pid,
        tuberia[0],
        mensaje
    );

    return 0;
}