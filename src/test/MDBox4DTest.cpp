#include <gtest/gtest.h>

#include <limits>

#include <boost/range/combine.hpp>

#include "TestUtil.h"

#include "MDBox.h"
#include "MDEvent.h"

constexpr size_t ND(4);
using IntT = uint16_t;
using MortonT = uint64_t;
constexpr auto interleaveFunc = interleave<ND, IntT, MortonT>;

using Event = MDEvent<ND>;
using Box = MDBox<ND, IntT, MortonT>;

TEST(MDBoxTest, test_simple) {
  Box root;

  /* Should be full integer range */
  using intLimits = std::numeric_limits<MortonT>;
  EXPECT_EQ(intLimits::min(), root.min());
  EXPECT_EQ(intLimits::max(), root.max());

  /* Morton number should be all set bits except for the most significant bit of
   * each interleaved number (as each coordinate was halved to produce the
   * "center" coordinate from which the box Morton number is calculated) */
  {
    const uint64_t expected = bit_string_to_int<MortonT>(
        "0000111111111111111111111111111111111111111111111111111111111111");
    EXPECT_EQ(expected, root.morton());
  }

  /* Should not have children */
  EXPECT_EQ(0, root.children().size());

  /* Should not have any events */
  EXPECT_EQ(0, root.eventCount());
}

TEST(MDBox4DTest, test_contains) {
  Box box({5, 5, 5, 5}, {8, 8, 8, 8});

  /* Event is inside box */
  EXPECT_TRUE(box.contains(Event(interleaveFunc({5, 5, 5, 5}))));
  EXPECT_TRUE(box.contains(Event(interleaveFunc({7, 7, 7, 7}))));
  EXPECT_TRUE(box.contains(Event(interleaveFunc({8, 8, 8, 8}))));

  /* Event Morton number is greater than box boundary */
  EXPECT_FALSE(box.contains(Event(interleaveFunc({3, 3, 3, 3}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 7, 7, 3}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 7, 3, 7}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 3, 7, 7}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({3, 7, 7, 7}))));

  /* Event Morton number is less than box boundary */
  EXPECT_FALSE(box.contains(Event(interleaveFunc({2, 2, 2, 2}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 7, 7, 2}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 7, 2, 7}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({7, 2, 7, 7}))));
  EXPECT_FALSE(box.contains(Event(interleaveFunc({2, 7, 7, 7}))));
}

