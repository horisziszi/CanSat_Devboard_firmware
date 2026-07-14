cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(Firmware)

target_sources(app PRIVATE
	src/main.c
	src/configure.c
	src/con/usb-print.c
	src/hal/battery.c
)
