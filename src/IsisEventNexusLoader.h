#include <string>

#include <h5cpp/hdf5.hpp>

#include "Instrument.h"
#include "TofEvent.h"

#pragma once

template <typename T>
void resize_and_read_dataset_range(std::vector<T> &data,
                                   hdf5::node::Dataset dataset, size_t start,
                                   size_t end) {
  hdf5::dataspace::Hyperslab slab({start}, {end - start}, {1}, {1});
  data.resize(slab.block()[0]);
  dataset.read(data, slab);
}

template <typename T>
void resize_and_read_dataset(std::vector<T> &data,
                             hdf5::node::Dataset dataset) {
  data.resize(dataset.dataspace().size());
  dataset.read(data);
}

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
