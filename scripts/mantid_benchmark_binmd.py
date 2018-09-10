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

import mantid_benchmark_common

from mantid import mtd
from mantid.simpleapi import Load, ConvertToMD, BinMD


for ds in mantid_benchmark_common.datasets:
    print(mantid_benchmark_common.MAGENTA + "Data file:", ds,
          mantid_benchmark_common.NORMAL)

    # Load data
    data = Load(Filename=ds)

    # Do conversion
    md = ConvertToMD(
            InputWorkspace=data,
            QDimensions='Q3D',
            dEAnalysisMode='Elastic',
            Q3DFrames='Q_lab'
        )

    # Determine binning parameters
    dim_x = md.getXDimension()
    dim_y = md.getYDimension()
    dim_z = md.getZDimension()
    bin_x = 'Q_lab_x,{},{},'.format(dim_x.getMinimum(), dim_x.getMaximum())
    bin_y = 'Q_lab_y,{},{},'.format(dim_y.getMinimum(), dim_y.getMaximum())
    bin_z = 'Q_lab_z,{},{},'.format(dim_z.getMinimum(), dim_z.getMaximum())

    def benchmark_for_bins(bins_per_axis):
        print(mantid_benchmark_common.MAGENTA + "Bins per axis:", bins_per_axis,
              mantid_benchmark_common.NORMAL)

        with mantid_benchmark_common.BenchmarkDriver(min_time=120) as b:
            for _ in b:
                bins_per_axis = '100'

                # Do binning
                binned = BinMD(
                    InputWorkspace=md,
                    AlignedDim0=bin_x+bins_per_axis,
                    AlignedDim1=bin_y+bins_per_axis,
                    AlignedDim2=bin_z+bins_per_axis,
                    Parallel=True
                )

                # Record algorithm execution duration from Mantid workspace history
                duration = binned.getHistory().getAlgorithmHistories()[-1].executionDuration()
                b.iteration_times.append(duration)

                # Remove result workspaces
                mtd.remove(binned.name())

    benchmark_for_bins('100')
    benchmark_for_bins('300')
    benchmark_for_bins('1000')
