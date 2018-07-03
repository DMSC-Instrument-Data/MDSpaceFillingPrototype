#include <vector>

#include "BitInterleaving.h"
#include "CoordinateConversion.h"
#include "Types.h"

#pragma once

template <size_t ND, typename IntT, typename MortonT> class MDEvent {
public:
  using ZCurve = std::vector<MDEvent<ND, IntT, MortonT>>;

public:
  MDEvent(MortonT mortonNumber = 0, float signal = 1.0f)
      : m_morton(mortonNumber), m_signal(signal) {}

  MDEvent(const MDCoordinate<ND> &coord, const MDSpaceBounds<ND> &space,
          float signal = 1.0f)
      : m_signal(signal) {
    const auto intCoord =
        ConvertCoordinatesToIntegerRange<ND, IntT>(space, coord);
    m_morton = interleave<ND, IntT, MortonT>(intCoord);
  }

  size_t dimensions() const { return ND; }

  MDCoordinate<ND> coordinates(const MDSpaceBounds<ND> &space) const {
    const auto intCoord = deinterleave<ND, IntT, MortonT>(m_morton);
    return ConvertCoordinatesFromIntegerRange<ND, IntT>(space, intCoord);
  }

  MortonT mortonNumber() const { return m_morton; }
  float signal() const { return m_signal; }

  bool operator<(const MDEvent<ND, IntT, MortonT> &other) const {
    return m_morton < other.m_morton;
  }

private:
  float m_signal;
  MortonT m_morton;
};
