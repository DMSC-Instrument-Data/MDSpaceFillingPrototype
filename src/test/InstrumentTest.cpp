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
    const auto &dir = get_detector_direction(inst, {10});
    EXPECT_FLOAT_EQ(-std::sqrt(2) / 2.0f, dir[0]);
    EXPECT_FLOAT_EQ(0.0f, dir[1]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[2]);
  }

  {
    const auto &dir = get_detector_direction(inst, {50});
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
