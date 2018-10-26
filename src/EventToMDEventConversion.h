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

#include <tuple>
#include <vector>

#include <Eigen/Dense>
#include <omp.h>

#include "Constants.h"
#include "Instrument.h"
#include "MDEvent.h"
#include "TofEvent.h"
#include "Types.h"

#pragma once

struct ConversionInfo {
  bool lorentz_correction = false;
  Eigen::Matrix3f ub_matrix;
};

struct PreprocessedEventInfo {
  std::vector<
      std::tuple<specid_t, TofEventList::iterator, TofEventList::iterator>>
      spectrum_to_events;
};

PreprocessedEventInfo preprocess_events(TofEventList &events);

template <typename IntT, typename MortonT>
void convert_events(std::vector<MDEvent<3, IntT, MortonT>> &mdEvents,
                    TofEventList &tofEvents, const ConversionInfo &convInfo,
                    const Instrument &inst, const MDSpaceBounds<3> &space) {
  /* Do preprocessing */
  const auto eventInfo = preprocess_events(tofEvents);

  /* Get common instrument parameters */
  const Eigen::Vector3f beamDirection = get_beam_direction(inst);
  const auto l1 = get_l1(inst);

  std::vector<std::vector<MDEvent<3, IntT, MortonT>>> threadMdEventsForSpectrum(
      omp_get_max_threads());
#pragma omp parallel for
  /* Iterate over spectra */
  for (size_t i = 0; i < eventInfo.spectrum_to_events.size(); i++) {
    const auto specInfo(eventInfo.spectrum_to_events[i]);

    const auto specId(std::get<0>(specInfo));
    const auto eventIteratorStart(std::get<1>(specInfo));
    const auto eventIteratorEnd(std::get<2>(specInfo));

    const auto detectorsForSpectrum = inst.spectrum_detector_mapping.at(specId);

    /* Get common detector parameters */
    const auto neutronFlightPath = l1 + get_l2(inst, detectorsForSpectrum);
    const Eigen::Vector3f qDirLabFrame =
        beamDirection - get_detector_direction(inst, detectorsForSpectrum);
    const Eigen::Vector3f qDir = convInfo.ub_matrix * qDirLabFrame;

    const auto conversionFactor =
        (NeutronMass * neutronFlightPath * 1e-10) / (1e-6 * h_bar);

    /* Lorentz correction */
    auto sinThetaSquared = 0.0;
    if (convInfo.lorentz_correction) {
      const auto theta = get_detector_two_theta(inst, detectorsForSpectrum);
      sinThetaSquared = sin(theta);
      sinThetaSquared *= sinThetaSquared;
    }

    /* Iterate over events for detector */
    auto &mdEventsForSpectrum = threadMdEventsForSpectrum[omp_get_thread_num()];
    for (auto eventIt = eventIteratorStart; eventIt != eventIteratorEnd;
         ++eventIt) {
      const auto wavenumber = conversionFactor / eventIt->tof;
      const Eigen::Vector3f center = qDir * wavenumber;

      auto weight = eventIt->weight;

      /* Ensure the event is within the bounds of the MD space */
      if (CheckCoordinatesInMDSpace<3>(space, center)) {
        /* Lorentz correction */
        if (convInfo.lorentz_correction) {
          const auto corr = sinThetaSquared * wavenumber * wavenumber *
                            wavenumber * wavenumber;
          weight *= corr;
        }

        /* Create event */
        mdEventsForSpectrum.emplace_back(center, space, weight);
      }
    }

    if (mdEventsForSpectrum.size() == 4 * 1024) {
#pragma omp critical
      {
        /* Add to event list */
        mdEvents.insert(mdEvents.cend(), mdEventsForSpectrum.begin(),
                        mdEventsForSpectrum.end());
      }
      mdEventsForSpectrum.clear();
    }
  }
  // Final cleanup step dealing with events that the critical section skipped.
  for (const auto &events : threadMdEventsForSpectrum)
    mdEvents.insert(mdEvents.cend(), events.begin(), events.end());
}
