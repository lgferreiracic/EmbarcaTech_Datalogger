import pandas as pd
import matplotlib.pyplot as plt
import os

SAMPLING_RATE = 100  # Hz

CSV_FILE = 'ArquivoDados\\mpu6050.csv'
CSV_PATH = os.path.abspath(CSV_FILE)
print(f"Tentando abrir o arquivo em: {CSV_PATH}")

# Leitura do CSV com separador correto (vírgula) e remoção de BOM
try:
    df = pd.read_csv(CSV_PATH, sep=',', encoding='utf-8-sig')
    df.columns = df.columns.str.strip().str.lower()  # limpa espaços e padroniza
    print("Colunas reconhecidas:", df.columns.tolist())
except FileNotFoundError:
    print(f"Erro: Arquivo '{CSV_PATH}' não encontrado.")
    exit()
except Exception as e:
    print(f"Erro ao ler o arquivo CSV: {e}")
    exit()

# Criação da coluna de tempo
df['tempo'] = df['sample'] / SAMPLING_RATE

# Gráfico
plt.figure(figsize=(12, 8))

# Aceleração
plt.subplot(2, 1, 1)
plt.plot(df['tempo'], df['accel_x'], 'r-', label='Accel X', alpha=0.8)
plt.plot(df['tempo'], df['accel_y'], 'g-', label='Accel Y', alpha=0.8)
plt.plot(df['tempo'], df['accel_z'], 'b-', label='Accel Z', alpha=0.8)
plt.title('Aceleração nos Eixos XYZ')
plt.ylabel('Aceleração (g)')
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend(loc='upper right')

# Giroscópio
plt.subplot(2, 1, 2)
plt.plot(df['tempo'], df['gyro_x'], 'r-', label='Gyro X', alpha=0.8)
plt.plot(df['tempo'], df['gyro_y'], 'g-', label='Gyro Y', alpha=0.8)
plt.plot(df['tempo'], df['gyro_z'], 'b-', label='Gyro Z', alpha=0.8)
plt.title('Velocidade Angular nos Eixos XYZ')
plt.xlabel('Tempo (s)')
plt.ylabel('Velocidade Angular (°/s)')
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend(loc='upper right')

plt.tight_layout()
plt.show()
