#include <map>

#include <Eigen/Dense>

#pragma once

using detid_t = size_t;

struct Detector {
  Eigen::Vector3f position;
};

struct Instrument {
  Eigen::Vector3f sample_position;
  Eigen::Vector3f source_position;
  std::map<detid_t, Detector> detectors;
};

float get_l1(const Instrument &inst);
float get_l2(const Instrument &inst, const detid_t detId);
Eigen::Vector3f get_beam_direction(const Instrument &inst);
Eigen::Vector3f get_detector_direction(const Instrument &inst,
                                       const detid_t detId);
float get_detector_two_theta(const Instrument &inst, const detid_t detId);

void load_instrument(Instrument &inst, const std::string &filename);
