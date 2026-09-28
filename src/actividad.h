#ifndef ACTIVIDAD_H
#define ACTIVIDAD_H

#include <string>
#include <vector>

enum class Estado {
    PENDIENTE, 
    EJECUTANDO, 
    TERMINADA, 
    FALLIDA, 
    CANCELADA 
};

struct Actividad {
    std::string id;
    std::string nombre;
    int tiempo;

    std::vector<std::string> dependencias;
    std::vector<std::string> dependientes;
     int dependenciasRestantes;

    Estado estado;
};

#endif
