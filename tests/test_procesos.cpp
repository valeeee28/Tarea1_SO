#include "../src/procesos.h"

#include <iostream>
#include <string>
#include <unistd.h>

int main() {

    // Actividad de prueba
    Actividad actividad;

    actividad.id = "1";
    actividad.nombre = "prender_carbon";
    actividad.tiempo = 500;
    actividad.estado = Estado::PENDIENTE;

    // Crear la tuberia
    int tuberia[2];

    if (crearPipe(tuberia) == -1) {
        return 1;
    }

    // Crear el proceso hijo
    pid_t pid = crearProceso(actividad, tuberia);

    if (pid == -1) {
        close(tuberia[0]);
        close(tuberia[1]);
        return 1;
    }

    // El padre recibe el mensaje del hijo
    std::string mensaje;

    int resultado = esperarYRecibir(
        pid,
        tuberia[0],
        mensaje
    );

    std::cout << "Mensaje recibido: "
              << mensaje << std::endl;

    std::cout << "Resultado del proceso: "
              << resultado << std::endl;

    return 0;
}