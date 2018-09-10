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
import os.path
import time


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


data_path = '/media/scratch/md_data/'

datasets = list(map(lambda f: os.path.join(data_path, f), [
    'WISH00034509.nxs',
    'WISH00034509_2x_larger.nxs',
    'WISH00037828_event.nxs',
    'WISH00037868_event.nxs',
    'WISH00038423_event.nxs',
    'TOPAZ_3132_event.nxs',
    'SXD23767_event.nxs',
]))
