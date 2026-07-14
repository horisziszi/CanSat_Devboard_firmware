#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "con/usb-print.h"
#include "configuration.h"
#include "hal/battery.h"
#include "state.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static Cansat_payload payload;

int main(void)
{
	battery_status_t battery;
	bool battery_ready = false;
	const default_config_t *config = configuration_get();
	uint32_t task_period_s = MAX(config->usb_timer, 1U);
	int ret;

	ret = usb_print_init(config->usb_enabled);
	if (ret != 0) {
		LOG_WRN("USB serial logging init failed (%d)", ret);
	}

	if (battery_init() == 0) {
		battery_ready = true;
		LOG_INF("Battery management initialized");
	} else {
		LOG_WRN("Battery management not available");
	}

	while (1) {
		if (battery_ready && battery_read(&battery) == 0) {
			int32_t current_ma_abs = battery.current_ma;

			if (current_ma_abs < 0) {
				current_ma_abs = -current_ma_abs;
			}

			payload.uptime = k_uptime_get_32() / 1000U;
			payload.battery_percentage = battery.percentage;
			payload.battery_voltage = battery.voltage_mv;
			payload.battery_current = (uint16_t)MIN(current_ma_abs, (int32_t)UINT16_MAX);

			LOG_INF("BAT: %u%%, %u mV, %d mA, %s",
				payload.battery_percentage,
				payload.battery_voltage,
				battery.current_ma,
				battery.charging ? "charging" : "discharging");
		}

		k_sleep(K_SECONDS(task_period_s));
	}

	return 0;
}
