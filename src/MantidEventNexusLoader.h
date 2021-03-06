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
