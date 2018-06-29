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

    EXPECT_EQ(it, det_to_events.at(0).first);
    std::advance(it, 2);
    EXPECT_EQ(it, det_to_events.at(0).second);

    EXPECT_EQ(it, det_to_events.at(1).first);
    std::advance(it, 5);
    EXPECT_EQ(it, det_to_events.at(1).second);

    EXPECT_EQ(it, det_to_events.at(2).first);
    std::advance(it, 3);
    EXPECT_EQ(it, det_to_events.at(2).second);

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

  convert_events(events, convInfo, inst);
}
