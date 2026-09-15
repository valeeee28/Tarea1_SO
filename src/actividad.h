#ifndef ACTIVIDAD_H
#define ACTIVIDAD_H

#include <string>
#include <vector>

enum class Estado {
    PENDIENTE, // aun no comienza
    EJECUTANDO, // el proceso está ejecutando la actividad
    TERMINADA, // terminó correctamente
    FALLIDA, // error
    CANCELADA // se cancelo
};

struct Actividad {
    std::string id;
    std::string nombre;
    int tiempo;

    std::vector<std::string> dependencias;
    std::vector<std::string> dependientes;

    Estado estado;
};

#endif
