#include <map>
#include <vector>

#include <Eigen/Dense>

#include "Instrument.h"
#include "MDEvent.h"
#include "TofEvent.h"
#include "Types.h"

#pragma once

struct ConversionInfo {
  bool lorentz_correction = false;
  Eigen::Matrix3f ub_matrix;
};

struct PreprocessedEventInfo {
  std::map<specid_t, std::pair<TofEventList::iterator, TofEventList::iterator>>
      spectrum_to_events;
};

PreprocessedEventInfo preprocess_events(TofEventList &events);

void convert_events(std::vector<MDEvent<3, uint16_t, uint64_t>> &mdEvents,
                    TofEventList &events, const ConversionInfo &convInfo,
                    const Instrument &inst, const MDSpaceBounds<3> &space);
