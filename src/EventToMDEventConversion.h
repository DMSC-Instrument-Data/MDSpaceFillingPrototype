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
  const auto beamDirection = get_beam_direction(inst);
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
    const auto qDirLabFrame =
        beamDirection - get_detector_direction(inst, detectorsForSpectrum);
    const auto qDir = convInfo.ub_matrix * qDirLabFrame;

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
    for (auto eventIt = eventIteratorStart; eventIt != eventIteratorEnd;
         ++eventIt) {
      auto weight = 1.0f;

      const auto wavenumber = conversionFactor / eventIt->tof;
      const auto center = qDir * wavenumber;

      /* Ensure the event is within the bounds of the MD space */
      if (CheckCoordinatesInMDSpace(space, center)) {
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
