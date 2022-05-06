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
#include "log.h"
#include "usb_itf.h"

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

uint8_t image_buf[IMAGE_SIZE];

static queue_t msg_rx_q;
static mutex_t tx_mtx;

static const struct ov2640_config camera_cfg = {
    .sccb = I2C_CAM,
    .pin_sioc = CONFIG_PIN_CAM_SIOC,
    .pin_siod = CONFIG_PIN_CAM_SIOD,
    .pin_resetb = CONFIG_PIN_CAM_RESETB,
    .pin_xclk = CONFIG_PIN_CAM_XCLK,
    .pin_vsync = CONFIG_PIN_CAM_VSYNC,
    .pin_y2_pio_base = CONFIG_PIN_CAM_Y2_PIO_BASE,
    .pio = PIO_CAM,
    .pio_sm = 0,
    .dma_channel = 0,
    .image_buf = image_buf,
    .image_buf_size = IMAGE_SIZE,
};

static void core1_entry(void) {
    queue_init(&msg_rx_q, sizeof(rx_msg_t), 10);
    usb_itf_init();
    log_debug("done\n");

    while (1) {
        tud_task();

        if (tud_cdc_n_connected(USB_PORT)) {
            uint8_t buffer[BUFFER_MAX_SIZE];
            uint32_t len = usb_recvfrom_host(buffer, BUFFER_MAX_SIZE);
            if (len) {
                uint8_t *p_data = (uint8_t *)malloc(len);  // Remember free memory after using
                if (p_data) {
                    // Push received msg into queue
                    memcpy(p_data, buffer, len);

                    rx_msg_t rx_msg;
                    rx_msg.p_data = p_data;
                    rx_msg.len = len;
                    queue_try_add(&msg_rx_q, &rx_msg);
                    log_debug("added a request message\n");
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
            log_error("error format\n");
            return msg;
        }
        msg.reg = atoi(ptr);

        // Get value
        ptr = strtok(NULL, delim);
        if (ptr == NULL) {
            log_error("error format\n");
            return msg;
        }
        msg.val = atoi(ptr);

        log_info("write at reg 0x%x value 0x%x\n", msg.reg, msg.val);
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
            log_error("error format\n");
            return msg;
        }

        msg.reg = atoi(ptr);

        log_info("read at reg 0x%x\n", msg.reg);
        msg.cmd = OV_READ;
        return msg;
    }

    return msg;
}

static void event_worker(uint8_t *p_data, uint32_t len) {
    ov_msg_t msg = ov_msg_parse(p_data, len);

    switch (msg.cmd) {
        case OV_CAPTURE: {
            log_debug("Capturing ...\n");
            ov2640_capture_frame(&camera_cfg);

            uint32_t offset = 0;
            uint32_t length = camera_cfg.image_buf_size;
            bool result = true;

            // This will chop the request into CHUNK_SIZE byte writes
            while (result == true && length > offset) {
                if (length - offset >= CHUNK_SIZE) {
                    result = usb_sendto_host(&camera_cfg.image_buf[offset], CHUNK_SIZE);
                    offset += CHUNK_SIZE;
                    log_debug("chunk sent, offset %d\n", offset);
                } else {
                    result = usb_sendto_host(&camera_cfg.image_buf[offset], length - offset);
                    offset = length;
                    log_debug("last chunk, offset %d\n", offset);
                }

                // Take a rest
                sleep_ms(1);
            }

            log_debug("sent %d bytes\n", offset);
            break;
        }

        case OV_WRITE: {
            log_debug("Write at reg 0x%x value 0x%x\n", msg.reg, msg.val);
            ov2640_raw_write(&camera_cfg, msg.reg, msg.val);
            break;
        }

        case OV_READ: {
            uint8_t value = ov2640_raw_read(&camera_cfg, msg.reg);
            log_debug("Read at reg 0x%x value 0x%x\n", msg.reg, value);
            usb_sendto_host(&value, 1);
            break;
        }

        default:
            break;
    }
};

int main(void) {
    stdio_init_all();
    mutex_init(&tx_mtx);
    multicore_launch_core1(core1_entry);

    ov2640_init(&camera_cfg);
    ov2640_raw_write(&camera_cfg, 0xff, 0x01);

    uint8_t midh = ov2640_raw_read(&camera_cfg, 0x1C);
    uint8_t midl = ov2640_raw_read(&camera_cfg, 0x1D);
    log_info("MIDH = 0x%02x, MIDL = 0x%02x\n", midh, midl);

    while (1) {
        rx_msg_t msg;
        if (queue_try_remove(&msg_rx_q, &msg) && msg.p_data != NULL) {
            mutex_enter_blocking(&tx_mtx);
            event_worker(msg.p_data, msg.len);
            free(msg.p_data);
            mutex_exit(&tx_mtx);
        }
    }

    return 0;
}