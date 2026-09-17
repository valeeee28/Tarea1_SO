#include "procesos.h"

#include <iostream>
#include <unistd.h>     // fork(), usleep(), _exit()
#include <sys/types.h>
#include <sys/wait.h>

pid_t crearProceso(const Actividad& actividad) {

pid_t pid = fork();//divide el proceso en dos

    if (pid < 0) {
        // Error al crear el proceso
        std::cerr << "Error al crear el proceso para: "
                  << actividad.nombre << std::endl;

        return -1;
    }

    if (pid == 0) {
        // Este bloque lo ejecuta solamente el proceso hijo

        std::cout << "Iniciando actividad: "
                  << actividad.nombre << std::endl;

        // El tiempo de la actividad viene en milisegundos.
        // usleep trabaja en microsegundos.
        usleep(actividad.tiempo * 1000);

        std::cout << "Actividad terminada: "
                  << actividad.nombre << std::endl;

        _exit(0);
    }

    // Este código lo ejecuta el proceso padre.
    // Devuelve el PID del hijo que acaba de crear.
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
