#include <stdint.h>
#include <stdbool.h>


typedef struct{
    uint32_t uptime;
    uint16_t battery_percentage;
    uint16_t battery_voltage;
    uint16_t battery_current;

    
} Cansat_payload;