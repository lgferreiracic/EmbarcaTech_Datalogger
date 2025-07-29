#include "my_tasks.h"


QueueHandle_t xQueueSensorData;
SemaphoreHandle_t xSDCardMutex;
SemaphoreHandle_t xCaptureMutex;

volatile uint32_t last_button_press_time = 0;
volatile bool capture_in_progress = false;
volatile bool cancel_capture = false;
static char filename[20] = "mpu6050.csv";   

bool sd_mounted = false;
bool capture_initialized = false;
int capture_index = 0;

FIL file;       
ssd1306_t ssd;  

// Inicializa as filas 
void init_queues() {
    xQueueSensorData = xQueueCreate(1, sizeof(SensorReadings));
    if (xQueueSensorData == NULL) {
        printf("Failed to create sensor data queue\n");
        panic_unsupported();
    }
}

// Inicializa os semáforos
void init_semaphores() {
    xSDCardMutex = xSemaphoreCreateBinary();
    xCaptureMutex = xSemaphoreCreateBinary();

    if (xSDCardMutex == NULL || xCaptureMutex == NULL) {
        printf("Failed to create semaphore\n");
        panic_unsupported();
    }
}

// Inicializa o hardware
void init_hardware() {
    stdio_init_all(); // Inicializa o console
    sleep_ms(5000); // Aguarda 5 segundos para o console ser inicializado
    time_init(); // Inicializa o relógio

    display_init(&ssd); // Inicializa o display
    button_init_all(); // Inicializa os botões
    led_init_all(); // Inicializa os LEDs
    buzzer_init_all(); // Inicializa os buzzers
    init_mpu6050(); // Inicializa o MPU6050

    printf("FatFS SPI example\n");
    printf("\033[2J\033[H"); // Limpa tela
    printf("\n> ");
    stdio_flush();

    white();   //Liga o led branco para indicar que o cartao esta desmontado
    status_display(&ssd, "SD status:", "Nao montado");  //Imprime no display que o cartao nao esta montado
    run_help();     //Chama a funcao para exibir os comandos disponiveis
}

//Função para alternar a montagem/desmontagem do SD card
void toggle_sd_card() {
    if (sd_mounted) {
        printf("Desmontando o SD. Aguarde...\n");
        run_unmount();
        status_display(&ssd, "Status SD:", "Desmontado");  //Imprime no display que o cartao nao esta montado
        printf("SD desmontado\n");
        sd_mounted = false;
    } else {
        printf("Montando o SD. Aguarde...\n");
        run_mount();
        status_display(&ssd, "Status SD:", "Montado");  //Imprime no display que o cartao esta montado
        printf("SD montado\n");
        yellow();
        sd_mounted = true;
    }
}

//Função para capturar/para a captura de dados do MPU
void toggle_capture() {
    if (capture_in_progress) {
        if(!sd_mounted){
            printf("Erro: Cartao SD nao montado\n");
            status_display(&ssd, "Erro:", "SD nao montado");  //Imprime no display que o cartao nao esta montado
            return;
        }

        FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (res != FR_OK) {
            printf("Erro ao abrir o arquivo: %d\n", res);
            status_display(&ssd, "Erro:", "Falha no arquivo");  //Imprime no display que o cartao nao esta montado
            return;
        }

        UINT bw;
        const char *header = "sample, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z\n";
        f_write(&file, header, strlen(header), &bw);

        capture_in_progress = true;
        capture_initialized = true;
        capture_index = 0;
        cancel_capture = false;
        play_success_sound();
        printf("Captura iniciada\n");
        status_display(&ssd, "Capturando:", "Dados");  //Imprime no display que o cartao esta montado
        red();
    } else {
        play_denied_sound();
        cancel_capture = true;
        printf("Captura cancelada\n");
        status_display(&ssd, "Captura:", "Cancelada");  //Imprime no display que o cartao esta montado
        black();
    }
}


