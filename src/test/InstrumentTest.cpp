#include <gtest/gtest.h>

#include "Instrument.h"

TEST(InstrumentTest, test_instrument_property_getters) {
  Instrument inst{Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                  Eigen::Vector3f(0.0f, 0.0f, -1.5f),
                  {
                      Detector{Eigen::Vector3f(-1.0f, 0.0f, 1.0f)},
                      Detector{Eigen::Vector3f(-0.5f, 0.0f, 1.0f)},
                      Detector{Eigen::Vector3f(0.0f, 0.0f, 1.0f)},
                      Detector{Eigen::Vector3f(0.5f, 0.0f, 1.0f)},
                      Detector{Eigen::Vector3f(1.0f, 0.0f, 1.0f)},
                  }};

  EXPECT_FLOAT_EQ(1.5f, get_l1(inst));
  EXPECT_EQ(Eigen::Vector3f(0.0f, 0.0f, 1.0f), get_beam_direction(inst));

  EXPECT_FLOAT_EQ(std::sqrt(2), get_l2(inst, inst.detectors[0]));
  EXPECT_FLOAT_EQ(1.0f, get_l2(inst, inst.detectors[2]));
  EXPECT_FLOAT_EQ(std::sqrt(2), get_l2(inst, inst.detectors[4]));

  {
    const auto &dir = get_detector_direction(inst, inst.detectors[0]);
    EXPECT_FLOAT_EQ(-std::sqrt(2) / 2.0f, dir[0]);
    EXPECT_FLOAT_EQ(0.0f, dir[1]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[2]);
  }

  {
    const auto &dir = get_detector_direction(inst, inst.detectors[4]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[0]);
    EXPECT_FLOAT_EQ(0.0f, dir[1]);
    EXPECT_FLOAT_EQ(std::sqrt(2) / 2.0f, dir[2]);
  }
}
