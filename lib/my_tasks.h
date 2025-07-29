#ifndef MY_TASKS_H
#define MY_TASKS_H

#include "ssd1306.h"
#include "button.h"
#include "buzzer.h"
#include "led_rgb.h"
#include "sensors.h"
#include "sdcard.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#define TOTAL_SAMPLES 128

void init_queues();
void init_semaphores();
void init_tasks();
void init_hardware();

#endif // MY_TASKS_H
