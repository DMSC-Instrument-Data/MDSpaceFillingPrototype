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
    214.820,
    486.264,
    5.809,
    6.451,
    2.213,
    3.185,
    1.584,
])

data_prototype_32bit = np.array([
    32.200,
    57.877,
    0.717,
    0.930,
    0.468,
    1.195,
    0.546,
])

data_prototype_64bit = np.array([
    29.133,
    61.581,
    0.820,
    1.127,
    0.510,
    1.232,
    0.635,
])

data_prototype_128bit = np.array([
    50.431,
    0.0,
    1.327,
    2.003,
    0.736,
    1.559,
    1.146,
])

data_prototype_256bit = np.array([
    82.654,
    0.0,
    2.535,
    4.432,
    1.507,
    2.371,
    2.286,
])

# Normalise all data to Mantid execution times
data_prototype_32bit /= data_mantid
data_prototype_64bit /= data_mantid
data_prototype_128bit /= data_mantid
data_prototype_256bit /= data_mantid

# Create figure
fig = plt.figure()

def show_data(pos, ds, title):
    ax = fig.add_subplot(pos)

    p = ax.bar(datasets, ds)#np.concatenate((ds, np.array([1.0]))))

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

    ax.axhline(y=1.0, color='r', linestyle='-', linewidth=3)
    ax.annotate('Mantid baseline\n', xy=(0, 1.0), color='r', verticalalignment='baseline', linespacing=0)

    # Set graph title
    ax.set_title(title)

show_data(221, data_prototype_32bit, '32bit Morton coordinates')
show_data(222, data_prototype_64bit, '64bit Morton coordinates')
show_data(223, data_prototype_128bit, '128bit Morton coordinates')
show_data(224, data_prototype_256bit, '256bit Morton coordinates')

plt.subplots_adjust(hspace=0.4, wspace=0.4)
fig.set_size_inches(15, 15)
plt.savefig('results.png')
