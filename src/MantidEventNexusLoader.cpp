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

#include "MantidEventNexusLoader.h"

#include <omp.h>

#include "NexusLoaderUtils.h"

using namespace hdf5;

MantidEventNexusLoader::MantidEventNexusLoader(const std::string &filename)
    : m_file(file::open(filename, file::AccessFlags::READONLY)) {
  m_eventDataGroup = m_file.root()["/mantid_workspace_1/event_workspace"];
  m_detectorsGroup = m_file.root()["/mantid_workspace_1/instrument/detector"];
}

size_t MantidEventNexusLoader::totalEventCount() const {
  node::Dataset dataset = m_eventDataGroup["tof"];
  const auto space = dataset.dataspace();
  return space.size();
}

bool MantidEventNexusLoader::eventsHaveWeight() const {
  return m_eventDataGroup.exists("weight");
}

void MantidEventNexusLoader::loadAllEvents(
    std::vector<TofEvent> &events) const {
  /* Load spectrum/event indices */
  std::vector<uint64_t> eventIndices;
  resize_and_read_dataset(eventIndices, m_eventDataGroup["indices"]);

  /* Reserve storage for output events */
  events.reserve(totalEventCount());

  /* Cache flags for weights */
  const bool haveWeights(eventsHaveWeight());

  uint32_t specId(0);

  /* Iterate over spectra */
  for (auto it = eventIndices.cbegin(); it != eventIndices.cend(); ++it) {
    const auto start(*it);

    /* Calculate end index */
    const auto endIt = it + 1;
    const auto end = endIt == eventIndices.cend() ? totalEventCount() : *endIt;

    /* Load ToF */
    std::vector<double> tof;
    resize_and_read_dataset_range(tof, m_eventDataGroup["tof"], start, end);

    /* Load weights */
    std::vector<float> weight;
    if (haveWeights) {
      resize_and_read_dataset_range(weight, m_eventDataGroup["weight"], start,
                                    end);
    }

    /* Fill events */
    for (size_t i = 0; i < tof.size(); i++) {
      const auto eventWeight = haveWeights ? weight[i] : 1.0f;
      events.push_back({specId, (float)tof[i], 0.0f, eventWeight});
    }
    specId++;
  }
}

void MantidEventNexusLoader::loadSpectrumDetectorMapping(
    SpectrumToDetectorMapping &mapping) const {
  std::vector<uint32_t> detectorIds;
  std::vector<uint32_t> detectorIndices;
  std::vector<uint32_t> detectorCounts;

  resize_and_read_dataset(detectorIds, m_detectorsGroup["detector_list"]);
  resize_and_read_dataset(detectorIndices, m_detectorsGroup["detector_index"]);
  resize_and_read_dataset(detectorCounts, m_detectorsGroup["detector_count"]);

#pragma omp parallel for
  for (size_t i = 0; i < detectorIndices.size(); i++) {
    const size_t detIndexForSpectrum(detectorIndices[i]);
    const size_t detCountForSpectrum(detectorCounts[i]);

    std::vector<detid_t> detectorsForSpectrum;
    for (size_t j = 0; j < detCountForSpectrum; j++) {
      detectorsForSpectrum.push_back(detectorIds[detIndexForSpectrum + j]);
    }

#pragma omp critical
    { mapping[i] = detectorsForSpectrum; }
  }
}
