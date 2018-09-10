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

#pragma once

#include <Eigen/Dense>

#include "Types.h"

template <size_t ND, typename IntT, typename MortonT>
AffineND<float, ND> generate_binning_transform(const MDSpaceBounds<ND> &mdSpace,
                                               const size_t numBins) {
  using Vector = Eigen::Matrix<float, ND, 1>;
  using Array = Eigen::Array<float, ND, 1>;

  const MDSpaceDimensions<ND> mdSpaceSize = mdSpace.col(1) - mdSpace.col(0);
  const AffineND<float, ND> intToFloat =
      GenerateIntToFloatTransformation<float, ND, IntT>(mdSpace);

  const Vector scale = Array::Constant(numBins - 1) / mdSpaceSize;
  const Vector origin = -Array(mdSpace.col(0).transpose()) * Array(scale);
  AffineND<float, ND> transform =
      Eigen::Translation<float, ND>(origin) * Eigen::Scaling(scale);

  return transform * intToFloat;
}
