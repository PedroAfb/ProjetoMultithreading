import subprocess
import matplotlib
matplotlib.use('Agg')  # Configura o backend para não interativo
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import re
from time import sleep

# Combinações de teste: (carros, bombas)
testes = [
    (500, 10),
    (500, 50),
    (500, 100),
    (1000, 10),
    (1000, 50),
    (1000, 100),
    (1000, 200),
    (1500, 50),
    (1500, 100),
    (1500, 200),
    (3186, 45),
    (1253, 67),
    (951, 89),
    (2107, 32),
    (3768, 21),
    (1799, 54),
    (2632, 47),
    (1689, 66),
    (4012, 90),
    (1356, 55),
    (2473, 44),
    (1801, 63),
]

resultados = []

print("Iniciando os testes...\n")

# Executando os testes e coletando os tempos
for carros, bombas in testes:
    print(f"Testando com {carros} carros e {bombas} bombas...")

    try:
        # Executa o programa com o comando 'time' e captura stdout + stderr
        comando = ["/usr/bin/time", "-p", "./output/main", str(carros), str(bombas)]
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

# Plotando os gráficos
fig = plt.figure(figsize=(12, 6))

# Gráfico 3D
ax = fig.add_subplot(121, projection='3d')
x = df['CARROS']
y = df['BOMBAS']
z = df['TEMPO_REAL']

ax.plot_trisurf(x, y, z, cmap='viridis', edgecolor='none')
ax.set_title('Tempo de Execução vs Carros e Bombas')
ax.set_xlabel('Carros')
ax.set_ylabel('Bombas')
ax.set_zlabel('Tempo (s)')

# Heatmap
tabela_heatmap = df.pivot_table(index='CARROS', columns='BOMBAS', values='TEMPO_REAL')
ax2 = plt.subplot(122)
sns.heatmap(tabela_heatmap, annot=True, fmt=".1f", cmap='YlOrRd', cbar_kws={'label': 'Tempo (s)'}, ax=ax2)
ax2.set_title('Mapa de Calor do Tempo de Execução')
ax2.set_xlabel('Bombas de Combustível')
ax2.set_ylabel('Quantidade de Carros')

plt.tight_layout()

# Salva o gráfico em um arquivo PNG
plt.savefig("grafico_tempo_execucao.png")
print("Gráfico salvo como 'grafico_tempo_execucao.png'.")

# Salvando resultados em CSV
df.to_csv("resultados_execucao.csv", index=False)
print("\n✅ Resultados salvos em 'resultados_execucao.csv'")