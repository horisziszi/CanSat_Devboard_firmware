# CanSat Devboard Firmware

Firmware for the CanSat development board. This project is a **test and evaluation platform** — it is not meant to be a final product. The goal is to try out the board's hardware features, validate that each subsystem works as expected, and identify any technical difficulties before committing to a full design.

## What this firmware does

Right now the firmware is a starting point for board bring-up. It currently:

- Boots on the nRF52840-based CanSat devboard using Zephyr RTOS.
- Exposes a USB CDC ACM serial console so you can talk to the board over USB.
- Echoes characters received over the serial port (simple UART loopback to confirm the USB serial path works).

The rest of the hardware is described in the board definition and ready to be exercised in future tests.

## Hardware under test

The devboard includes the following subsystems we want to evaluate:

| Subsystem | Chip / Interface | Purpose |
| --- | --- | --- |
| MCU | nRF52840 (QIAA) | Main controller |
| LoRa radio | Semtech SX1262 over SPI2 | Long-range radio link |
| microSD card | SD card over SPI3 | Data logging / file system |
| Battery management | Nordic nPM1300 over I2C0 | Power, charging, battery monitoring |
| USB serial | CDC ACM | Console / debug interface |

## Project layout

```
src/main.c                              Entry point — USB serial echo test
prj.conf                                Zephyr Kconfig configuration
CMakeLists.txt                          Build definition
boards/ksp/cansat_devboard/             Custom board definition (DTS, pinctrl, Kconfig)
pm_static_cansat_devboard_nrf52840.yml  Static partition map for MCUboot
```

## Building

This firmware is built with the nRF Connect SDK (NCS) and Zephyr. From the project directory:

```bash
west build -b cansat_devboard
```

Then flash to the board:

```bash
west flash
```

## Using it

Once flashed and connected over USB, open a serial terminal (e.g. the nRF Connect Serial Terminal, `minicom`, or `screen`) at any baud rate — CDC ACM does not require a specific baud. You should see:

```
Cansat Devboard ready. Type a message and press Enter.
```

Anything you type is echoed back, confirming the USB serial path is working.

## Status

This is an early-stage bring-up firmware. Each subsystem (LoRa, SD card, PMIC) will be tested incrementally as the project progresses. See the board device tree for the full hardware description.

## License

See `LICENSE.txt`.
