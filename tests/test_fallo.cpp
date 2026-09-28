#include "../src/procesos.h"

#include <iostream>
#include <unistd.h>

int main() {

    std::cout << "Iniciando prueba de fallo..." << std::endl;

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "No se pudo crear el proceso de prueba." << std::endl;
        return 1;
    }

    if (pid == 0) {
        
        std::cout << "La actividad de prueba ha fallado." << std::endl;

        _exit(1);
    }

    
    int resultado = esperarProceso(pid);

    if (resultado == 0) {
        std::cout << "La actividad termino correctamente." << std::endl;
    } else {
        std::cout << "Fallo detectado. Codigo: "
                  << resultado << std::endl;

        std::cout << "El planificador sigue funcionando."
                  << std::endl;
    }

    return 0;
}