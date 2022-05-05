import atexit
import IPython
import math
from PIL import Image
import serial
import struct
import sys

ser = serial.Serial(sys.argv[1], 115200)
atexit.register(lambda: ser.close())

#
# PRIVATE FUNCTIONS
#


def _raw_write(reg, val):
    ser.reset_input_buffer
    ser.reset_output_buffer
    assert isinstance(reg, int)
    assert 0x00 <= reg <= 0xFF
    assert isinstance(val, int)
    assert 0x00 <= val <= 0xFF

    ser.write(struct.pack('OV+WRITE=', reg, ',', val))


def _raw_read(reg):
    ser.reset_input_buffer
    ser.reset_output_buffer
    assert isinstance(reg, int)
    assert 0x00 <= reg <= 0xFF

    ser.write(struct.pack('OV+READ=', reg))
    return ser.read(1)[0]


def _set_bit(reg, bit):
    val = _raw_read(reg)
    val |= 1 << bit
    _raw_write(reg, val)


def _clear_bit(reg, bit):
    val = _raw_read(reg)
    val &= 0xFF ^ (1 << bit)
    _raw_write(reg, val)

#
# PUBLIC FUNCTIONS
#


def set_hflip(hflip):
    _raw_write(0xFF, 0x11)
    if hflip:
        _set_bit(0x04, 7)
    else:
        _clear_bit(0x04, 7)


def set_vflip(vflip):
    _raw_write(0xFF, 0x01)
    if vflip:
        _set_bit(0x04, 6)
    else:
        _clear_bit(0x04, 6)


def capture(filename):
    ser.reset_input_buffer
    ser.reset_output_buffer
    ser.write(struct.pack('OV+CAPTURE?'))

    raw = ser.read(352*288*2)
    img = Image.new('RGB', (352, 288))
    width, height = img.size
    data = img.load()

    for y in range(height):
        for x in range(width):
            idx = y * width + x
            v = struct.unpack('<H', raw[2*idx:2*(idx+1)])[0]

            r, g, b = v >> (5 + 6), (v >> 5) & 0b111111, v & 0b11111

            r = math.floor(r / 0x1f * 0xff)
            g = math.floor(g / 0x3f * 0xff)
            b = math.floor(b / 0x1f * 0xff)
            data[x, y] = (r, g, b)

    img.save(filename)


IPython.embed()
