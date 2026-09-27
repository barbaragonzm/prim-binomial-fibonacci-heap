
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')

CSV_TOTAL = 'resultados_costo_total.csv'
CSV_AMORT = 'resultados_costo_amortizado.csv'
OUT_DIR = '.'


def ajustar_constante(y_real, y_forma):
    """Ajusta C tal que C * y_forma ~= y_real (minimos cuadrados, C > 0)."""
    y_forma = np.asarray(y_forma, dtype=float)
    y_real = np.asarray(y_real, dtype=float)
    denom = np.sum(y_forma ** 2)
    if denom == 0:
        return 0.0
    c = float(np.sum(y_real * y_forma) / denom)
    return max(c, 1e-12)


# ---------------------------------------------------------------------------
#  - Costo total: 4 graficos
# ---------------------------------------------------------------------------
def graficar_costo_total():
    if not os.path.exists(CSV_TOTAL):
        print(f"[!] No se encontro {CSV_TOTAL}")
        return
    df = pd.read_csv(CSV_TOTAL)

    series_info = {
        'A': {'xcol': 'E', 'fijo_col': 'i', 'fijo_nombre': 'V'},
        'B': {'xcol': 'V', 'fijo_col': 'j', 'fijo_nombre': 'E'},
    }
    estructuras = {
        'Binomial':  {'col': 'Tiempo_Binomial_s',  'forma': lambda v, e: e * np.log2(v)},
        'Fibonacci': {'col': 'Tiempo_Fibonacci_s',  'forma': lambda v, e: e + v * np.log2(v)},
    }

    # Escala compartida por serie (mismo eje Y para las 2 estructuras de esa serie)
    for serie, info in series_info.items():
        sub = df[df['Serie'] == serie].sort_values(info['xcol'])
        if sub.empty:
            continue
        y_max = max(sub['Tiempo_Binomial_s'].max(), sub['Tiempo_Fibonacci_s'].max()) * 1.15
        valor_fijo = sub[info['fijo_col']].iloc[0]
        xlabel = f"Numero de {'aristas E' if info['xcol']=='E' else 'vertices V'} ({info['fijo_nombre']}=2^{valor_fijo} fijo)"

        for nombre_est, est in estructuras.items():
            y_real = sub[est['col']].values
            y_forma = est['forma'](sub['V'].values, sub['E'].values)
            c = ajustar_constante(y_real, y_forma)
            y_teorico = c * y_forma

            plt.figure(figsize=(8, 5))
            plt.plot(sub[info['xcol']], y_real, 'o-', color='navy', label=f'{nombre_est} (medido)')
            plt.plot(sub[info['xcol']], y_teorico, '--', color='crimson',
                     label=f'Cota teorica (C={c:.3e})')
            plt.title(f'Serie {serie}: Cola {nombre_est} - Tiempo total vs {info["xcol"]}')
            plt.xlabel(xlabel)
            plt.ylabel('Tiempo total (s)')
            plt.xscale('log', base=2)
            plt.ylim(0, y_max)
            plt.legend()
            plt.tight_layout()
            fname = f'grafico_costo_total_{nombre_est}_serie{serie}.png'
            plt.savefig(fname, dpi=300)
            plt.close()
            print(f"  -> {fname}")


# ---------------------------------------------------------------------------
#  Costo amortizado: 8 graficos
# ---------------------------------------------------------------------------
def graficar_costo_amortizado():
    if not os.path.exists(CSV_AMORT):
        print(f"[!] No se encontro {CSV_AMORT}")
        return
    df = pd.read_csv(CSV_AMORT)

    metricas = {
        'tiempo': {'col': 'Tiempo_Acumulado_s', 'ylabel': 'Tiempo acumulado en decreaseKey (s)'},
        'ops':    {'col': 'Operaciones_Acumuladas', 'ylabel': 'Operaciones estructurales acumuladas'},
    }
    # forma teorica de la operacion individual decreaseKey, multiplicada por
    # la cantidad de llamadas acumuladas (llamadas * O(log V) o llamadas * O(1))
    formas = {
        'Binomial':  lambda llamadas, v: llamadas * np.log2(v),
        'Fibonacci': lambda llamadas, v: llamadas * 1.0,
    }

    for serie in ['C', 'D']:
        df_serie = df[df['Serie'] == serie]
        if df_serie.empty:
            continue
        configs = sorted(df_serie['Configuracion'].unique(),
                          key=lambda c: df_serie[df_serie['Configuracion'] == c]['V'].iloc[0]
                          * df_serie[df_serie['Configuracion'] == c]['E'].iloc[0])

        for estructura in ['Binomial', 'Fibonacci']:
            for nombre_metrica, met in metricas.items():
                plt.figure(figsize=(8, 5))
                cmap = plt.cm.viridis(np.linspace(0, 1, max(len(configs), 2)))

                y_max = 0.0
                for idx, cfg in enumerate(configs):
                    sub = df_serie[(df_serie['Configuracion'] == cfg) &
                                    (df_serie['Estructura'] == estructura)].sort_values('Llamadas_DecreaseKey')
                    if sub.empty:
                        continue
                    v_val = sub['V'].iloc[0]
                    llamadas = sub['Llamadas_DecreaseKey'].values
                    y_real = sub[met['col']].values
                    y_max = max(y_max, y_real.max())

                    color = cmap[idx]
                    label_base = f"V=2^{sub['i'].iloc[0]}, E=2^{sub['j'].iloc[0]}"
                    plt.plot(llamadas, y_real, 'o-', color=color, label=label_base)

                    y_forma = formas[estructura](llamadas, v_val)
                    c = ajustar_constante(y_real, y_forma)
                    plt.plot(llamadas, c * y_forma, '--', color=color, alpha=0.6)

                plt.title(f'Serie {serie}: Cola {estructura} - {met["ylabel"]} vs llamadas a decreaseKey')
                plt.xlabel('Cantidad de llamadas a decreaseKey realizadas')
                plt.ylabel(met['ylabel'])
                plt.ylim(0, y_max * 1.15 if y_max > 0 else 1)
                plt.legend(fontsize=8)
                plt.tight_layout()
                fname = f'grafico_amortizado_{estructura}_{nombre_metrica}_serie{serie}.png'
                plt.savefig(fname, dpi=300)
                plt.close()
                print(f"  -> {fname}")


if __name__ == '__main__':
    print("Generando graficos de costo total ...")
    graficar_costo_total()
    print("Generando graficos de costo amortizado ...")
    graficar_costo_amortizado()
    print("Listo: se generaron los 12 graficos pedidos en el enunciado.")
