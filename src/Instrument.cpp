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

#include "Instrument.h"

#include <cmath>
#include <istream>

#include <h5cpp/hdf5.hpp>

using namespace hdf5;

/**
 * Gets L1 (source to sample) distance.
 * @param inst Instrument
 */
float get_l1(const Instrument &inst) {
  return (inst.sample_position - inst.source_position).norm();
}

/**
 * Gets L2 (sample to detector) distance.
 * @param inst Instrument
 * @param detIds Detector IDs
 */
float get_l2(const Instrument &inst, const DetectorIdList &detIds) {
  float l2(0.0f);
  for (const auto detId : detIds) {
    l2 += (inst.detectors.at(detId).position - inst.sample_position).norm();
  }
  return l2 / detIds.size();
}

/**
 * Gets beam direction vector.
 * @param inst Instrument
 */
Eigen::Vector3f get_beam_direction(const Instrument &inst) {
  const auto &beamline = inst.sample_position - inst.source_position;
  return beamline / beamline.norm();
}

/**
 * Gets detector direction vector.
 * @param inst Instrument
 * @param detIds Detector IDs
 */
Eigen::Vector3f get_detector_direction(const Instrument &inst,
                                       const DetectorIdList &detIds) {
  Eigen::Vector3f pos(Eigen::Vector3f::Zero());
  for (const auto detId : detIds) {
    const auto &detPos =
        inst.detectors.at(detId).position - inst.sample_position;
    pos += (detPos / detPos.norm());
  }
  return pos / detIds.size();
}

/**
 * Gets the scattering angle two Theta.
 *
 * Angle between incident beam direction and sample to detector direction.
 * @param inst Instrument
 * @param detIds Detector IDs
 */
float get_detector_two_theta(const Instrument &inst,
                             const DetectorIdList &detIds) {
  const auto beamDir = get_beam_direction(inst);
  const auto detectorDir = get_detector_direction(inst, detIds);
  return acos(beamDir.dot(detectorDir));
}

Eigen::Vector3f read_eigen3_from_hdf5_dataset(const node::Dataset &dataset) {
  std::vector<float> vec(3);
  dataset.read(vec);
  return Eigen::Vector3f(vec[0], vec[1], vec[2]);
}

/**
 * Loads an instrument from parameters stored in a HDF5 file.
 *
 * @param inst Reference to output instrument
 * @param filename Filename of the HDF5 file to load from
 */
void load_instrument(Instrument &inst, const std::string &filename) {
  const auto file = file::open(filename, file::AccessFlags::READONLY);
  const node::Group instrumentGroup = file.root()["instrument"];

  /* Read source position */
  inst.source_position =
      read_eigen3_from_hdf5_dataset(instrumentGroup["source_position"]);

  /* Read sample position */
  inst.sample_position =
      read_eigen3_from_hdf5_dataset(instrumentGroup["sample_position"]);

  /* Detectors */
  const node::Dataset detIdDataset = instrumentGroup["detector_ids"];
  const node::Dataset detPosDataset = instrumentGroup["detector_positions"];
  const size_t numDetectors(detIdDataset.dataspace().size());

  /* Read detector IDs */
  std::vector<detid_t> detectorIds(numDetectors);
  detIdDataset.read(detectorIds);

  /* Read detector positions */
  std::vector<float> detectorPositions(numDetectors * 3);
  detPosDataset.read(detectorPositions);

  /* Create detectors */
  auto posCoordIt = detectorPositions.cbegin();
  for (const auto detId : detectorIds) {
    inst.detectors[detId] = Detector{
        Eigen::Vector3f(*(posCoordIt++), *(posCoordIt++), *(posCoordIt++))};
  }
}

/**
 * Generates a one to one spectrum/detector mapping for the detectors loaded
 * into an existing instrument.
 *
 * @param inst Reference to instrument to modify
 */
void generate_1_to_1_spec_det_mapping(Instrument &inst) {
  specid_t specNo = 0;

  /* For each detector */
  for (const auto &det : inst.detectors) {
    /* Get detector ID */
    const auto detId(std::get<0>(det));

    /* Add entry in mapping with consecutive spectrum numbers */
    inst.spectrum_detector_mapping[specNo++] = std::vector<detid_t>{detId};
  }
}
