#ifndef COLA_FIBONACCI_H
#define COLA_FIBONACCI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/**
 * @brief Estructura que representa un nodo dentro de una Cola de Fibonacci.
 */
typedef struct NodoFibonacci {
    int id_vertice;                   // ID del vértice representado (0 a V-1)
    double clave;                     // Clave/costo del vértice
    int grado;                        // Cantidad de hijos directos
    bool marcado;                     // Indica si el nodo ha perdido un hijo desde que fue raíz
    struct NodoFibonacci* padre;     // Puntero al padre del nodo
    struct NodoFibonacci* hijo;      // Puntero a alguno de sus hijos
    struct NodoFibonacci* izq;       // Puntero al hermano izquierdo (lista circular)
    struct NodoFibonacci* der;       // Puntero al hermano derecho (lista circular)
} NodoFibonacci;

/**
 * @brief Estructura principal de la Cola de Fibonacci.
 */
typedef struct ColaFibonacci {
    NodoFibonacci* min;               // Puntero a la raíz con la clave mínima
    int num_nodos;                    // Cantidad total de elementos en la cola
    int capacidad_v;                  // Cantidad total de vértices |V|
    NodoFibonacci** mapa_nodos;        // Arreglo de acceso directo O(1) por id_vertice
    long long num_cortes;             // Contador de cortes en cascada (métricas de la Tarea)
} ColaFibonacci;

/**
 * @brief Crea e inicializa una Cola de Fibonacci.
 * @param capacidad_v Cantidad total de vértices |V| para el mapeo directo.
 * @return Puntero a la nueva ColaFibonacci asignada dinámicamente.
 */
ColaFibonacci* crearColaFibonacci(int capacidad_v);

/**
 * @brief Libera toda la memoria asignada a la Cola de Fibonacci.
 * @param cola Puntero a la cola a liberar.
 */
void liberarColaFibonacci(ColaFibonacci* cola);

/**
 * @brief Inserta un par (clave, id_vertice) en O(1) en la Cola de Fibonacci.
 * @param cola Puntero a la cola.
 * @param id_vertice ID del vértice a agregar.
 * @param clave Clave/costo del vértice.
 * @return Puntero al nodo creado.
 */
NodoFibonacci* insertarColaFibonacci(ColaFibonacci* cola, int id_vertice, double clave);

/**
 * @brief Obtiene y elimina la raíz de menor clave en O(log n) amortizado.
 * @param cola Puntero a la cola.
 * @param id_vertice_min Puntero para retornar el ID del vértice extraído.
 * @param clave_min Puntero para retornar la clave mínima extraída.
 * @return true si se extrajo correctamente, false si la cola estaba vacía.
 */
bool extraerMinimoColaFibonacci(ColaFibonacci* cola, int* id_vertice_min, double* clave_min);

/**
 * @brief Disminuye la clave de un vértice en O(1) amortizado aplicando cortes en cascada.
 * @param cola Puntero a la cola de Fibonacci.
 * @param id_vertice ID del vértice al que se le actualizará la clave.
 * @param nueva_clave Nuevo valor numérico (debe ser estrictamente menor).
 */
void reducirClaveColaFibonacci(ColaFibonacci* cola, int id_vertice, double nueva_clave);

/**
 * @brief Verifica si la cola no contiene elementos.
 * @param cola Puntero a la cola.
 * @return true si la cola está vacía, false en caso contrario.
 */
bool estaVaciaColaFibonacci(ColaFibonacci* cola);

#endif // COLA_FIBONACCI_H