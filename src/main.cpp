#include "parser.h"
#include "dag.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Uso: ./planificador plan.txt K" << std::endl;
        return 1;
    }

    std::string nombreArchivo = argv[1];
    int K = std::stoi(argv[2]);

    std::cout << "Archivo: " << nombreArchivo << std::endl;
    std::cout << "Limite de concurrencia K: " << K << std::endl;

    std::vector<Actividad> actividades = cargarPlan(nombreArchivo);

    construirDependientes(actividades);
if (validarDAG(actividades)) {
    std::cout << "DAG valido" << std::endl;
} else {
    std::cout << "Error: el plan contiene un ciclo" << std::endl;
    return 1;
}
    for (const auto& actividad : actividades) {

        std::cout << "Actividad " << actividad.id
                  << " depende de: ";

        for (const auto& dependencia : actividad.dependencias) {
            std::cout << dependencia << " ";
        }

        std::cout << "| Dependen de ella: ";

        for (const auto& dependiente : actividad.dependientes) {
            std::cout << dependiente << " ";
        }

        std::cout << std::endl;
    }

    std::cout << "Cantidad de actividades cargadas: "
              << actividades.size() << std::endl;

    return 0;
}