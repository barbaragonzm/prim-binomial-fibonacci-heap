#ifndef PRIM_H
#define PRIM_H

#include "grafo.h"
#include "cola_binomial.h"
#include "cola_fibonacci.h"
#include <stdbool.h>
#include <time.h>

// Cantidad de puntos de muestreo (checkpoints) registrados a lo largo de una
// ejecución, usados para graficar la curva de costo amortizado de
// decreaseKey .
#define NUM_CHECKPOINTS 20

/**
 * @brief Estructura para almacenar las métricas del algoritmo de Prim.
 */
typedef struct ResultadoPrim {
    double peso_total_mst;                 // Suma del peso de todas las aristas del MST
    double tiempo_total_segundos;          // Tiempo de ejecución total del algoritmo de Prim
    double tiempo_reducir_clave_segundos;  // Tiempo acumulado únicamente en la función reducirClave
    long long num_operaciones;             // Intercambios (Binomial) o cortes en cascada (Fibonacci)

    // Muestreo para el análisis de costo amortizado :
    // en cada uno de los NUM_CHECKPOINTS puntos (espaciados uniformemente en
    // cantidad de llamadas a decreaseKey) se registra cuántas llamadas se han
    // hecho, el tiempo acumulado gastado en decreaseKey y la cantidad
    // acumulada de operaciones estructurales (swaps o cortes en cascada).
    long long ck_llamadas[NUM_CHECKPOINTS];
    double ck_tiempo_acumulado[NUM_CHECKPOINTS];
    long long ck_ops_acumuladas[NUM_CHECKPOINTS];
    int ck_usados; // cuántas entradas de los arreglos anteriores son válidas
} ResultadoPrim;

/**
 * @brief Ejecuta el algoritmo de Prim utilizando una Cola Binomial.
 * @param grafo Puntero al grafo conexo de entrada.
 * @param nodo_raiz Nodo inicial r para comenzar la construcción del MST (ej: 0).
 * @return Estructura ResultadoPrim con métricas de tiempo, costo de operaciones y peso total del MST.
 */
ResultadoPrim ejecutarPrimBinomial(Grafo* grafo, int nodo_raiz);

/**
 * @brief Ejecuta el algoritmo de Prim utilizando una Cola de Fibonacci.
 * @param grafo Puntero al grafo conexo de entrada.
 * @param nodo_raiz Nodo inicial r para comenzar la construcción del MST (ej: 0).
 * @return Estructura ResultadoPrim con métricas de tiempo, costo de operaciones y peso total del MST.
 */
ResultadoPrim ejecutarPrimFibonacci(Grafo* grafo, int nodo_raiz);

#endif // PRIM_H