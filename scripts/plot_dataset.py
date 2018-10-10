#!/usr/bin/env python

# Space filling curve prototype for MD event data structure
# Copyright (C) 2018 European Spallation Source
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
