#include "../src/procesos.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

int main() {

    const int N = 10000;

    std::vector<Actividad> actividades;

    actividades.reserve(N);

    // Crear una cadena grande:
    // 1 -> 2 -> 3 -> 4 -> ... -> 10000

    for (int i = 1; i <= N; i++) {

        Actividad actividad;

        actividad.id = std::to_string(i);
        actividad.nombre = "actividad_" + std::to_string(i);
        actividad.tiempo = 1;
        actividad.estado = Estado::PENDIENTE;

        if (i == 1) {
            actividad.dependenciasRestantes = 0;
        } else {
            actividad.dependencias.push_back(
                std::to_string(i - 1)
            );

            actividad.dependenciasRestantes = 1;
        }

        if (i < N) {
            actividad.dependientes.push_back(
                std::to_string(i + 1)
            );
        }

        actividades.push_back(actividad);
    }

    std::cout << "Actividades creadas: "
              << actividades.size()
              << std::endl;

    auto inicio = std::chrono::high_resolution_clock::now();

    // Simular fallo en la actividad 1
    cancelarRamaPorFallo(
        actividades,
        "1"
    );

    auto fin = std::chrono::high_resolution_clock::now();

    auto duracion =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            fin - inicio
        );

    int fallidas = 0;
    int canceladas = 0;
    int pendientes = 0;

    for (const auto& actividad : actividades) {

        if (actividad.estado == Estado::FALLIDA) {
            fallidas++;
        }

        else if (actividad.estado == Estado::CANCELADA) {
            canceladas++;
        }

        else if (actividad.estado == Estado::PENDIENTE) {
            pendientes++;
        }
    }

    std::cout << "Fallidas: "
              << fallidas
              << std::endl;

    std::cout << "Canceladas: "
              << canceladas
              << std::endl;

    std::cout << "Pendientes: "
              << pendientes
              << std::endl;

    std::cout << "Tiempo de cancelacion: "
              << duracion.count()
              << " ms"
              << std::endl;

    return 0;
}