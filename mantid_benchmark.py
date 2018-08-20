#!/usr/bin/env python

from __future__ import print_function
import os.path
import time

from mantid import mtd
from mantid.simpleapi import Load, ConvertToMD


CYAN = '\x1b[36m'
YELLOW = '\x1b[33m'
MAGENTA = '\x1b[35m'
NORMAL = '\x1b[0m'


class BenchmarkDriver(object):

    def __init__(self, min_time):
        self.min_time = min_time

        self.iteration_times = []

    @property
    def total_time(self):
        return sum(self.iteration_times)

    @property
    def average_time(self):
        return self.total_time / len(self.iteration_times) if self.iteration_times else 0.0

    def __enter__(self):
        return self

    def __exit__(self, *args):
        print(CYAN + "Iteration count:", YELLOW, len(self.iteration_times))
        print(CYAN + "Total time:", YELLOW, self.total_time)
        print(CYAN + "Average time:", YELLOW, self.average_time)
        print(NORMAL)

    def __iter__(self):
        return self

    def __next__(self):
        if self.total_time > self.min_time:
            raise StopIteration()

        return len(self.iteration_times) + 1

    next = __next__


data_path = '/home/dan'

datasets = [
    'WISH00034509.nxs',
    'WISH00034509_2x_larger.nxs',
    'WISH00037828_event.nxs',
    'WISH00037868_event.nxs',
    'WISH00038423_event.nxs',
    'TOPAZ_3132_event.nxs',
    'SXD23767_event.nxs',
]


for ds in datasets:
    filepath = os.path.join(data_path, ds)

    # Load data
    print(MAGENTA + "Data file:", filepath, NORMAL)
    data = Load(Filename=filepath)

    with BenchmarkDriver(min_time=300) as b:
        for _ in b:
            # Do conversion
            md = ConvertToMD(
                    InputWorkspace=data,
                    QDimensions='Q3D',
                    dEAnalysisMode='Elastic',
                    Q3DFrames='Q_lab'
                )

            # Record algorithm execution duration from Mantid workspace history
            duration = md.getHistory().getAlgorithmHistories()[-1].executionDuration()
            b.iteration_times.append(duration)

            # Remove result workspaces
            mtd.remove(md.name())
            mtd.remove('PreprocessedDetectorWS')
