#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "actividad.h"

std::vector<Actividad> cargarPlan(const std::string& nombreArchivo);

#endif
