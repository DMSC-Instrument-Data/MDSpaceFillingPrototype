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

fig = plt.figure()
ax = fig.add_subplot(111)

barplot(ax, data)

ax.set_yscale('log')

ax.set_ylabel("Execution Time (ms)")
ax.set_xlabel("Dataset")

ax.grid(b=True, which='major', color='b', linestyle='-')
ax.grid(b=True, which='minor', color='y', linestyle='--')

plt.show()
