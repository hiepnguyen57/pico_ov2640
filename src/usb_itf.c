/**
 * @file usb_itf.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <tusb.h>
#include <pico/stdlib.h>

#include "usb_itf.h"
#include "config_board.h"

static mutex_t _mtx;

static uint32_t usb_cdc_read_msg(uint8_t itf, uint8_t *p_buf, const uint32_t len);
static uint32_t usb_cdc_write_msg(uint8_t itf, const uint8_t *p_data, const uint32_t len);

void usb_itf_init(void) {
    tusb_init();
    mutex_init(&_mtx);
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

uint32_t usb_recvfrom_host(uint8_t *p_data, uint32_t len) {
    return usb_cdc_read_msg(USB_PORT, p_data, len);
}

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