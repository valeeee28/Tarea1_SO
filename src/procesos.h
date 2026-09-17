#ifndef PROCESOS_H
#define PROCESOS_H

#include "actividad.h"
#include <sys/types.h>
#include <string>


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


#endif