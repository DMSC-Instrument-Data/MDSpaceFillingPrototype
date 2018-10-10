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

#include <sstream>

#include "Instrument.h"

constexpr auto deg2rad(auto deg) { return deg * M_PI / 180.0; }

TEST(InstrumentTest, test_instrument_property_getters) {
  Instrument inst{Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                  Eigen::Vector3f(0.0f, 0.0f, -1.5f),
                  {
                      {10, Detector{Eigen::Vector3f(-1.0f, 0.0f, 1.0f)}},
                      {20, Detector{Eigen::Vector3f(-0.5f, 0.0f, 1.0f)}},
                      {30, Detector{Eigen::Vector3f(0.0f, 0.0f, 1.0f)}},
                      {40, Detector{Eigen::Vector3f(0.5f, 0.0f, 1.0f)}},
                      {50, Detector{Eigen::Vector3f(1.0f, 0.0f, 1.0f)}},
                      {60, Detector{Eigen::Vector3f(0.0f, 1.0f, 1.0f)}},
                      {70, Detector{Eigen::Vector3f(0.0f, 1.0f, 0.0f)}},
                  }};

  EXPECT_FLOAT_EQ(1.5f, get_l1(inst));
  EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 1.0f), get_beam_direction(inst));

  EXPECT_FLOAT_EQ(std::sqrt(2), get_l2(inst, {10}));
  EXPECT_FLOAT_EQ(1.0f, get_l2(inst, {30}));
  EXPECT_FLOAT_EQ(std::sqrt(2), get_l2(inst, {50}));

  {
    const Eigen::Vector3f &dir = get_detector_direction(inst, {10});
    EXPECT_FLOAT_EQ(-std::sqrt(2) / 2.0f, dir[0]);
    EXPECT_FLOAT_EQ(0.0f, dir[1]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[2]);
  }

  {
    const Eigen::Vector3f &dir = get_detector_direction(inst, {50});
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[0]);
    EXPECT_FLOAT_EQ(0.0f, dir[1]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[2]);
  }

  EXPECT_FLOAT_EQ(0.0f, get_detector_two_theta(inst, {30}));
  EXPECT_FLOAT_EQ(M_PI / 4.0f, get_detector_two_theta(inst, {60}));
  EXPECT_FLOAT_EQ(M_PI / 2.0f, get_detector_two_theta(inst, {70}));
}

TEST(InstrumentTest, test_instrument_property_getters_WISH_small) {
  Instrument inst{
      Eigen::Vector3f(0.0f, 0.0f, 0.0f),
      Eigen::Vector3f(0.0f, 0.0f, -40.0f),
      {
          {1707000, Detector{Eigen::Vector3f(-0.3863891422122102f, -0.517983f,
                                             2.169422723964781f)}},
          {5400300, Detector{Eigen::Vector3f(-0.9404389225826723f, 0.0889775f,
                                             -1.992881186268733f)}},
      }};

  EXPECT_FLOAT_EQ(40.0f, get_l1(inst));
  EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 1.0f), get_beam_direction(inst));

  /* Expected values taken from Mantid detector view */

  EXPECT_FLOAT_EQ(2.2636249496688396f, get_l2(inst, {1707000}));
  EXPECT_FLOAT_EQ(deg2rad(16.58760639183f),
                  get_detector_two_theta(inst, {1707000}));

  EXPECT_FLOAT_EQ(2.205429161228848f, get_l2(inst, {5400300}));
  EXPECT_FLOAT_EQ(deg2rad(154.6387415122f),
                  get_detector_two_theta(inst, {5400300}));
}

TEST(InstrumentTest, test_instrument_generate_1_to_1_spec_det_mapping) {
  Instrument inst{Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                  Eigen::Vector3f(0.0f, 0.0f, -1.5f),
                  {
                      {10, Detector{Eigen::Vector3f(-1.0f, 0.0f, 1.0f)}},
                      {20, Detector{Eigen::Vector3f(-0.5f, 0.0f, 1.0f)}},
                      {30, Detector{Eigen::Vector3f(0.0f, 0.0f, 1.0f)}},
                      {40, Detector{Eigen::Vector3f(0.5f, 0.0f, 1.0f)}},
                      {50, Detector{Eigen::Vector3f(1.0f, 0.0f, 1.0f)}},
                      {60, Detector{Eigen::Vector3f(0.0f, 1.0f, 1.0f)}},
                      {70, Detector{Eigen::Vector3f(0.0f, 1.0f, 0.0f)}},
                  }};

  generate_1_to_1_spec_det_mapping(inst);

  const auto specDetMap = inst.spectrum_detector_mapping;

  /* Test for correct number of mappings */
  EXPECT_EQ(7, specDetMap.size());

  /* Test detector list of each spectrum number */
  for (size_t i = 0; i < 7; i++) {
    const auto detList(specDetMap.at(i));

    /* Detector list should have a single detector ID */
    EXPECT_EQ(1, detList.size());

    /* Test detector ID */
    const specid_t expected((i + 1) * 10);
    EXPECT_EQ(expected, detList.at(0));
  }
}
