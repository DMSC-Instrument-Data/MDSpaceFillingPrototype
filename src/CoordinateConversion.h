#pragma once

#include "Types.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

/**
 * Calculates the required width of the interleaved integer to accurately
 * represent coordinates in a given space at a given resolution.
 */
template <size_t ND>
size_t CalculateRequiredCoordinateIntegerWidth(const MDSpaceBounds<ND> &bounds,
                                               const MDSpaceSteps<ND> &steps) {
  size_t maxBits(0);
  for (size_t i = 0; i < ND; ++i) {
    /* Calculate the required integer width to accurately represent coordinates
     * on axis i. */
    size_t bits =
        std::ceil(std::log2((bounds(i, 1) - bounds(i, 0)) / steps(i)));

    /* Record highest bit width requirement */
    maxBits = std::max(maxBits, bits);
  }
  return maxBits;
}

/**
 * Expands a coordinate space enough that floating point error cannot cause an
 * overflow when mapping a value at the upper limit of the range to an integer
 * range.
 *
 * Sets bounds to the next lowest (for lower axis bound) or highest (for upper
 * axis bound) representable floating point value.
 */
template <size_t ND> void ExpandBounds(MDSpaceBounds<ND> &bounds) {
  for (size_t i = 0; i < ND; ++i) {
    bounds(i, 0) =
        std::nexttoward(bounds(i, 0), std::numeric_limits<float>::lowest());
    bounds(i, 1) =
        std::nexttoward(bounds(i, 1), std::numeric_limits<float>::max());
  }
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
 * Converts a point expressed as integer coordinates back to floating point,
 * given bounds of the original coordinate space.
 */
template <size_t ND, typename IntT>
MDCoordinate<ND>
ConvertCoordinatesFromIntegerRange(const MDSpaceBounds<ND> &bounds,
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
