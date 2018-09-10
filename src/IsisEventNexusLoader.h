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

#include <string>

#include <h5cpp/hdf5.hpp>

#include "Instrument.h"
#include "TofEvent.h"

#pragma once

void generate_spectrum_detector_mapping(SpectrumToDetectorMapping &mapping,
                                        const std::vector<int32_t> &spec,
                                        const std::vector<int32_t> &udet);

class IsisEventNexusLoader {
public:
  IsisEventNexusLoader(const std::string &filename,
                       const std::string &dataPath);

  size_t totalEventCount() const;
  size_t frameCount() const;

  const std::vector<uint64_t> &eventIndex() const;
  const std::vector<double> &eventTimeZero() const;

  void eventId(std::vector<uint32_t> &data, size_t start, size_t end) const;
  void eventTimeOffset(std::vector<float> &data, size_t start,
                       size_t end) const;

  std::pair<size_t, size_t> getFrameEventRange(size_t frameIdx) const;

  void loadFrames(std::vector<TofEvent> &events,
                  const std::vector<size_t> &frameIdxs) const;

  void loadSpectrumDetectorMapping(SpectrumToDetectorMapping &mapping) const;

private:
  hdf5::file::File m_file;
  hdf5::node::Group m_datasetGroup;
  hdf5::node::Group m_vmsCompatGroup;

  std::vector<uint64_t> m_eventIndex;
  std::vector<double> m_eventTimeZero;
};
