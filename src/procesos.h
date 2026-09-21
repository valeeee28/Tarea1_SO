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

// Espera a que termine un proceso hijo
int esperarProceso(pid_t pid);
// Crea una tuberia
// tuberia[0] = lectura
// tuberia[1] = escritura
int crearPipe(int tuberia[2]);

// Envia un mensaje por la tuberia
int enviarMensaje(int fdEscritura, const std::string& mensaje);

// Recibe un mensaje desde la tuberia
std::string recibirMensaje(int fdLectura);

// Espera al proceso hijo y recibe su mensaje
int esperarYRecibir(
    pid_t pid,
    int fdLectura,
    std::string& mensaje
);

// Configura el manejo de Ctrl+C (SIGINT)
void configurarSIGINT();

// Registra un proceso hijo como activo
void registrarProceso(pid_t pid);

// Elimina un proceso de la lista de activos
void eliminarProceso(pid_t pid);

// Marca una actividad como fallida y cancela toda su rama dependiente
void cancelarRamaPorFallo(
    std::vector<Actividad>& actividades,
    const std::string& idFallida
);

// Espera a cualquiera de los procesos hijos activos.
// Devuelve el PID del proceso que termino.
// codigoSalida indica si termino correctamente o con error.
pid_t esperarCualquierProceso(int& codigoSalida);

// Busca un proceso por su PID.
// Devuelve su posicion dentro del vector o -1 si no existe.
int buscarProcesoPorPid(
    const std::vector<ProcesoActividad>& procesos,
    pid_t pid
);

// Crea el proceso de una actividad y lo registra
// dentro de la lista de procesos en ejecucion.
pid_t lanzarActividad(
    const Actividad& actividad,
    std::vector<ProcesoActividad>& procesos
);

// Espera al primer proceso que termine, identifica su actividad
// y recibe el mensaje enviado por el hijo.
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