#include "parser.h"
#include "dag.h"
#include "planificador.h"
//#include "procesos.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Uso: ./planificador plan.txt K" << std::endl;
        return 1;
    }

    std::string nombreArchivo = argv[1];
    int K = std::stoi(argv[2]);
    //configurarSIGINT();

    std::cout << "Archivo: " << nombreArchivo << std::endl;
    std::cout << "Limite de concurrencia K: " << K << std::endl;

    std::vector<Actividad> actividades = cargarPlan(nombreArchivo);
    //std::vector<ProcesoActividad> procesosEnEjecucion;

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

std::vector<std::string> listas =
    obtenerActividadesListas(actividades, K);

std::cout << "Actividades listas para ejecutar: ";

for (const auto& id : listas) {
    std::cout << id << " ";
}

std::cout << std::endl;

iniciarActividades(actividades, listas);



std::cout << "Estados despues de iniciar:" << std::endl;

for (const auto& actividad : actividades) {
    std::cout << "Actividad " << actividad.id
              << ": ";

    if (actividad.estado == Estado::PENDIENTE) {
        std::cout << "PENDIENTE";
    } else if (actividad.estado == Estado::EJECUTANDO) {
        std::cout << "EJECUTANDO";
    } else if (actividad.estado == Estado::TERMINADA) {
        std::cout << "TERMINADA";
    }

    std::cout << std::endl;
}

std::vector<std::string> terminadas = {"1", "2"};
terminarActividades(actividades, terminadas);


std::vector<std::string> nuevasListas =
    obtenerActividadesListas(actividades, K);

std::cout << "Nuevas actividades listas para ejecutar: ";

for (const auto& id : nuevasListas) {
    std::cout << id << " ";
}
terminarActividades(actividades, nuevasListas);

std::vector<std::string> siguientes =
    obtenerActividadesListas(actividades, K);

std::cout << "Siguientes actividades listas para ejecutar: ";

for (const auto& id : siguientes) {
    std::cout << id << " ";
}

std::cout << std::endl;
terminarActividades(actividades, siguientes);

std::vector<std::string> finales =
    obtenerActividadesListas(actividades, K);

std::cout << "Ultima actividad lista para ejecutar: ";

for (const auto& id : finales) {
    std::cout << id << " ";
}

std::cout << std::endl;
    return 0;
}