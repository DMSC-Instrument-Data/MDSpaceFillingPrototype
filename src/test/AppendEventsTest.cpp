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


constexpr size_t ND(4);
using IntT = uint16_t;
using MortonT = uint64_t;
constexpr auto interleaveFunc = interleave<ND, IntT, MortonT>;

using Event = MDEvent<ND, IntT, MortonT>;
using Box = MDBox<ND, IntT, MortonT>;

TEST(MDBox4DTest, test_append_events) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve, curveNew;
  {
    const IntT step = std::numeric_limits<IntT>::max() / 4;
    const IntT a(step);
    const IntT b(step * 3);

    curve.emplace_back(interleaveFunc({a, a, a, a}));
    curve.emplace_back(interleaveFunc({a, a, a + 1, a}));
    curve.emplace_back(interleaveFunc({a, a, a + 2, a}));
    curve.emplace_back(interleaveFunc({a, a, a, b}));
    curve.emplace_back(interleaveFunc({a, a, b, a}));
    curve.emplace_back(interleaveFunc({a, a, b, b}));
    curve.emplace_back(interleaveFunc({a, b, a, a}));
    curve.emplace_back(interleaveFunc({a, b, a, b}));
    curve.emplace_back(interleaveFunc({a, b, b, a}));
    curve.emplace_back(interleaveFunc({a, b, b, b}));
    curve.emplace_back(interleaveFunc({b, a, a, a}));
    curve.emplace_back(interleaveFunc({b + 1, a, a, a}));
    curve.emplace_back(interleaveFunc({b + 2, a, a, a}));
    curve.emplace_back(interleaveFunc({b, a, a, b}));
    curveNew.emplace_back(interleaveFunc({b, a, b, a}));
    curveNew.emplace_back(interleaveFunc({b, a, b, b}));
    curveNew.emplace_back(interleaveFunc({b, b, a, a}));
    curveNew.emplace_back(interleaveFunc({b, b, a, b}));
    curveNew.emplace_back(interleaveFunc({b, b, b, a}));
    curveNew.emplace_back(interleaveFunc({b, b, b, b}));

    std::sort(curve.begin(), curve.end());
    std::sort(curveNew.begin(), curveNew.end());
  }

  /* Create root box and set events */
  Box root(curve.cbegin(), curve.cend());

  /* Distribute events, split when a box contains 2 or greater events, limit to
   * three levels of child boxes */
  root.distributeEvents(2, 4);
  root.appendEvents(curveNew, 2, 4);

  /* Should contain all events */
  EXPECT_EQ(20, root.totalEvents());




  /* Expected structure */
  ExpectedBox expectedRoot{20,
                           {
                               {3,
                                {
                                    {1, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {2,
                                     {
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {2, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                     }},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                }},
                               {3,
                                {
                                    {3,
                                     {
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {0, {}},
                                         {3, {}},
                                     }},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                    {0, {}},
                                }},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
                               {1, {}},
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

