#!/usr/bin/env python

# Plotting code mostly taken from: http://emptypipes.org/2013/11/09/matplotlib-multicategory-barchart/

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import operator as o

import numpy as np


data = np.array([
    ['Mantid', '3D', 319.085],
    ['Prototype 64bit', '3D', 301.418],
    ['Prototype 128bit', '3D', 301.429],

    # ['Mantid', '4D', 0],
    # ['Prototype 64bit', '4D', 172.842e6],
    # ['Prototype 128bit', '4D', 172.03e6],
])

# Create figure
fig = plt.figure()
ax = fig.add_subplot(111)

# The order we want benchmarks to appear in
benchmarks = [
    'Prototype 64bit',
    'Prototype 128bit',
    'Mantid',
]

# Sort datasets by Mantid execution time
datasets = [(c, np.mean(data[data[:,1] == c][0,2].astype(float)))
              for c in np.unique(data[:,1])]
datasets = [c[0] for c in sorted(datasets, key=o.itemgetter(1))]

data = np.array(sorted(data, key=lambda x: datasets.index(x[1])))

# The space between each set of bars
space = 0.3
width = (1 - space) / (len(benchmarks))

# Create a set of bars at each position
bars = []
for i, cond in enumerate(benchmarks):
    indeces = range(1, len(datasets) + 1)
    vals = data[data[:,0] == cond][:,2].astype(np.float)
    pos = [j - (1 - space) / 2.0 + i * width for j in indeces]
    bars.append(ax.bar(pos, vals, width=width, label=cond,
                       color=cm.Accent(float(i) / len(benchmarks))))
bars = np.array(bars)

# Show factor of Mantid execution time at top of bars
for i in range(len(datasets)):
    ds_bars = bars[:, i]

    # Normalise bar heights to Mantid execution time (assumes Mantid is last in
    # benchmarks list)
    heights = np.array([r.get_height() for r in ds_bars])
    heights /= heights[-1]

    # Plot text at top of bars
    for (rect, v) in zip(ds_bars, heights):
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '{0:.2f}'.format(v), ha='center', va='bottom', rotation='vertical')

ax.set_xticks([])

# Add a legend
handles, labels = ax.get_legend_handles_labels()
ax.legend(handles[::-1], labels[::-1], loc='upper left')

# Axis labels
ax.set_ylabel("Event rate (M events/second)")

# Grid lines
ax.grid(b=True, which='major', color='b', linestyle='-')
ax.grid(b=True, which='minor', color='y', linestyle='--')

plt.show()
