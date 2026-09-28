#ifndef PROCESOS_H
#define PROCESOS_H

#include "actividad.h"
#include <sys/types.h>
#include <string>
#include <vector>

struct ProcesoActividad {
    pid_t pid;
    std::string idActividad;
    int fdLectura;
};


pid_t crearProceso(const Actividad& actividad, int tuberia[2]);


int esperarProceso(pid_t pid);

int crearPipe(int tuberia[2]);


int enviarMensaje(int fdEscritura, const std::string& mensaje);


std::string recibirMensaje(int fdLectura);


int esperarYRecibir(
    pid_t pid,
    int fdLectura,
    std::string& mensaje
);


void configurarSIGINT();


void registrarProceso(pid_t pid);


void eliminarProceso(pid_t pid);


void cancelarRamaPorFallo(
    std::vector<Actividad>& actividades,
    const std::string& idFallida
);


pid_t esperarCualquierProceso(int& codigoSalida);


int buscarProcesoPorPid(
    const std::vector<ProcesoActividad>& procesos,
    pid_t pid
);


pid_t lanzarActividad(
    const Actividad& actividad,
    std::vector<ProcesoActividad>& procesos
);


bool recogerProcesoTerminado(
    std::vector<ProcesoActividad>& procesos,
    std::string& idActividad,
    std::string& mensaje,
    int& codigoSalida
);
pid_t lanzarActividadConInsumos(
    const Actividad& actividad,
    std::vector<ProcesoActividad>& procesos,
    const std::vector<std::string>& insumos
);
#endif