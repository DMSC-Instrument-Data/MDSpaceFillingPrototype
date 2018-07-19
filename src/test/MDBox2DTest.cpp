#include <gtest/gtest.h>

#include <limits>

#include "TestUtil.h"

#include "MDBox.h"
#include "MDEvent.h"

constexpr size_t ND(2);
using IntT = uint16_t;
using MortonT = uint32_t;
constexpr auto interleaveFunc = interleave<ND, IntT, MortonT>;

using Event = MDEvent<ND>;
using Box = MDBox<ND, IntT, MortonT>;

TEST(MDBox2DTest, test_split) {
  Box root;
  root.split();

  /* Should have 2^2 children */
  EXPECT_EQ(4, root.children().size());

  /* Children should not have children */
  for (const auto &child : root.children()) {
    EXPECT_EQ(0, child.children().size());
  }

  /* Test dimension splitting */
  using intLimits = std::numeric_limits<IntT>;
  const auto a = intLimits::min();
  const auto c = intLimits::max();
  const auto bu = c / 2;
  const auto bl = bu + 1;

  {
    const auto box = root.children()[0];
    EXPECT_EQ(interleaveFunc({a, a}), box.min());
    EXPECT_EQ(interleaveFunc({bu, bu}), box.max());
  }

  {
    const auto box = root.children()[1];
    EXPECT_EQ(interleaveFunc({bl, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, bu}), box.max());
  }

  {
    const auto box = root.children()[2];
    EXPECT_EQ(interleaveFunc({a, bl}), box.min());
    EXPECT_EQ(interleaveFunc({bu, c}), box.max());
  }

  {
    const auto box = root.children()[3];
    EXPECT_EQ(interleaveFunc({bl, bl}), box.min());
    EXPECT_EQ(interleaveFunc({c, c}), box.max());
  }
}

TEST(MDBox2DTest, test_fill_events) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve;
  {
    const IntT step = std::numeric_limits<IntT>::max() / 4;
    const IntT a(step);
    const IntT b(step * 3);

    curve.emplace_back(interleaveFunc({a, a}));
    curve.emplace_back(interleaveFunc({a, b}));
    curve.emplace_back(interleaveFunc({b, a}));
    curve.emplace_back(interleaveFunc({b, b}));

    std::sort(curve.begin(), curve.end());
  }

  /* Create root box and set events */
  Box root(curve.cbegin(), curve.cend());

  /* Distribute events, split when a box contains 4 or greater events, limit to
   * one level of child boxes */
  root.distributeEvents(4, 1);

  /* Should contain all events */
  EXPECT_EQ(4, root.eventCount());
  EXPECT_EQ(curve.cbegin(), root.eventBegin());
  EXPECT_EQ(curve.cend(), root.eventEnd());

  /* Should have 16 child boxes */
  EXPECT_EQ(4, root.children().size());

  /* All child boxes should have 1 event each */
  Box::ZCurveIterator curveIt = curve.cbegin();
  for (auto &child : root.children()) {
    EXPECT_EQ(1, child.eventCount());
    EXPECT_EQ(curveIt, child.eventBegin());
    EXPECT_EQ(++curveIt, child.eventEnd());
  }
}
