#pragma once

#include "Types.h"

#include <iomanip>
#include <iostream>

template <size_t ND> void ExpandBounds(MDSpace<ND> &bounds) {
  bounds.col(0) = bounds.col(0) -
                  (bounds.col(0) * std::numeric_limits<float>::epsilon()).abs();
  bounds.col(1) = bounds.col(1) +
                  (bounds.col(1) * std::numeric_limits<float>::epsilon()).abs();
}

template <size_t ND, typename IntT>
Eigen::Array<IntT, ND, 1> ConvertCoordinatesToIntegerRange(const auto &bounds,
                                                           const auto &coord) {
  const auto range = bounds.col(1) - bounds.col(0);
  const auto coordFactorOfRange = (coord - bounds.col(0)) / range;
  const auto n = coordFactorOfRange * std::numeric_limits<IntT>::max();
  return n.template cast<IntT>();
}

template <size_t ND, typename IntT>
Eigen::Array<IntT, ND, 1>
ConvertCoordinatesToIntegerRangeDouble(const MDSpace<ND> &bounds,
                                       const MDCoordinate<ND> &coord) {
  return ConvertCoordinatesToIntegerRange<ND, IntT>(
      bounds.template cast<double>(), coord.template cast<double>());
}

template <size_t ND, typename IntT>
MDCoordinate<ND>
ConvertCoordinatesFromIntegerRange(const MDSpace<ND> &bounds,
                                   const Eigen::Array<IntT, ND, 1> &intCoord) {
  const auto range = bounds.col(1) - bounds.col(0);
  const auto coordFactorOfRange =
      intCoord.template cast<float>() / (float)std::numeric_limits<IntT>::max();
  return bounds.col(0) + (coordFactorOfRange * range);
}

template <typename IntT>
IntT ConvertCoordinateToIntegerRange(float value, float lower, float upper) {
  const float coordFactorOfRange = (value - lower) / (upper - lower);
  const float intValue = coordFactorOfRange * std::numeric_limits<IntT>::max();
  return (IntT)intValue;
}

template <typename IntT>
float ConvertCoordinateFromIntegerRange(IntT value, float lower, float upper) {
  const float coordFactorOfRange =
      (float)value / (float)std::numeric_limits<IntT>::max();
  return lower + (coordFactorOfRange * (upper - lower));
}
