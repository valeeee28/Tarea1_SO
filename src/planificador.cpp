#include "planificador.h"

bool dependenciasTerminadas(
    const Actividad& actividad,
    const std::vector<Actividad>&
) {
    return actividad.dependenciasRestantes == 0;
}
std::vector<std::string> obtenerActividadesListas(
    const std::vector<Actividad>& actividades,
    int K
) {
    std::vector<std::string> listas;

    int ejecutando = 0;

    for (const auto& actividad : actividades) {
        if (actividad.estado == Estado::EJECUTANDO) {
            ejecutando++;
        }
    }

    int cupos = K - ejecutando;

    if (cupos <= 0) {
        return listas;
    }

    for (const auto& actividad : actividades) {

        if (actividad.estado != Estado::PENDIENTE) {
            continue;
        }

        if (dependenciasTerminadas(actividad, actividades)) {

            listas.push_back(actividad.id);

            cupos--;

            if (cupos == 0) {
                break;
            }
        }
    }

    return listas;
}

void iniciarActividades(
    std::vector<Actividad>& actividades,
    const std::vector<std::string>& ids
) {
    for (auto& actividad : actividades) {

        for (const auto& id : ids) {

          if (actividad.id == id &&
    actividad.estado == Estado::PENDIENTE) {
    actividad.estado = Estado::EJECUTANDO;
    break;
}
        }
    }
}
void terminarActividades(
    std::vector<Actividad>& actividades,
    const std::vector<std::string>& ids
) {
    for (auto& actividad : actividades) {

        for (const auto& id : ids) {

            if (actividad.id == id) {

                if (actividad.estado == Estado::TERMINADA) {
                    break;
                }

                actividad.estado = Estado::TERMINADA;

                for (const auto& dependiente :
                     actividad.dependientes) {

                    for (auto& actividadDependiente :
                         actividades) {

                        if (actividadDependiente.id == dependiente) {

                            if (actividadDependiente.dependenciasRestantes > 0) {
                                actividadDependiente.dependenciasRestantes--;
                            }

                            break;
                        }
                    }
                }

                break;
            }
        }
    }
}