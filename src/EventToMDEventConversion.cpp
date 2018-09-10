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
