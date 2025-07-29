import pandas as pd
import matplotlib.pyplot as plt

SAMPLING_RATE = 100  # Hz
CSV_FILE = 'mpu6050.csv'

# Leitura do arquivo CSV
try:
    df = pd.read_csv(CSV_FILE)
except FileNotFoundError:
    print(f"Erro: Arquivo '{CSV_FILE}' não encontrado.")
    exit()
except Exception as e:
    print(f"Erro ao ler o arquivo CSV: {e}")
    exit()

# Criação da coluna de tempo (em segundos)
df['tempo'] = df['sample'] / SAMPLING_RATE

# Configuração da figura
plt.figure(figsize=(12, 8))

# Gráfico de Aceleração
plt.subplot(2, 1, 1)
plt.plot(df['tempo'], df['accel_x'], 'r-', label='Accel X', alpha=0.8)
plt.plot(df['tempo'], df['accel_y'], 'g-', label='Accel Y', alpha=0.8)
plt.plot(df['tempo'], df['accel_z'], 'b-', label='Accel Z', alpha=0.8)
plt.title('Aceleração nos Eixos XYZ')
plt.ylabel('Aceleração (g)')
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend(loc='upper right')

# Gráfico de Giroscópio
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
