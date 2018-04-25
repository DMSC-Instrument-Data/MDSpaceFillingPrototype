#include <cstring>

#include "BitInterleaving.h"
#include "CoordinateConversion.h"
#include "Types.h"

#pragma once

template <size_t ND> class MDEvent {
public:
  MDEvent(const MDCoordinate<ND> &coord, const MDSpace<ND> &space,
          float signal = 1.0f)
      : m_signal(signal) {
    const auto intCoord =
        ConvertCoordinatesToIntegerRange<ND, uint16_t>(space, coord);
    m_spaceFillingCurveOrder =
        Interleave_4_16_64(intCoord[0], intCoord[1], intCoord[2], intCoord[3]);
  }

  uint64_t spaceFillingCurveOrder() const { return m_spaceFillingCurveOrder; }

protected:
  float m_signal;
  uint64_t m_spaceFillingCurveOrder;
};
