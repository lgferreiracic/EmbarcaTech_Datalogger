# EmbarcaTech_Datalogger
<p align="center">
  <img src="EmbarcaTechLogo.png" alt="EmbarcaTech" width="300">
</p>

## Projeto: Datalogger MPU6050 com FreeRTOS

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-%23000000.svg?style=for-the-badge&logo=freertos&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Windows 11](https://img.shields.io/badge/Windows%2011-%230079d5.svg?style=for-the-badge&logo=Windows%2011&logoColor=white)

## Descrição do Projeto

Este projeto implementa um **datalogger inteligente** utilizando o microcontrolador **Raspberry Pi Pico W** com **FreeRTOS** para gerenciamento de tarefas em tempo real.

O sistema coleta dados de movimento através do sensor **MPU6050** (acelerômetro e giroscópio), exibindo as informações em um **display OLED SSD1306** e armazenando-as em **cartão SD** no formato CSV.

A arquitetura utiliza **múltiplas tarefas FreeRTOS** para operação concorrente, **filas** para comunicação entre tarefas, **semáforos** para sincronização, garantindo operação estável e eficiente durante a coleta e armazenamento de dados.

## Componentes Utilizados

- **Microcontrolador Raspberry Pi Pico W (RP2040)**: Controle central
- **Sensor MPU6050 (I2C)**: Medição de aceleração e velocidade angular nos 3 eixos
- **Display SSD1306 OLED (I2C)**: GPIOs 14 e 15 - Exibição local dos dados e status
- **Cartão SD (SPI)**: Armazenamento dos dados coletados
- **LED RGB**: GPIOs 11, 12 e 13 - Indicação visual de status do sistema
- **Botões A e B**: GPIOs 5 e 6 - Controle local (montagem SD e captura)
- **Botão do Joystick**: GPIO 22 - Reset do sistema
- **Buzzers (1 e 2)**: GPIOs 10 e 21 - Feedback sonoro

## Ambiente de Desenvolvimento

- **VS Code** com extensão da Raspberry Pi Pico
- **Linguagem C** utilizando o **Pico SDK**
- **FreeRTOS** para gerenciamento de tarefas em tempo real
- **FatFS** para sistema de arquivos do cartão SD
- **CMake** para build system

## Arquitetura do Sistema

### Tarefas FreeRTOS
- **vSensorTask**: Coleta dados do sensor MPU6050 a cada 100ms
- **vSDCardTask**: Gerencia montagem/desmontagem do cartão SD
- **vCaptureTask**: Controla captura e gravação de dados no arquivo CSV
- **vReadTask**: Lê e exibe conteúdo dos arquivos salvos

### Comunicação
- **Filas**: Compartilhamento de dados de sensores entre tarefas
- **Semáforos Binários**: Sincronização de botões e controle de acesso
- **Estados**: Enum para controle do estado de captura

## Funcionalidades

### 📊 Coleta de Dados em Tempo Real
- Aceleração nos eixos X, Y, Z (em g)
- Velocidade angular nos eixos X, Y, Z (em °/s)
- Atualização a cada 100ms
- Armazenamento de até 128 amostras por arquivo

### 💾 Sistema de Armazenamento
- Gravação em cartão SD no formato CSV
- Arquivo: `mpu6050.csv`
- Formato: `sample, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z`
- Controle de montagem/desmontagem segura do SD

### 🖥️ Interface Local
- Display OLED mostra status do sistema
- Indicação visual do estado: SD montado/desmontado, capturando, lendo
- LEDs RGB para feedback visual:
  - **Branco**: SD desmontado
  - **Amarelo**: Montando/inicializando
  - **Verde**: SD montado e pronto
  - **Vermelho**: Capturando dados
  - **Magenta**: Erro no sistema

### 🎛️ Controles Locais
- **Botão A**: Montar/desmontar cartão SD
- **Botão B**: Iniciar/parar captura de dados
- **Botão Joystick**: Reset do sistema
- **Buzzers**: Feedback sonoro para sucesso/erro

## Guia de Instalação

1. **Clone o repositório**
   ```bash
   git clone [URL_DO_REPOSITORIO]
   ```

2. **Configure o ambiente**
   - Instale VS Code com extensão Raspberry Pi Pico
   - Configure Pico SDK e toolchain
   - Instale FreeRTOS Kernel

3. **Compile o projeto**
   - Use Ctrl+Shift+P → "Raspberry Pi Pico: Compile Project"

4. **Flash no dispositivo**
   - Conecte Pico W com botão BOOTSEL pressionado
   - Copie arquivo `.uf2` gerado para o dispositivo

## Guia de Uso

### Operação Básica
1. **Montar SD**: Pressione Botão A para montar o cartão SD
2. **Iniciar Captura**: Pressione Botão B para começar a gravar dados
3. **Parar Captura**: Pressione Botão B novamente para finalizar
4. **Ler Dados**: Pressione Botão do Joystick para exibir arquivo no terminal
5. **Reset**: Mantenha pressionado o Botão do Joystick

### Estados do Sistema
- **CAPTURE_IDLE**: Sistema pronto, aguardando comandos
- **CAPTURE_RUNNING**: Coletando e gravando dados (128 amostras)
- **CAPTURE_COMPLETED**: Captura finalizada com sucesso

### Indicadores Visuais
- **Display**: Mostra status atual e informações do sistema
- **LED RGB**: Indica estado operacional
- **Buzzers**: Confirmação sonora de ações

## Estrutura do Projeto

```
├── Datalogger.c           # Arquivo principal
├── hw_config.c            # Configuração de hardware
├── lib/
│   ├── my_tasks.c         # Implementação das tarefas FreeRTOS
│   ├── my_tasks.h         # Headers das tarefas
│   ├── mpu6050.c/h        # Driver sensor MPU6050
│   ├── sdcard.c/h         # Interface com cartão SD
│   ├── sensors.c/h        # Interface com sensores
│   ├── ssd1306.c/h        # Driver display OLED
│   ├── button.c/h         # Controle de botões
│   ├── led_rgb.c/h        # Controle LED RGB
│   ├── buzzer.c/h         # Controle buzzers
│   └── FatFs_SPI/         # Sistema de arquivos FatFS
└── CMakeLists.txt         # Configuração de build
```

## Testes e Validação

- ✅ Leitura precisa do sensor MPU6050
- ✅ Gravação estável em cartão SD
- ✅ Sistema de tarefas FreeRTOS funcionando
- ✅ Controles locais responsivos
- ✅ Indicadores visuais e sonoros
- ✅ Gerenciamento seguro de arquivos
- ✅ Coleta de 128 amostras por sessão

## Desenvolvedor

[Lucas Gabriel Ferreira](https://github.com/usuario-lider)

## Vídeo da Solução

[Link do YouTube](https://www.youtube.com/watch?v=1kkM6SFJDTk)
