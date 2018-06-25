#include "EventToMDEventConversion.h"

#include <boost/sort/sort.hpp>

PreprocessedEventInfo preprocess_events(TofEventList &events) {
  PreprocessedEventInfo eventInfo;

  /* Sort TOF events by detector ID */
  boost::sort::block_indirect_sort(
      events.begin(), events.end(),
      [](const TofEvent &a, const TofEvent &b) { return a.id < b.id; });

  /* Get iterators to each detector range in TOF event list */
  auto rangeStart = events.begin();
  for (auto it = events.begin() + 1; it != events.end(); ++it) {
    if (it->id != rangeStart->id) {
      eventInfo.detector_to_events[rangeStart->id] =
          std::make_pair(rangeStart, it);
      rangeStart = it;
    }
  }
  eventInfo.detector_to_events[rangeStart->id] =
      std::make_pair(rangeStart, events.end());

  return eventInfo;
}

void convert_events_for_detector(TofEventList::const_iterator begin,
                                 TofEventList::const_iterator end) {
  /* TODO */
}

void convert_events(TofEventList &events) {
  const auto info = preprocess_events(events);

  for (const auto i : info.detector_to_events) {
    std::cout << "detector " << i.first << ": "
              << std::distance(events.begin(), i.second.first) << '-'
              << std::distance(events.begin(), i.second.second) << '\n';
  }
}
