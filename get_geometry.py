#!/usr/bin/env python3

# Usage: get_geometry.py IDF_FILENAME OUTPUT_FILENAME
# Requires: https://github.com/ess-dmsc/python-nexus-utilities

import sys

import h5py
import numpy as np
from nexusutils.idfparser import IDFParser


def axis_angle_to_rotation_matrix(axis, angle):
    """
    Create a 3D rotation matrix for a given rotation axis and angle in radians.
    """
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

# Record source and sample positions in HDF5 file
hdf_file['instrument']['source_position'] = parser.get_source_position()
hdf_file['instrument']['sample_position'] = parser.get_sample_position()

detector_ids = []
detector_positions = []

def process_bank(bank):
    bank_position = bank['location']

    # Get bank orientation
    bank_orientation = bank.get('orientation')
    if bank_orientation:
        bank_rot_axis = bank_orientation['axis']
        bank_rot_angle = np.deg2rad(bank_orientation['angle'])
        bank_orientation_matrix = axis_angle_to_rotation_matrix(bank_rot_axis, bank_rot_angle)
    else:
        bank_orientation_matrix = np.array([
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0]
        ])

    # Iterate over all detectors in the bank
    num_dets_in_bank = bank['idlist'].size
    ids = np.reshape(bank['idlist'], (num_dets_in_bank,))
    offsets = np.reshape(bank['offsets'], (num_dets_in_bank, 3))
    for det in zip(ids, offsets):
        # Rotate detector offset by bank by bank orientation and add to bank
        # position to get final detector position
        det_position = bank_position + np.dot(bank_orientation_matrix, det[1])

        # Add detector to lists
        detector_ids.append(det[0])
        detector_positions.append(det_position)

# Iterate over all detector banks
for bank in parser.get_detectors():
    process_bank(bank)

# Iterate over all rectangular detector banks
for bank in parser.get_rectangular_detectors():
    process_bank(bank)

# Record detectors in HDF5 file
hdf_file['instrument']['detector_ids'] = detector_ids
hdf_file['instrument']['detector_positions'] = detector_positions
