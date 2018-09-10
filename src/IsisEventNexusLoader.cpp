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

#include "IsisEventNexusLoader.h"

#include "NexusLoaderUtils.h"

using namespace hdf5;

void generate_spectrum_detector_mapping(SpectrumToDetectorMapping &mapping,
                                        const std::vector<int32_t> &spec,
                                        const std::vector<int32_t> &udet) {
  if (spec.size() != udet.size()) {
    return;
  }

  auto specLast = spec.cbegin();
  auto udetLast = udet.cbegin();

  auto specIt = spec.cbegin() + 1;
  auto udetIt = udet.cbegin() + 1;

  for (; specIt != spec.end() && udetIt != udet.end();) {
    if (*specIt != *specLast) {
      mapping[*specLast] = std::vector<detid_t>(udetLast, udetIt);

      specLast = specIt;
      udetLast = udetIt;
    }

    ++specIt;
    ++udetIt;
  }
  mapping[*specLast] = std::vector<detid_t>(udetLast, udet.end());
}

IsisEventNexusLoader::IsisEventNexusLoader(const std::string &filename,
                                           const std::string &dataPath)
    : m_file(file::open(filename, file::AccessFlags::READONLY)) {
  m_datasetGroup = m_file.root()[dataPath];
  m_vmsCompatGroup = m_datasetGroup.link().parent()["isis_vms_compat"];

  resize_and_read_dataset(m_eventIndex, m_datasetGroup["event_index"]);
  resize_and_read_dataset(m_eventTimeZero, m_datasetGroup["event_time_zero"]);
}

size_t IsisEventNexusLoader::totalEventCount() const {
  node::Dataset dataset = m_datasetGroup["event_id"];
  const auto space = dataset.dataspace();
  return space.size();
}

size_t IsisEventNexusLoader::frameCount() const { return m_eventIndex.size(); }

const std::vector<uint64_t> &IsisEventNexusLoader::eventIndex() const {
  return m_eventIndex;
}

const std::vector<double> &IsisEventNexusLoader::eventTimeZero() const {
  return m_eventTimeZero;
}

void IsisEventNexusLoader::eventId(std::vector<uint32_t> &data, size_t start,
                                   size_t end) const {
  resize_and_read_dataset_range(data, m_datasetGroup["event_id"], start, end);
}

void IsisEventNexusLoader::eventTimeOffset(std::vector<float> &data,
                                           size_t start, size_t end) const {
  resize_and_read_dataset_range(data, m_datasetGroup["event_time_offset"],
                                start, end);
}

std::pair<size_t, size_t>
IsisEventNexusLoader::getFrameEventRange(size_t frameIdx) const {
  const auto start = m_eventIndex[frameIdx];
  const auto end = frameIdx >= m_eventIndex.size() - 1
                       ? totalEventCount()
                       : m_eventIndex[frameIdx + 1];

  return std::make_pair(start, end - start);
}

void IsisEventNexusLoader::loadFrames(
    std::vector<TofEvent> &events, const std::vector<size_t> &frameIdxs) const {
  struct FrameParams {
    size_t nexusEventStart;
    size_t memoryEventStart;
    size_t numEvents;
    double timeZero;
  };

  /* Calculate total number of events and frame parameters */
  size_t numEvents = 0;
  std::vector<FrameParams> frameParams;

  for (const auto &frameIdx : frameIdxs) {
    /* Get event start and end range for frame */
    const auto eventRange = getFrameEventRange(frameIdx);

    /* Record frame event ranges */
    frameParams.push_back({eventRange.first, numEvents, eventRange.second,
                           m_eventTimeZero[frameIdx]});

    /* Increment total event count */
    numEvents += eventRange.second;
  }

  /* Allocate vector of correct size */
  events.resize(numEvents);

  /* Retrieve datasets */
  node::Dataset eventIdDataset = m_datasetGroup["event_id"];
  node::Dataset eventTimeOffsetDataset = m_datasetGroup["event_time_offset"];

  /* Create storage for events to be read */
  std::vector<uint32_t> eventId;
  std::vector<float> eventTimeOffset;

  /* Load each frame */
  for (size_t i = 0; i < frameParams.size(); ++i) {
    const auto params = frameParams[i];

    /* Obtain iterator to start of memory for this frame */
    auto eventIt = events.begin();
    std::advance(eventIt, params.memoryEventStart);

    /* Create slab */
    const dataspace::Hyperslab slab({params.nexusEventStart},
                                    {params.numEvents}, {1}, {1});

    /* Resize containers to correct size for this frame */
    eventId.resize(params.numEvents);
    eventTimeOffset.resize(params.numEvents);

    /* Read event data */
    eventIdDataset.read(eventId, slab);
    eventTimeOffsetDataset.read(eventTimeOffset, slab);

    /* Populate events in output storage */
    for (size_t i = 0; i < params.numEvents; ++i) {
      *(eventIt++) = {eventId[i], eventTimeOffset[i], params.timeZero, 1.0f};
    }
  }
}

void IsisEventNexusLoader::loadSpectrumDetectorMapping(
    SpectrumToDetectorMapping &mapping) const {
  /* Get datasets */
  node::Dataset specDataset = m_vmsCompatGroup["SPEC"];
  node::Dataset udetDataset = m_vmsCompatGroup["UDET"];

  /* Validate dimensions */
  const size_t numSpec = specDataset.dataspace().size();
  const size_t numUdet = udetDataset.dataspace().size();

  if (numSpec != numUdet) {
    return;
  }

  /* Read data */
  std::vector<int32_t> spec(numSpec);
  std::vector<int32_t> udet(numUdet);

  specDataset.read(spec);
  udetDataset.read(udet);

  /* Do mapping generation */
  generate_spectrum_detector_mapping(mapping, spec, udet);
}
