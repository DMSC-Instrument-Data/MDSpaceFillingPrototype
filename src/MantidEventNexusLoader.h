#include <string>

#include <h5cpp/hdf5.hpp>

#include "Instrument.h"
#include "TofEvent.h"

#pragma once

class MantidEventNexusLoader {
public:
  MantidEventNexusLoader(const std::string &filename);

  size_t totalEventCount() const;

  bool eventsHaveWeight() const;

  void loadAllEvents(std::vector<TofEvent> &events) const;

  void loadSpectrumDetectorMapping(SpectrumToDetectorMapping &mapping) const;

private:
  hdf5::file::File m_file;

  hdf5::node::Group m_eventDataGroup;
  hdf5::node::Group m_detectorsGroup;
};
