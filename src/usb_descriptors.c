#include <string.h>
#include <tusb.h>
//-------------------------------------------------------------------------------------------------+
// DEFINE MARCOS
//-------------------------------------------------------------------------------------------------+
#define PICO_USBD_VID (0x2E8A)  // Raspberry Pi
#define PICO_USBD_PID (0x000A)  // Raspberry Pi Pico SDK CDC

#define PICO_USBD_0_STR (0x00)
#define PICO_USBD_MANUF_STR (0x01)
#define PICO_USBD_PRODUCT_STR (0x02)
#define PICO_USBD_SERIAL_STR (0x03)
#define PICO_USBD_CDC_STR (0x04)
#define PICO_USBD_RESET_STR (0x05)

#define PICO_USBD_RESET_DESC_LEN  (9)
#define PICO_USBD_CDC_CMD_MAX_SIZE (8)
#define PICO_USBD_CDC_IN_OUT_MAX_SIZE (64)
#define PICO_USBD_MAX_POWER_MA (200)

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN * CFG_TUD_CDC)

// RP2040 USB ENDPOINTS
#define PICO_EPNUM_CDC_0_NOTIF (0x81)
#define PICO_EPNUM_CDC_0_DATA (0x02)
//-------------------------------------------------------------------------------------------------+
// DEFINE TYPES
//-------------------------------------------------------------------------------------------------+
enum {
    PICO_USBD_ITF_CDC_0 = 0,
    PICO_USBD_ITF_CDC_0_DATA,
    PICO_USBD_ITF_TOTAL
};
//-------------------------------------------------------------------------------------------------+
// MODULE VARIABLES
//-------------------------------------------------------------------------------------------------+
static uint16_t _desc_str[32];
//-------------------------------------------------------------------------------------------------+
// CONSTANTS
//-------------------------------------------------------------------------------------------------+
//
// Device Descriptors
//
static const tusb_desc_device_t usb_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = PICO_USBD_VID,
    .idProduct = PICO_USBD_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = PICO_USBD_MANUF_STR,
    .iProduct = PICO_USBD_PRODUCT_STR,
    .iSerialNumber = PICO_USBD_SERIAL_STR,

    .bNumConfigurations = 0x01
};

static uint8_t const desc_fs_cfg[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, PICO_USBD_ITF_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, PICO_USBD_MAX_POWER_MA),

    // 1st CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(PICO_USBD_ITF_CDC_0, 4, PICO_EPNUM_CDC_0_NOTIF,
                       PICO_USBD_CDC_CMD_MAX_SIZE, PICO_EPNUM_CDC_0_DATA, 0x80 | PICO_EPNUM_CDC_0_DATA,
                       PICO_USBD_CDC_IN_OUT_MAX_SIZE),
};

// Array of pointer to string descriptors
static const char *string_desc_arr[] = {
    [PICO_USBD_0_STR] = (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    [PICO_USBD_MANUF_STR] = "Raspberry",             // 1: Manufacturer
    [PICO_USBD_PRODUCT_STR] = "Pico",                // 2: Product
    [PICO_USBD_SERIAL_STR] = "1234",                 // 3: Serials, should use chip ID
    [PICO_USBD_CDC_STR] = "Board CDC",               // 4: CDC Interface
};
//-------------------------------------------------------------------------------------------------+
// PUBIC FUNCTIONS
//-------------------------------------------------------------------------------------------------+
// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&usb_desc_device;
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations
    return desc_fs_cfg;
}

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
            return NULL;

        const char *str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31)
            chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}
//-------------------------------------------------------------------------------------------------+