#pragma once

#include "Types.h"

#include <iomanip>
#include <iostream>

/**
 * Expands a coordinate space enough that floating point error cannot cause an
 * overflow when mapping a value at the tupper limit of the range to an integer
 * range.
 */
template <size_t ND> void ExpandBounds(MDSpace<ND> &bounds) {
  bounds.col(0) = bounds.col(0) -
                  (bounds.col(0) * std::numeric_limits<float>::epsilon()).abs();
  bounds.col(1) = bounds.col(1) +
                  (bounds.col(1) * std::numeric_limits<float>::epsilon()).abs();
}

/**
 * Converts a point to integer range given a range of floating point
 * coordinates.
 */
template <size_t ND, typename IntT>
Eigen::Array<IntT, ND, 1> ConvertCoordinatesToIntegerRange(const auto &bounds,
                                                           const auto &coord) {
  const auto range = bounds.col(1) - bounds.col(0);
  const auto coordFactorOfRange = (coord - bounds.col(0)) / range;
  const auto n = coordFactorOfRange * std::numeric_limits<IntT>::max();
  return n.template cast<IntT>();
}

/**
 * Converts a point to integer range given a range of floating point
 * coordinates after casting to double precision floating point coordinate
 * values.
 */
template <size_t ND, typename IntT>
Eigen::Array<IntT, ND, 1>
ConvertCoordinatesToIntegerRangeDouble(const MDSpace<ND> &bounds,
                                       const MDCoordinate<ND> &coord) {
  return ConvertCoordinatesToIntegerRange<ND, IntT>(
      bounds.template cast<double>(), coord.template cast<double>());
}

/**
 * Converts a point expressed as integer coordinates back to floating point,
 * given bounds of the original coordinate space.
 */
template <size_t ND, typename IntT>
MDCoordinate<ND>
ConvertCoordinatesFromIntegerRange(const MDSpace<ND> &bounds,
                                   const Eigen::Array<IntT, ND, 1> &intCoord) {
  const auto range = bounds.col(1) - bounds.col(0);
  const auto coordFactorOfRange =
      intCoord.template cast<float>() / (float)std::numeric_limits<IntT>::max();
  return bounds.col(0) + (coordFactorOfRange * range);
}

/**
 * Converts a single floating point coordinate to an integer range.
 */
template <typename IntT>
IntT ConvertCoordinateToIntegerRange(float value, float lower, float upper) {
  const float coordFactorOfRange = (value - lower) / (upper - lower);
  const float intValue = coordFactorOfRange * std::numeric_limits<IntT>::max();
  return (IntT)intValue;
}

/**
 * Converts a single integer coordinate to a floating point.
 */
template <typename IntT>
float ConvertCoordinateFromIntegerRange(IntT value, float lower, float upper) {
  const float coordFactorOfRange =
      (float)value / (float)std::numeric_limits<IntT>::max();
  return lower + (coordFactorOfRange * (upper - lower));
}
