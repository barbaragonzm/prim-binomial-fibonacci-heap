# Tarea 1 — Algoritmo de Prim y Análisis Amortizado
### CC4102 - Diseño y Análisis de Algoritmos

Implementación desde cero de una cola binomial y una cola de Fibonacci,
usadas como cola de prioridad del algoritmo de Prim para construir el árbol
cobertor mínimo (MST) de grafos aleatorios generados por el propio programa.

## Contenido del proyecto
├── grafo.h / grafo.c Generador de grafos aleatorios conexos
├── cola_binomial.h / cola_binomial.c Cola de prioridad binomial
├── cola_fibonacci.h / cola_fibonacci.c Cola de prioridad de Fibonacci
├── prim.h / prim.c Algoritmo de Prim (versión binomial y Fibonacci)
├── main.c Driver de experimentos (Series A, B, C, D)
├── generar_graficos.py Genera los 12 gráficos pedidos en el enunciado
└── README.md Este archivo

## Requisitos

- Compilador de C (GCC o Clang, C99 o superior).
- Python 3 con `pandas`, `matplotlib` y `numpy` (solo para los gráficos).

Instalación en Debian/Ubuntu:

```bash
sudo apt update && sudo apt install build-essential python3-pip
pip3 install pandas matplotlib numpy --break-system-packages
```

## Compilación

```bash
gcc -O2 -Wall -Wextra -o tarea1_prim grafo.c cola_binomial.c cola_fibonacci.c prim.c main.c -lm
```

(`-O2` es necesario; sin optimizaciones los tiempos medidos no sirven).

## Ejecución

```bash
./tarea1_prim
```

Corre automáticamente las 4 series del enunciado, 10 repeticiones cada una:

| Serie | Fijo | Variable | Mide |
|---|---|---|---|
| A | $V=2^{20}$ | $E=2^{20}$–$2^{24}$ | Costo total (6.3.1) |
| B | $E=2^{24}$ | $V=2^{18}$–$2^{22}$ | Costo total (6.3.1) |
| C | $V=2^{18}$ | $E=2^{18}$–$2^{22}$ | Costo amortizado (6.3.2) |
| D | $E=2^{22}$ | $V=2^{14}$–$2^{18}$ | Costo amortizado (6.3.2) |

Genera dos CSV:
- `resultados_costo_total.csv`: tiempos promedio (Series A/B) + verificación
  de que ambos MST pesan igual (`MST_Coincide`).
- `resultados_costo_amortizado.csv`: tiempo y operaciones acumuladas de
  `decreaseKey` (Series C/D).

**Tiempo estimado:** 20–60 minutos en un computador normal. Recomendado
correr en segundo plano:

La configuración más grande (Serie B, $V=2^{22}$, $E=2^{24}$) usa ~1 GB de
RAM; verificar con `free -h` antes de correr en máquinas con poca memoria.

## Gráficos

```bash
python3 generar_graficos.py
```

Genera los 12 PNG pedidos en 6.3: 4 de costo total (binomial/Fibonacci ×
Serie A/B) y 8 de costo amortizado (binomial/Fibonacci × tiempo/operaciones ×
Serie C/D), cada uno con su cota teórica ajustada.

## Flujo completo

```bash
gcc -O2 -Wall -Wextra -o tarea1_prim grafo.c cola_binomial.c cola_fibonacci.c prim.c main.c -lm
./tarea1_prim
pip3 install pandas matplotlib numpy --break-system-packages   # solo la primera vez
python3 generar_graficos.py
```

Al terminar quedan en la carpeta los 2 CSV y los 12 PNG, listos para el informe.