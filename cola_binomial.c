#include "cola_binomial.h"

/**
 * @brief Crea e inicializa una cola binomial vacía.
 */
ColaBinomial* crearColaBinomial(int capacidad_v) {
    ColaBinomial* cola = (ColaBinomial*) malloc(sizeof(ColaBinomial));
    cola->cabeza = NULL;
    cola->min = NULL;
    cola->capacidad_v = capacidad_v;
    cola->num_intercambios = 0;
    cola->mapa_nodos = (NodoBinomial**) calloc(capacidad_v, sizeof(NodoBinomial*));
    return cola;
}

/**
 * @brief Función auxiliar recursiva para liberar nodos de un árbol binomial.
 */
static void liberarNodoBinomial(NodoBinomial* nodo) {
    if (nodo == NULL) return;
    liberarNodoBinomial(nodo->hijo);
    liberarNodoBinomial(nodo->hermano);
    free(nodo);
}

/**
 * @brief Libera la memoria de la cola y del arreglo de mapeo.
 */
void liberarColaBinomial(ColaBinomial* cola) {
    if (cola == NULL) return;
    liberarNodoBinomial(cola->cabeza);
    free(cola->mapa_nodos);
    free(cola);
}

/**
 * @brief Verifica si la cola está vacía.
 */
bool estaVaciaColaBinomial(ColaBinomial* cola) {
    return (cola == NULL || cola->cabeza == NULL);
}

/**
 * @brief Actualiza el puntero 'min' de la cola buscando entre las raíces.
 */
static void actualizarMinimo(ColaBinomial* cola) {
    if (cola->cabeza == NULL) {
        cola->min = NULL;
        return;
    }
    NodoBinomial* actual = cola->cabeza;
    NodoBinomial* min_nodo = actual;
    while (actual != NULL) {
        if (actual->clave < min_nodo->clave) {
            min_nodo = actual;
        }
        actual = actual->hermano;
    }
    cola->min = min_nodo;
}

/**
 * @brief Enlaza un árbol B_k como hijo de otro árbol B_k (convirtiéndose en B_{k+1}).
 */
static void enlazarArboles(NodoBinomial* padre, NodoBinomial* hijo) {
    hijo->padre = padre;
    hijo->hermano = padre->hijo;
    padre->hijo = hijo;
    padre->grado++;
}

/**
 * @brief Fusiona dos listas de raíces ordenadas por grado de menor a mayor.
 */
static NodoBinomial* meclarListasRaices(NodoBinomial* h1, NodoBinomial* h2) {
    if (h1 == NULL) return h2;
    if (h2 == NULL) return h1;

    NodoBinomial* cabeza = NULL;
    NodoBinomial** cola_ptr = &cabeza;

    while (h1 != NULL && h2 != NULL) {
        if (h1->grado <= h2->grado) {
            *cola_ptr = h1;
            h1 = h1->hermano;
        } else {
            *cola_ptr = h2;
            h2 = h2->hermano;
        }
        cola_ptr = &((*cola_ptr)->hermano);
    }

    if (h1 != NULL) *cola_ptr = h1;
    else *cola_ptr = h2;

    return cabeza;
}

/**
 * @brief Une dos colas binomiales reestructurando árboles del mismo grado.
 */
static NodoBinomial* unionColasBinomiales(NodoBinomial* h1, NodoBinomial* h2) {
    NodoBinomial* nueva_cabeza = meclarListasRaices(h1, h2);
    if (nueva_cabeza == NULL) return NULL;

    NodoBinomial* prev = NULL;
    NodoBinomial* curr = nueva_cabeza;
    NodoBinomial* next = curr->hermano;

    while (next != NULL) {
        if ((curr->grado != next->grado) || 
            (next->hermano != NULL && next->hermano->grado == curr->grado)) {
            prev = curr;
            curr = next;
        } else {
            if (curr->clave <= next->clave) {
                curr->hermano = next->hermano;
                enlazarArboles(curr, next);
            } else {
                if (prev == NULL) nueva_cabeza = next;
                else prev->hermano = next;
                enlazarArboles(next, curr);
                curr = next;
            }
        }
        next = curr->hermano;
    }

    return nueva_cabeza;
}

