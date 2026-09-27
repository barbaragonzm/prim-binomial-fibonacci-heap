#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "grafo.h"
#include "prim.h"

#define REPETICIONES 10

// Estructura para almacenar promedios por configuración
typedef struct {
    int i;
    int j;
    double tiempo_prom_bin;
    double tiempo_prom_fib;
    bool pesos_coinciden;

    // Promedio (sobre las repeticiones) de los checkpoints de costo amortizado.
    // Se promedia posición a posición; se usa el mínimo ck_usados observado
    // entre repeticiones para no promediar con datos faltantes.
    int ck_usados;
    double ck_llamadas_bin[NUM_CHECKPOINTS];
    double ck_tiempo_bin[NUM_CHECKPOINTS];
    double ck_ops_bin[NUM_CHECKPOINTS];
    double ck_llamadas_fib[NUM_CHECKPOINTS];
    double ck_tiempo_fib[NUM_CHECKPOINTS];
    double ck_ops_fib[NUM_CHECKPOINTS];
} PromediosConfiguracion;

// Límites opcionales (para pruebas rápidas en una máquina más modesta).
// Por defecto no hay límite: se usan exactamente los (i,j) pedidos en el
// enunciado. Se pueden acotar seteando las variables de entorno
// TAREA1_MAX_I y TAREA1_MAX_J (usado solo para validar el pipeline).
static int limite_i() {
    const char* v = getenv("TAREA1_MAX_I");
    return v ? atoi(v) : INT_MAX;
}
static int limite_j() {
    const char* v = getenv("TAREA1_MAX_J");
    return v ? atoi(v) : INT_MAX;
}

PromediosConfiguracion ejecutarExperimento(int i, int j) {
    PromediosConfiguracion prom = {0};
    prom.i = i;
    prom.j = j;
    prom.pesos_coinciden = true;

    double suma_tiempo_bin = 0.0, suma_tiempo_fib = 0.0;

    long long suma_ck_llamadas_bin[NUM_CHECKPOINTS] = {0};
    double suma_ck_tiempo_bin[NUM_CHECKPOINTS] = {0};
    long long suma_ck_ops_bin[NUM_CHECKPOINTS] = {0};
    long long suma_ck_llamadas_fib[NUM_CHECKPOINTS] = {0};
    double suma_ck_tiempo_fib[NUM_CHECKPOINTS] = {0};
    long long suma_ck_ops_fib[NUM_CHECKPOINTS] = {0};

    int min_ck_bin = NUM_CHECKPOINTS;
    int min_ck_fib = NUM_CHECKPOINTS;

    printf("\n=== Ejecutando Configuracion: i=%d (V=2^%d), j=%d (E=2^%d) ===\n", i, i, j, j);

    for (int rep = 1; rep <= REPETICIONES; ++rep) {
        unsigned int semilla = 1000u * (unsigned int)rep + (unsigned int)i * 100u + (unsigned int)j;
        Grafo* g = generarGrafoConexoAleatorio(i, j, semilla);

        ResultadoPrim res_bin = ejecutarPrimBinomial(g, 0);
        ResultadoPrim res_fib = ejecutarPrimFibonacci(g, 0);

        suma_tiempo_bin += res_bin.tiempo_total_segundos;
        suma_tiempo_fib += res_fib.tiempo_total_segundos;

        if (res_bin.ck_usados < min_ck_bin) min_ck_bin = res_bin.ck_usados;
        if (res_fib.ck_usados < min_ck_fib) min_ck_fib = res_fib.ck_usados;

        for (int k = 0; k < res_bin.ck_usados && k < NUM_CHECKPOINTS; ++k) {
            suma_ck_llamadas_bin[k] += res_bin.ck_llamadas[k];
            suma_ck_tiempo_bin[k]   += res_bin.ck_tiempo_acumulado[k];
            suma_ck_ops_bin[k]      += res_bin.ck_ops_acumuladas[k];
        }
        for (int k = 0; k < res_fib.ck_usados && k < NUM_CHECKPOINTS; ++k) {
            suma_ck_llamadas_fib[k] += res_fib.ck_llamadas[k];
            suma_ck_tiempo_fib[k]   += res_fib.ck_tiempo_acumulado[k];
            suma_ck_ops_fib[k]      += res_fib.ck_ops_acumuladas[k];
        }

        if (fabs(res_bin.peso_total_mst - res_fib.peso_total_mst) > 1e-5) {
            prom.pesos_coinciden = false;
        }

        liberarGrafo(g);
        printf("  Repeticion %d/%d completada.\r", rep, REPETICIONES);
        fflush(stdout);
    }
    printf("\n");

    prom.tiempo_prom_bin = suma_tiempo_bin / REPETICIONES;
    prom.tiempo_prom_fib = suma_tiempo_fib / REPETICIONES;

    printf("  -> Finalizado. Binomial: %.4fs | Fibonacci: %.4fs\n",
           prom.tiempo_prom_bin, prom.tiempo_prom_fib);

    prom.ck_usados = (min_ck_bin < min_ck_fib) ? min_ck_bin : min_ck_fib;
    for (int k = 0; k < prom.ck_usados; ++k) {
        prom.ck_llamadas_bin[k] = (double)suma_ck_llamadas_bin[k] / REPETICIONES;
        prom.ck_tiempo_bin[k]   = suma_ck_tiempo_bin[k] / REPETICIONES;
        prom.ck_ops_bin[k]      = (double)suma_ck_ops_bin[k] / REPETICIONES;
        prom.ck_llamadas_fib[k] = (double)suma_ck_llamadas_fib[k] / REPETICIONES;
        prom.ck_tiempo_fib[k]   = suma_ck_tiempo_fib[k] / REPETICIONES;
        prom.ck_ops_fib[k]      = (double)suma_ck_ops_fib[k] / REPETICIONES;
    }

    return prom;
}

