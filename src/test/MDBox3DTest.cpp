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

#include <limits>

#include "TestUtil.h"

#include "MDBox.h"
#include "MDEvent.h"

constexpr size_t ND(3);
using IntT = uint16_t;
using MortonT = uint64_t;
constexpr auto interleaveFunc = interleave<ND, IntT, MortonT>;

using Event = MDEvent<ND, IntT, MortonT>;
using Box = MDBox<ND, IntT, MortonT>;

TEST(MDBox4DTest, test_fill_events) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve;
  {
    const IntT step = std::numeric_limits<IntT>::max() / 4;
    const IntT a(step);
    const IntT b(step * 3);

    curve.emplace_back(interleaveFunc({a, a, a}));
    curve.emplace_back(interleaveFunc({a, a, b}));
    curve.emplace_back(interleaveFunc({a, b, a}));
    curve.emplace_back(interleaveFunc({a, b, b}));
    curve.emplace_back(interleaveFunc({b, a, a}));
    curve.emplace_back(interleaveFunc({b, a, b}));
    curve.emplace_back(interleaveFunc({b, b, a}));
    curve.emplace_back(interleaveFunc({b, b, b}));

    std::sort(curve.begin(), curve.end());
  }

  /* Create root box and set events */
  Box root(curve.cbegin(), curve.cend());

  /* Distribute events, split when a box contains 2 or greater events, limit to
   * three levels of child boxes */
  root.distributeEvents(2, 4);

  /* Should contain all events */
  EXPECT_EQ(8, root.eventCount());
  EXPECT_EQ(curve.cbegin(), root.eventBegin());
  EXPECT_EQ(curve.cend(), root.eventEnd());

  /* Expected structure */
  ExpectedBox expectedRoot{8,
                           {
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                           }};

  /* Validate structure */
  Box::ZCurveIterator curveIt = curve.cbegin();
  recursive_box_tree_validation(root, curveIt, expectedRoot);
}
