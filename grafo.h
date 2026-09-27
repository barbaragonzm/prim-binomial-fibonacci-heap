#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Estructura que representa un nodo en la lista de adyacencia de un vértice.
 * Contiene el destino de la arista, su peso y el puntero al siguiente nodo adyacente.
 */
typedef struct NodoAdyacencia {
    int destino;                  // ID del nodo destino
    double peso;                  // Peso w(e) de la arista en (0, 1]
    struct NodoAdyacencia* sig;   // Puntero al siguiente elemento de la lista de adyacencia
} NodoAdyacencia;

/**
 * @brief Estructura que representa la lista de adyacencia de un vértice.
 */
typedef struct ListaAdyacencia {
    NodoAdyacencia* cabeza;       // Puntero al primer nodo de la lista
} ListaAdyacencia;

/**
 * @brief Estructura que representa un Grafo conexo no dirigido con pesos.
 * Almacena el número de vértices, aristas, el arreglo de listas de adyacencia, un bloque de nodos y un indice del bloque.
 */
typedef struct Grafo {
    int V;                        // Cantidad total de vértices |V|
    int E;                        // Cantidad total de aristas |E|
    ListaAdyacencia* arreglo;     // Arreglo dinámico de listas de adyacencia
    NodoAdyacencia* bloque_nodos; // Reservará toda la memoria de un solo golpe
    int indice_bloque;
} Grafo;

/**
 * @brief Crea e inicializa una estructura de Grafo con V vértices.
 * @param V Número de vértices del grafo.
 * @return Puntero a la nueva estructura Grafo asignada dinámicamente.
 */
Grafo* crearGrafo(int V, int E);

/**
 * @brief Agrega una arista no dirigida con peso entre dos vértices.
 * @param grafo Puntero al grafo donde se agregará la arista.
 * @param origen ID del vértice origen (0 a V-1).
 * @param destino ID del vértice destino (0 a V-1).
 * @param peso Peso positivo w(e) asignado a la arista.
 */
void agregarArista(Grafo* grafo, int origen, int destino, double peso);

/**
 * @brief Libera la memoria RAM ocupada por el grafo y sus listas de adyacencia.
 * @param grafo Puntero al grafo a liberar.
 */
void liberarGrafo(Grafo* grafo);

/**
 * @brief Genera un grafo conexo aleatorio según los requerimientos de la tarea.
 * @param potencia_i Exponente i tal que el número de vértices V = 2^i.
 * @param potencia_j Exponente j tal que el número de aristas E = 2^j.
 * @param semilla Semilla para el generador de números aleatorios (reproducibilidad).
 * @return Puntero al Grafo conexo generado dinámicamente.
 */
Grafo* generarGrafoConexoAleatorio(int potencia_i, int potencia_j, unsigned int semilla);

#endif // GRAFO_H