/**
 * @brief Inserta un nuevo elemento creando un nodo unitario y haciendo union.
 */
NodoBinomial* insertarColaBinomial(ColaBinomial* cola, int id_vertice, double clave) {
    NodoBinomial* nuevo = (NodoBinomial*) malloc(sizeof(NodoBinomial));
    nuevo->id_vertice = id_vertice;
    nuevo->clave = clave;
    nuevo->grado = 0;
    nuevo->padre = NULL;
    nuevo->hijo = NULL;
    nuevo->hermano = NULL;

    // Registrar en el mapeo directo O(1)
    cola->mapa_nodos[id_vertice] = nuevo;

    // Unir el nuevo nodo con la cola actual
    cola->cabeza = unionColasBinomiales(cola->cabeza, nuevo);
    actualizarMinimo(cola);

    return nuevo;
}

/**
 * @brief Extrae el mínimo de la cola en O(log n).
 */
bool extraerMinimoColaBinomial(ColaBinomial* cola, int* id_vertice_min, double* clave_min) {
    if (estaVaciaColaBinomial(cola) || cola->min == NULL) return false;

    NodoBinomial* min_nodo = cola->min;
    *id_vertice_min = min_nodo->id_vertice;
    *clave_min = min_nodo->clave;

    // Remover min_nodo de la lista de raíces
    NodoBinomial* prev = NULL;
    NodoBinomial* curr = cola->cabeza;
    while (curr != NULL && curr != min_nodo) {
        prev = curr;
        curr = curr->hermano;
    }

    if (prev == NULL) cola->cabeza = min_nodo->hermano;
    else prev->hermano = min_nodo->hermano;

    // Invertir la lista de hijos del nodo mínimo
    NodoBinomial* hijo_curr = min_nodo->hijo;
    NodoBinomial* lista_hijos_invertida = NULL;

    while (hijo_curr != NULL) {
        NodoBinomial* siguiente = hijo_curr->hermano;
        hijo_curr->hermano = lista_hijos_invertida;
        hijo_curr->padre = NULL;
        lista_hijos_invertida = hijo_curr;
        hijo_curr = siguiente;
    }

    // Desregistrar de mapa de nodos
    cola->mapa_nodos[min_nodo->id_vertice] = NULL;
    free(min_nodo);

    // Unir la cola resultante con la lista de hijos invertida
    cola->cabeza = unionColasBinomiales(cola->cabeza, lista_hijos_invertida);
    actualizarMinimo(cola);

    return true;
}

/**
 * @brief reduceKey 
 * Intercambia claves e IDs de vértices subiendo hacia los padres,
 * actualizando el arreglo mapa_nodos para mantener la referencia correcta.
 */
void reducirClaveColaBinomial(ColaBinomial* cola, int id_vertice, double nueva_clave) {
    NodoBinomial* x = cola->mapa_nodos[id_vertice];
    if (x == NULL || nueva_clave >= x->clave) return;

    x->clave = nueva_clave;
    NodoBinomial* y = x->padre;

    while (y != NULL && x->clave < y->clave) {
        // Contar el intercambio para análisis experimental 
        cola->num_intercambios++;

        // Intercambiar datos del nodo
        double temp_clave = x->clave;
        int temp_id = x->id_vertice;

        x->clave = y->clave;
        x->id_vertice = y->id_vertice;

        y->clave = temp_clave;
        y->id_vertice = temp_id;

        //  Actualizar referencias en mapa_nodos
        cola->mapa_nodos[x->id_vertice] = x;
        cola->mapa_nodos[y->id_vertice] = y;

        // Avanzar hacia arriba
        x = y;
        y = x->padre;
    }

    if (cola->min == NULL || x->clave < cola->min->clave) {
        cola->min = x;
    }
}