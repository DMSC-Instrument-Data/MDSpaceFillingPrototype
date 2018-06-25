#include <vector>

#pragma once

struct TofEvent {
  uint32_t id;
  float tof;
  double pulse_time;
};

using TofEventList = std::vector<TofEvent>;
