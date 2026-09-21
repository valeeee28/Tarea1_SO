#include "parser.h"
#include "dag.h"
#include "planificador.h"
#include "procesos.h"

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {

    // =========================================
    // VALIDAR ARGUMENTOS
    // =========================================

    if (argc != 3) {
        std::cerr
            << "Uso: ./planificador plan.txt K"
            << std::endl;

        return 1;
    }

    std::string nombreArchivo = argv[1];
    int K = std::stoi(argv[2]);

    if (K <= 0) {
        std::cerr
            << "Error: K debe ser mayor que 0."
            << std::endl;

        return 1;
    }

    // =========================================
    // ACTIVAR CTRL+C / SIGINT
    // =========================================

    configurarSIGINT();

    std::cout
        << "Archivo: "
        << nombreArchivo
        << std::endl;

    std::cout
        << "Limite de concurrencia K: "
        << K
        << std::endl;


    // =========================================
    // CARGAR PLAN
    // =========================================

    std::vector<Actividad> actividades =
        cargarPlan(nombreArchivo);

    construirDependientes(actividades);


    // =========================================
    // VALIDAR DAG
    // =========================================

    if (!validarDAG(actividades)) {

        std::cerr
            << "Error: el plan contiene un ciclo."
            << std::endl;

        return 1;
    }

    std::cout
        << "DAG valido"
        << std::endl;

    std::cout
        << "Cantidad de actividades cargadas: "
        << actividades.size()
        << std::endl;


    // =========================================
    // PROCESOS ACTUALMENTE EJECUTANDOSE
    // =========================================

    std::vector<ProcesoActividad> procesosEnEjecucion;


    // =========================================
    // CICLO PRINCIPAL DEL PLANIFICADOR
    // =========================================

    while (true) {

        // -------------------------------------
        // BUSCAR ACTIVIDADES QUE PUEDEN INICIAR
        // -------------------------------------

        std::vector<std::string> listas =
            obtenerActividadesListas(
                actividades,
                K
            );


        // -------------------------------------
        // MARCARLAS COMO EJECUTANDO
        // -------------------------------------

        if (!listas.empty()) {

            iniciarActividades(
                actividades,
                listas
            );


            // ---------------------------------
            // CREAR LOS PROCESOS HIJOS
            // ---------------------------------

            for (const auto& id : listas) {

                for (const auto& actividad : actividades) {

                    if (actividad.id == id) {

                        pid_t pid =
                            lanzarActividad(
                                actividad,
                                procesosEnEjecucion
                            );

                        if (pid == -1) {

                            std::cerr
                                << "Error al lanzar actividad "
                                << id
                                << std::endl;

                            // Si ni siquiera puede crearse
                            // el proceso, se considera fallo.
                            cancelarRamaPorFallo(
                                actividades,
                                id
                            );
                        }

                        break;
                    }
                }
            }
        }


        // =====================================
        // SI NO QUEDA NINGUN PROCESO ACTIVO
        // =====================================

        if (procesosEnEjecucion.empty()) {

            /*
             * Si no habia actividades listas,
             * significa que la planificacion termino.
             *
             * Si habia listas pero todas fallaron
             * al crear sus procesos, repetimos el
             * ciclo para buscar otras ramas.
             */

            if (listas.empty()) {
                break;
            }

            continue;
        }


        // =====================================
        // ESPERAR AL PRIMER HIJO QUE TERMINE
        // =====================================

        std::string idFinalizada;
        std::string mensaje;

        int codigoSalida = -1;

        bool recibido =
            recogerProcesoTerminado(
                procesosEnEjecucion,
                idFinalizada,
                mensaje,
                codigoSalida
            );

        if (!recibido) {

            std::cerr
                << "Error al recoger un proceso terminado."
                << std::endl;

            break;
        }


        // =====================================
        // MOSTRAR RESULTADO
        // =====================================

        std::cout
            << "\nActividad finalizada: "
            << idFinalizada
            << std::endl;

        std::cout
            << "Mensaje recibido: "
            << mensaje
            << std::endl;


        // =====================================
        // TERMINO CORRECTAMENTE
        // =====================================

        if (codigoSalida == 0) {

            std::vector<std::string> terminadas = {
                idFinalizada
            };

            /*
             * Esta funcion de Persona 1:
             *
             * - marca la actividad TERMINADA
             * - disminuye dependenciasRestantes
             *   de sus actividades dependientes
             */

            terminarActividades(
                actividades,
                terminadas
            );

            std::cout
                << "Estado: TERMINADA"
                << std::endl;
        }


        // =====================================
        // EL PROCESO FALLO
        // =====================================

        else {

            /*
             * Esta funcion de Persona 2:
             *
             * - actividad original = FALLIDA
             * - descendientes = CANCELADA
             * - otras ramas siguen funcionando
             */

            cancelarRamaPorFallo(
                actividades,
                idFinalizada
            );

            std::cout
                << "Estado: FALLIDA"
                << std::endl;

            std::cout
                << "Se cancelo solamente su rama dependiente."
                << std::endl;
        }

        /*
         * Volvemos arriba.
         *
         * obtenerActividadesListas() revisara:
         *
         * - dependenciasRestantes
         * - estado PENDIENTE
         * - cantidad actualmente EJECUTANDO
         * - limite K
         *
         * y lanzara las siguientes actividades.
         */
    }


    // =========================================
    // RESULTADO FINAL
    // =========================================

    std::cout
        << "\nPlanificacion finalizada."
        << std::endl;


    // Opcional: mostrar estado final de cada actividad

    for (const auto& actividad : actividades) {

        std::cout
            << "Actividad "
            << actividad.id
            << ": ";

        if (actividad.estado == Estado::PENDIENTE) {
            std::cout << "PENDIENTE";
        }

        else if (actividad.estado == Estado::EJECUTANDO) {
            std::cout << "EJECUTANDO";
        }

        else if (actividad.estado == Estado::TERMINADA) {
            std::cout << "TERMINADA";
        }

        else if (actividad.estado == Estado::FALLIDA) {
            std::cout << "FALLIDA";
        }

        else if (actividad.estado == Estado::CANCELADA) {
            std::cout << "CANCELADA";
        }

        std::cout << std::endl;
    }


    return 0;
}