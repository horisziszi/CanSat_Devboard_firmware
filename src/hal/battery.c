#include "battery.h"
#include "../state.h"

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/mfd/npm13xx.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/sensor/npm13xx_charger.h>


#define NPM13XX_DEVICE(label) DEVICE_DT_GET(DT_NODELABEL(label))

static const struct device *pmic =
    DEVICE_DT_GET(DT_NODELABEL(pmic));

static const struct device *buck1 =
    DEVICE_DT_GET(DT_NODELABEL(buck1));

static const struct device *buck2 =
    DEVICE_DT_GET(DT_NODELABEL(buck2));

static const struct device *ldo1 =
    DEVICE_DT_GET(DT_NODELABEL(ldo1));

static const struct device *charger =
    DEVICE_DT_GET(DT_NODELABEL(npm13xx_charger));


static void event_callback(const struct device *dev, struct gpio_callback *cb, uint32_t events)
{
	if (events & BIT(NPM13XX_EVENT_SHIPHOLD_PRESS)) {
		printk("SHPHLD pressed\n");
	}
	if (events & BIT(NPM13XX_EVENT_SHIPHOLD_RELEASE)) {
		printk("SHPHLD released\n");
	}
}

void configure_events(void)
{
	if (!device_is_ready(pmic)) {
		printk("Pmic device not ready.\n");
		return;
	}

	/* Setup callback for shiphold button press */
	static struct gpio_callback event_cb;

	gpio_init_callback(&event_cb, event_callback, BIT(NPM13XX_EVENT_SHIPHOLD_PRESS) |
			BIT(NPM13XX_EVENT_SHIPHOLD_RELEASE));

	mfd_npm13xx_add_callback(pmic, &event_cb);
}

void read_sensors(void)
{
	struct sensor_value volt;
	struct sensor_value current;
	struct sensor_value temp;
	struct sensor_value error;
	struct sensor_value status;
	struct sensor_value vbus_present;

	sensor_sample_fetch(charger);
	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_VOLTAGE, &volt);
	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_AVG_CURRENT, &current);
	sensor_channel_get(charger, SENSOR_CHAN_GAUGE_TEMP, &temp);
	sensor_channel_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM13XX_CHARGER_STATUS, &status);
	sensor_channel_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM13XX_CHARGER_ERROR, &error);
	sensor_attr_get(charger, (enum sensor_channel)SENSOR_CHAN_NPM13XX_CHARGER_VBUS_STATUS,
			(enum sensor_attribute)SENSOR_ATTR_NPM13XX_CHARGER_VBUS_PRESENT,
			&vbus_present);

	printk("V: %d.%03d ", volt.val1, volt.val2 / 1000);

	printk("I: %s%d.%04d ", ((current.val1 < 0) || (current.val2 < 0)) ? "-" : "",
	    abs(current.val1), abs(current.val2) / 100);

	printk("T: %s%d.%02d\n", ((temp.val1 < 0) || (temp.val2 < 0)) ? "-" : "", abs(temp.val1),
	    abs(temp.val2) / 10000);

	printk("Charger Status: %d, Error: %d, VBUS: %s\n", status.val1, error.val1,
	    vbus_present.val1 ? "connected" : "disconnected");

	
}


void battery_thread(void)
{
    k_mutex_lock(&cansat_payload_lock, K_FOREVER);

    cansat_payload.battery_percentage = 85;
    cansat_payload.battery_voltage = 3950;

    k_mutex_unlock(&cansat_payload_lock);
}