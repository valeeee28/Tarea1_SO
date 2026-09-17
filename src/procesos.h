#ifndef PROCESOS_H
#define PROCESOS_H

#include "actividad.h"
#include <sys/types.h>
#include <string>

// Crea un proceso hijo para ejecutar una actividad
pid_t crearProceso(const Actividad& actividad);

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

#endif