#pragma once

#include <stdint.h>

#define OV2640_ADDR (0x60 >> 1)
#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

struct remap_regs {
    uint8_t reg;
    uint8_t val;
};

static const struct remap_regs ov2640_uxga_cif[] = {
    {.reg = 0xff, .val = 0x00},
    {.reg = 0xe0, .val = 0x04},
    {.reg = 0xc0, .val = 0xc8},
    {.reg = 0xc1, .val = 0x96},
    {.reg = 0x86, .val = 0x35},
    {.reg = 0x50, .val = 0x92},
    {.reg = 0x51, .val = 0x90},
    {.reg = 0x52, .val = 0x2c},
    {.reg = 0x53, .val = 0x00},
    {.reg = 0x54, .val = 0x00},
    {.reg = 0x55, .val = 0x88},
    {.reg = 0x57, .val = 0x00},
    {.reg = 0x5a, .val = 0x58},
    {.reg = 0x5b, .val = 0x48},
    {.reg = 0x5c, .val = 0x00},
    {.reg = 0xd3, .val = 0x08},
    {.reg = 0xe0, .val = 0x00},
};

static const struct remap_regs ov2640_vga[] = {
    {.reg = 0xff, .val = 0x00}, /* Device control register list Table 12 */
    {.reg = 0x2c, .val = 0xff}, /* Reserved                              */
    {.reg = 0x2e, .val = 0xdf}, /* Reserved                              */
    {.reg = 0xff, .val = 0x01}, /* Device control register list Table 13 */
    {.reg = 0x3c, .val = 0x32}, /* Reserved                              */
    {.reg = 0x11, .val = 0x00}, /* Clock Rate Control                    */
    {.reg = 0x09, .val = 0x02}, /* Common control 2                      */
    {.reg = 0x04, .val = 0xA8}, /* Mirror                                */
    {.reg = 0x13, .val = 0xe5}, /* Common control 8                      */
    {.reg = 0x14, .val = 0x48}, /* Common control 9                      */
    {.reg = 0x2c, .val = 0x0c}, /* Reserved                              */
    {.reg = 0x33, .val = 0x78}, /* Reserved                              */
    {.reg = 0x3a, .val = 0x33}, /* Reserved                              */
    {.reg = 0x3b, .val = 0xfB}, /* Reserved                              */
    {.reg = 0x3e, .val = 0x00}, /* Reserved                              */
    {.reg = 0x43, .val = 0x11}, /* Reserved                              */
    {.reg = 0x16, .val = 0x10}, /* Reserved                              */
    {.reg = 0x4a, .val = 0x81}, /* Reserved                              */
    {.reg = 0x21, .val = 0x99}, /* Reserved                              */
    {.reg = 0x24, .val = 0x40}, /* Luminance signal High range           */
    {.reg = 0x25, .val = 0x38}, /* Luminance signal low range            */
    {.reg = 0x26, .val = 0x82}, /*                                       */
    {.reg = 0x5c, .val = 0x00}, /* Reserved                              */
    {.reg = 0x63, .val = 0x00}, /* Reserved                              */
    {.reg = 0x46, .val = 0x3f}, /* Frame length adjustment               */
    {.reg = 0x0c, .val = 0x3c}, /* Common control 3                      */
    {.reg = 0x61, .val = 0x70}, /* Histogram algo low level              */
    {.reg = 0x62, .val = 0x80}, /* Histogram algo high level             */
    {.reg = 0x7c, .val = 0x05}, /* Reserved                              */
    {.reg = 0x20, .val = 0x80}, /* Reserved                              */
    {.reg = 0x28, .val = 0x30}, /* Reserved                              */
    {.reg = 0x6c, .val = 0x00}, /* Reserved                              */
    {.reg = 0x6d, .val = 0x80}, /* Reserved                              */
    {.reg = 0x6e, .val = 0x00}, /* Reserved                              */
    {.reg = 0x70, .val = 0x02}, /* Reserved                              */
    {.reg = 0x71, .val = 0x94}, /* Reserved                              */
    {.reg = 0x73, .val = 0xc1}, /* Reserved                              */
    {.reg = 0x3d, .val = 0x34}, /* Reserved                              */
    {.reg = 0x5a, .val = 0x57}, /* Reserved                              */
    {.reg = 0x12, .val = 0x00}, /* Common control 7                      */
    {.reg = 0x11, .val = 0x00}, /* Clock Rate Control                   2*/
    {.reg = 0x17, .val = 0x11}, /* Horiz window start MSB 8bits          */
    {.reg = 0x18, .val = 0x75}, /* Horiz window end MSB 8bits            */
    {.reg = 0x19, .val = 0x01}, /* Vert window line start MSB 8bits      */
    {.reg = 0x1a, .val = 0x97}, /* Vert window line end MSB 8bits        */
    {.reg = 0x32, .val = 0x36},
    {.reg = 0x03, .val = 0x0f},
    {.reg = 0x37, .val = 0x40},
    {.reg = 0x4f, .val = 0xbb},
    {.reg = 0x50, .val = 0x9c},
    {.reg = 0x5a, .val = 0x57},
    {.reg = 0x6d, .val = 0x80},
    {.reg = 0x6d, .val = 0x38},
    {.reg = 0x39, .val = 0x02},
    {.reg = 0x35, .val = 0x88},
    {.reg = 0x22, .val = 0x0a},
    {.reg = 0x37, .val = 0x40},
    {.reg = 0x23, .val = 0x00},
    {.reg = 0x34, .val = 0xa0},
    {.reg = 0x36, .val = 0x1a},
    {.reg = 0x06, .val = 0x02},
    {.reg = 0x07, .val = 0xc0},
    {.reg = 0x0d, .val = 0xb7},
    {.reg = 0x0e, .val = 0x01},
    {.reg = 0x4c, .val = 0x00},
    {.reg = 0xff, .val = 0x00},
    {.reg = 0xe5, .val = 0x7f},
    {.reg = 0xf9, .val = 0xc0},
    {.reg = 0x41, .val = 0x24},
    {.reg = 0xe0, .val = 0x14},
    {.reg = 0x76, .val = 0xff},
    {.reg = 0x33, .val = 0xa0},
    {.reg = 0x42, .val = 0x20},
    {.reg = 0x43, .val = 0x18},
    {.reg = 0x4c, .val = 0x00},
    {.reg = 0x87, .val = 0xd0},
    {.reg = 0x88, .val = 0x3f},
    {.reg = 0xd7, .val = 0x03},
    {.reg = 0xd9, .val = 0x10},
    {.reg = 0xd3, .val = 0x82},
    {.reg = 0xc8, .val = 0x08},
    {.reg = 0xc9, .val = 0x80},
    {.reg = 0x7d, .val = 0x00},
    {.reg = 0x7c, .val = 0x03},
    {.reg = 0x7d, .val = 0x48},
    {.reg = 0x7c, .val = 0x08},
    {.reg = 0x7d, .val = 0x20},
    {.reg = 0x7d, .val = 0x10},
    {.reg = 0x7d, .val = 0x0e},
    {.reg = 0x90, .val = 0x00},
    {.reg = 0x91, .val = 0x0e},
    {.reg = 0x91, .val = 0x1a},
    {.reg = 0x91, .val = 0x31},
    {.reg = 0x91, .val = 0x5a},
    {.reg = 0x91, .val = 0x69},
    {.reg = 0x91, .val = 0x75},
    {.reg = 0x91, .val = 0x7e},
    {.reg = 0x91, .val = 0x88},
    {.reg = 0x91, .val = 0x8f},
    {.reg = 0x91, .val = 0x96},
    {.reg = 0x91, .val = 0xa3},
    {.reg = 0x91, .val = 0xaf},
    {.reg = 0x91, .val = 0xc4},
    {.reg = 0x91, .val = 0xd7},
    {.reg = 0x91, .val = 0xe8},
    {.reg = 0x91, .val = 0x20},
    {.reg = 0x92, .val = 0x00},
    {.reg = 0x93, .val = 0x06},
    {.reg = 0x93, .val = 0xe3},
    {.reg = 0x93, .val = 0x02},
    {.reg = 0x93, .val = 0x02},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x04},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x03},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x93, .val = 0x00},
    {.reg = 0x96, .val = 0x00},
    {.reg = 0x97, .val = 0x08},
    {.reg = 0x97, .val = 0x19},
    {.reg = 0x97, .val = 0x02},
    {.reg = 0x97, .val = 0x0c},
    {.reg = 0x97, .val = 0x24},
    {.reg = 0x97, .val = 0x30},
    {.reg = 0x97, .val = 0x28},
    {.reg = 0x97, .val = 0x26},
    {.reg = 0x97, .val = 0x02},
    {.reg = 0x97, .val = 0x98},
    {.reg = 0x97, .val = 0x80},
    {.reg = 0x97, .val = 0x00},
    {.reg = 0x97, .val = 0x00},
    {.reg = 0xc3, .val = 0xef},
    {.reg = 0xff, .val = 0x00},
    {.reg = 0xba, .val = 0xdc},
    {.reg = 0xbb, .val = 0x08},
    {.reg = 0xb6, .val = 0x24},
    {.reg = 0xb8, .val = 0x33},
    {.reg = 0xb7, .val = 0x20},
    {.reg = 0xb9, .val = 0x30},
    {.reg = 0xb3, .val = 0xb4},
    {.reg = 0xb4, .val = 0xca},
    {.reg = 0xb5, .val = 0x43},
    {.reg = 0xb0, .val = 0x5c},
    {.reg = 0xb1, .val = 0x4f},
    {.reg = 0xb2, .val = 0x06},
    {.reg = 0xc7, .val = 0x00},
    {.reg = 0xc6, .val = 0x51},
    {.reg = 0xc5, .val = 0x11},
    {.reg = 0xc4, .val = 0x9c},
    {.reg = 0xbf, .val = 0x00},
    {.reg = 0xbc, .val = 0x64},
    {.reg = 0xa6, .val = 0x00},
    {.reg = 0xa7, .val = 0x1e},
    {.reg = 0xa7, .val = 0x6b},
    {.reg = 0xa7, .val = 0x47},
    {.reg = 0xa7, .val = 0x33},
    {.reg = 0xa7, .val = 0x00},
    {.reg = 0xa7, .val = 0x23},
    {.reg = 0xa7, .val = 0x2e},
    {.reg = 0xa7, .val = 0x85},
    {.reg = 0xa7, .val = 0x42},
    {.reg = 0xa7, .val = 0x33},
    {.reg = 0xa7, .val = 0x00},
    {.reg = 0xa7, .val = 0x23},
    {.reg = 0xa7, .val = 0x1b},
    {.reg = 0xa7, .val = 0x74},
    {.reg = 0xa7, .val = 0x42},
    {.reg = 0xa7, .val = 0x33},
    {.reg = 0xa7, .val = 0x00},
    {.reg = 0xa7, .val = 0x23},
    {.reg = 0xc0, .val = 0xc8},
    {.reg = 0xc1, .val = 0x96},
    {.reg = 0x8c, .val = 0x00},
    {.reg = 0x86, .val = 0x3d},
    {.reg = 0x50, .val = 0x92},
    {.reg = 0x51, .val = 0x90},
    {.reg = 0x52, .val = 0x2c},
    {.reg = 0x53, .val = 0x00},
    {.reg = 0x54, .val = 0x00},
    {.reg = 0x55, .val = 0x88},
    {.reg = 0x5a, .val = 0x50},
    {.reg = 0x5b, .val = 0x3c},
    {.reg = 0x5c, .val = 0x00},
    {.reg = 0xd3, .val = 0x04},
    {.reg = 0x7f, .val = 0x00},
    {.reg = 0xda, .val = 0x00},
    {.reg = 0xe5, .val = 0x1f},
    {.reg = 0xe1, .val = 0x67},
    {.reg = 0xe0, .val = 0x00},
    {.reg = 0xdd, .val = 0x7f},
    {.reg = 0x05, .val = 0x00},
    {.reg = 0xff, .val = 0x00},
    {.reg = 0xe0, .val = 0x04},
    {.reg = 0xc0, .val = 0xc8},
    {.reg = 0xc1, .val = 0x96},
    {.reg = 0x86, .val = 0x3d},
    {.reg = 0x50, .val = 0x92},
    {.reg = 0x51, .val = 0x90},
    {.reg = 0x52, .val = 0x2c},
    {.reg = 0x53, .val = 0x00},
    {.reg = 0x54, .val = 0x00},
    {.reg = 0x55, .val = 0x88},
    {.reg = 0x57, .val = 0x00},
    {.reg = 0x5a, .val = 0x50},
    {.reg = 0x5b, .val = 0x3c},
    {.reg = 0x5c, .val = 0x00},
    {.reg = 0xd3, .val = 0x04},
    {.reg = 0xe0, .val = 0x00},
    {.reg = 0xFF, .val = 0x00},
    {.reg = 0x05, .val = 0x00},
    {.reg = 0xDA, .val = 0x08},
    {.reg = 0xda, .val = 0x09},
    {.reg = 0x98, .val = 0x00},
    {.reg = 0x99, .val = 0x00},
    {.reg = 0x00, .val = 0x00},
    {.reg = 0xff, .val = 0x00},
    {.reg = 0xe0, .val = 0x04},
    {.reg = 0xc0, .val = 0xc8},
    {.reg = 0xc1, .val = 0x96},
    {.reg = 0x86, .val = 0x3d},
    {.reg = 0x50, .val = 0x89},
    {.reg = 0x51, .val = 0x90},
    {.reg = 0x52, .val = 0x2c},
    {.reg = 0x53, .val = 0x00},
    {.reg = 0x54, .val = 0x00},
    {.reg = 0x55, .val = 0x88},
    {.reg = 0x57, .val = 0x00},
    {.reg = 0x5a, .val = 0xA0},
    {.reg = 0x5b, .val = 0x78},
    {.reg = 0x5c, .val = 0x00},
    {.reg = 0xd3, .val = 0x02},
    {.reg = 0xe0, .val = 0x00},
};