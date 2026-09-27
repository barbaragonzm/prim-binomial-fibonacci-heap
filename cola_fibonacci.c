#include "cola_fibonacci.h"

/**
 * @brief Crea e inicializa una Cola de Fibonacci vacía.
 */
ColaFibonacci* crearColaFibonacci(int capacidad_v) {
    ColaFibonacci* cola = (ColaFibonacci*) malloc(sizeof(ColaFibonacci));
    cola->min = NULL;
    cola->num_nodos = 0;
    cola->capacidad_v = capacidad_v;
    cola->num_cortes = 0;
    cola->mapa_nodos = (NodoFibonacci**) calloc(capacidad_v, sizeof(NodoFibonacci*));
    return cola;
}

/**
 * @brief Función auxiliar recursiva para liberar todos los nodos.
 */
static void liberarNodoFibonacci(NodoFibonacci* nodo) {
    if (nodo == NULL) return;

    NodoFibonacci* actual = nodo;
    do {
        NodoFibonacci* temp = actual;
        actual = actual->der;
        liberarNodoFibonacci(temp->hijo);
        free(temp);
    } while (actual != NULL && actual != nodo);
}

/**
 * @brief Libera la memoria total de la cola.
 */
void liberarColaFibonacci(ColaFibonacci* cola) {
    if (cola == NULL) return;
    if (cola->min != NULL) {
        // Romper la circularidad para la liberación sistemática
        cola->min->izq->der = NULL;
        liberarNodoFibonacci(cola->min);
    }
    free(cola->mapa_nodos);
    free(cola);
}

/**
 * @brief Retorna true si la cola no posee raíces.
 */
bool estaVaciaColaFibonacci(ColaFibonacci* cola) {
    return (cola == NULL || cola->min == NULL);
}

/**
 * @brief Añade un nodo a la lista circular doblemente enlazada de raíces.
 */
static void agregarAListaRaices(ColaFibonacci* cola, NodoFibonacci* nodo) {
    nodo->padre = NULL;
    if (cola->min == NULL) {
        nodo->izq = nodo;
        nodo->der = nodo;
        cola->min = nodo;
    } else {
        nodo->izq = cola->min;
        nodo->der = cola->min->der;
        cola->min->der->izq = nodo;
        cola->min->der = nodo;
    }
}

/**
 * @brief Remueve un nodo de una lista circular doblemente enlazada.
 */
static void removerDeListaCircular(NodoFibonacci* nodo) {
    nodo->izq->der = nodo->der;
    nodo->der->izq = nodo->izq;
}

/**
 * @brief Inserta un vértice en la cola en O(1) tiempo real.
 */
NodoFibonacci* insertarColaFibonacci(ColaFibonacci* cola, int id_vertice, double clave) {
    NodoFibonacci* nuevo = (NodoFibonacci*) malloc(sizeof(NodoFibonacci));
    nuevo->id_vertice = id_vertice;
    nuevo->clave = clave;
    nuevo->grado = 0;
    nuevo->marcado = false;
    nuevo->padre = NULL;
    nuevo->hijo = NULL;

    agregarAListaRaices(cola, nuevo);

    if (nuevo->clave < cola->min->clave) {
        cola->min = nuevo;
    }

    cola->mapa_nodos[id_vertice] = nuevo;
    cola->num_nodos++;
    return nuevo;
}

/**
 * @brief Enlaza el nodo y como hijo del nodo x.
 */
static void enlaceFibonacci(ColaFibonacci* cola, NodoFibonacci* y, NodoFibonacci* x) {
    removerDeListaCircular(y);
    y->padre = x;

    if (x->hijo == NULL) {
        x->hijo = y;
        y->izq = y;
        y->der = y;
    } else {
        y->izq = x->hijo;
        y->der = x->hijo->der;
        x->hijo->der->izq = y;
        x->hijo->der = y;
    }

    x->grado++;
    y->marcado = false;
}

/**
 * @brief Consolidación de árboles de igual grado.
 */
