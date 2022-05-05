#pragma once

#include <stdint.h>

//
// APIs
//
void usb_itf_init(void);
bool usb_sendto_host(const uint8_t *p_data, uint16_t len);
uint32_t usb_recvfrom_host(uint8_t *p_data, uint32_t len);
