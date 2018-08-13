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
  root.distributeEvents(4, 2);

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

    /* Ensure child has correct Morton "size"/"width" */
    EXPECT_EQ(std::numeric_limits<MortonT>::max() / 4,
              child.max() - child.min());
  }

  /* Positions in brackets assume axis origin in top left of coordinate space */

  /* Test axis dimensions of box 0 (upper left) */
  {
    IntArray<ND, IntT> lower =
        deinterleave<ND, IntT, MortonT>(root.children()[0].min());
    EXPECT_EQ(0, lower[0]);
    EXPECT_EQ(0, lower[1]);

    IntArray<ND, IntT> upper =
        deinterleave<ND, IntT, MortonT>(root.children()[0].max());
    EXPECT_EQ(32767, upper[0]);
    EXPECT_EQ(32767, upper[1]);
  }

  /* Test axis dimensions of box 1 (upper right) */
  {
    IntArray<ND, IntT> lower =
        deinterleave<ND, IntT, MortonT>(root.children()[1].min());
    EXPECT_EQ(32768, lower[0]);
    EXPECT_EQ(0, lower[1]);

    IntArray<ND, IntT> upper =
        deinterleave<ND, IntT, MortonT>(root.children()[1].max());
    EXPECT_EQ(65535, upper[0]);
    EXPECT_EQ(32767, upper[1]);
  }

  /* Test axis dimensions of box 2 (lower left) */
  {
    IntArray<ND, IntT> lower =
        deinterleave<ND, IntT, MortonT>(root.children()[2].min());
    EXPECT_EQ(0, lower[0]);
    EXPECT_EQ(32768, lower[1]);

    IntArray<ND, IntT> upper =
        deinterleave<ND, IntT, MortonT>(root.children()[2].max());
    EXPECT_EQ(32767, upper[0]);
    EXPECT_EQ(65535, upper[1]);
  }

  /* Test axis dimensions of box 3 (lower right) */
  {
    IntArray<ND, IntT> lower =
        deinterleave<ND, IntT, MortonT>(root.children()[3].min());
    EXPECT_EQ(32768, lower[0]);
    EXPECT_EQ(32768, lower[1]);

    IntArray<ND, IntT> upper =
        deinterleave<ND, IntT, MortonT>(root.children()[3].max());
    EXPECT_EQ(65535, upper[0]);
    EXPECT_EQ(65535, upper[1]);
  }
}
