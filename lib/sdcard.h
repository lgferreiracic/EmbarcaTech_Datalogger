#ifndef SDCARD_H
#define SDCARD_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "rtc.h"
#include "sd_card.h"

typedef void (*p_fn_t)();
typedef struct
{
    char const *const command;
    p_fn_t const function;
    char const *const help;
} cmd_def_t;

sd_card_t *sd_get_by_name(const char *const name);
FATFS *sd_get_fs_by_name(const char *name);
void run_setrtc();
void run_format();
void run_mount();
void run_unmount();
void run_getfree();
void run_ls();
void run_cat();
void capture_adc_data_and_save();
void read_file(const char *filename);
void run_help();
void process_stdio(int cRxedChar);
void get_user_input();

#endif