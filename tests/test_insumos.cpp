#include "../src/procesos.h"

#include <iostream>
#include <vector>
#include <string>

int main() {

    Actividad actividad;

    actividad.id = "3";
    actividad.nombre = "actividad_dependiente";
    actividad.tiempo = 500;

    std::vector<ProcesoActividad> procesos;

    std::vector<std::string> insumos = {
        "TERMINADA:1",
        "TERMINADA:2"
    };

    pid_t pid = lanzarActividadConInsumos(
        actividad,
        procesos,
        insumos
    );

    if (pid == -1) {
        std::cerr << "Error al lanzar actividad." << std::endl;
        return 1;
    }

    std::string idActividad;
    std::string mensaje;
    int codigoSalida = -1;

    bool resultado = recogerProcesoTerminado(
        procesos,
        idActividad,
        mensaje,
        codigoSalida
    );

    if (!resultado) {
        std::cerr << "Error al recoger proceso." << std::endl;
        return 1;
    }

    std::cout << "ID finalizado: "
              << idActividad
              << std::endl;

    std::cout << "Mensaje al padre: "
              << mensaje
              << std::endl;

    std::cout << "Codigo de salida: "
              << codigoSalida
              << std::endl;

    return 0;
}