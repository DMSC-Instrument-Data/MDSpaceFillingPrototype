#include <vector>

#pragma once

struct TofEvent {
  uint32_t id;
  float tof;
  double pulse_time;
  float weight;
};

using TofEventList = std::vector<TofEvent>;
