#include "my_tasks.h"


QueueHandle_t xQueueSensorData;
SemaphoreHandle_t xSDCardBinarySemaphore;
SemaphoreHandle_t xCaptureBinarySemaphore;
SemaphoreHandle_t xReadBinarySemaphore;

typedef enum {
    IDLE,
    CAPTURE_RUNNING,
    READ_RUNNIG
} CaptureState;

volatile CaptureState capture_state = IDLE;
volatile uint32_t last_button_press_time = 0;
volatile bool cancel_capture = false;
static char filename[20] = "mpu6050.csv";   

bool sd_mounted = false;
int capture_index = 0;

FIL file;       
ssd1306_t ssd;  

// Inicializa as filas 
void init_queues() {
    xQueueSensorData = xQueueCreate(10, sizeof(SensorReadings));
    if (xQueueSensorData == NULL) {
        printf("Failed to create sensor data queue\n");
        panic_unsupported();
    }
}

// Inicializa os semáforos
void init_semaphores() {
    xSDCardBinarySemaphore = xSemaphoreCreateBinary();
    xCaptureBinarySemaphore = xSemaphoreCreateBinary();
    xReadBinarySemaphore = xSemaphoreCreateBinary();

    if (xSDCardBinarySemaphore == NULL || xCaptureBinarySemaphore == NULL || xReadBinarySemaphore == NULL) {
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
    start_display(&ssd);
    button_init_all(); // Inicializa os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
    led_init_all(); // Inicializa os LEDs
    buzzer_init_all(); // Inicializa os buzzers
    init_mpu6050(); // Inicializa o MPU6050

    printf("FatFS SPI example\n");
    printf("\033[2J\033[H"); // Limpa tela
    printf("\n> ");
    stdio_flush();

    white();   //Liga o led branco para indicar que o cartao esta desmontado
    status_display(&ssd, "SD status:", "Desmontado");  //Imprime no display que o cartao Desesta montado
}

//Função para alternar a montagem/desmontagem do SD card
void toggle_sd_card() {
    if (sd_mounted) {
        printf("Desmontando o SD. Aguarde...\n");
        run_unmount();
        status_display(&ssd, "Status SD:", "Desmontado");  //Imprime no display que o cartao Desesta montado
        printf("SD desmontado\n");
        black();
        sd_mounted = false;
    } else {
        yellow();
        printf("Montando o SD. Aguarde...\n");
        run_mount();
        status_display(&ssd, "Status SD:", "Montado");  //Imprime no display que o cartao esta montado
        printf("SD montado\n");
        sd_mounted = true;
    }
}

//Função para capturar/para a captura de dados do MPU
void toggle_capture() {
    if (capture_state == IDLE) {
        if(!sd_mounted){
            printf("Erro: Cartao SD Desmontado\n");
            status_display(&ssd, "Erro:", "SD Desmontado");
            return;
        }

        FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
        if (res != FR_OK) {
            printf("Erro ao abrir o arquivo: %d\n", res);
            status_display(&ssd, "Erro:", "Falha no arquivo");
            return;
        }

        UINT bw;
        const char *header = "sample, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z\n";
        f_write(&file, header, strlen(header), &bw);

        capture_state = CAPTURE_RUNNING;
        capture_index = 0;
        cancel_capture = false;
        play_success_sound();
        printf("Captura iniciada\n");
        status_display(&ssd, "Capturando:", "Dados");
        red();
    } else if (capture_state == CAPTURE_RUNNING) {
        f_close(&file);
        play_denied_sound();
        cancel_capture = true;
        capture_state = IDLE;
        printf("Captura cancelada\n");
        status_display(&ssd, "Captura:", "Cancelada");
        black();
    }
}

// Função de interrupção para gerenciar os botões
void irq_handler(uint gpio, uint32_t events){
        if (debounce(&last_button_press_time)){
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (gpio == BUTTON_A_PIN && capture_state == IDLE){
                xSemaphoreGiveFromISR(xSDCardBinarySemaphore, &xHigherPriorityTaskWoken);
            } else if (gpio == BUTTON_B_PIN && capture_state == IDLE ){
                xSemaphoreGiveFromISR(xCaptureBinarySemaphore, &xHigherPriorityTaskWoken);
            } else if (gpio == JOYSTICK_BUTTON_PIN && capture_state == IDLE){
                xSemaphoreGiveFromISR(xReadBinarySemaphore, &xHigherPriorityTaskWoken);
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
        xQueueSend(xQueueSensorData, &readings, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 1 segundo
    }
}

// Tarefa para gerenciar o cartão SD
void vSDCardTask(void *params) {
    while (1) {
        if (xSemaphoreTake(xSDCardBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            printf("Botao A pressionado\n");

            // Aguarda pequeno tempo para garantir que a tarefa de captura atualizou seu estado
            vTaskDelay(pdMS_TO_TICKS(200));

            if (capture_state == CAPTURE_RUNNING) {
                printf("Erro: Não é possível desmontar o SD durante a captura\n");
                status_display(&ssd, "Erro:", "Captura ativa");
                continue;
            }

            toggle_sd_card();
            status_display(&ssd, "Status SD:", sd_mounted ? "Montado" : "Desmontado");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa para capturar os dados do sensor
void vCaptureTask(void *params) {
    while (1) {
        if (xSemaphoreTake(xCaptureBinarySemaphore, 0) == pdTRUE) {
            toggle_capture();
        }
        if(sd_mounted && capture_state == IDLE)
            green();
        if (capture_state == CAPTURE_RUNNING) {
            if (capture_index >= TOTAL_SAMPLES || cancel_capture) {
                f_sync(&file);
                FRESULT res_close = f_close(&file);
                printf("f_close retornou: %d\n", res_close);
                printf("Salvando dados no arquivo %s\n", filename);
                
                if (cancel_capture) {
                    printf("Captura encerrada\n");
                    status_display(&ssd, "Captura:", "Encerrada");
                } else {
                    printf("Captura concluída\n");
                    status_display(&ssd, "Captura:", "Concluída");
                }                
                capture_state = IDLE;
            } else {
                SensorReadings readings;
                if (xQueueReceive(xQueueSensorData, &readings, pdMS_TO_TICKS(200)) == pdTRUE) {
                    char buffer[150];
                    printf("Capturando dados...\n");
                    printf("Acelerômetro: %f, %f, %f\n", readings.acel_x, readings.acel_y, readings.acel_z);
                    printf("Giroscópio: %f, %f, %f\n", readings.giro_x, readings.giro_y, readings.giro_z);
                    
                    sprintf(buffer, "%d, %f, %f, %f, %f, %f, %f\n", 
                            capture_index, 
                            readings.acel_x, readings.acel_y, readings.acel_z, 
                            readings.giro_x, readings.giro_y, readings.giro_z);
                    
                    UINT bw;
                    FRESULT res = f_write(&file, buffer, strlen(buffer), &bw);
                    capture_index++;

                    if (res != FR_OK) {
                        printf("Erro ao escrever no arquivo: %d\n", res);
                        status_display(&ssd, "Erro:", "Falha no arquivo");
                        for(int i = 0; i < 5; i++){
                            magenta();
                            pdMS_TO_TICKS(100);
                            black();
                        }
                        cancel_capture = true;
                    }

                    printf("Dados capturados: %d\n", capture_index);
                } else {
                    printf("Erro ao ler a fila de sensores\n");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa para ler os dados do arquivo
void vReadTask(void *params) {
    while (1) {
        if (xSemaphoreTake(xReadBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            if(!sd_mounted){
                printf("Erro: Cartao SD Desmontado\n");
                status_display(&ssd, "Erro:", "Desmontado");
                continue;
            }
            capture_state = READ_RUNNIG;
            status_display(&ssd, "Lendo:", "Arquivo");
            read_file(filename);  
            status_display(&ssd, "Leitura:", "Concluida");  
            capture_state = IDLE;     
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Função para inicializar as tarefas
void init_tasks() {
    xTaskCreate(vSensorTask, "Sensor Task", 256, NULL, 1, NULL);
    xTaskCreate(vSDCardTask, "SD Card Task", 1024, NULL, 1, NULL);
    xTaskCreate(vCaptureTask, "Capture Task", 1024, NULL, 1, NULL);
    xTaskCreate(vReadTask, "Read Task", 1024, NULL, 1, NULL);  // Tarefa estava faltando
}


