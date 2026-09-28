#include "procesos.h"

#include <iostream>
#include <unistd.h>     
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unordered_map>
#include <cerrno>

static const int MAX_PROCESOS_ACTIVOS = 10000;

static pid_t procesosActivos[MAX_PROCESOS_ACTIVOS];
static int cantidadProcesosActivos = 0;
static std::string recibirHastaCierre(int fdLectura) {

    char buffer[256];
    std::string resultado;

    while (true) {

        ssize_t bytesLeidos = read(
            fdLectura,
            buffer,
            sizeof(buffer)
        );

        if (bytesLeidos > 0) {

            resultado.append(
                buffer,
                static_cast<size_t>(bytesLeidos)
            );

            continue;
        }

        if (bytesLeidos == 0) {
            break;
        }

        if (errno == EINTR) {
            continue;
        }

        std::cerr
            << "Error al recibir insumos por la tuberia."
            << std::endl;

        break;
    }

    return resultado;
}
pid_t crearProceso(const Actividad& actividad, int tuberia[2]) {

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error al crear el proceso para: "
                  << actividad.nombre << std::endl;

        return -1;
    }

    if (pid == 0) {
        
        signal(SIGINT, SIG_DFL);

        
        close(tuberia[0]);

        std::cout << "Iniciando actividad: "
                  << actividad.nombre << std::endl;

        
        usleep(actividad.tiempo * 1000);

        std::cout << "Actividad terminada: "
                  << actividad.nombre << std::endl;

       std::string mensaje = "TERMINADA:" + actividad.id;

        if (enviarMensaje(tuberia[1], mensaje) == -1) {

        close(tuberia[1]);

        
         _exit(1);
        }

        
        close(tuberia[1]);

        
        _exit(0);
    }

    
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

    
    mensaje = recibirMensaje(fdLectura);

    
    close(fdLectura);

    
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

    
    for (int i = 0; i < cantidadProcesosActivos; i++) {
        kill(procesosActivos[i], SIGTERM);
    }

    
    const char mensaje[] =
        "\nSIGINT recibido. Cancelando todas las actividades...\n";

    write(STDERR_FILENO, mensaje, sizeof(mensaje) - 1);

    
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

    
    std::unordered_map<std::string, size_t> indice;

    indice.reserve(actividades.size());

    for (size_t i = 0; i < actividades.size(); i++) {
        indice[actividades[i].id] = i;
    }

    
    auto itFallida = indice.find(idFallida);

    if (itFallida == indice.end()) {
        std::cerr << "No se encontro la actividad fallida: "
                  << idFallida << std::endl;
        return;
    }

    
    Actividad& fallida = actividades[itFallida->second];

    fallida.estado = Estado::FALLIDA;

    
    std::vector<size_t> pendientes;

    for (const auto& idDependiente : fallida.dependientes) {

        auto it = indice.find(idDependiente);

        if (it != indice.end()) {
            pendientes.push_back(it->second);
        }
    }

    
    while (!pendientes.empty()) {

        size_t posicion = pendientes.back();
        pendientes.pop_back();

        Actividad& actividad = actividades[posicion];

        
        if (actividad.estado == Estado::CANCELADA ||
            actividad.estado == Estado::FALLIDA ||
            actividad.estado == Estado::TERMINADA) {

            continue;
        }

        actividad.estado = Estado::CANCELADA;

        
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

    
    pid_t pid = waitpid(-1, &estado, 0);

    if (pid == -1) {
        std::cerr << "Error al esperar un proceso hijo." << std::endl;
        codigoSalida = -1;
        return -1;
    }

    
    eliminarProceso(pid);

    if (WIFEXITED(estado)) {
        codigoSalida = WEXITSTATUS(estado);
    }

    
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

    
    if (crearPipe(tuberia) == -1) {
        return -1;
    }

    
    pid_t pid = crearProceso(actividad, tuberia);

    if (pid == -1) {
        close(tuberia[0]);
        close(tuberia[1]);
        return -1;
    }

    
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

    
    pid_t pidTerminado = esperarCualquierProceso(codigoSalida);

    if (pidTerminado == -1) {
        return false;
    }

    
    int posicion = buscarProcesoPorPid(procesos, pidTerminado);

    if (posicion == -1) {
        std::cerr << "No se encontro el proceso terminado."
                  << std::endl;

        return false;
    }

    
    idActividad = procesos[posicion].idActividad;

    
    mensaje = recibirMensaje(
        procesos[posicion].fdLectura
    );

    
    close(procesos[posicion].fdLectura);

    
    procesos.erase(
        procesos.begin() + posicion
    );

    return true;
}
pid_t lanzarActividadConInsumos(
    const Actividad& actividad,
    std::vector<ProcesoActividad>& procesos,
    const std::vector<std::string>& insumos
) {

    
    int pipeResultado[2];

    
    int pipeEntrada[2];

    if (crearPipe(pipeResultado) == -1) {
        return -1;
    }

    if (crearPipe(pipeEntrada) == -1) {

        close(pipeResultado[0]);
        close(pipeResultado[1]);

        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {

        std::cerr
            << "Error al crear el proceso para: "
            << actividad.nombre
            << std::endl;

        close(pipeResultado[0]);
        close(pipeResultado[1]);

        close(pipeEntrada[0]);
        close(pipeEntrada[1]);

        return -1;
    }

    

    if (pid == 0) {

        signal(SIGINT, SIG_DFL);

        
        close(pipeResultado[0]);

        
        close(pipeEntrada[1]);

        std::string mensajesRecibidos =
            recibirHastaCierre(pipeEntrada[0]);

        close(pipeEntrada[0]);

        if (!mensajesRecibidos.empty()) {

            std::cout
                << "Actividad "
                << actividad.id
                << " recibio insumos:"
                << std::endl
                << mensajesRecibidos;
        }

        std::cout
            << "Iniciando actividad: "
            << actividad.nombre
            << std::endl;

        usleep(actividad.tiempo * 1000);

        std::cout
            << "Actividad terminada: "
            << actividad.nombre
            << std::endl;

        std::string mensaje =
            "TERMINADA:" + actividad.id;

        if (
            enviarMensaje(
                pipeResultado[1],
                mensaje
            ) == -1
        ) {

            close(pipeResultado[1]);
            _exit(1);
        }

        close(pipeResultado[1]);

        _exit(0);
    }

    

    
    close(pipeResultado[1]);

    
    close(pipeEntrada[0]);

    registrarProceso(pid);

    
    for (const auto& insumo : insumos) {

        std::string mensajeAcotado =
            insumo.substr(0, 250);

        mensajeAcotado += "\n";

        if (
            enviarMensaje(
                pipeEntrada[1],
                mensajeAcotado
            ) == -1
        ) {

            std::cerr
                << "Error al enviar insumos a la actividad "
                << actividad.id
                << std::endl;

            close(pipeEntrada[1]);
            close(pipeResultado[0]);

            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);

            eliminarProceso(pid);

            return -1;
        }
    }

    
    close(pipeEntrada[1]);

    ProcesoActividad proceso;

    proceso.pid = pid;
    proceso.idActividad = actividad.id;
    proceso.fdLectura = pipeResultado[0];

    procesos.push_back(proceso);

    return pid;
}
