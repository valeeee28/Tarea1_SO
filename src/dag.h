#ifndef DAG_H
#define DAG_H

#include <vector>
#include "actividad.h"

void construirDependientes(std::vector<Actividad>& actividades);

bool validarDAG(const std::vector<Actividad>& actividades);
#endif