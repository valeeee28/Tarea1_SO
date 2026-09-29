# Tarea 1 - Sistemas Operativos

Integrantes
- Valentina Araya
- Valentina Díaz


## Introducción

En esta tarea se desarrolló un simulador y planificador de actividades en C++, utilizando procesos, pipes y señales en Linux.

Las actividades se representan mediante un Grafo Acíclico Dirigido (DAG), donde una actividad puede depender de otras y solo puede comenzar cuando todas sus dependencias hayan terminado correctamente.

Además, el programa recibe un valor `K`, que corresponde a la cantidad máxima de actividades que pueden ejecutarse al mismo tiempo.


### Archivo de planificación

El programa lee un archivo de texto con el siguiente formato:

ID : nombre : tiempo_ms : dependencias
Ejemplo:```text
1 : prender_carbon : 500 :
2 : comprar_carne : 1200 :
3 : comprar_pan : 300 :
4 : asar_longaniza : 800 : 1, 2
5 : armar_choripan : 250 : 3, 4
6 : servir_mesa : 100 : 5
```

Si una actividad no tiene tiempo definido, se le asigna un tiempo entre 100 y 5000 milisegundos.

También se ignoran las líneas vacías del archivo para evitar que sean interpretadas como actividades.


### Modelado del DAG

Cada actividad almacena su ID, nombre, duración, dependencias, dependientes y estado.

Los estados utilizados son:

PENDIENTE
EJECUTANDO
TERMINADA
FALLIDA
CANCELADA

El programa construye las relaciones entre las actividades y valida que la planificación no contenga ciclos.

La construcción del DAG funciona independientemente del orden en que las actividades aparezcan en el archivo.


### Creación de procesos

Cada actividad se ejecuta mediante un proceso hijo creado utilizando fork().

El proceso principal funciona como planificador y se encarga de decidir qué actividades pueden comenzar, controlar la cantidad de procesos activos y recibir los resultados de las actividades.


### Control de concurrencia

El parámetro K define el máximo de procesos que pueden ejecutarse al mismo tiempo.

Por ejemplo:

./planificador plan.txt 2

permite ejecutar como máximo dos actividades simultáneamente.

Para esperar la finalización de los procesos se utiliza  waitpid(), evitando el uso de busy waiting.


### Comunicación con pipes

Cuando una actividad termina, envía mediante un pipe un mensaje indicando su finalización.

Por ejemplo:

TERMINADA:1

El proceso principal recibe este mensaje y lo guarda como insumo para las actividades que dependan de ella.

Cuando una actividad dependiente comienza, recibe mediante otro pipe los mensajes generados por sus actividades anteriores.

Por ejemplo:

Actividad 4 recibio insumos:
TERMINADA:1
TERMINADA:2


### Fallos

Si una actividad falla, el programa no termina completamente.

La actividad que presenta el problema queda en estado FALLIDA y solamente se cancelan las actividades que dependen de esa rama, quedando en estado CANCELADA.

Las ramas independientes pueden continuar ejecutándose normalmente.


### Ctrl+C

El programa maneja la señal SIGINT.

Si el usuario presiona:

Ctrl + C

el proceso principal termina los procesos hijos que continúan activos y finaliza la planificación.


### Carga de estrés

Durante el desarrollo se realizaron pruebas con planificaciones de hasta 10000 actividades para comprobar el funcionamiento del programa bajo una carga mayor.


## Funciones principales del proyecto

- actividad.h : contiene la estructura y los estados de las actividades.
- parser.cpp / parser.h: realizan la lectura del archivo de planificación.
- dag.cpp / dag.h: construyen y validan el DAG.
- planificador.cpp / planificador.h: controlan las dependencias y la concurrencia.
- procesos.cpp / procesos.h : manejan procesos, pipes, señales y fallos.

La carpeta tests/ contiene las pruebas utilizadas durante el desarrollo para verificar procesos, pipes, concurrencia, fallos, SIGINT y carga de estrés.


## Compilación

Desde la carpeta principal del proyecto se puede compilar utilizando:


g++ -Wall -Wextra -std=c++17 src/main.cpp src/parser.cpp src/dag.cpp src/planificador.cpp src/procesos.cpp -o planificador -lpthread

Aunque la compilación incluye -lpthread, el programa no utiliza threads ni mecanismos de sincronización de hilos.


## Ejecución

La forma general de ejecutar el programa es:

./planificador archivo.txt K

Por ejemplo:

./planificador plan.txt 2
- plan.txt, es el archivo que contiene las actividades.
- 2, es el máximo de procesos que pueden ejecutarse simultáneamente.



## Decisiones de diseño

Se decidió mantener al proceso padre como planificador central.

Los procesos hijos se encargan de simular la ejecución de cada actividad, mientras que el proceso padre administra la concurrencia, recibe los mensajes, actualiza las dependencias y controla los posibles errores.

Los pipes se utilizan para la comunicación entre los procesos y para entregar los mensajes generados por una actividad a sus dependientes.

También se utiliza waitpid() para esperar la finalización de procesos de manera bloqueante, evitando realizar revisiones constantes y evitando busy waiting.

Esta organización permitió separar la lógica del DAG, la planificación y el manejo de procesos.

