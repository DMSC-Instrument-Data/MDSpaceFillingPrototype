#include "Instrument.h"

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
 * @param det Detector
 */
float get_l2(const Instrument &inst, const detid_t detId) {
  return (inst.detectors.at(detId).position - inst.sample_position).norm();
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
 * @param det Detector
 */
Eigen::Vector3f get_detector_direction(const Instrument &inst,
                                       const detid_t detId) {
  const auto &detPos = inst.detectors.at(detId).position - inst.sample_position;
  return detPos / detPos.norm();
}
