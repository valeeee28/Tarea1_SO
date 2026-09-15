#include "parser.h"
#include <fstream>
#include <iostream>

std::vector<Actividad> cargarPlan(const std::string& nombreArchivo) {  
    std::vector<Actividad> actividades;

    std::ifstream archivo(nombreArchivo);// abre el archivo recibido

    if (!archivo.is_open()) { // si el archivo no esta abierto...
        std::cerr << "Error: no se pudo abrir el archivo "
                  << nombreArchivo << std::endl;
        return actividades; // devuelve el vector vacio
    }

    std::string linea;

    while (std::getline(archivo, linea)) { // lee una linea del archivo y la guarda
        std::cout << linea << std::endl;
    }

    archivo.close();

    return actividades;
}
