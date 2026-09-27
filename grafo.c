#include "grafo.h"
#include <math.h>

/**
 * @brief Crea un nuevo grafo inicializando sus listas de adyacencia en NULL.
 */
Grafo* crearGrafo(int V,int E) {
    Grafo* grafo = (Grafo*) malloc(sizeof(Grafo));
    grafo->V = V;
    grafo->E = 0;
    grafo->arreglo = (ListaAdyacencia*) malloc(V * sizeof(ListaAdyacencia));
    
    // Un solo malloc para TODAS las aristas del grafo
    grafo->bloque_nodos = (NodoAdyacencia*) malloc(2 * E * sizeof(NodoAdyacencia));
    grafo->indice_bloque = 0;

    for (int i = 0; i < V; ++i) {
        grafo->arreglo[i].cabeza = NULL;
    }

    return grafo;
}

/**
 * @brief Agrega la arista en ambos sentidos  a las listas de adyacencia.
 */
void agregarArista(Grafo* grafo, int origen, int destino, double peso) {
    // Agregar arista origen -> destino
    NodoAdyacencia* nuevoNodo1 = &grafo->bloque_nodos[grafo->indice_bloque++];
    nuevoNodo1->destino = destino;
    nuevoNodo1->peso = peso;
    nuevoNodo1->sig = grafo->arreglo[origen].cabeza;
    grafo->arreglo[origen].cabeza = nuevoNodo1;

    // Agregar arista destino -> origen
    NodoAdyacencia* nuevoNodo2 = &grafo->bloque_nodos[grafo->indice_bloque++];
    nuevoNodo2->destino = origen;
    nuevoNodo2->peso = peso;
    nuevoNodo2->sig = grafo->arreglo[destino].cabeza;
    grafo->arreglo[destino].cabeza = nuevoNodo2;
    
    grafo->E++;
}

/**
 * @brief Libera recursivamente los nodos de adyacencia de cada vértice y el arreglo principal.
 */
void liberarGrafo(Grafo* grafo) {
    if (grafo == NULL) return;
    free(grafo->bloque_nodos); // Liberación instantánea 
    free(grafo->arreglo);
    free(grafo);
}

/**
 * @brief Retorna un número flotante pseudoaleatorio en el rango (0, 1].
 */
static double obtenerPesoAleatorio() {
    return ((double)rand() / (double)RAND_MAX) * 0.9999 + 0.0001;
}

/**
 * @brief Tabla hash abierta  para detectar en O(1) esperado
 **/
  
typedef struct {
    long long* claves; 
    long long capacidad;
} TablaAristas;

static TablaAristas crearTablaAristas(long long capacidad_min) {
    TablaAristas t;
    t.capacidad = 1;
    while (t.capacidad < capacidad_min * 2) t.capacidad <<= 1; // factor de carga <= 0.5
    t.claves = (long long*) malloc(t.capacidad * sizeof(long long));
    for (long long k = 0; k < t.capacidad; ++k) t.claves[k] = -1;
    return t;
}

static void liberarTablaAristas(TablaAristas* t) {
    free(t->claves);
    t->claves = NULL;
}

static inline long long claveArista(int u, int v, int V) {
    int a = (u < v) ? u : v;
    int b = (u < v) ? v : u;
    return (long long)a * (long long)V + (long long)b;
}

static inline long long hashMezcla(long long x) {
    // Función de hash para dispersar bien las claves.
    x ^= (unsigned long long)x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= (unsigned long long)x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= (unsigned long long)x >> 31;
    return x;
}

// Retorna true si {u,v} ya estaba en la tabla; si no estaba, la inserta y
// retorna false.
static bool existeOInsertaArista(TablaAristas* t, int u, int v, int V) {
    long long clave = claveArista(u, v, V);
    long long idx = ((unsigned long long)hashMezcla(clave)) & (t->capacidad - 1);
    while (t->claves[idx] != -1) {
        if (t->claves[idx] == clave) return true;
        idx = (idx + 1) & (t->capacidad - 1);
    }
    t->claves[idx] = clave;
    return false;
}

/**
 * @brief Genera un grafo conexo aleatorio respetando la regla del árbol cobertor inicial.
 */
Grafo* generarGrafoConexoAleatorio(int potencia_i, int potencia_j, unsigned int semilla) {
    srand(semilla);
    int v = 1 << potencia_i; // 2^i vértices
    long long e = 1LL << potencia_j; // 2^j aristas

    Grafo* grafo = crearGrafo(v, (int)e);
    TablaAristas tabla = crearTablaAristas(e + v);

    //  Garantizar conectividad creando un árbol cobertor:
    // Para cada nodo v_i, lo conecta a un nodo aleatorio elegido en [0 .. i-1]
    for (int i = 1; i < v; ++i) {
        int padre = rand() % i;
        double peso = obtenerPesoAleatorio();
        agregarArista(grafo, i, padre, peso);
        existeOInsertaArista(&tabla, i, padre, v);
    }

    //  Añadir las 2^j - v + 1 aristas restantes de manera aleatoria
    long long aristas_restantes = e - (v - 1);
    while (aristas_restantes > 0) {
        int u = rand() % v;
        int w = rand() % v;

        // Descartar aristas repetidas y reflexivas
        if (u != w && !existeOInsertaArista(&tabla, u, w, v)) {
            double peso = obtenerPesoAleatorio();
            agregarArista(grafo, u, w, peso);
            aristas_restantes--;
        }
    }

    liberarTablaAristas(&tabla);
    return grafo;
}
