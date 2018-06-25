#include <map>
#include <vector>

#include <Eigen/Dense>

#include "TofEvent.h"
#include "Types.h"

#pragma once

struct PreprocessedEventInfo {
  std::map<size_t, std::pair<TofEventList::iterator, TofEventList::iterator>>
      detector_to_events;
};

PreprocessedEventInfo preprocess_events(TofEventList &events);

void convert_events_for_detector(TofEventList::const_iterator begin,
                                 TofEventList::const_iterator end);

void convert_events(TofEventList &events);
