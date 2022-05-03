#pragma once

#include <hardware/i2c.h>
#include <hardware/pio.h>
#include <pico/stdlib.h>
#include <stdint.h>

#include "ov2640_defs.h"
struct ov2640_config {
    i2c_inst_t *sccb;
    uint pin_sioc;
    uint pin_siod;

    uint pin_resetb;
    uint pin_xclk;
    uint pin_vsync;

    uint pin_y2_pio_base;

    PIO pio;
    uint pio_sm;

    uint dma_channel;
    uint8_t *image_buf;
    size_t image_buf_size;
};

//
// APIs
//
void ov2640_init(const struct ov2640_config *config);
void ov2640_capture_frame(const struct ov2640_config *config);
bool ov2640_raw_write(const struct ov2640_config *config, uint8_t reg, uint8_t value);
uint8_t ov2640_raw_read(const struct ov2640_config *config, uint8_t reg);