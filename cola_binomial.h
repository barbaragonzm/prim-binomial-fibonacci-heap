#ifndef COLA_BINOMIAL_H
#define COLA_BINOMIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

/**
 * @brief Estructura que representa un nodo dentro de un Árbol Binomial.
 */
typedef struct NodoBinomial {
    int id_vertice;                 // ID del vértice representado (0 a V-1)
    double clave;                   // Peso/costo asociado al nodo
    int grado;                      // Número de hijos (k para Bk)
    struct NodoBinomial* padre;     // Puntero al nodo padre
    struct NodoBinomial* hijo;      // Puntero al primer hijo (hijo más a la izquierda)
    struct NodoBinomial* hermano;   // Puntero al siguiente hermano a la derecha
} NodoBinomial;

/**
 * @brief Estructura principal de la Cola Binomial.
 * Almacena la lista de raíces, la referencia al mínimo y el mapeo directo a nodos.
 */
typedef struct ColaBinomial {
    NodoBinomial* cabeza;           // Puntero al inicio de la lista de raíces
    NodoBinomial* min;              // Puntero directo al nodo con la clave mínima
    NodoBinomial** mapa_nodos;      // Arreglo de punteros a nodos indexado por id_vertice
    int capacidad_v;                // Cantidad total de vértices |V|
    long long num_intercambios;     // Contador de intercambios en reducirClave (para métricas)
} ColaBinomial;

/**
 * @brief Crea e inicializa una Cola Binomial.
 * @param capacidad_v Cantidad de vértices |V| para dimensionar el arreglo de acceso directo.
 * @return Puntero a la nueva ColaBinomial creada en memoria.
 */
ColaBinomial* crearColaBinomial(int capacidad_v);

/**
 * @brief Libera toda la memoria asignada a la Cola Binomial y sus nodos.
 * @param cola Puntero a la Cola Binomial a liberar.
 */
void liberarColaBinomial(ColaBinomial* cola);

/**
 * @brief Inserta un nuevo par (clave, id_vertice) en la cola en O(log n) peor caso (O(1) amortizado).
 * @param cola Puntero a la cola binomial.
 * @param id_vertice ID del vértice a insertar.
 * @param clave Valor numérico de la clave/costo del vértice.
 * @return Puntero al nodo creado.
 */
NodoBinomial* insertarColaBinomial(ColaBinomial* cola, int id_vertice, double clave);

/**
 * @brief Obtiene y elimina el nodo de menor clave de la cola binomial en O(log n).
 * @param cola Puntero a la cola.
 * @param id_vertice_min Puntero donde se almacenará el ID del vértice extraído.
 * @param clave_min Puntero donde se almacenará la clave mínima extraída.
 * @return true si la extracción fue exitosa, false si la cola estaba vacía.
 */
bool extraerMinimoColaBinomial(ColaBinomial* cola, int* id_vertice_min, double* clave_min);

/**
 * @brief Reduce la clave de un vértice en la cola en O(log n) peor caso.
 * @param cola Puntero a la cola binomial.
 * @param id_vertice ID del vértice al cual se le reducirá la clave.
 * @param nueva_clave Nuevo valor de la clave (debe ser menor a la clave actual).
 */
void reducirClaveColaBinomial(ColaBinomial* cola, int id_vertice, double nueva_clave);

/**
 * @brief Consulta si la cola binomial no tiene elementos.
 * @param cola Puntero a la cola binomial.
 * @return true si la cola está vacía, false en caso contrario.
 */
bool estaVaciaColaBinomial(ColaBinomial* cola);

#endif // COLA_BINOMIAL_H