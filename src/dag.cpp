#include "dag.h"

#include <queue>
#include <unordered_map>


void construirDependientes(std::vector<Actividad>& actividades) {

    // Primera pasada:
    // limpiar todos los dependientes e inicializar
    // la cantidad de dependencias pendientes.
    for (auto& actividad : actividades) {

        actividad.dependientes.clear();

        actividad.dependenciasRestantes =
            static_cast<int>(
                actividad.dependencias.size()
            );
    }

    // Segunda pasada:
    // construir las relaciones entre actividades.
    for (const auto& actividad : actividades) {

        for (const auto& dependencia : actividad.dependencias) {

            for (auto& actividadDependencia : actividades) {

                if (actividadDependencia.id == dependencia) {

                    actividadDependencia.dependientes.push_back(
                        actividad.id
                    );

                    break;
                }
            }
        }
    }
}


bool validarDAG(const std::vector<Actividad>& actividades) {

    std::unordered_map<std::string, int> pendientes;

    for (const auto& actividad : actividades) {

        pendientes[actividad.id] =
            actividad.dependencias.size();
    }


    std::queue<std::string> disponibles;

    for (const auto& actividad : actividades) {

        if (pendientes[actividad.id] == 0) {

            disponibles.push(
                actividad.id
            );
        }
    }


    std::size_t procesadas = 0;


    while (!disponibles.empty()) {

        std::string id =
            disponibles.front();

        disponibles.pop();

        procesadas++;


        for (const auto& dependiente : actividades) {

            for (const auto& dependencia : dependiente.dependencias) {

                if (dependencia == id) {

                    pendientes[
                        dependiente.id
                    ]--;

                    if (
                        pendientes[
                            dependiente.id
                        ] == 0
                    ) {

                        disponibles.push(
                            dependiente.id
                        );
                    }
                }
            }
        }
    }


    return procesadas == actividades.size();
}