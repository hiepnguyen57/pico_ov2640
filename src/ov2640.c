#include <hardware/dma.h>
#include <hardware/i2c.h>
#include <hardware/pwm.h>
#include <image.pio.h>
#include <stdio.h>

#include "ov2640.h"
#include "log.h"

static void ov2640_regs_write(const struct ov2640_config *config, const struct remap_regs *remap, uint32_t len);

void ov2640_init(const struct ov2640_config *config) {
    // XCLK generation (~20.83 MHz)
    gpio_set_function(config->pin_xclk, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(config->pin_xclk);
    pwm_set_wrap(slice_num, 5);  // 6 cycles (0 to 5), 125 MHz / 6 = ~20.83 MHz wrap rate
    pwm_set_gpio_level(config->pin_xclk, 3);
    pwm_set_enabled(slice_num, true);

    // SCCB I2C @ 100 kHz
    // Pull ups to keep signal high when no data is being sent
    i2c_init(config->sccb, 100 * 1000);
    gpio_set_function(config->pin_sioc, GPIO_FUNC_I2C);
    gpio_set_function(config->pin_siod, GPIO_FUNC_I2C);
    gpio_pull_up(config->pin_sioc);
    gpio_pull_up(config->pin_siod);

    // Initialise reset pin
    gpio_init(config->pin_resetb);
    gpio_set_dir(config->pin_resetb, GPIO_OUT);

    // Reset camera, and give it some time to wake back up
    gpio_put(config->pin_resetb, 0);
    sleep_ms(100);
    gpio_put(config->pin_resetb, 1);
    sleep_ms(100);

    // Initialise the camera itself over SCCB
    ov2640_regs_write(config, ov2640_vga, ARRAY_SIZE(ov2640_vga));
    ov2640_regs_write(config, ov2640_uxga_cif, ARRAY_SIZE(ov2640_uxga_cif));

    // Set RGB565 output mode
    ov2640_raw_write(config, 0xff, 0x00);
    ov2640_raw_write(config, 0xDA, (ov2640_raw_read(config, 0xDA) & 0xC) | 0x8);

    // Enable image RX PIO
    uint offset = pio_add_program(config->pio, &image_program);
    image_program_init(config->pio, config->pio_sm, offset, config->pin_y2_pio_base);
}

void ov2640_capture_frame(const struct ov2640_config *config) {
	dma_channel_config c = dma_channel_get_default_config(config->dma_channel);
	channel_config_set_read_increment(&c, false);
	channel_config_set_write_increment(&c, true);
	channel_config_set_dreq(&c, pio_get_dreq(config->pio, config->pio_sm, false));
	channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	    
	dma_channel_configure(
		config->dma_channel, &c,
		config->image_buf,
		&config->pio->rxf[config->pio_sm],
		config->image_buf_size,
		false
	);

	// Wait for vsync rising edge to start frame
	while (gpio_get(config->pin_vsync) == true);
	while (gpio_get(config->pin_vsync) == false);

	dma_channel_start(config->dma_channel);
	dma_channel_wait_for_finish_blocking(config->dma_channel);
}

bool ov2640_raw_write(const struct ov2640_config *config, uint8_t reg, uint8_t value) {
    uint8_t data[] = {reg, value};
    if (i2c_write_blocking(config->sccb, OV2640_ADDR, data, sizeof(data), false) > 0) {
        return true;
    } else {
        log_error("failed to write");
        return false;
    }
}

uint8_t ov2640_raw_read(const struct ov2640_config *config, uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(config->sccb, OV2640_ADDR, &reg, 1, false);
    i2c_read_blocking(config->sccb, OV2640_ADDR, &value, 1, false);
    return value;
}

static void ov2640_regs_write(const struct ov2640_config *config, const struct remap_regs *remap, uint32_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t reg = remap[i].reg;
        uint8_t val = remap[i].val;
        if (!ov2640_raw_write(config, reg, val)) {
            log_error("failed to write\n");
			break;
        }
		sleep_us(5); // Take a rest
    }
}