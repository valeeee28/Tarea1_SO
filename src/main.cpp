#include "parser.h"
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

    std::cout << "Cantidad de actividades cargadas: "
              << actividades.size() << std::endl;

    return 0;
}
