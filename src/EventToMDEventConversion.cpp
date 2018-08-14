#include "EventToMDEventConversion.h"

#include <boost/sort/sort.hpp>
#include <omp.h>

#include "Constants.h"

PreprocessedEventInfo preprocess_events(TofEventList &tofEvents) {
  PreprocessedEventInfo eventInfo;

  /* Sort TOF events by spectrum ID */
  boost::sort::block_indirect_sort(
      tofEvents.begin(), tofEvents.end(),
      [](const TofEvent &a, const TofEvent &b) { return a.id < b.id; });

  /* Get iterators to each spectrum range in TOF event list */
  auto rangeStart = tofEvents.begin();
  for (auto eventIt = tofEvents.begin() + 1; eventIt != tofEvents.end();
       ++eventIt) {
    if (eventIt->id != rangeStart->id) {
      eventInfo.spectrum_to_events.push_back(
          std::make_tuple(rangeStart->id, rangeStart, eventIt));
      rangeStart = eventIt;
    }
  }
  eventInfo.spectrum_to_events.push_back(
      std::make_tuple(rangeStart->id, rangeStart, tofEvents.end()));

  return eventInfo;
}

void convert_events(std::vector<MDEvent<3, uint16_t, uint64_t>> &mdEvents,
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
    std::vector<MDEvent<3, uint16_t, uint64_t>> mdEventsForSpectrum;
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
