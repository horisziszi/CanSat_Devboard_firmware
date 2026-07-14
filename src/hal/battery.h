/*
 * nPM1300 battery management HAL
 */

#ifndef HAL_BATTERY_H_
#define HAL_BATTERY_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint16_t voltage_mv;
	int16_t current_ma;
	uint16_t percentage;
	bool charging;
} battery_status_t;

int battery_init(void);
int battery_read(battery_status_t *status);

#endif /* HAL_BATTERY_H_ */
