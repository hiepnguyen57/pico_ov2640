#pragma once

#include <hardware/i2c.h>
#include <hardware/pio.h>

// #define CONFIG_NDEBUG

#define CONFIG_PIN_CAM_RESETB (2)
#define CONFIG_PIN_CAM_XCLK (3)
#define CONFIG_PIN_CAM_SIOD (4)  // I2C0 SDA
#define CONFIG_PIN_CAM_SIOC (5)  // I2C0 SCL
#define CONFIG_PIN_CAM_Y2_PIO_BASE (6)
#define CONFIG_PIN_CAM_VSYNC (16)  //   GP14 -> PCLK, GP15 -> HSYNC

#define I2C_CAM (i2c0)
#define PIO_CAM (pio0)

#define USB_PORT (0)
#define BUFFER_MAX_SIZE (256)
#define IMAGE_SIZE (352 * 288 * 2)
#define CHUNK_SIZE (200)