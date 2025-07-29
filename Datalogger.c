#include <stdio.h>
#include "pico/stdlib.h"
#include "my_tasks.h"

int main(){
    init_hardware(); // Inicializa o hardware
    init_queues(); // Inicializa as filas
    init_semaphores(); // Inicializa os semáforos
    init_tasks(); // Inicializa as tarefas
    vTaskStartScheduler(); // Inicializa o escalonador de tarefas
    return 0;
}