TEST(MDBox4DTest, test_split) {
  Box root;
  root.split();

  /* Should have 2^4 children */
  EXPECT_EQ(16, root.children().size());

  /* Children should not have children */
  for (const auto &child : root.children()) {
    EXPECT_EQ(0, child.children().size());
  }

  /* Test dimension splitting */
  using intLimits = std::numeric_limits<IntT>;
  const IntT a = intLimits::min();
  const IntT c = intLimits::max();
  const IntT bu = c / 2;
  const IntT bl = bu + 1;

  {
    const auto box = root.children()[0];
    EXPECT_EQ(interleaveFunc({a, a, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({bu, bu, bu, bu}), box.max());
  }

  {
    const auto box = root.children()[1];
    EXPECT_EQ(interleaveFunc({bl, a, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, bu, bu, bu}), box.max());
  }

  {
    const auto box = root.children()[2];
    EXPECT_EQ(interleaveFunc({a, bl, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({bu, c, bu, bu}), box.max());
  }

  {
    const auto box = root.children()[3];
    EXPECT_EQ(interleaveFunc({bl, bl, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, bu, bu}), box.max());
  }

  {
    const auto box = root.children()[4];
    EXPECT_EQ(interleaveFunc({a, a, bl, a}), box.min());
    EXPECT_EQ(interleaveFunc({bu, bu, c, bu}), box.max());
  }

  {
    const auto box = root.children()[5];
    EXPECT_EQ(interleaveFunc({bl, a, bl, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, bu, c, bu}), box.max());
  }

  {
    const auto box = root.children()[6];
    EXPECT_EQ(interleaveFunc({a, bl, bl, a}), box.min());
    EXPECT_EQ(interleaveFunc({bu, c, c, bu}), box.max());
  }

  {
    const auto box = root.children()[7];
    EXPECT_EQ(interleaveFunc({bl, bl, bl, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, c, bu}), box.max());
  }

  {
    const auto box = root.children()[8];
    EXPECT_EQ(interleaveFunc({a, a, a, bl}), box.min());
    EXPECT_EQ(interleaveFunc({bu, bu, bu, c}), box.max());
  }

  {
    const auto box = root.children()[9];
    EXPECT_EQ(interleaveFunc({bl, a, a, bl}), box.min());
    EXPECT_EQ(interleaveFunc({c, bu, bu, c}), box.max());
  }

  {
    const auto box = root.children()[10];
    EXPECT_EQ(interleaveFunc({a, bl, a, bl}), box.min());
    EXPECT_EQ(interleaveFunc({bu, c, bu, c}), box.max());
  }

  {
    const auto box = root.children()[11];
    EXPECT_EQ(interleaveFunc({bl, bl, a, bl}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, bu, c}), box.max());
  }

  {
    const auto box = root.children()[12];
    EXPECT_EQ(interleaveFunc({a, a, bl, bl}), box.min());
    EXPECT_EQ(interleaveFunc({bu, bu, c, c}), box.max());
  }

  {
    const auto box = root.children()[13];
    EXPECT_EQ(interleaveFunc({bl, a, bl, bl}), box.min());
    EXPECT_EQ(interleaveFunc({c, bu, c, c}), box.max());
  }

  {
    const auto box = root.children()[14];
    EXPECT_EQ(interleaveFunc({a, bl, bl, bl}), box.min());
    EXPECT_EQ(interleaveFunc({bu, c, c, c}), box.max());
  }

  {
    const auto box = root.children()[15];
    EXPECT_EQ(interleaveFunc({bl, bl, bl, bl}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, c, c}), box.max());
  }
}

TEST(MDBox4DTest, test_split_3times) {
  Box root;
  root.split(3);

  /* Should have 2^4 children */
  EXPECT_EQ(16, root.children().size());

  /* Children should also have 2^4 children */
  for (const auto &child : root.children()) {
    EXPECT_EQ(16, child.children().size());

    /* So should their children */
    for (const auto &child2 : child.children()) {
      EXPECT_EQ(16, child2.children().size());

      /* But not their children */
      for (const auto &child3 : child2.children()) {
        EXPECT_EQ(0, child3.children().size());
      }
    }
  }
}

struct ExpectedBox {
  size_t event_count;
  std::vector<ExpectedBox> children;
};

void recursive_box_tree_validation(const Box &box, Box::ZCurveIterator &curveIt,
                                   const ExpectedBox &expected) {
  ASSERT_EQ(expected.event_count, box.eventCount());

  EXPECT_EQ(curveIt, box.eventBegin());

  ASSERT_EQ(expected.children.size(), box.children().size());

  if (expected.children.empty()) {
    std::advance(curveIt, expected.event_count);
  } else {
    for (auto tup : boost::combine(expected.children, box.children())) {
      ExpectedBox expectedChild;
      Box child;
      boost::tie(expectedChild, child) = tup;

      recursive_box_tree_validation(child, curveIt, expectedChild);
    }
  }

  EXPECT_EQ(curveIt, box.eventEnd());
}

TEST(MDBox4DTest, test_fill_events) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve;
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
    curve.emplace_back(interleaveFunc({b, a, b, a}));
    curve.emplace_back(interleaveFunc({b, a, b, b}));
    curve.emplace_back(interleaveFunc({b, b, a, a}));
    curve.emplace_back(interleaveFunc({b, b, a, b}));
    curve.emplace_back(interleaveFunc({b, b, b, a}));
    curve.emplace_back(interleaveFunc({b, b, b, b}));

    std::sort(curve.begin(), curve.end());
  }

  /* Create root box and set events */
  Box root(curve.cbegin(), curve.cend());

  /* Distribute events, split when a box contains 2 or greater events, limit to
   * three levels of child boxes */
  root.distributeEvents(2, 3);

  /* Should contain all events */
  EXPECT_EQ(20, root.eventCount());
  EXPECT_EQ(curve.cbegin(), root.eventBegin());
  EXPECT_EQ(curve.cend(), root.eventEnd());

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
