#!/usr/bin/env python

from __future__ import print_function
import os.path
import time


class BenchmarkDriver(object):

    def __init__(self, min_time):
        self.min_time = min_time

        self.iteration_times = []

        self.current_total_time = None
        self.current_start_time = None

    @property
    def total_time(self):
        return sum(self.iteration_times)

    @property
    def average_time(self):
        return self.total_time / len(self.iteration_times)

    def finish_iteration(self):
        self.stop_timer()

        if self.current_total_time is not None:
            self.iteration_times.append(self.current_total_time)

        self.current_total_time = None

    def start_timer(self):
        if self.current_start_time is not None:
            return

        self.current_start_time = time.time()

    def stop_timer(self):
        if self.current_start_time is None:
            return

        duration = time.time() - self.current_start_time

        if self.current_total_time is None:
            self.current_total_time = 0
        self.current_total_time += duration

        self.current_start_time = None

    def __enter__(self):
        return self

    def __exit__(self, *args):
        print("Iteration count:", len(self.iteration_times))
        print("Total time:", self.total_time)
        print("Average time:", self.average_time)

    def __iter__(self):
        return self

    def __next__(self):
        self.finish_iteration()

        if self.total_time > self.min_time:
            raise StopIteration()

        self.start_timer()

        return len(self.iteration_times) + 1

    next = __next__


data_path = '/home/dan'

datasets = [
    'WISH00034509.nxs',
    # 'WISH00037828_event.nxs',
    # 'WISH00037868_event.nxs',
    # 'WISH00038423_event.nxs',
    # 'TOPAZ_3132_event.nxs',
    # 'SXD23767_event.nxs',
]


for ds in datasets:
    filepath = os.path.join(data_path, ds)
    print("Data file:", filepath)

    with BenchmarkDriver(5) as b:
        for _ in b:
            print("z")
            time.sleep(0.3)
            b.stop_timer()
            print("zz")
            time.sleep(0.5)
            b.start_timer()
            print("zzz")
            time.sleep(0.2)
