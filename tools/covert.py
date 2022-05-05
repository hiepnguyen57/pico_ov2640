import numpy as np
from PIL import Image
import sys

filename = sys.argv[1]

# Define width and height
w, h = 352, 288

# Read file using numpy "fromfile()"
with open(filename, mode='rb') as f:
    d = np.fromfile(f,dtype=np.uint8,count=w*h).reshape(h,w)

# Make into PIL Image and save
PILimage = Image.fromarray(d)
PILimage.save(filename + '.png')