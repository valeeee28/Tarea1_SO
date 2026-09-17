#include "procesos.h"

#include <iostream>
#include <unistd.h>     // fork(), usleep(), _exit()
#include <sys/types.h>
#include <sys/wait.h>

pid_t crearProceso(const Actividad& actividad, int tuberia[2]) {

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error al crear el proceso para: "
                  << actividad.nombre << std::endl;

        return -1;
    }

    if (pid == 0) {
        // PROCESO HIJO

        // El hijo no necesita leer del pipe
        close(tuberia[0]);

        std::cout << "Iniciando actividad: "
                  << actividad.nombre << std::endl;

        // Simula la duracion de la actividad
        usleep(actividad.tiempo * 1000);

        std::cout << "Actividad terminada: "
                  << actividad.nombre << std::endl;

        // El hijo avisa que termino
        std::string mensaje = "TERMINADA:" + actividad.id;

        enviarMensaje(tuberia[1], mensaje);

        // Ya no necesita escribir
        close(tuberia[1]);

        _exit(0);
    }

    // PROCESO PADRE

    // El padre no escribe en este pipe
    close(tuberia[1]);

    return pid;
}

int esperarProceso(pid_t pid) {

    int estado;

    pid_t resultado = waitpid(pid, &estado, 0);

    if (resultado == -1) {
        std::cerr << "Error al esperar el proceso." << std::endl;
        return -1;
    }

    if (WIFEXITED(estado)) {
        return WEXITSTATUS(estado);
    }

    return -1;
}
int crearPipe(int tuberia[2]) {

    if (pipe(tuberia) == -1) {
        std::cerr << "Error al crear la tuberia." << std::endl;
        return -1;
    }

    return 0;
}
int enviarMensaje(int fdEscritura, const std::string& mensaje) {

    ssize_t bytesEscritos = write(
        fdEscritura,
        mensaje.c_str(),
        mensaje.size()
    );

    if (bytesEscritos == -1) {
        std::cerr << "Error al enviar mensaje por la tuberia." << std::endl;
        return -1;
    }

    return 0;
}

std::string recibirMensaje(int fdLectura) {

    char buffer[256];

    ssize_t bytesLeidos = read(
        fdLectura,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytesLeidos == -1) {
        std::cerr << "Error al recibir mensaje por la tuberia." << std::endl;
        return "";
    }

    if (bytesLeidos == 0) {
        return "";
    }

    buffer[bytesLeidos] = '\0';

    return std::string(buffer);
}

int esperarYRecibir(
    pid_t pid,
    int fdLectura,
    std::string& mensaje
) {

    // El padre recibe el mensaje enviado por el hijo
    mensaje = recibirMensaje(fdLectura);

    // Ya no necesitamos seguir leyendo de este pipe
    close(fdLectura);

    // Esperamos a que termine el proceso hijo
    int resultado = esperarProceso(pid);

    return resultado;
}
