#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <string>
#include <vector>
#include "actividad.h"

bool dependenciasTerminadas(
    const Actividad& actividad,
    const std::vector<Actividad>& actividades
);

std::vector<std::string> obtenerActividadesListas(
    const std::vector<Actividad>& actividades,
    int K
);

void iniciarActividades(
    std::vector<Actividad>& actividades,
    const std::vector<std::string>& ids
);
void terminarActividades(
    std::vector<Actividad>& actividades,
    const std::vector<std::string>& ids
);

#endif