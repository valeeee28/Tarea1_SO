#include "procesos.h"

#include <iostream>
#include <unistd.h>     // fork(), usleep(), _exit()
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unordered_map>

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

       std::string mensaje = "TERMINADA:" + actividad.id;

        if (enviarMensaje(tuberia[1], mensaje) == -1) {

        close(tuberia[1]);

        // El proceso termina indicando que ocurrió un error
         _exit(1);
        }

        // Ya no necesita escribir
        close(tuberia[1]);

        // 0 significa que la actividad terminó correctamente
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


void cancelarRamaPorFallo(
    std::vector<Actividad>& actividades,
    const std::string& idFallida
) {

    // Relaciona cada ID con su posicion dentro del vector.
    // Esto permite buscar rapidamente incluso con muchas actividades.
    std::unordered_map<std::string, size_t> indice;

    indice.reserve(actividades.size());

    for (size_t i = 0; i < actividades.size(); i++) {
        indice[actividades[i].id] = i;
    }

    // Buscar la actividad que fallo
    auto itFallida = indice.find(idFallida);

    if (itFallida == indice.end()) {
        std::cerr << "No se encontro la actividad fallida: "
                  << idFallida << std::endl;
        return;
    }

    // Marcar solamente la actividad original como FALLIDA
    Actividad& fallida = actividades[itFallida->second];

    fallida.estado = Estado::FALLIDA;

    // Pila para recorrer todos los dependientes de la rama
    std::vector<size_t> pendientes;

    for (const auto& idDependiente : fallida.dependientes) {

        auto it = indice.find(idDependiente);

        if (it != indice.end()) {
            pendientes.push_back(it->second);
        }
    }

    // Cancelar toda la rama descendiente
    while (!pendientes.empty()) {

        size_t posicion = pendientes.back();
        pendientes.pop_back();

        Actividad& actividad = actividades[posicion];

        // Evitar procesar dos veces una actividad
        if (actividad.estado == Estado::CANCELADA ||
            actividad.estado == Estado::FALLIDA ||
            actividad.estado == Estado::TERMINADA) {

            continue;
        }

        actividad.estado = Estado::CANCELADA;

        // Agregar también sus dependientes
        for (const auto& idDependiente : actividad.dependientes) {

            auto it = indice.find(idDependiente);

            if (it != indice.end()) {
                pendientes.push_back(it->second);
            }
        }
    }
}

pid_t esperarCualquierProceso(int& codigoSalida) {

    int estado;

    // -1 significa: esperar al primer hijo que termine
    pid_t pid = waitpid(-1, &estado, 0);

    if (pid == -1) {
        std::cerr << "Error al esperar un proceso hijo." << std::endl;
        codigoSalida = -1;
        return -1;
    }

    // Ya termino, por lo tanto deja de estar activo
    eliminarProceso(pid);

    // Termino normalmente
    if (WIFEXITED(estado)) {
        codigoSalida = WEXITSTATUS(estado);
    }

    // Termino debido a una señal
    else if (WIFSIGNALED(estado)) {
        codigoSalida = 128 + WTERMSIG(estado);
    }

    else {
        codigoSalida = -1;
    }

    return pid;
}

int buscarProcesoPorPid(
    const std::vector<ProcesoActividad>& procesos,
    pid_t pid
) {

    for (size_t i = 0; i < procesos.size(); i++) {

        if (procesos[i].pid == pid) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

pid_t lanzarActividad(
    const Actividad& actividad,
    std::vector<ProcesoActividad>& procesos
) {

    int tuberia[2];

    // Crear pipe para esta actividad
    if (crearPipe(tuberia) == -1) {
        return -1;
    }

    // Crear proceso hijo
    pid_t pid = crearProceso(actividad, tuberia);

    if (pid == -1) {
        close(tuberia[0]);
        close(tuberia[1]);
        return -1;
    }

    // Guardar relacion entre PID, actividad y pipe
    ProcesoActividad proceso;

    proceso.pid = pid;
    proceso.idActividad = actividad.id;
    proceso.fdLectura = tuberia[0];

    procesos.push_back(proceso);

    return pid;
}

bool recogerProcesoTerminado(
    std::vector<ProcesoActividad>& procesos,
    std::string& idActividad,
    std::string& mensaje,
    int& codigoSalida
) {

    if (procesos.empty()) {
        return false;
    }

    // Esperar al primer hijo que termine
    pid_t pidTerminado = esperarCualquierProceso(codigoSalida);

    if (pidTerminado == -1) {
        return false;
    }

    // Averiguar a que actividad pertenecia ese PID
    int posicion = buscarProcesoPorPid(procesos, pidTerminado);

    if (posicion == -1) {
        std::cerr << "No se encontro el proceso terminado."
                  << std::endl;

        return false;
    }

    // Obtener el ID de la actividad
    idActividad = procesos[posicion].idActividad;

    // Leer el mensaje que dejo el hijo en su pipe
    mensaje = recibirMensaje(
        procesos[posicion].fdLectura
    );

    // Cerrar el extremo de lectura
    close(procesos[posicion].fdLectura);

    // Sacar este proceso de la lista de procesos en ejecucion
    procesos.erase(
        procesos.begin() + posicion
    );

    return true;
}