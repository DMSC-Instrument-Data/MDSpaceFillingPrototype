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
  /* Dummy event list required to pass iterators to MDBox constructor */
  std::vector<Event> dummyEventList;

  Box root(dummyEventList.cbegin(), dummyEventList.cend());

  /* Should be full integer range */
  using intLimits = std::numeric_limits<MortonT>;
  EXPECT_EQ(intLimits::min(), root.min());
  EXPECT_EQ(intLimits::max(), root.max());

  /* Should not have children */
  EXPECT_EQ(0, root.children().size());

  /* Should not have any events */
  EXPECT_EQ(0, root.eventCount());
}

TEST(MDBox4DTest, test_contains) {
  /* Dummy event list required to pass iterators to MDBox constructor */
  std::vector<Event> dummyEventList;

  Box box(dummyEventList.cbegin(), dummyEventList.cend(),
          interleaveFunc({5, 5, 5, 5}), interleaveFunc({8, 8, 8, 8}));

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
      auto child = boost::get<1>(tup);
      auto expectedChild = boost::get<0>(tup);

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
  root.distributeEvents(2, 4);

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
