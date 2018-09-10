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
    return ConvertCoordinatesFromIntegerRange<ND, IntT>(space,
                                                        integerCoordinates());
  }

  IntArray<ND, IntT> integerCoordinates() const {
    return deinterleave<ND, IntT, MortonT>(m_morton);
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
