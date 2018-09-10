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
#include "Types.h"

TEST(MDEventTest, test_float_coord_round_trip_4_64bit) {
  using Event = MDEvent<4, uint16_t, uint64_t>;

  MDSpaceBounds<4> space;
  // clang-format off
  space <<
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f;
  // clang-format on

  MDCoordinate<4> coord;
  coord << 5.0f, 6.0f, 7.0f, 3.0f;

  Event e(coord, space);

  const auto retrievedCoord = e.coordinates(space);

  EXPECT_NEAR(5.0f, retrievedCoord[0], 1e-4f);
  EXPECT_NEAR(6.0f, retrievedCoord[1], 1e-4f);
  EXPECT_NEAR(7.0f, retrievedCoord[2], 1e-4f);
  EXPECT_NEAR(3.0f, retrievedCoord[3], 1e-4f);
}
