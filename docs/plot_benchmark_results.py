#!/usr/bin/env python

# Plotting code mostly taken from: http://emptypipes.org/2013/11/09/matplotlib-multicategory-barchart/

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import operator as o

import numpy as np


data = np.array([
    ['Mantid', 'WISH_34509', 214820],
    ['Prototype 32bit', 'WISH_34509', 54290],
    ['Prototype 64bit', 'WISH_34509', 42710],
    ['Prototype 128bit', 'WISH_34509', 76505],
    ['Prototype 256bit', 'WISH_34509', 155937],

    ['Mantid', 'WISH_34509_2x', 486264],
    ['Prototype 32bit', 'WISH_34509_2x', 87575],
    ['Prototype 64bit', 'WISH_34509_2x', 87186],
    ['Prototype 128bit', 'WISH_34509_2x', 152652],
    ['Prototype 256bit', 'WISH_34509_2x', 309423],

    ['Mantid', 'WISH_38423', 5809],
    ['Prototype 32bit', 'WISH_38423', 676],
    ['Prototype 64bit', 'WISH_38423', 1024],
    ['Prototype 128bit', 'WISH_38423', 1917],
    ['Prototype 256bit', 'WISH_38423', 4495],

    ['Mantid', 'WISH_37828', 6451],
    ['Prototype 32bit', 'WISH_37828', 911],
    ['Prototype 64bit', 'WISH_37828', 1383],
    ['Prototype 128bit', 'WISH_37828', 2646],
    ['Prototype 256bit', 'WISH_37828', 6897],

    ['Mantid', 'WISH_37868', 2213],
    ['Prototype 32bit', 'WISH_37868', 445],
    ['Prototype 64bit', 'WISH_37868', 621],
    ['Prototype 128bit', 'WISH_37868', 1047],
    ['Prototype 256bit', 'WISH_37868', 2399],

    ['Mantid', 'TOPAZ_3132', 3185],
    ['Prototype 32bit', 'TOPAZ_3132', 1239],
    ['Prototype 64bit', 'TOPAZ_3132', 1388],
    ['Prototype 128bit', 'TOPAZ_3132', 1942],
    ['Prototype 256bit', 'TOPAZ_3132', 3391],

    ['Mantid', 'SXD_23767', 1584],
    ['Prototype 32bit', 'SXD_23767', 497],
    ['Prototype 64bit', 'SXD_23767', 812],
    ['Prototype 128bit', 'SXD_23767', 1674],
    ['Prototype 256bit', 'SXD_23767', 4107],
])

# Create figure
fig = plt.figure()
ax = fig.add_subplot(111)

# The order we want benchmarks to appear in
benchmarks = [
    'Prototype 32bit',
    'Prototype 64bit',
    'Prototype 128bit',
    'Prototype 256bit',
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

# Set the x-axis tick labels to be equal to the datasets
ax.set_xticks(indeces)
ax.set_xticklabels(datasets)
plt.setp(plt.xticks()[1], rotation=90)

# Add a legend
handles, labels = ax.get_legend_handles_labels()
ax.legend(handles[::-1], labels[::-1], loc='upper left')

# Logarithmic time axis
ax.set_yscale('log')

# Axis labels
ax.set_ylabel("Execution Time (ms)")
ax.set_xlabel("Dataset")

# Grid lines
ax.grid(b=True, which='major', color='b', linestyle='-')
ax.grid(b=True, which='minor', color='y', linestyle='--')

plt.show()
