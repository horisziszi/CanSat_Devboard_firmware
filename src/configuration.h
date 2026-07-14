#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint32_t lora_frequency;
	uint32_t lora_power;
	uint32_t lora_timer;
	bool lora_enabled;

	uint32_t sd_card_timer;
	bool sd_card_enabled;

	uint32_t usb_timer;
	bool usb_enabled;
} default_config_t;

const default_config_t *configuration_get(void);

#endif /* CONFIGURATION_H_ */