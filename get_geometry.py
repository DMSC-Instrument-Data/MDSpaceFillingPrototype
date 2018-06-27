#!/usr/bin/env python3

# Requires: https://github.com/ess-dmsc/python-nexus-utilities

import sys

import h5py
import numpy as np
from nexusutils.idfparser import IDFParser


def axis_angle_to_rotation_matrix(axis, angle):
    c = np.cos(angle)
    s = np.sin(angle)
    t = 1.0 - c
    axis /= np.linalg.norm(axis)
    x = axis[0]
    y = axis[1]
    z = axis[2]

    return np.array([
        [t * x * x + c,      t * y * x - z * s,  t * z * x + y * s],
        [t * x * y + z * s,  t * y * y + c,      t * z * y - x * s],
        [t * x * z - y * s,  t * y * z + x * s,  t * z * z + c    ]
    ])

parser = IDFParser(sys.argv[1])

hdf_file = h5py.File(sys.argv[2])
hdf_file.create_group('instrument')

hdf_file['instrument']['source_position'] = parser.get_source_position()
hdf_file['instrument']['sample_position'] = parser.get_sample_position()

detector_ids = []
detector_positions = []
for bank in parser.get_detectors():
    bank_position = bank['location']

    bank_rot_axis = bank['orientation']['axis']
    bank_rot_angle = np.deg2rad(bank['orientation']['angle'])
    bank_orientation_matrix = axis_angle_to_rotation_matrix(bank_rot_axis, bank_rot_angle)

    for det in zip(bank['idlist'], bank['offsets']):
        det_position = bank_position + np.dot(bank_orientation_matrix, det[1])
        detector_ids.append(det[0])
        detector_positions.append(det_position)

hdf_file['instrument']['detector_ids'] = detector_ids
hdf_file['instrument']['detector_positions'] = detector_positions
