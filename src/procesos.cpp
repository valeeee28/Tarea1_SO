#include "procesos.h"

#include <iostream>
#include <unistd.h>     // fork(), usleep(), _exit()
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Lista de procesos hijos que se encuentran activos
static const int MAX_PROCESOS_ACTIVOS = 10000;

static pid_t procesosActivos[MAX_PROCESOS_ACTIVOS];
static int cantidadProcesosActivos = 0;

pid_t crearProceso(const Actividad& actividad, int tuberia[2]) {

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error al crear el proceso para: "
                  << actividad.nombre << std::endl;

        return -1;
    }

    if (pid == 0) {
        // PROCESO HIJO
        // El hijo usa el comportamiento normal de SIGINT
        signal(SIGINT, SIG_DFL);

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

    registrarProceso(pid);


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

    eliminarProceso(pid);


    return resultado;
}

void registrarProceso(pid_t pid) {

    if (cantidadProcesosActivos < MAX_PROCESOS_ACTIVOS) {
        procesosActivos[cantidadProcesosActivos] = pid;
        cantidadProcesosActivos++;
    }
}

void eliminarProceso(pid_t pid) {

    for (int i = 0; i < cantidadProcesosActivos; i++) {

        if (procesosActivos[i] == pid) {

            procesosActivos[i] =
                procesosActivos[cantidadProcesosActivos - 1];

            cantidadProcesosActivos--;

            return;
        }
    }
}

static void manejarSIGINT(int) {

    // Terminar todos los procesos hijos que siguen activos
    for (int i = 0; i < cantidadProcesosActivos; i++) {
        kill(procesosActivos[i], SIGTERM);
    }

    // Mensaje simple y seguro dentro de una señal
    const char mensaje[] =
        "\nSIGINT recibido. Cancelando todas las actividades...\n";

    write(STDERR_FILENO, mensaje, sizeof(mensaje) - 1);

    // Termina inmediatamente el proceso padre
    _exit(130);
}

void configurarSIGINT() {

    struct sigaction accion{};

    accion.sa_handler = manejarSIGINT;

    sigemptyset(&accion.sa_mask);

    accion.sa_flags = 0;

    sigaction(SIGINT, &accion, nullptr);
}