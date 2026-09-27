#include "prim.h"

// Función auxiliar para obtener el tiempo actual en segundos con alta precisión
static double obtenerTiempoActual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/**
 * @brief Implementación de Prim con Cola Binomial .
 */
ResultadoPrim ejecutarPrimBinomial(Grafo* grafo, int nodo_raiz) {
    int V = grafo->V;
    double* costos = (double*) malloc(V * sizeof(double));
    int* padre = (int*) malloc(V * sizeof(int));
    bool* en_Q = (bool*) malloc(V * sizeof(bool));

    //  Inicialización
    for (int v = 0; v < V; ++v) {
        costos[v] = DBL_MAX;
        padre[v] = -1;
        en_Q[v] = true;
    }

    costos[nodo_raiz] = 0.0;

    // Construcción de Q 
    ColaBinomial* Q = crearColaBinomial(V);
    for (int v = 0; v < V; ++v) {
        insertarColaBinomial(Q, v, costos[v]);
    }

    ResultadoPrim res;
    res.peso_total_mst = 0.0;
    res.tiempo_reducir_clave_segundos = 0.0;
    res.ck_usados = 0;

    // Intervalo de muestreo: cada cuántas llamadas a decreaseKey se guarda un
    // checkpoint. Se basa en |E| (cota superior de llamadas a decreaseKey).
    long long intervalo_muestreo = (grafo->E > NUM_CHECKPOINTS) ? (grafo->E / NUM_CHECKPOINTS) : 1;
    long long contador_dk = 0;

    double inicio_total = obtenerTiempoActual();

    //  Bucle Principal de Prim
    while (!estaVaciaColaBinomial(Q)) {
        int u;
        double c;
        if (!extraerMinimoColaBinomial(Q, &u, &c)) break;

        en_Q[u] = false;
        res.peso_total_mst += c;

        // Explorar vecinos de u
        NodoAdyacencia* vecino = grafo->arreglo[u].cabeza;
        while (vecino != NULL) {
            int v = vecino->destino;
            double peso_uv = vecino->peso;

            if (en_Q[v] && peso_uv < costos[v]) {
                costos[v] = peso_uv;
                padre[v] = u;

                // Medición del tiempo exclusivo de reducirClave
                double inicio_dk = obtenerTiempoActual();
                reducirClaveColaBinomial(Q, v, peso_uv);
                double fin_dk = obtenerTiempoActual();

                res.tiempo_reducir_clave_segundos += (fin_dk - inicio_dk);
                contador_dk++;

                // Registrar checkpoint para el análisis amortizado
                if (contador_dk % intervalo_muestreo == 0 && res.ck_usados < NUM_CHECKPOINTS) {
                    res.ck_llamadas[res.ck_usados] = contador_dk;
                    res.ck_tiempo_acumulado[res.ck_usados] = res.tiempo_reducir_clave_segundos;
                    res.ck_ops_acumuladas[res.ck_usados] = Q->num_intercambios;
                    res.ck_usados++;
                }
            }
            vecino = vecino->sig;
        }
    }

    double fin_total = obtenerTiempoActual();
    res.tiempo_total_segundos = fin_total - inicio_total;
    res.num_operaciones = Q->num_intercambios;

    // Liberar memoria auxiliar
    free(costos);
    free(padre);
    free(en_Q);
    liberarColaBinomial(Q);

    return res;
}

/**
 * @brief Implementación de Prim con Cola de Fibonacci .
 */
ResultadoPrim ejecutarPrimFibonacci(Grafo* grafo, int nodo_raiz) {
    int V = grafo->V;
    double* costos = (double*) malloc(V * sizeof(double));
    int* padre = (int*) malloc(V * sizeof(int));
    bool* en_Q = (bool*) malloc(V * sizeof(bool));

    // Inicialización
    for (int v = 0; v < V; ++v) {
        costos[v] = DBL_MAX;
        padre[v] = -1;
        en_Q[v] = true;
    }

    costos[nodo_raiz] = 0.0;

    // Construcción de Q
    ColaFibonacci* Q = crearColaFibonacci(V);
    for (int v = 0; v < V; ++v) {
        insertarColaFibonacci(Q, v, costos[v]);
    }

    ResultadoPrim res;
    res.peso_total_mst = 0.0;
    res.tiempo_reducir_clave_segundos = 0.0;
    res.ck_usados = 0;

    long long intervalo_muestreo = (grafo->E > NUM_CHECKPOINTS) ? (grafo->E / NUM_CHECKPOINTS) : 1;
    long long contador_dk = 0;

    double inicio_total = obtenerTiempoActual();

    //  Bucle Principal de Prim
    while (!estaVaciaColaFibonacci(Q)) {
        int u;
        double c;
        if (!extraerMinimoColaFibonacci(Q, &u, &c)) break;

        en_Q[u] = false;
        res.peso_total_mst += c;

        // Explorar vecinos de u
        NodoAdyacencia* vecino = grafo->arreglo[u].cabeza;
        while (vecino != NULL) {
            int v = vecino->destino;
            double peso_uv = vecino->peso;

            if (en_Q[v] && peso_uv < costos[v]) {
                costos[v] = peso_uv;
                padre[v] = u;

                // Medición del tiempo exclusivo de reducirClave
                double inicio_dk = obtenerTiempoActual();
                reducirClaveColaFibonacci(Q, v, peso_uv);
                double fin_dk = obtenerTiempoActual();

                res.tiempo_reducir_clave_segundos += (fin_dk - inicio_dk);
                contador_dk++;

                if (contador_dk % intervalo_muestreo == 0 && res.ck_usados < NUM_CHECKPOINTS) {
                    res.ck_llamadas[res.ck_usados] = contador_dk;
                    res.ck_tiempo_acumulado[res.ck_usados] = res.tiempo_reducir_clave_segundos;
                    res.ck_ops_acumuladas[res.ck_usados] = Q->num_cortes;
                    res.ck_usados++;
                }
            }
            vecino = vecino->sig;
        }
    }

    double fin_total = obtenerTiempoActual();
    res.tiempo_total_segundos = fin_total - inicio_total;
    res.num_operaciones = Q->num_cortes;

    // Liberar memoria auxiliar
    free(costos);
    free(padre);
    free(en_Q);
    liberarColaFibonacci(Q);

    return res;
}