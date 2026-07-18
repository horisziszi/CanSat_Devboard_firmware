#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/kernel.h>

typedef struct{
    uint32_t uptime;
    uint16_t battery_percentage;
    uint16_t battery_voltage;
    uint16_t battery_current;
    uint16_t battery_temperature;

    
} Cansat_payload;

extern Cansat_payload cansat_payload;
extern struct k_mutex cansat_payload_lock;