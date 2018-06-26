#include "EventToMDEventConversion.h"

#include <boost/sort/sort.hpp>

#include "Constants.h"

PreprocessedEventInfo preprocess_events(TofEventList &events) {
  PreprocessedEventInfo eventInfo;

  /* Sort TOF events by detector ID */
  boost::sort::block_indirect_sort(
      events.begin(), events.end(),
      [](const TofEvent &a, const TofEvent &b) { return a.id < b.id; });

  /* Get iterators to each detector range in TOF event list */
  auto rangeStart = events.begin();
  for (auto eventIt = events.begin() + 1; eventIt != events.end(); ++eventIt) {
    if (eventIt->id != rangeStart->id) {
      eventInfo.detector_to_events[rangeStart->id] =
          std::make_pair(rangeStart, eventIt);
      rangeStart = eventIt;
    }
  }
  eventInfo.detector_to_events[rangeStart->id] =
      std::make_pair(rangeStart, events.end());

  return eventInfo;
}

void convert_events(TofEventList &events, const ConversionInfo &convInfo,
                    const Instrument &inst) {
  /* Do preprocessing */
  const auto eventInfo = preprocess_events(events);

  /* Get common instrument parameters */
  const auto beamDirection = get_beam_direction(inst);
  const auto l1 = get_l1(inst);

  /* Iterate over detectors */
  for (const auto detectorInfo : eventInfo.detector_to_events) {
    const auto detId(detectorInfo.first);
    const auto eventIteratorRange(detectorInfo.second);

    /* Get common detector parameters */
    const auto neutronFlightPath = l1 + get_l2(inst, detId);
    const auto qDirLabFrame =
        beamDirection - get_detector_direction(inst, detId);
    const auto qDir = convInfo.ub_matrix * qDirLabFrame;

    const auto conversionFactor =
        (NeutronMass * neutronFlightPath * 1e-10) / (1e-6 * h_bar);

    /* Lorentz correction */
    auto sinThetaSquared = 0.0;
    if (convInfo.lorentz_correction) {
      const auto theta = inst.detectors.at(detId).twoTheta;
      sinThetaSquared = sin(theta);
      sinThetaSquared *= sinThetaSquared;
    }

    std::cout << "detector " << detId << ": "
              << "L=" << neutronFlightPath << " : "
              << "f=" << conversionFactor << " : "
              << std::distance(events.begin(), eventIteratorRange.first) << '-'
              << std::distance(events.begin(), eventIteratorRange.second)
              << '\n';

    /* Iterate over events for detector */
    for (auto eventIt = eventIteratorRange.first;
         eventIt != eventIteratorRange.second; ++eventIt) {
      auto weight = 1.0f;

      const auto wavenumber = conversionFactor / eventIt->tof;
      const auto center = qDir * wavenumber;

      /* Lorentz correction */
      if (convInfo.lorentz_correction) {
        const auto corr =
            sinThetaSquared * wavenumber * wavenumber * wavenumber * wavenumber;
        weight *= corr;
      }

      // TODO
      std::cout << center << '\n' << weight << "\n\n";
    }

    std::cout << "\n\n";
  }
}
