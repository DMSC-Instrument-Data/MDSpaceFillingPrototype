#!/usr/bin/env python
# Plotting code mostly taken from: http://emptypipes.org/2013/11/09/matplotlib-multicategory-barchart/

import matplotlib.pyplot as plt
import matplotlib.cm as cm
import operator as o

import numpy as np


def barplot(ax, dpoints):
    '''
    Create a barchart for data across different categories with
    multiple conditions for each category.

    @param ax: The plotting axes from matplotlib.
    @param dpoints: The data set as an (n, 3) numpy array
    '''

    # Aggregate the conditions and the categories according to their
    # mean values
    conditions = [(c, np.mean(dpoints[dpoints[:,0] == c][:,2].astype(float)))
                  for c in np.unique(dpoints[:,0])]
    categories = [(c, np.mean(dpoints[dpoints[:,1] == c][:,2].astype(float)))
                  for c in np.unique(dpoints[:,1])]

    # sort the conditions, categories and data so that the bars in
    # the plot will be ordered by category and condition
    conditions = [c[0] for c in sorted(conditions, key=o.itemgetter(1))]
    categories = [c[0] for c in sorted(categories, key=o.itemgetter(1))]

    dpoints = np.array(sorted(dpoints, key=lambda x: categories.index(x[1])))

    # the space between each set of bars
    space = 0.3
    n = len(conditions)
    width = (1 - space) / (len(conditions))

    # Create a set of bars at each position
    for i,cond in enumerate(conditions):
        indeces = range(1, len(categories)+1)
        vals = dpoints[dpoints[:,0] == cond][:,2].astype(np.float)
        pos = [j - (1 - space) / 2. + i * width for j in indeces]
        ax.bar(pos, vals, width=width, label=cond,
               color=cm.Accent(float(i) / n))

    # Set the x-axis tick labels to be equal to the categories
    ax.set_xticks(indeces)
    ax.set_xticklabels(categories)
    plt.setp(plt.xticks()[1], rotation=90)

    # Add a legend
    handles, labels = ax.get_legend_handles_labels()
    ax.legend(handles[::-1], labels[::-1], loc='upper left')


data = np.array([
    ['Mantid', 'WISH_34509', 214820],
    ['Prototype 64bit', 'WISH_34509', 48130],
    ['Prototype 128bit', 'WISH_34509', 74236],
    ['Mantid', 'WISH_34509_2x', 486264],
    ['Prototype 64bit', 'WISH_34509_2x', 84967],
    ['Prototype 128bit', 'WISH_34509_2x', 151640],
    ['Mantid', 'WISH_38423', 5809],
    ['Prototype 64bit', 'WISH_38423', 1089],
    ['Prototype 128bit', 'WISH_38423', 1995],
    ['Mantid', 'WISH_37828', 6451],
    ['Prototype 64bit', 'WISH_37828', 1461],
    ['Prototype 128bit', 'WISH_37828', 2747],
    ['Mantid', 'WISH_37868', 2213],
    ['Prototype 64bit', 'WISH_37868', 621],
    ['Prototype 128bit', 'WISH_37868', 1041],
    ['Mantid', 'TOPAZ_3132', 3185],
    ['Prototype 64bit', 'TOPAZ_3132', 1391],
    ['Prototype 128bit', 'TOPAZ_3132', 1921],
    ['Mantid', 'SXD_23767', 1584],
    ['Prototype 64bit', 'SXD_23767', 825],
    ['Prototype 128bit', 'SXD_23767', 1706],
])

fig = plt.figure()
ax = fig.add_subplot(111)

barplot(ax, data)

ax.set_yscale('log')

ax.set_ylabel("Execution Time (ms)")
ax.set_xlabel("Dataset")

ax.grid(b=True, which='major', color='b', linestyle='-')
ax.grid(b=True, which='minor', color='y', linestyle='--')

plt.show()
