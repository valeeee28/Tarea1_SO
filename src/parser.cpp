#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

std::string quitarEspacios(const std::string& texto) {
    std::string resultado = texto;

    while (!resultado.empty() && resultado.front() == ' ') {
        resultado.erase(0, 1);
    }

    while (!resultado.empty() && resultado.back() == ' ') {
        resultado.pop_back();
    }

    return resultado;
}

std::vector<Actividad> cargarPlan(const std::string& nombreArchivo) {
    std::vector<Actividad> actividades;

    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo "
                  << nombreArchivo << std::endl;
        return actividades;
    }

    std::string linea;

    while (std::getline(archivo, linea)) {
     
        if(quitarEspacios(linea).empty()) {
            continue;
            }
        std::stringstream ss(linea);

        std::string id;
        std::getline(ss, id, ':');

        std::string nombre;
        std::getline(ss, nombre, ':');

        std::string tiempoTexto;
        std::getline(ss, tiempoTexto, ':');

        std::string dependenciasTexto;
        std::getline(ss, dependenciasTexto);

        Actividad actividad;

        actividad.id = quitarEspacios(id);
        actividad.nombre = quitarEspacios(nombre);

        tiempoTexto = quitarEspacios(tiempoTexto);

        if (tiempoTexto.empty()) {
            actividad.tiempo = 100 + std::rand() % 4901;
        } else {
            actividad.tiempo = std::stoi(tiempoTexto);
        }

        actividad.estado = Estado::PENDIENTE;

        std::stringstream ssDependencias(dependenciasTexto);
        std::string dependencia;

        while (std::getline(ssDependencias, dependencia, ',')) {
            dependencia = quitarEspacios(dependencia);

            if (!dependencia.empty()) {
                actividad.dependencias.push_back(dependencia);
            }
        }

        actividades.push_back(actividad);

        std::cout << "ID: " << actividad.id << std::endl;
        std::cout << "Nombre: " << actividad.nombre << std::endl;
        std::cout << "Tiempo: " << actividad.tiempo << std::endl;
        std::cout << "Dependencias: " << dependenciasTexto << std::endl;
    }

    archivo.close();

    return actividades;
}