#include "EventToMDEventConversion.h"

#include <boost/sort/sort.hpp>

PreprocessedEventInfo preprocess_events(TofEventList &tofEvents) {
  PreprocessedEventInfo eventInfo;

  /* Sort TOF events by spectrum ID */
  boost::sort::block_indirect_sort(
      tofEvents.begin(), tofEvents.end(),
      [](const TofEvent &a, const TofEvent &b) { return a.id < b.id; });

  /* Get iterators to each spectrum range in TOF event list */
  auto rangeStart = tofEvents.begin();
  for (auto eventIt = tofEvents.begin() + 1; eventIt != tofEvents.end();
       ++eventIt) {
    if (eventIt->id != rangeStart->id) {
      eventInfo.spectrum_to_events.push_back(
          std::make_tuple(rangeStart->id, rangeStart, eventIt));
      rangeStart = eventIt;
    }
  }
  eventInfo.spectrum_to_events.push_back(
      std::make_tuple(rangeStart->id, rangeStart, tofEvents.end()));

  return eventInfo;
}
