#include "../src/procesos.h"

#include <iostream>
#include <string>

int main() {

    // Activar el manejo de Ctrl+C
    configurarSIGINT();

    // Actividad larga para tener tiempo de presionar Ctrl+C
    Actividad actividad;

    actividad.id = "99";
    actividad.nombre = "actividad_larga";
    actividad.tiempo = 10000; // 10 segundos
    actividad.estado = Estado::PENDIENTE;

    // Crear pipe
    int tuberia[2];

    if (crearPipe(tuberia) == -1) {
        return 1;
    }

    // Crear proceso hijo
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