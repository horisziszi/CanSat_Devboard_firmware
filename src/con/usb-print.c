#include "usb-print.h"

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(usb_print, LOG_LEVEL_INF);

#define USB_PRINT_RING_BUF_SIZE 1024

static uint8_t ring_buffer[USB_PRINT_RING_BUF_SIZE];
static struct ring_buf ringbuf;
static bool initialized;

static const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf), sizeof(buffer));

			if (len == 0) {
				uart_irq_rx_disable(dev);
				continue;
			}

			int recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len > 0) {
				ring_buf_put(&ringbuf, buffer, recv_len);
				uart_irq_tx_enable(dev);
			}
		}

		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));

			if (rb_len == 0) {
				uart_irq_tx_disable(dev);
				continue;
			}

			uart_fifo_fill(dev, buffer, rb_len);
		}
	}
}

int usb_print_init(bool usb_enabled)
{
	uint32_t dtr = 0;

	if (!usb_enabled) {
		LOG_INF("USB serial logging disabled by config");
		return 0;
	}

	if (initialized) {
		return 0;
	}

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("CDC ACM device not ready");
		return -ENODEV;
	}

	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	LOG_INF("Waiting for USB connection...");
	while (!dtr) {
		uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_DTR, &dtr);
		k_sleep(K_MSEC(100));
	}

	LOG_INF("DTR set, USB serial ready");
	printk("Cansat Devboard ready. Type a message and press Enter.\r\n");

	uart_irq_callback_set(uart_dev, interrupt_handler);
	uart_irq_rx_enable(uart_dev);

	initialized = true;

	return 0;
}
