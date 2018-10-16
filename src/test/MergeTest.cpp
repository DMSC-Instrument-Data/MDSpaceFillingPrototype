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
#include "Merge.h"

#include "TestUtil.h"

constexpr size_t ND(3);
using IntT = uint16_t;
using MortonT = uint64_t;

using Event = MDEvent<ND, IntT, MortonT>;

TEST(MergeTest, test_merge_event_curves_inplace) {
  /* Add first set of events */
  Event::ZCurve events1{Event(80), Event(40), Event(50),
                        Event(10), Event(30), Event(90)};
  std::sort(events1.begin(), events1.end());

  /* Add second set of events */
  Event::ZCurve events2{Event(91), Event(41), Event(51),
                        Event(11), Event(31), Event(21)};
  std::sort(events2.begin(), events2.end());

  merge_event_curves_inplace<Event>(events1, events2);

  /* Curve contains all events */
  EXPECT_EQ(12, events1.size());

  /* Check events now form a single sorted curve */
  EXPECT_EQ(10, events1[0].mortonNumber());
  EXPECT_EQ(11, events1[1].mortonNumber());
  EXPECT_EQ(21, events1[2].mortonNumber());
  EXPECT_EQ(30, events1[3].mortonNumber());
  EXPECT_EQ(31, events1[4].mortonNumber());
  EXPECT_EQ(40, events1[5].mortonNumber());
  EXPECT_EQ(41, events1[6].mortonNumber());
  EXPECT_EQ(50, events1[7].mortonNumber());
  EXPECT_EQ(51, events1[8].mortonNumber());
  EXPECT_EQ(80, events1[9].mortonNumber());
  EXPECT_EQ(90, events1[10].mortonNumber());
  EXPECT_EQ(91, events1[11].mortonNumber());
}

TEST(MergeTest, test_merge_event_curves) {
  /* Add first set of events */
  Event::ZCurve events1{Event(80), Event(40), Event(50),
                        Event(10), Event(30), Event(90)};
  std::sort(events1.begin(), events1.end());

  /* Add second set of events */
  Event::ZCurve events2{Event(91), Event(41), Event(51),
                        Event(11), Event(31), Event(21)};
  std::sort(events2.begin(), events2.end());

  Event::ZCurve curve;
  merge_event_curves<Event>(curve, events1, events2);

  /* Curve contains all events */
  EXPECT_EQ(12, curve.size());

  /* Check events now form a single sorted curve */
  EXPECT_EQ(10, curve[0].mortonNumber());
  EXPECT_EQ(11, curve[1].mortonNumber());
  EXPECT_EQ(21, curve[2].mortonNumber());
  EXPECT_EQ(30, curve[3].mortonNumber());
  EXPECT_EQ(31, curve[4].mortonNumber());
  EXPECT_EQ(40, curve[5].mortonNumber());
  EXPECT_EQ(41, curve[6].mortonNumber());
  EXPECT_EQ(50, curve[7].mortonNumber());
  EXPECT_EQ(51, curve[8].mortonNumber());
  EXPECT_EQ(80, curve[9].mortonNumber());
  EXPECT_EQ(90, curve[10].mortonNumber());
  EXPECT_EQ(91, curve[11].mortonNumber());
}
