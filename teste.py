import subprocess
import matplotlib
matplotlib.use('Agg')  # Configura o backend para não interativo
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import re
from time import sleep
import plotly.graph_objects as go  # Para gráficos interativos

# Combinações de teste: (carros, bombas)
carros = list(range(25, 525, 25))
bombas = list(range(25, 525, 25))

# Gerando todas as combinações possíveis entre carros e bombas
testes = [(c, b) for c in carros for b in bombas]

resultados = []

print("Iniciando os testes...\n")

# Executando os testes e coletando os tempos
for carros, bombas in testes:
    print(f"Testando com {carros} carros e {bombas} bombas...")

    try:
        # Executa o programa com o comando 'time' e captura stdout + stderr
        comando = ["/usr/bin/time", "-p", "./main", str(carros), str(bombas)]
        processo = subprocess.run(comando, capture_output=True, text=True)

        # Captura a saída do stderr (onde o 'time -p' escreve)
        stderr = processo.stderr

        # Expressão regular para pegar o tempo real
        match = re.search(r"real (\d+\.\d+)", stderr)
        if match:
            tempo_real = float(match.group(1))
            print(f"✔ Tempo registrado: {tempo_real} segundos\n")
            resultados.append((carros, bombas, tempo_real))
        else:
            print("❌ Tempo não encontrado. Ignorando esse teste.\n")

    except Exception as e:
        print(f"Erro ao executar o programa: {e}")

    sleep(1)  # Pequena pausa entre testes


# Transformando os dados em DataFrame
df = pd.DataFrame(resultados, columns=["CARROS", "BOMBAS", "TEMPO_REAL"])

# --- Gráfico 3D Interativo ---
tabela = df.pivot_table(
    index='CARROS',
    columns='BOMBAS',
    values='TEMPO_REAL'
)

x_vals = tabela.columns.values   # bombas
y_vals = tabela.index.values     # carros
z_vals = tabela.values           # matriz [carros × bombas]

fig_3d = go.Figure()
fig_3d.add_trace(go.Surface(
    x=x_vals,
    y=y_vals,
    z=z_vals,
    colorscale='Viridis'
))
fig_3d.update_layout(
    title="Tempo de Execução vs Carros e Bombas",
    scene=dict(
        xaxis_title="Bombas",
        yaxis_title="Carros",
        zaxis_title="Tempo (s)"
    )
)

# Salva o gráfico interativo como HTML
fig_3d.write_html("grafico_tempo.html")
print("Gráfico interativo salvo como 'grafico_tempo.html'.")

# --- Heatmap Melhorado ---
plt.figure(figsize=(10, 8))
tabela_heatmap = df.pivot_table(index='CARROS', columns='BOMBAS', values='TEMPO_REAL')

sns.heatmap(
    tabela_heatmap,
    annot=True,
    fmt=".2f",
    cmap="coolwarm",
    linewidths=0.5,
    cbar_kws={'label': 'Tempo (s)'}
)

plt.title("Mapa de Calor do Tempo de Execução", fontsize=16)
plt.xlabel("Bombas de Combustível", fontsize=12)
plt.ylabel("Quantidade de Carros", fontsize=12)
plt.tight_layout()

# Salva o heatmap como PNG
plt.savefig("heatmap.png")
print("Heatmap salvo como 'heatmap.png'.")

# Salvando resultados em CSV
df.to_csv("resultados_execucao.csv", index=False)
print("\n✅ Resultados salvos em 'resultados_execucao.csv'")
