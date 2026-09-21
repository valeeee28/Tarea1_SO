#include "../src/procesos.h"

#include <iostream>
#include <vector>
#include <string>

int main() {

    configurarSIGINT();

    std::vector<ProcesoActividad> procesos;

    // -------------------------
    // ACTIVIDAD 1
    // -------------------------
    Actividad a1;

    a1.id = "1";
    a1.nombre = "actividad_1";
    a1.tiempo = 2000;
    a1.dependenciasRestantes = 0;
    a1.estado = Estado::PENDIENTE;

    // -------------------------
    // ACTIVIDAD 2
    // -------------------------
    Actividad a2;

    a2.id = "2";
    a2.nombre = "actividad_2";
    a2.tiempo = 500;
    a2.dependenciasRestantes = 0;
    a2.estado = Estado::PENDIENTE;

    // -------------------------
    // ACTIVIDAD 3
    // -------------------------
    Actividad a3;

    a3.id = "3";
    a3.nombre = "actividad_3";
    a3.tiempo = 1200;
    a3.dependenciasRestantes = 0;
    a3.estado = Estado::PENDIENTE;

    // Lanzar las tres actividades sin esperar entre medio
    lanzarActividad(a1, procesos);
    lanzarActividad(a2, procesos);
    lanzarActividad(a3, procesos);

    std::cout << "\nSe lanzaron 3 procesos." << std::endl;
    std::cout << "Esperando resultados...\n" << std::endl;

    // Recoger los tres procesos a medida que terminan
    while (!procesos.empty()) {

        std::string idActividad;
        std::string mensaje;
        int codigoSalida;

        bool resultado = recogerProcesoTerminado(
            procesos,
            idActividad,
            mensaje,
            codigoSalida
        );

        if (!resultado) {
            std::cerr << "Error al recoger un proceso." << std::endl;
            return 1;
        }

        std::cout << "Proceso terminado:" << std::endl;
        std::cout << "  Actividad: " << idActividad << std::endl;
        std::cout << "  Mensaje: " << mensaje << std::endl;
        std::cout << "  Codigo: " << codigoSalida << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Todas las actividades terminaron." << std::endl;

    return 0;
}