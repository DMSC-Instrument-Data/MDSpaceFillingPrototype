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

#include "EventToMDEventConversion.h"

TEST(EventToMDEventConversionTest, preprocess_events) {
  // clang-format off
  TofEventList events{
    TofEvent{0, 0.0f, 0.0},
    TofEvent{1, 0.0f, 0.0},
    TofEvent{2, 0.0f, 0.0},
    TofEvent{2, 0.0f, 0.0},
    TofEvent{1, 0.0f, 0.0},
    TofEvent{1, 0.0f, 0.0},
    TofEvent{0, 0.0f, 0.0},
    TofEvent{1, 0.0f, 0.0},
    TofEvent{1, 0.0f, 0.0},
    TofEvent{2, 0.0f, 0.0},
  };
  // clang-format on

  const auto info = preprocess_events(events);
  const auto det_to_events = info.spectrum_to_events;

  EXPECT_EQ(3, det_to_events.size());

  /* Test for correct detector ID mapping */
  {
    auto it = events.begin();

    {
      const auto mapping = det_to_events[0];
      EXPECT_EQ(0, std::get<0>(mapping));
      EXPECT_EQ(it, std::get<1>(mapping));
      std::advance(it, 2);
      EXPECT_EQ(it, std::get<2>(mapping));
    }

    {
      const auto mapping = det_to_events[1];
      EXPECT_EQ(1, std::get<0>(mapping));
      EXPECT_EQ(it, std::get<1>(mapping));
      std::advance(it, 5);
      EXPECT_EQ(it, std::get<2>(mapping));
    }

    {
      const auto mapping = det_to_events[2];
      EXPECT_EQ(2, std::get<0>(mapping));
      EXPECT_EQ(it, std::get<1>(mapping));
      std::advance(it, 3);
      EXPECT_EQ(it, std::get<2>(mapping));
    }

    EXPECT_EQ(events.end(), it);
  }
}

TEST(EventToMDEventConversionTest, convert_events) {
  // clang-format off
  TofEventList events{
    TofEvent{1, 6000.0f, 0.0},
    TofEvent{2, 6000.0f, 0.0},
    TofEvent{5, 6000.0f, 0.0},
    TofEvent{5, 6000.0f, 0.0},
    TofEvent{2, 6000.0f, 0.0},
    TofEvent{2, 6000.0f, 0.0},
    TofEvent{1, 6000.0f, 0.0},
    TofEvent{2, 6000.0f, 0.0},
    TofEvent{2, 6000.0f, 0.0},
    TofEvent{5, 6000.0f, 0.0},
  };
  // clang-format on

  ConversionInfo convInfo{true, Eigen::Matrix3f::Identity()};

  Instrument inst{Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                  Eigen::Vector3f(0.0f, 0.0f, -1.5f),
                  {
                      {10, Detector{Eigen::Vector3f(-1.0f, 0.0f, 1.0f)}},
                      {20, Detector{Eigen::Vector3f(-0.5f, 0.0f, 1.0f)}},
                      {30, Detector{Eigen::Vector3f(0.0f, 0.0f, 1.0f)}},
                      {40, Detector{Eigen::Vector3f(0.5f, 0.0f, 1.0f)}},
                      {50, Detector{Eigen::Vector3f(1.0f, 0.0f, 1.0f)}},
                  },
                  {
                      {1, {10}}, {2, {20}}, {3, {30}}, {4, {40}}, {5, {50}},
                  }};

  MDSpaceBounds<3> space;
  // clang-format off
  space <<
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    -10.0f, 10.0f;
  // clang-format on

  std::vector<MDEvent<3, uint16_t, uint64_t>> mdEvents;

  convert_events(mdEvents, events, convInfo, inst, space);
}
