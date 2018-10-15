#!/usr/bin/env python

# Plotting code mostly taken from: http://emptypipes.org/2013/11/09/matplotlib-multicategory-barchart/

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import operator as o

import numpy as np


datasets = [
    'WISH_34509',
    'WISH_34509_2x',
    'WISH_38423',
    'WISH_37828',
    'WISH_37868',
    'TOPAZ_3132',
    'SXD_23767',
]

data_mantid = np.array([
    214820,
    486264,
    5809,
    6451,
    2213,
    3185,
    1584,
], dtype=np.float)

data_prototype_32bit = np.array([
    32200,
    57877,
    717,
    930,
    468,
    1195,
    546,
], dtype=np.float)

data_prototype_64bit = np.array([
    29133,
    61581,
    820,
    1127,
    510,
    1232,
    635,
], dtype=np.float)

data_prototype_128bit = np.array([
    50431,
    0,
    1327,
    2003,
    736,
    1559,
    1146,
], dtype=np.float)

data_prototype_256bit = np.array([
    82654,
    0,
    2535,
    4432,
    1507,
    2371,
    2286,
], dtype=np.float)

# Normalise all data to Mantid execution times
data_prototype_32bit /= data_mantid
data_prototype_64bit /= data_mantid
data_prototype_128bit /= data_mantid
data_prototype_256bit /= data_mantid

# Create figure
fig = plt.figure()

def show_data(pos, ds, title):
    ax = fig.add_subplot(pos)

    p = ax.bar(datasets, ds)

    # Show label at top of bars
    for b in p.patches:
        height = b.get_height()
        ax.text(b.get_x() + b.get_width()/2., 1.05*height,
                '{0:.2f}'.format(height), ha='center', va='bottom', rotation='vertical')

    # Set the x-axis tick labels to be equal to the datasets
    ax.set_xticklabels(datasets)
    for tick in ax.get_xticklabels():
        tick.set_rotation(30)

    # Axis labels
    ax.set_ylabel("Factor of Mantid execution time")
    ax.set_xlabel("Dataset")

    # Grid lines
    ax.grid(b=True, axis='y', which='major', color='r', linestyle='-')

    # Set graph title
    ax.set_title(title)

show_data(221, data_prototype_32bit, '32bit Morton coordinates')
show_data(222, data_prototype_64bit, '64bit Morton coordinates')
show_data(223, data_prototype_128bit, '128bit Morton coordinates')
show_data(224, data_prototype_256bit, '256bit Morton coordinates')

plt.subplots_adjust(hspace=0.4, wspace=0.4)
fig.set_size_inches(15, 15)
plt.savefig('results.png')