// Função de interrupção para gerenciar os botões
void irq_handler(uint gpio, uint32_t events){
        if (debounce(&last_button_press_time)){
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (gpio == BUTTON_A_PIN){
                xSemaphoreGiveFromISR(xSDCardMutex, &xHigherPriorityTaskWoken);
            } else if (gpio == BUTTON_B_PIN){
                xSemaphoreGiveFromISR(xCaptureMutex, &xHigherPriorityTaskWoken);
            } else if (gpio == JOYSTICK_BUTTON_PIN){
                ssd1306_clear(&ssd); // Limpa o display
                clear_matrix(); // Limpa a matriz
                reset_usb_boot(0, 0); // Reinicia o dispositivo
            }
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Troca o contexto da tarefa
        }
}

// Tarefa para leitura dos sensores
void vSensorTask(void *params) {
    init_i2c_sensor();      
    init_mpu6050();
    yellow();
    while (1) {
        SensorReadings readings = get_sensor_readings();
        xQueueOverwrite(xQueueSensorData, &readings); // Atualiza a fila com as leituras dos sensores
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay de 1 segundo
    }
}

// Tarefa para gerenciar o display
void vDisplayTask(void *params) {
    display_init(&ssd); // Inicializa o display
    SensorReadings readings;

    while (1) {
        if (xQueuePeek(xQueueSensorData, &readings, portMAX_DELAY) == pdTRUE) {
            
            
            //xSemaphoreGive(xAlertMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100 ms
    }
}

// Tarefa para montar/dismontar o SD
void vSDCardTask(void *params) {
    while (1) {
        if (xSemaphoreTake(xSDCardMutex, portMAX_DELAY) == pdTRUE) {
            toggle_sd_card();
            sleep_ms(500);
            status_display(&ssd, "Status SD:", sd_mounted ? "Montado" : "Desmontado");  //Imprime no display que o cartao esta montado
            xSemaphoreGive(xSDCardMutex);
        }
    }
}

// Tarefa para capturar os dados do MPU6050 e salvar no SD card
void vCaptureTask(void *params) {
    while (1) {
        if (xSemaphoreTake(xCaptureMutex, portMAX_DELAY) == pdTRUE) {
            toggle_capture();
            xSemaphoreGive(xCaptureMutex);
        }
        if (capture_in_progress && capture_initialized) {
            if (capture_index >= TOTAL_SAMPLES || cancel_capture) {
                f_close(&file);
                capture_in_progress = false;
                capture_initialized = false;

                if(cancel_capture){
                    printf("Captura encerrada\n");
                    status_display(&ssd, "Captura:", "Encerrada");  //Imprime no display que o cartao esta montado
                }else{
                    printf("Captura concluida\n");
                    status_display(&ssd, "Captura:", "Concluida");  //Imprime no display que o cartao esta montado
                }
                green();
            }
        }
        else{
            SensorReadings readings;
            if (xQueuePeek(xQueueSensorData, &readings, portMAX_DELAY) == pdTRUE) {
                char buffer[100];
                sprintf(buffer, "%d, %f, %f, %f, %f, %f, %f\n", capture_index, readings.acel_x, readings.acel_y, readings.acel_z, readings.giro_x, readings.giro_y, readings.giro_z);
                UINT bw;
                FRESULT res = f_write(&file, buffer, strlen(buffer), &bw);
                capture_index++;
                if (res != FR_OK) {
                    printf("Erro ao escrever no arquivo: %d\n", res);
                    status_display(&ssd, "Erro:", "Falha no arquivo");  //Imprime no display que o cartao nao esta montado
                    magenta();
                    cancel_capture = true;
                }
                xSemaphoreGive(xCaptureMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100 ms
    }
}

// Tarefa para controlar os comandos do usuário
void vCommandTask(void *params) {
    while (1) {
        get_user_input();
        blue();
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100 ms
    }
}

// Função para inicializar as tarefas
void init_tasks() {
    xTaskCreate(vSensorTask, "Sensor Task", 256, NULL, 1, NULL);
    xTaskCreate(vSDCardTask, "SD Card Task", 256, NULL, 1, NULL);
    xTaskCreate(vCaptureTask, "Capture Task", 256, NULL, 1, NULL);
    xTaskCreate(vCommandTask, "Command Task", 256, NULL, 1, NULL);
}



