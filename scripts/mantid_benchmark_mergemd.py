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
from mantid.simpleapi import Load, ConvertToMD, BinMD, MergeMD
import os

os.remove("mantid_benchmark_mergemd.log")

pairs_to_merge = [("TOPAZ_3132_event_10_100.nxs", "TOPAZ_3132_event_0_10.nxs"),
                  ("TOPAZ_3132_event_20_100.nxs", "TOPAZ_3132_event_0_20.nxs"),
                  ("TOPAZ_3132_event_30_100.nxs", "TOPAZ_3132_event_0_30.nxs"),
                  ("TOPAZ_3132_event_50_100.nxs", "TOPAZ_3132_event_0_50.nxs")
                  ]
for base, merge in pairs_to_merge:
    # Load data
    data1 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, base))
    data2 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, merge))

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

    with mantid_benchmark_common.BenchmarkDriver(min_time=30, name=base, log_file="mantid_benchmark_mergemd.log") as b:
        for _ in b:
            # Merge workspaces
            merged = MergeMD(InputWorkspaces='md1,md2')

            # Record algorithm execution duration from Mantid workspace history
            duration = merged.getHistory().getAlgorithmHistories()[-1].executionDuration()
            b.iteration_times.append(duration)

            # Remove result workspaces
            mtd.remove(merged.name())

pairs_to_merge = [("SXD30535_event_0_60.nxs", "SXD30535_event_60_100.nxs"),
                  ("SXD30535_event_0_87.5.nxs", "SXD30535_event_87.5_100.nxs"),
                  ("SXD30535_event_0_98.nxs", "SXD30535_event_98_100.nxs")
                  ]
for base, merge in pairs_to_merge:
    # Load data
    data1 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, base))
    data2 = Load(Filename=os.path.join(mantid_benchmark_common.data_path, merge))

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

    with mantid_benchmark_common.BenchmarkDriver(min_time=30, name=base, log_file="mantid_benchmark_mergemd.log") as b:
        for _ in b:
            # Merge workspaces
            merged = MergeMD(InputWorkspaces='md1,md2')

            # Record algorithm execution duration from Mantid workspace history
            duration = merged.getHistory().getAlgorithmHistories()[-1].executionDuration()
            b.iteration_times.append(duration)

            # Remove result workspaces
            mtd.remove(merged.name())
