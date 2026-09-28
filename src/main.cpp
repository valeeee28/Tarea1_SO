#include "parser.h"
#include "dag.h"
#include "planificador.h"
#include "procesos.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[]) {

   
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

   

    configurarSIGINT();

    std::cout
        << "Archivo: "
        << nombreArchivo
        << std::endl;

    std::cout
        << "Limite de concurrencia K: "
        << K
        << std::endl;


    

    std::vector<Actividad> actividades =
        cargarPlan(nombreArchivo);

    construirDependientes(actividades);


    

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


    

    std::vector<ProcesoActividad> procesosEnEjecucion;

std::unordered_map<
    std::string,
    std::vector<std::string>
           >insumosPendientes;
    

    while (true) {

        

        std::vector<std::string> listas =
            obtenerActividadesListas(
                actividades,
                K
            );


      

        if (!listas.empty()) {

            iniciarActividades(
                actividades,
                listas
            );


           
            for (const auto& id : listas) {

                for (const auto& actividad : actividades) {

                    if (actividad.id == id) {

                        std::vector<std::string> insumos =
                          insumosPendientes[id];

                            pid_t pid = 
                         lanzarActividadConInsumos(
                    actividad,
                    procesosEnEjecucion,
                     insumos
    );

        if (pid != -1) {
            insumosPendientes.erase(id);
        }
                        if (pid == -1) {

                            std::cerr
                                << "Error al lanzar actividad "
                                << id
                                << std::endl;

                            
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


        

        if (procesosEnEjecucion.empty()) {

            

            if (listas.empty()) {
                break;
            }

            continue;
        }


        
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



        std::cout
            << "\nActividad finalizada: "
            << idFinalizada
            << std::endl;

        std::cout
            << "Mensaje recibido: "
            << mensaje
            << std::endl;



        if (codigoSalida == 0) {
           
            for (const auto& actividad : actividades) {

                if (actividad.id == idFinalizada) {

                    for (const auto& idDependiente : actividad.dependientes) {

                        insumosPendientes[
                            idDependiente
                        ].push_back(mensaje);
                    }

                 break;
             }
            }

            std::vector<std::string> terminadas = {
                idFinalizada
            };

            
            terminarActividades(
                actividades,
                terminadas
            );

            std::cout
                << "Estado: TERMINADA"
                << std::endl;
        }


       

        else {

           

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

       
    }



    std::cout
        << "\nPlanificacion finalizada."
        << std::endl;


    

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