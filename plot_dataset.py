#!/usr/bin/env python

from __future__ import print_function

import sys

import h5py
import matplotlib.pyplot as plt
import numpy as np


# Open HDF5 file
f = h5py.File(sys.argv[1], 'r')

# Read values
values = f[sys.argv[2]][:]
print(len(values), "values")

# Create histogram
hist = np.histogram(values, int(sys.argv[3]))

# Plot
plt.plot(hist[0])
plt.show()
