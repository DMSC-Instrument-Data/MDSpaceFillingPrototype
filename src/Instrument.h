#include <Eigen/Dense>

#pragma once

struct Detector {
  Eigen::Vector3f position;
};

struct Instrument {
  Eigen::Vector3f sample_position;
  Eigen::Vector3f source_position;
  std::vector<Detector> detectors;
};

float get_l1(const Instrument &inst);
float get_l2(const Instrument &inst, const Detector &det);
Eigen::Vector3f get_beam_direction(const Instrument &inst);
Eigen::Vector3f get_detector_direction(const Instrument &inst,
                                       const Detector &det);
