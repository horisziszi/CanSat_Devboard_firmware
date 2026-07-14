#include "configuration.h"

static const default_config_t default_config = {
	.lora_frequency = 868000000U,
	.lora_power = 14U,
	.lora_timer = 30U,
	.lora_enabled = false,
	.sd_card_timer = 5U,
	.sd_card_enabled = false,
	.usb_timer = 1U,
	.usb_enabled = true,
};

const default_config_t *configuration_get(void)
{
	return &default_config;
}


