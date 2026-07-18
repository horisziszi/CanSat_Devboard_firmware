#include "battery.h"

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/mfd/npm13xx.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/sensor/npm13xx_charger.h>


#define I2C_NODE DT_NODELABEL(pmic)



