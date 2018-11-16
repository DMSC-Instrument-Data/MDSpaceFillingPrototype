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

#pragma once

struct TofEvent {
  uint32_t id;
  float tof;
  double pulse_time;
  float weight;
};


using TofEventList = std::vector<TofEvent>;

// Mantid event types and storage
struct MantidNativeTofEvent {
    int64_t pulseTimePadding;
    double m_tof;
    MantidNativeTofEvent(const double& tof, const double& p) : m_tof(tof) {}
    double tof() {return m_tof;}

};
struct MantidNativeWeightedEvent : public MantidNativeTofEvent {
  /// The weight of this neutron.
  float weight;

  /// The SQUARE of the error that this neutron contributes.
  float errorSquared;

  MantidNativeWeightedEvent(double tof = 0, double it = 0, float w = 0, float e = 0) :
  MantidNativeTofEvent(tof, it), weight(w), errorSquared(e) {}
};
using MantidNativeEventList = std::vector<std::vector<MantidNativeWeightedEvent >* >;