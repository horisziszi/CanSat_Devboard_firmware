/*
 * nPM1300 battery management HAL
 */

#include "battery.h"

#include <errno.h>
#include <limits.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/npm13xx_charger.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(battery, LOG_LEVEL_INF);

#define BATTERY_CHARGER_NODE DT_NODELABEL(npm1300_charger)

#if !DT_NODE_EXISTS(BATTERY_CHARGER_NODE)
#error "Missing devicetree node label: npm1300_charger"
#endif

/* A simple, conservative voltage-to-SoC mapping for 1-cell Li-ion/LiPo. */
#define BATTERY_EMPTY_MV 3300
#define BATTERY_FULL_MV  4200

static const struct device *const charger_dev = DEVICE_DT_GET(BATTERY_CHARGER_NODE);

static uint16_t estimate_percentage(uint16_t voltage_mv)
{
	if (voltage_mv <= BATTERY_EMPTY_MV) {
		return 0;
	}

	if (voltage_mv >= BATTERY_FULL_MV) {
		return 100;
	}

	return (uint16_t)(((uint32_t)(voltage_mv - BATTERY_EMPTY_MV) * 100U) /
				  (BATTERY_FULL_MV - BATTERY_EMPTY_MV));
}

int battery_init(void)
{
	if (!device_is_ready(charger_dev)) {
		LOG_ERR("nPM1300 charger device is not ready");
		return -ENODEV;
	}

	return 0;
}

int battery_read(battery_status_t *status)
{
	struct sensor_value voltage;
	struct sensor_value current;
	struct sensor_value chg_state;
	int ret;
	int32_t voltage_mv;
	int32_t current_ma;

	if (status == NULL) {
		return -EINVAL;
	}

	ret = sensor_sample_fetch(charger_dev);
	if (ret != 0) {
		return ret;
	}

	ret = sensor_channel_get(charger_dev, SENSOR_CHAN_GAUGE_VOLTAGE, &voltage);
	if (ret != 0) {
		return ret;
	}

	ret = sensor_channel_get(charger_dev, SENSOR_CHAN_GAUGE_AVG_CURRENT, &current);
	if (ret != 0) {
		return ret;
	}

	ret = sensor_channel_get(charger_dev, SENSOR_CHAN_NPM13XX_CHARGER_STATUS, &chg_state);
	if (ret != 0) {
		return ret;
	}

	voltage_mv = (int32_t)sensor_value_to_milli(&voltage);
	if (voltage_mv < 0) {
		voltage_mv = 0;
	}

	current_ma = (int32_t)sensor_value_to_milli(&current);

	status->voltage_mv = (uint16_t)MIN(voltage_mv, UINT16_MAX);
	status->percentage = estimate_percentage(status->voltage_mv);
	status->charging = (chg_state.val1 != 0);

	if (current_ma > INT16_MAX) {
		status->current_ma = INT16_MAX;
	} else if (current_ma < INT16_MIN) {
		status->current_ma = INT16_MIN;
	} else {
		status->current_ma = (int16_t)current_ma;
	}

	return 0;
}
