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
from mantid.simpleapi import Load, ConvertToMD


for ds in mantid_benchmark_common.datasets:
    print(mantid_benchmark_common.MAGENTA + "Data file:", ds,
          mantid_benchmark_common.NORMAL)

    # Load data
    data = Load(Filename=ds)

    with mantid_benchmark_common.BenchmarkDriver(min_time=300) as b:
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
