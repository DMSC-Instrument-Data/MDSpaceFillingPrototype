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

#include <gtest/gtest.h>

#include "MDEvent.h"
#include "Rebin.h"

#include "TestUtil.h"

TEST(RebinTest, test_existing_binmd_demo) {
  constexpr size_t ND(3);

  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -1.0f, 5.0f,
     2.0f, 6.0f,
    -5.0f, 5.0f;
  // clang-format on

  MDSpaceDimensions<ND> mdSpaceSize = mdSpace.col(1) - mdSpace.col(0);

  constexpr size_t binCount(100);

  Eigen::Quaternionf rot = Eigen::Quaternionf::Identity();
  Eigen::Vector3f scale = Eigen::Array3f::Constant(binCount - 1) / mdSpaceSize;
  Eigen::Vector3f origin =
      -Eigen::Array3f(mdSpace.col(0).transpose()) * Eigen::Array3f(scale);

  Eigen::Affine3f transform;
  transform.fromPositionOrientationScale(origin, rot, scale);

  {
    MDCoordinate<ND> coord{-1.0f, 4.0f, 5.0f};
    BinIndices<ND> transformed = (transform * coord).cast<size_t>();
    BinIndices<ND> expected{0, 49, 99};
    EXPECT_EQ(expected, transformed);
  }

  {
    MDCoordinate<ND> coord{5.0f, 4.0f, -5.0f};
    BinIndices<ND> transformed = (transform * coord).cast<size_t>();
    BinIndices<ND> expected{99, 49, 0};
    EXPECT_EQ(expected, transformed);
  }
}

TEST(RebinTest, test_prototype_binmd_demo) {
  constexpr size_t ND(3);
  using IntT = uint16_t;
  using MortonT = uint64_t;
  using Event = MDEvent<ND, IntT, MortonT>;

  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -1.0f, 5.0f,
     2.0f, 6.0f,
    -5.0f, 5.0f;
  // clang-format on

  const AffineND<float, 3> transform =
      generate_binning_transform<ND, IntT, MortonT>(mdSpace, 100);

  {
    const Event event({-1.0f, 4.0f, 5.0f}, mdSpace);

    const BinIndices<ND> transformed =
        (transform * event.integerCoordinates().cast<float>()).cast<size_t>();

    const BinIndices<ND> expected{0, 49, 98};
    EXPECT_EQ(expected, transformed) << "Transformed = " << transformed;
  }

  {
    const Event event({5.0f, 4.0f, -5.0f}, mdSpace);

    const BinIndices<ND> transformed =
        (transform * event.integerCoordinates().cast<float>()).cast<size_t>();

    const BinIndices<ND> expected{99, 49, 0};
    EXPECT_EQ(expected, transformed);
  }
}
