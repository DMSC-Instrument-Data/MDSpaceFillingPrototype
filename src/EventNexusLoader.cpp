#include "EventNexusLoader.h"

using namespace hdf5;

EventNexusLoader::EventNexusLoader(const std::string &filename,
                                   const std::string &dataPath)
    : m_file(file::open(filename, file::AccessFlags::READONLY)) {
  m_datasetGroup = m_file.root()[dataPath];

  resize_and_read_dataset(m_eventIndex, m_datasetGroup["event_index"]);
  resize_and_read_dataset(m_eventTimeZero, m_datasetGroup["event_time_zero"]);
}

size_t EventNexusLoader::totalEventCount() const {
  node::Dataset dataset = m_datasetGroup["event_id"];
  const auto space = dataset.dataspace();
  return space.size();
}

size_t EventNexusLoader::frameCount() const { return m_eventIndex.size(); }

const std::vector<uint64_t> &EventNexusLoader::eventIndex() const {
  return m_eventIndex;
}

const std::vector<double> &EventNexusLoader::eventTimeZero() const {
  return m_eventTimeZero;
}

void EventNexusLoader::eventId(std::vector<uint32_t> &data, size_t start,
                               size_t end) const {
  resize_and_read_dataset_range(data, m_datasetGroup["event_id"], start, end);
}

void EventNexusLoader::eventTimeOffset(std::vector<float> &data, size_t start,
                                       size_t end) const {
  resize_and_read_dataset_range(data, m_datasetGroup["event_time_offset"],
                                start, end);
}

std::pair<size_t, size_t>
EventNexusLoader::getFrameEventRange(size_t frameIdx) const {
  const auto start = m_eventIndex[frameIdx];
  const auto end = frameIdx >= m_eventIndex.size() - 1
                       ? totalEventCount()
                       : m_eventIndex[frameIdx + 1];

  return std::make_pair(start, end - start);
}

void EventNexusLoader::loadFrames(std::vector<Event> &events,
                                  const std::vector<size_t> &frameIdxs) const {
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
      *(eventIt++) = {eventId[i], eventTimeOffset[i], params.timeZero};
    }
  }
}
