/**
 * @file main.c
 * @author Hiep Nguyen (hoahiepk10@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>
#include <stdlib.h>
#include <string.h>
#include <tusb.h>
#include <stdbool.h>
#include <hardware/timer.h>
#include <inttypes.h>

#include "ov2640.h"

#define USB_PORT (0)
#define BUFFER_MAX_SIZE (256)

#define OV_CAPTURE_STR "OV+CAPTURE?"
#define OV_WRITE_STR "OV+WRITE="
#define OV_WRITE_STR_LEN (9)
#define OV_READ_STR "OV+READ="
#define OV_READ_STR_LEN (8)

typedef enum {
    OV_CAPTURE = 0,
    OV_WRITE,
    OV_READ,
    OV_UNKNOWN
} ov_cmd_t;

typedef struct {
    ov_cmd_t cmd;
    uint8_t reg;
    uint8_t val;
} ov_msg_t;

typedef struct {
    uint8_t *p_data;
    uint32_t len;
} rx_msg_t;

static const int PIN_CAM_SIOC = 5; // I2C0 SCL
static const int PIN_CAM_SIOD = 4; // I2C0 SDA
static const int PIN_CAM_RESETB = 2;
static const int PIN_CAM_XCLK = 3;
static const int PIN_CAM_VSYNC = 16;
static const int PIN_CAM_Y2_PIO_BASE = 6;

static const uint8_t CMD_REG_WRITE = 0xAA;
static const uint8_t CMD_REG_READ = 0xBB;
static const uint8_t CMD_CAPTURE = 0xCC;

static uint8_t image_buf[352*288*2];

static queue_t _rx_queue;
static mutex_t _mtx;
static mutex_t t_mtx;

struct ov2640_config _config;

static uint32_t usb_cdc_read_msg(uint8_t itf, uint8_t *p_buf, const uint32_t len) {
    uint32_t bytes_read = 0;
    mutex_enter_blocking(&_mtx);

    if (tud_cdc_n_available(itf)) {
        bytes_read = tud_cdc_n_read(itf, p_buf, len);
        tud_cdc_n_read_flush(itf);
    }

    mutex_exit(&_mtx);
    return bytes_read;
}

static uint32_t usb_cdc_write_msg(uint8_t itf, const uint8_t *p_data, const uint32_t len) {
    uint32_t bytes_write = 0;
    mutex_enter_blocking(&_mtx);

    bytes_write = tud_cdc_n_write(itf, p_data, len);
    if (bytes_write) {
        tud_cdc_n_write_flush(itf);
    }

    mutex_exit(&_mtx);
    return bytes_write;
}

bool usb_sendto_host(const uint8_t *p_data, uint16_t len) {
    bool result = false;
    uint8_t retry = 3;
    do {
        if (len == usb_cdc_write_msg(USB_PORT, p_data, len)) {
            result = true;
            break;
        }
    } while (retry--);

    return result;
}

static void tx_done(void) {
    usb_sendto_host("\r\n", strlen("\r\n"));
}

static void core1_entry(void) {
    tusb_init();
    mutex_init(&_mtx);
    queue_init(&_rx_queue, sizeof(rx_msg_t), 10);
    printf("%s: enter\n", __func__);

    while (1) {
        tud_task();

        if (tud_cdc_n_connected(USB_PORT)) {
            uint8_t buffer[BUFFER_MAX_SIZE];
            uint32_t len = usb_cdc_read_msg(USB_PORT, buffer, BUFFER_MAX_SIZE);
            if (len) {
                uint8_t *p_data = (uint8_t *)malloc(len);  // Remember free memory after using
                if (p_data) {
                    // Push received msg into queue
                    memcpy(p_data, buffer, len);

                    rx_msg_t rx_msg;
                    rx_msg.p_data = p_data;
                    rx_msg.len = len;
                    queue_try_add(&_rx_queue, &rx_msg);
                    printf("added a ov request message\n");
                }
            }
        }
    }
}

static ov_msg_t ov_msg_parse(uint8_t *p_data, uint32_t len) {
    char buf[32] = {0};
    memcpy(buf, p_data, len);

    ov_msg_t msg;
    msg.cmd = OV_UNKNOWN;

    // OV+CAPTURE?
    char capture_str[] = OV_CAPTURE_STR;
    if (strcmp(buf, capture_str) == 0) {
        msg.cmd = OV_CAPTURE;
        return msg;
    }

    // OV+WRITE=reg,val
    char write_str[] = OV_WRITE_STR;
    char write_hdr[OV_WRITE_STR_LEN + 1] = {0};
    strncpy(write_hdr, buf, OV_WRITE_STR_LEN);
    if (strcmp(write_hdr, write_str) == 0) {
        char val_str[32] = {0};
        memcpy(val_str, buf + OV_WRITE_STR_LEN, len - OV_WRITE_STR_LEN);

        char delim[] = ",";
        char *ptr = strtok(val_str, delim);

        // Get register
        if (ptr == NULL) {
            printf("%s: error format\n", __func__);
            return msg;
        }
        msg.reg = atoi(ptr);

        // Get value
        ptr = strtok(NULL, delim);
        if (ptr == NULL) {
            printf("%s: error format\n", __func__);
            return msg;
        }
        msg.val = atoi(ptr);

        printf("%s write at reg 0x%x value 0x%x\n", __func__, msg.reg, msg.val);
        msg.cmd = OV_WRITE;
        return msg;
    }

    // OV+READ=reg
    char read_str[] = OV_READ_STR;
    char read_hdr[OV_READ_STR_LEN + 1] = {0};
    strncpy(read_hdr, buf, OV_READ_STR_LEN);
    if (strcmp(read_hdr, read_str) == 0) {
        char val_str[16] = {0};
        memcpy(val_str, buf + OV_READ_STR_LEN, len - OV_READ_STR_LEN);

        char delim[] = ",";
        char *ptr = strtok(val_str, delim);

        // Get register
        if (ptr == NULL) {
            printf("%s: error format\n", __func__);
            return msg;
        }
        msg.reg = atoi(ptr);

        printf("%s read at reg 0x%x\n", __func__, msg.reg);
        msg.cmd = OV_READ;
        return msg;
    }

    return msg;
}

static void event_worker(uint8_t *p_data, uint32_t len) {
    ov_msg_t msg = ov_msg_parse(p_data, len);

    switch (msg.cmd) {
        case OV_CAPTURE: {
            printf("%s: Capturing ...\n", __func__);
            ov2640_capture_frame(&_config);

            uint16_t offset = 0;
            uint16_t length = _config.image_buf_size;
            bool result = true;

            // This will chop the request into 200 byte writes
            while (result == true && length > offset) {
                if (length - offset >= 200) {
                    result = usb_sendto_host(&_config.image_buf[offset], 200);
                    offset += 200;
                } else {
                    result = usb_sendto_host(&_config.image_buf[offset], length - offset);
                    offset = length;
                }

                // Take a rest
                sleep_ms(50);
            }

            printf("%s: sent %d bytes done\n", __func__, _config.image_buf_size);
            break;
        }

        case OV_WRITE: {
            printf("%s: Write at reg 0x%x value 0x%x\n", __func__, msg.reg, msg.val);
            ov2640_reg_write(&_config, msg.reg, msg.val);
            break;
        }

        case OV_READ: {
            uint8_t value = ov2640_reg_read(&_config, msg.reg);
            printf("%s: Read at reg 0x%x value 0x%x\n", __func__, msg.reg, value);
            usb_sendto_host(&value, 1);
            break;
        }

        default:
            break;
    }

    // transfer done
    tx_done();
};

int main(void) {
    stdio_init_all();
    multicore_launch_core1(core1_entry);
    mutex_init(&t_mtx);

    _config.sccb = i2c0;
    _config.pin_sioc = PIN_CAM_SIOC;
    _config.pin_siod = PIN_CAM_SIOD;

    _config.pin_resetb = PIN_CAM_RESETB;
    _config.pin_xclk = PIN_CAM_XCLK;
    _config.pin_vsync = PIN_CAM_VSYNC;
    _config.pin_y2_pio_base = PIN_CAM_Y2_PIO_BASE;

    _config.pio = pio0;
    _config.pio_sm = 0;

    _config.dma_channel = 0;
    _config.image_buf = image_buf;
    _config.image_buf_size = sizeof(image_buf);

    ov2640_init(&_config);

    ov2640_reg_write(&_config, 0xff, 0x01);
    uint8_t midh = ov2640_reg_read(&_config, 0x1C);
    uint8_t midl = ov2640_reg_read(&_config, 0x1D);

    printf("MIDH = 0x%02x, MIDL = 0x%02x\n", midh, midl);

    while (1) {
        rx_msg_t msg;
        if (queue_try_remove(&_rx_queue, &msg) && msg.p_data != NULL) {
            mutex_enter_blocking(&t_mtx);
            event_worker(msg.p_data, msg.len);
            free(msg.p_data);
            mutex_exit(&t_mtx);
        }
    }

    return 0;
}