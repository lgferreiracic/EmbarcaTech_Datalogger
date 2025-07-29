# EmbarcaTech_EstacaoMeteorologica
<p align="center">
  <img src="Group 658.png" alt="EmbarcaTech" width="300">
</p>

## Projeto: Estação Meteorológica com Webserver

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![HTML](https://img.shields.io/badge/HTML-%23E34F26.svg?style=for-the-badge&logo=html5&logoColor=white)
![CSS](https://img.shields.io/badge/CSS-1572B6?style=for-the-badge&logo=css3&logoColor=fff)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-%23000000.svg?style=for-the-badge&logo=freertos&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Windows 11](https://img.shields.io/badge/Windows%2011-%230079d5.svg?style=for-the-badge&logo=Windows%2011&logoColor=white)

## Descrição do Projeto

Este projeto implementa uma **estação meteorológica inteligente** utilizando o microcontrolador **Raspberry Pi Pico W** com **FreeRTOS** para gerenciamento de tarefas em tempo real.

O sistema coleta dados ambientais através de sensores **BMP280** (temperatura, pressão e altitude) e **AHT20** (temperatura e umidade), exibindo as informações em um **display OLED SSD1306** e disponibilizando-as através de um **servidor web embarcado** com interface responsiva.

A arquitetura utiliza **múltiplas tarefas FreeRTOS** para operação concorrente, **filas** para comunicação entre tarefas, **semáforos** para sincronização e **timers** para controle de alertas, garantindo operação estável e eficiente.

## Componentes Utilizados

- **Microcontrolador Raspberry Pi Pico W (RP2040)**: Controle central com Wi-Fi integrado
- **Sensor BMP280 (I2C)**: Medição de temperatura, pressão atmosférica e altitude
- **Sensor AHT20 (I2C)**: Medição de temperatura e umidade relativa
- **Display SSD1306 OLED (I2C)**: GPIOs 14 e 15 - Exibição local dos dados
- **LED RGB**: GPIOs 11, 12 e 13 - Indicação visual de status
- **Botões A e B**: GPIOs 5 e 6 - Controle local
- **Botão do Joystick**: GPIO 22 - Alternância de display
- **Matriz de LEDs WS2812B**: GPIO 7 - Alertas visuais
- **Buzzers (1 e 2)**: GPIOs 10 e 21 - Alertas sonoros

## Ambiente de Desenvolvimento

- **VS Code** com extensão da Raspberry Pi Pico
- **Linguagem C** utilizando o **Pico SDK**
- **FreeRTOS** para gerenciamento de tarefas em tempo real
- **Biblioteca LwIP** para comunicação TCP/IP
- **HTML/CSS/JavaScript** para interface web responsiva
- **Chart.js** para gráficos em tempo real

## Arquitetura do Sistema

### Tarefas FreeRTOS
- **vSensorTask**: Coleta dados dos sensores BMP280 e AHT20
- **vAlertTask**: Gerencia alertas sonoros e visuais baseados em thresholds
- **vDisplayTask**: Atualiza display OLED com dados meteorológicos
- **vWebServerTask**: Servidor HTTP para interface web

### Comunicação
- **Filas**: Compartilhamento de dados entre tarefas
- **Semáforos**: Proteção de recursos compartilhados
- **Timers**: Controle de silenciamento de alertas (60 segundos)

## Funcionalidades

### 📊 Monitoramento em Tempo Real
- Temperatura (BMP280 e AHT20)
- Umidade relativa do ar
- Altitude calculada
- Atualização a cada 1 segundo

### 🖥️ Interface Web Responsiva
- Dashboard com gráficos em tempo real (Chart.js)
- Visualização de até 10 pontos históricos por sensor
- Status de conexão em tempo real
- Controle remoto de alertas
- Design responsivo para mobile e desktop

### ⚠️ Sistema de Alertas Inteligente
- Thresholds configuráveis para cada sensor
- Alertas visuais (LED RGB + Matriz WS2812B)
- Alertas sonoros (Buzzers)
- Silenciamento por 60 segundos (botão físico ou web)
- Reativação automática após período de silêncio

### 🎛️ Controles Locais
- **Botão A**: Reset do sistema
- **Botão B**: Silenciar alarmes

## Guia de Instalação

1. **Clone o repositório**
   ```bash
   git clone [URL_DO_REPOSITORIO]
   ```

2. **Configure o ambiente**
   - Instale VS Code com extensão Raspberry Pi Pico
   - Configure Pico SDK e toolchain

3. **Configure Wi-Fi**
   - Edite `WIFI_SSID` e `WIFI_PASSWORD` em `lib/web_server.h`

4. **Compile o projeto**
   - Use Ctrl+Shift+P → "Raspberry Pi Pico: Compile Project"

5. **Flash no dispositivo**
   - Conecte Pico W com botão BOOTSEL pressionado
   - Copie arquivo `.uf2` gerado para o dispositivo

## Guia de Uso

### Acesso Web
1. Conecte o Pico W à rede Wi-Fi
2. Observe o IP exibido no terminal serial
3. Acesse `http://[IP_DO_DISPOSITIVO]` no navegador
4. Monitore dados em tempo real e controle alertas

### Thresholds de Alerta (Configuráveis)
- **Temperatura**: > valor definido em `ALERT_THRESHOLD_TEMPERATURE`
- **Umidade**: > valor definido em `ALERT_THRESHOLD_HUMIDITY`  
- **Pressão**: > valor definido em `ALERT_THRESHOLD_PRESSURE`

### Display OLED
- **Modo Normal**: Dados meteorológicos + status do servidor
- **Modo Alerta**: Dados + indicação visual de alarme

## Testes e Validação

- ✅ Leitura precisa dos sensores I2C
- ✅ Comunicação Wi-Fi estável
- ✅ Interface web responsiva
- ✅ Sistema de alertas funcionando
- ✅ Sincronização entre tarefas FreeRTOS
- ✅ Controles locais e remotos
- ✅ Gráficos em tempo real
- ✅ Silenciamento temporizado de alertas

## Estrutura do Projeto

```
├── Estacao.c              # Arquivo principal
├── lib/
│   ├── my_tasks.c         # Implementação das tarefas FreeRTOS
│   ├── my_tasks.h         # Headers das tarefas
│   ├── web_server.c       # Servidor HTTP embarcado
│   ├── web_server.h       # Headers do servidor web
│   ├── sensors.c          # Interface com sensores
│   ├── sensors.h          # Headers dos sensores
│   ├── bmp280.c/h         # Driver sensor BMP280
│   ├── aht20.c/h          # Driver sensor AHT20
│   ├── ssd1306.c/h        # Driver display OLED
│   └── [outros drivers]   # LEDs, buzzers, botões, etc.
└── CMakeLists.txt         # Configuração de build
```

## Desenvolvedor

[Lucas Gabriel Ferreira](https://github.com/usuario-lider)

## Vídeo da Solução

[Link do YouTube](https://www.youtube.com/watch?v=1kkM6SFJDTk)
