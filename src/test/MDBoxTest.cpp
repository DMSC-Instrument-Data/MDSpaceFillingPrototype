#include <gtest/gtest.h>

#include <limits>

#include "TestUtil.h"

#include "MDBox.h"
#include "MDEvent.h"

const size_t ND(4);
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

  /* Test getting all leaf boxes (only one here since no splitting has been
   * performed) */
  {
    std::vector<std::reference_wrapper<Box>> leafBoxes;
    root.getLeafBoxes(leafBoxes);
    EXPECT_EQ(1, leafBoxes.size());
  }
}

TEST(MDBoxTest, test_split) {
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
  const auto a = intLimits::min();
  const auto c = intLimits::max();
  const auto b = c / 2;

  {
    const auto box = root.children()[0];
    EXPECT_EQ(interleaveFunc({a, a, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({b, b, b, b}), box.max());
  }

  {
    const auto box = root.children()[1];
    EXPECT_EQ(interleaveFunc({b, a, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, b, b, b}), box.max());
  }

  {
    const auto box = root.children()[2];
    EXPECT_EQ(interleaveFunc({a, b, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({b, c, b, b}), box.max());
  }

  {
    const auto box = root.children()[3];
    EXPECT_EQ(interleaveFunc({b, b, a, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, b, b}), box.max());
  }

  {
    const auto box = root.children()[4];
    EXPECT_EQ(interleaveFunc({a, a, b, a}), box.min());
    EXPECT_EQ(interleaveFunc({b, b, c, b}), box.max());
  }

  {
    const auto box = root.children()[5];
    EXPECT_EQ(interleaveFunc({b, a, b, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, b, c, b}), box.max());
  }

  {
    const auto box = root.children()[6];
    EXPECT_EQ(interleaveFunc({a, b, b, a}), box.min());
    EXPECT_EQ(interleaveFunc({b, c, c, b}), box.max());
  }

  {
    const auto box = root.children()[7];
    EXPECT_EQ(interleaveFunc({b, b, b, a}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, c, b}), box.max());
  }

  {
    const auto box = root.children()[8];
    EXPECT_EQ(interleaveFunc({a, a, a, b}), box.min());
    EXPECT_EQ(interleaveFunc({b, b, b, c}), box.max());
  }

  {
    const auto box = root.children()[9];
    EXPECT_EQ(interleaveFunc({b, a, a, b}), box.min());
    EXPECT_EQ(interleaveFunc({c, b, b, c}), box.max());
  }

  {
    const auto box = root.children()[10];
    EXPECT_EQ(interleaveFunc({a, b, a, b}), box.min());
    EXPECT_EQ(interleaveFunc({b, c, b, c}), box.max());
  }

  {
    const auto box = root.children()[11];
    EXPECT_EQ(interleaveFunc({b, b, a, b}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, b, c}), box.max());
  }

  {
    const auto box = root.children()[12];
    EXPECT_EQ(interleaveFunc({a, a, b, b}), box.min());
    EXPECT_EQ(interleaveFunc({b, b, c, c}), box.max());
  }

  {
    const auto box = root.children()[13];
    EXPECT_EQ(interleaveFunc({b, a, b, b}), box.min());
    EXPECT_EQ(interleaveFunc({c, b, c, c}), box.max());
  }

  {
    const auto box = root.children()[14];
    EXPECT_EQ(interleaveFunc({a, b, b, b}), box.min());
    EXPECT_EQ(interleaveFunc({b, c, c, c}), box.max());
  }

  {
    const auto box = root.children()[15];
    EXPECT_EQ(interleaveFunc({b, b, b, b}), box.min());
    EXPECT_EQ(interleaveFunc({c, c, c, c}), box.max());
  }

  /* Test getting all leaf boxes */
  {
    Box::LeafBoxVector leafBoxes;
    root.getLeafBoxes(leafBoxes);
    EXPECT_EQ(16, leafBoxes.size());
  }
}

TEST(MDBoxTest, test_split_3) {
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

  /* Test getting all leaf boxes (should be 2^4 x depth) */
  {
    Box::LeafBoxVector leafBoxes;
    root.getLeafBoxes(leafBoxes);
    EXPECT_EQ(4096, leafBoxes.size());
  }
}

TEST(MDBoxTest, test_contains) {
  Box box({5, 5, 5, 5}, {8, 8, 8, 8});

  /* Event is inside box */
  EXPECT_TRUE(box.contains(Event(interleaveFunc({7, 7, 7, 7}))));

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

TEST(MDBoxTest, test_fill_events) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve;
  {
    const IntT step = std::numeric_limits<IntT>::max() / 4;
    const IntT a(step);
    const IntT b(step * 3);

    curve.emplace_back(interleaveFunc({a, a, a, a}));
    curve.emplace_back(interleaveFunc({a, a, a, b}));
    curve.emplace_back(interleaveFunc({a, a, b, a}));
    curve.emplace_back(interleaveFunc({a, a, b, b}));
    curve.emplace_back(interleaveFunc({a, b, a, a}));
    curve.emplace_back(interleaveFunc({a, b, a, b}));
    curve.emplace_back(interleaveFunc({a, b, b, a}));
    curve.emplace_back(interleaveFunc({a, b, b, b}));
    curve.emplace_back(interleaveFunc({b, a, a, a}));
    curve.emplace_back(interleaveFunc({b, a, a, b}));
    curve.emplace_back(interleaveFunc({b, a, b, a}));
    curve.emplace_back(interleaveFunc({b, a, b, b}));
    curve.emplace_back(interleaveFunc({b, b, a, a}));
    curve.emplace_back(interleaveFunc({b, b, a, b}));
    curve.emplace_back(interleaveFunc({b, b, b, a}));
    curve.emplace_back(interleaveFunc({b, b, b, b}));

    std::sort(curve.begin(), curve.end());
  }

  /* Create root box and split once */
  Box root;
  root.split(1);
  EXPECT_EQ(16, root.children().size());

  /* Add events to box structure */
  {
    Box::LeafBoxVector leafBoxes;
    root.getLeafBoxes(leafBoxes);
    std::sort(leafBoxes.begin(), leafBoxes.end(),
              [](const auto &a, const auto &b) {
                return a.get().morton() < b.get().morton();
              });

    Box::InsertEventsToLeafBoxes(leafBoxes, curve);

    root.setIteratorsFromChildren();
  }

  /* Root box should have all 16 events */
  EXPECT_EQ(16, root.eventCount());
  EXPECT_EQ(curve.cbegin(), root.eventBegin());
  EXPECT_EQ(curve.cend(), root.eventEnd());

  /* All child boxes should have 1 event each */
  Box::ZCurveIterator curveIt = curve.cbegin();
  for (auto &child : root.children()) {
    EXPECT_EQ(1, child.eventCount());
    EXPECT_EQ(curveIt, child.eventBegin());
    EXPECT_EQ(++curveIt, child.eventEnd());
  }
}

TEST(MDBoxTest, test_fill_events_and_prune) {
  /* Create test MD event Z-curve */
  Event::ZCurve curve;
  {
    const IntT step = std::numeric_limits<IntT>::max() / 8;
    const IntT a(step);
    const IntT b(step * 3);

    curve.emplace_back(interleaveFunc({a, a, a, a}));
    curve.emplace_back(interleaveFunc({a, a, a, b}));
    curve.emplace_back(interleaveFunc({a, a, b, a}));
    curve.emplace_back(interleaveFunc({a, a, b, b}));
    curve.emplace_back(interleaveFunc({a, b, a, a}));
    curve.emplace_back(interleaveFunc({a, b, a, b}));
    curve.emplace_back(interleaveFunc({a, b, b, a}));
    curve.emplace_back(interleaveFunc({a, b, b, b}));
    curve.emplace_back(interleaveFunc({b, a, a, a}));
    curve.emplace_back(interleaveFunc({b, a, a, b}));
    curve.emplace_back(interleaveFunc({b, a, b, a}));
    curve.emplace_back(interleaveFunc({b, a, b, b}));
    curve.emplace_back(interleaveFunc({b, b, a, a}));
    curve.emplace_back(interleaveFunc({b, b, a, b}));
    curve.emplace_back(interleaveFunc({b, b, b, a}));
    curve.emplace_back(interleaveFunc({b, b, b, b}));

    std::sort(curve.begin(), curve.end());
  }

  /* Create root box and split twice */
  Box root;
  root.split(2);
  EXPECT_EQ(16, root.children().size());

  /* Add events to box structure */
  {
    Box::LeafBoxVector leafBoxes;
    root.getLeafBoxes(leafBoxes);
    std::sort(leafBoxes.begin(), leafBoxes.end(),
              [](const auto &a, const auto &b) {
                return a.get().morton() < b.get().morton();
              });

    Box::InsertEventsToLeafBoxes(leafBoxes, curve);

    root.setIteratorsFromChildren();
  }

  /* Root box should have all 16 events */
  EXPECT_EQ(16, root.eventCount());
  EXPECT_EQ(curve.cbegin(), root.eventBegin());
  EXPECT_EQ(curve.cend(), root.eventEnd());

  {
    /* "First" child box should contain all 16 events */
    auto childIt = root.children().begin();
    EXPECT_EQ(16, childIt->eventCount());
    EXPECT_EQ(curve.cbegin(), childIt->eventBegin());
    EXPECT_EQ(curve.cend(), childIt->eventEnd());
    ++childIt;

    /* Remaining child boxes should all contain zero events */
    for (; childIt != root.children().end(); ++childIt) {
      EXPECT_EQ(0, childIt->eventCount());
      EXPECT_EQ(curve.cend(), childIt->eventBegin());
      EXPECT_EQ(curve.cend(), childIt->eventEnd());
    }
  }

  /* First child box should contain all events in correct order */
  {
    Box::ZCurveIterator curveIt = curve.cbegin();
    for (auto &child : root.children()[0].children()) {
      EXPECT_EQ(1, child.eventCount());
      EXPECT_EQ(curveIt, child.eventBegin());
      EXPECT_EQ(++curveIt, child.eventEnd());
    }
  }

  /* Prune boxes with less than 1 event */
  root.prune(1);

  /* Root box should still have all children */
  EXPECT_EQ(16, root.children().size());

  {
    /* "First" child box should still contain all 16 events in 16 boxes */
    auto childIt = root.children().begin();
    EXPECT_EQ(16, childIt->eventCount());
    EXPECT_EQ(curve.cbegin(), childIt->eventBegin());
    EXPECT_EQ(curve.cend(), childIt->eventEnd());
    EXPECT_EQ(16, childIt->children().size());
    ++childIt;

    /* Remaining child boxes should all contain zero events and have zero
     * children */
    for (; childIt != root.children().end(); ++childIt) {
      EXPECT_EQ(0, childIt->eventCount());
      EXPECT_EQ(curve.cend(), childIt->eventBegin());
      EXPECT_EQ(curve.cend(), childIt->eventEnd());
      EXPECT_EQ(0, childIt->children().size());
    }
  }
}