static void consolidar(ColaFibonacci* cola) {
    int max_grado = (int)(log2(cola->capacidad_v + 1) + 5);
    NodoFibonacci** A = (NodoFibonacci**) calloc(max_grado, sizeof(NodoFibonacci*));

    // Obtener la cantidad de raíces
    int num_raices = 0;
    if (cola->min != NULL) {
        NodoFibonacci* w = cola->min;
        do {
            num_raices++;
            w = w->der;
        } while (w != cola->min);
    }

    NodoFibonacci* x = cola->min;
    for (int i = 0; i < num_raices; ++i) {
        NodoFibonacci* siguiente = x->der;
        int d = x->grado;

        while (d < max_grado && A[d] != NULL) {
            NodoFibonacci* y = A[d];
            if (x->clave > y->clave) {
                NodoFibonacci* temp = x;
                x = y;
                y = temp;
            }
            enlaceFibonacci(cola, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
        x = siguiente;
    }

    cola->min = NULL;

    for (int i = 0; i < max_grado; ++i) {
        if (A[i] != NULL) {
            if (cola->min == NULL) {
                A[i]->izq = A[i];
                A[i]->der = A[i];
                cola->min = A[i];
            } else {
                agregarAListaRaices(cola, A[i]);
                if (A[i]->clave < cola->min->clave) {
                    cola->min = A[i];
                }
            }
        }
    }

    free(A);
}

/**
 * @brief Extrae el mínimo de la cola.
 */
bool extraerMinimoColaFibonacci(ColaFibonacci* cola, int* id_vertice_min, double* clave_min) {
    NodoFibonacci* z = cola->min;
    if (z == NULL) return false;

    *id_vertice_min = z->id_vertice;
    *clave_min = z->clave;

    // Pasar los hijos de z a la lista de raíces
    if (z->hijo != NULL) {
        NodoFibonacci* hijo = z->hijo;
        NodoFibonacci* inicio = hijo;
        do {
            NodoFibonacci* sig = hijo->der;
            agregarAListaRaices(cola, hijo);
            hijo->padre = NULL;
            hijo = sig;
        } while (hijo != inicio);
    }

    removerDeListaCircular(z);

    if (z == z->der) {
        cola->min = NULL;
    } else {
        cola->min = z->der;
        consolidar(cola);
    }

    cola->mapa_nodos[z->id_vertice] = NULL;
    free(z);
    cola->num_nodos--;
    return true;
}

/**
 * @brief Implementación de CUT(Q, x, y).
 */
static void corte(ColaFibonacci* cola, NodoFibonacci* x, NodoFibonacci* y) {
    // Quitar x de la lista de hijos de y
    if (x->der == x) {
        y->hijo = NULL;
    } else {
        if (y->hijo == x) {
            y->hijo = x->der;
        }
        removerDeListaCircular(x);
    }
    y->grado--;

    // Agregar x a la lista de raíces de Q
    agregarAListaRaices(cola, x);
    x->padre = NULL;
    x->marcado = false;
}

/**
 * @brief Implementación de CASCADING-CUT(Q, y) .
 */
static void corteEnCascada(ColaFibonacci* cola, NodoFibonacci* y) {
    NodoFibonacci* z = y->padre;
    if (z != NULL) {
        if (!y->marcado) {
            y->marcado = true;
        } else {
            cola->num_cortes++; // Incrementar contador de operaciones 
            corte(cola, y, z);
            corteEnCascada(cola, z);
        }
    }
}

/**
 * @brief Implementación de DECREASEKEY
 */
void reducirClaveColaFibonacci(ColaFibonacci* cola, int id_vertice, double nueva_clave) {
    NodoFibonacci* x = cola->mapa_nodos[id_vertice];
    if (x == NULL || nueva_clave >= x->clave) return;

    x->clave = nueva_clave;
    NodoFibonacci* y = x->padre;

    if (y != NULL && x->clave < y->clave) {
        corte(cola, x, y);
        corteEnCascada(cola, y);
    }

    if (x->clave < cola->min->clave) {
        cola->min = x;
    }
}