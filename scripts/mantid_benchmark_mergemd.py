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


# Load data
data1 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, 'SXD30528_event.nxs'))
data2 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, 'SXD30529_event.nxs'))

# Do conversion
md1 = ConvertToMD(
          InputWorkspace=data1,
          QDimensions='Q3D',
          dEAnalysisMode='Elastic',
          Q3DFrames='Q_lab'
      )
md2 = ConvertToMD(
          InputWorkspace=data2,
          QDimensions='Q3D',
          dEAnalysisMode='Elastic',
          Q3DFrames='Q_lab'
      )

with mantid_benchmark_common.BenchmarkDriver(min_time=120) as b:
    for _ in b:
        # Merge workspaces
        merged = MergeMD(InputWorkspaces='md1,md2')

        # Record algorithm execution duration from Mantid workspace history
        duration = merged.getHistory().getAlgorithmHistories()[-1].executionDuration()
        b.iteration_times.append(duration)

        # Remove result workspaces
        mtd.remove(merged.name())
