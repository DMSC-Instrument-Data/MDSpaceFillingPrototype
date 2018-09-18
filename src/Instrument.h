/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>
#include <vector>

#include <Eigen/Dense>

#pragma once

using detid_t = size_t;
using specid_t = size_t;

struct Detector {
  Eigen::Vector3f position;
};

using Detectors = std::map<detid_t, Detector>;
using DetectorIdList = std::vector<detid_t>;
using SpectrumToDetectorMapping = std::map<specid_t, DetectorIdList>;

struct Instrument {
  Eigen::Vector3f sample_position;
  Eigen::Vector3f source_position;
  Detectors detectors;
  SpectrumToDetectorMapping spectrum_detector_mapping;
};

float get_l1(const Instrument &inst);
float get_l2(const Instrument &inst, const DetectorIdList &detIds);
Eigen::Vector3f get_beam_direction(const Instrument &inst);
Eigen::Vector3f get_detector_direction(const Instrument &inst,
                                       const DetectorIdList &detIds);
float get_detector_two_theta(const Instrument &inst,
                             const DetectorIdList &detIds);

void load_instrument(Instrument &inst, const std::string &filename);

void generate_1_to_1_spec_det_mapping(Instrument &inst);
