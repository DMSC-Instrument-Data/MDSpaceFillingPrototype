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
#include <thread>
#include <type_traits>
#include <utility>
#include <numeric>

#include "Constants.h"
#include "Instrument.h"
#include "MDEvent.h"
#include "TofEvent.h"
#include "Types.h"
#include "MantidDataHandling/LoadEventNexus.h"

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


MantidNativeEventList getMantidNativeEventList(PreprocessedEventInfo& pi);


template <typename IntT, typename MortonT>
void convert_events(std::vector<MDEvent<3, IntT, MortonT>> &mdEvents,
                    TofEventList &tofEvents, const ConversionInfo &convInfo,
                    const Instrument &inst, const MDSpaceBounds<3> &space) {
  /* Do preprocessing */
  const auto eventInfo = preprocess_events(tofEvents); //sorting here, avoid sorting
  /* Preallocate space for md events */
  mdEvents.reserve(mdEvents.size() + tofEvents.size());

  /* Get common instrument parameters */
  const Eigen::Vector3f beamDirection = get_beam_direction(inst);
  const auto l1 = get_l1(inst);

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
    std::vector<MDEvent<3, IntT, MortonT>> mdEventsForSpectrum;
    // add preallocation
    mdEventsForSpectrum.reserve(eventIteratorEnd - eventIteratorStart);
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

#pragma omp critical
    {
      /* Add to event list */
      mdEvents.insert(mdEvents.cend(), mdEventsForSpectrum.begin(),
                      mdEventsForSpectrum.end());
    }
  }
}


// function that more similar to what we have in Mantid
template <typename IntT, typename MortonT>
void convert_events_native(std::vector<MDEvent<3, IntT, MortonT>> &mdEvents,
                    const MantidNativeEventList& events, const ConversionInfo &convInfo,
                    const Instrument &inst, const MDSpaceBounds<3> &space) {
  std::size_t sz = 0;
  for(auto& it: events)
    sz += it->size();

  mdEvents.reserve(mdEvents.size() + sz);

  /* Get common instrument parameters */
  const Eigen::Vector3f beamDirection = get_beam_direction(inst);
  const auto l1 = get_l1(inst);

#pragma omp parallel for
  /* Iterate over spectra */
  for (size_t i = 0; i < events.size(); i++) {
    if(events[i]->empty())
      continue;
    const auto specId(i);
    const auto eventIteratorStart(events[i]->begin());
    const auto eventIteratorEnd(events[i]->end());
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
    std::vector<MDEvent<3, IntT, MortonT>> mdEventsForSpectrum;
    // add preallocation
    mdEventsForSpectrum.reserve(eventIteratorEnd - eventIteratorStart);
    for (auto eventIt = eventIteratorStart; eventIt != eventIteratorEnd;
         ++eventIt) {
      const auto wavenumber = conversionFactor / eventIt->tof();
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

#pragma omp critical
    {
      /* Add to event list */
      mdEvents.insert(mdEvents.cend(), mdEventsForSpectrum.begin(),
                      mdEventsForSpectrum.end());
    }
  }
}



// The same as previous function using native c++ threading
template <typename IntT, typename MortonT>
void convert_events_native_1(std::vector<MDEvent<3, IntT, MortonT>> &mdEvents,
                    MantidNativeEventList& events, const ConversionInfo &convInfo,
                    const Instrument &inst, const MDSpaceBounds<3> &space) {
  /* Preallocate space for md events */
  std::size_t sz = 0;
  for(auto& it: events)
    sz += it->size();

  mdEvents.resize(mdEvents.size() + sz);

  /* Get common instrument parameters */
  const Eigen::Vector3f beamDirection = get_beam_direction(inst);
  const auto l1 = get_l1(inst);


  auto numThreads = std::thread::hardware_concurrency();
//  std::vector<std::vector<std::vector<MDEvent<3, IntT, MortonT> > > > threadsData(numThreads);
//  std::vector<std::size_t>  counts(numThreads + 1, 0);
  std::atomic<std::size_t> curIdx{0};
  const auto portion = events.size() / numThreads / 3;
  std::atomic<std::size_t> cnt{0};

  std::vector<std::thread> workers;

  for (auto i = 0; i < numThreads; ++i)
    workers.emplace_back(
        [&](unsigned thIdx) {
          auto numSpecra = events.size();
          for(auto startSpectrum = cnt.fetch_add(portion); startSpectrum < numSpecra; startSpectrum = cnt.fetch_add(portion)) {
            auto toSpectrum = std::min(startSpectrum + portion, numSpecra);
            for (auto spectrum = startSpectrum; spectrum < toSpectrum; ++spectrum) {
              if(events[spectrum]->empty())
                continue;
              const auto start = events[spectrum]->begin();
              const auto finish = events[spectrum]->end();
              const auto detectorsForSpectrum = inst.spectrum_detector_mapping.at(spectrum);

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
              std::vector<MDEvent<3, IntT, MortonT>> mdEventsForSpectrum;
              // add preallocation
              mdEventsForSpectrum.reserve(finish - start);
              for (auto iter = start; iter != finish; ++iter) {
                const auto wavenumber = conversionFactor / iter->tof();
                const Eigen::Vector3f center = qDir * wavenumber;

                auto weight = iter->weight;

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
              auto from = curIdx.fetch_add(mdEventsForSpectrum.size());
              std::copy(mdEventsForSpectrum.begin(), mdEventsForSpectrum.end(), mdEvents.begin() + from);
            }
          }
        },
        i);

  for(auto& worker: workers)
    worker.join();

  mdEvents.resize(curIdx);
}