static void escribirFilaTotal(FILE* f, const char* serie, PromediosConfiguracion p) {
    fprintf(f, "%s,%d,%d,%lld,%lld,%.6f,%.6f,%s\n",
            serie, p.i, p.j, 1LL << p.i, 1LL << p.j,
            p.tiempo_prom_bin, p.tiempo_prom_fib,
            p.pesos_coinciden ? "OK" : "ERROR");
}

static void escribirFilasAmortizado(FILE* f, const char* serie, PromediosConfiguracion p) {
    char cfg[16];
    snprintf(cfg, sizeof(cfg), "%s_i%dj%d", serie, p.i, p.j);
    for (int k = 0; k < p.ck_usados; ++k) {
        fprintf(f, "%s,%s,%d,%d,%lld,%lld,Binomial,%.0f,%.8f,%.0f\n",
                serie, cfg, p.i, p.j, 1LL << p.i, 1LL << p.j,
                p.ck_llamadas_bin[k], p.ck_tiempo_bin[k], p.ck_ops_bin[k]);
    }
    for (int k = 0; k < p.ck_usados; ++k) {
        fprintf(f, "%s,%s,%d,%d,%lld,%lld,Fibonacci,%.0f,%.8f,%.0f\n",
                serie, cfg, p.i, p.j, 1LL << p.i, 1LL << p.j,
                p.ck_llamadas_fib[k], p.ck_tiempo_fib[k], p.ck_ops_fib[k]);
    }
}

int main() {
    printf("====================== Algoritmo de Prim (Tarea 1) ======================\n");

    int MAX_I = limite_i();
    int MAX_J = limite_j();

    FILE* csv_totales = fopen("resultados_costo_total.csv", "w");
    if (!csv_totales) { perror("Error al crear resultados_costo_total.csv"); return 1; }
    fprintf(csv_totales, "Serie,i,j,V,E,Tiempo_Binomial_s,Tiempo_Fibonacci_s,MST_Coincide\n");

    FILE* csv_amortizado = fopen("resultados_costo_amortizado.csv", "w");
    if (!csv_amortizado) { perror("Error al crear resultados_costo_amortizado.csv"); fclose(csv_totales); return 1; }
    fprintf(csv_amortizado, "Serie,Configuracion,i,j,V,E,Estructura,Llamadas_DecreaseKey,Tiempo_Acumulado_s,Operaciones_Acumuladas\n");

    // ---- SERIE A (v fijo: i=20, j in {20,...,24}) - Costo total ----
    printf("\n>>> SERIE A: V fijo (i=20), E variable (j=20..24) <<<\n");
    {
        int i_A = 20;
        for (int j = 20; j <= 24; ++j) {
            if (i_A > MAX_I || j > MAX_J) continue;
            PromediosConfiguracion p = ejecutarExperimento(i_A, j);
            escribirFilaTotal(csv_totales, "A", p);
        }
    }

    // ---- SERIE B (e fijo: j=24, i in {18,...,22}) - Costo total ----
    printf("\n>>> SERIE B: E fijo (j=24), V variable (i=18..22) <<<\n");
    {
        int j_B = 24;
        for (int i = 18; i <= 22; ++i) {
            if (i > MAX_I || j_B > MAX_J) continue;
            PromediosConfiguracion p = ejecutarExperimento(i, j_B);
            escribirFilaTotal(csv_totales, "B", p);
        }
    }

    // ---- SERIE C (v fijo: i=18, j in {18,...,22}) - Costo amortizado ----
    printf("\n>>> SERIE C: Costo amortizado - V fijo (i=18), E variable (j=18..22) <<<\n");
    {
        int i_C = 18;
        for (int j = 18; j <= 22; ++j) {
            if (i_C > MAX_I || j > MAX_J) continue;
            PromediosConfiguracion p = ejecutarExperimento(i_C, j);
            escribirFilasAmortizado(csv_amortizado, "C", p);
        }
    }

    // ---- SERIE D (e fijo: j=22, i in {14,...,18}) - Costo amortizado ----
    printf("\n>>> SERIE D: Costo amortizado - E fijo (j=22), V variable (i=14..18) <<<\n");
    {
        int j_D = 22;
        for (int i = 14; i <= 18; ++i) {
            if (i > MAX_I || j_D > MAX_J) continue;
            PromediosConfiguracion p = ejecutarExperimento(i, j_D);
            escribirFilasAmortizado(csv_amortizado, "D", p);
        }
    }

    fclose(csv_totales);
    fclose(csv_amortizado);

    printf("\n===============================================================\n");
    printf(" Experimentos finalizados.\n");
    printf(" Resultados guardados en:\n");
    printf("   1. resultados_costo_total.csv\n");
    printf("   2. resultados_costo_amortizado.csv\n");
    printf("===============================================================\n");

    return 0;
}
