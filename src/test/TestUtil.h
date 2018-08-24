#pragma once

#include <limits>
#include <string>

#include <boost/range/combine.hpp>

/* Reverse wrapper taken from: https://stackoverflow.com/a/28139075 */

template <typename T> struct reversion_wrapper { T &iterable; };

template <typename T> auto begin(reversion_wrapper<T> w) {
  return std::rbegin(w.iterable);
}

template <typename T> auto end(reversion_wrapper<T> w) {
  return std::rend(w.iterable);
}

template <typename T> reversion_wrapper<T> reverse(T &&iterable) {
  return {iterable};
}

/**
 * Converts from a bit pattern represented as a string to an integer of a given
 * fixed width.
 *
 * Assumes sizeof(IntT) == bitStr.size() / 8 and that the least significant bit
 * is at the end of the string.
 */
template <typename IntT> IntT bit_string_to_int(const std::string &bitStr) {
  IntT res = 0;
  size_t i = 0;
  for (const auto &c : reverse(bitStr)) {
    res |= (IntT)(c == '1' ? 1 : 0) << i++;
  }
  return res;
}

template <typename IntT> std::string int_to_bit_string(const IntT val) {
  std::string res;
  const auto bits(std::numeric_limits<IntT>::digits);
  for (size_t i = 0; i < bits; ++i) {
    const bool set = (val >> i) & 1;
    res.append(set ? "1" : "0");
  }
  std::reverse(res.begin(), res.end());
  return res;
}

struct ExpectedBox {
  size_t event_count;
  std::vector<ExpectedBox> children;
};

void recursive_box_tree_validation(const auto &box, auto &curveIt,
                                   const ExpectedBox &expected,
                                   size_t level = 0, size_t boxIdx = 0) {
  ASSERT_EQ(expected.event_count, box.eventCount())
      << "in box " << boxIdx << " at tree level " << level;

  /* EXPECT_EQ(curveIt, box.eventBegin()); */

  ASSERT_EQ(expected.children.size(), box.children().size())
      << "in box " << boxIdx << " at tree level " << level;

  if (expected.children.empty()) {
    std::advance(curveIt, expected.event_count);
  } else {
    size_t childBoxIdx(0);
    for (auto tup : boost::combine(expected.children, box.children())) {
      auto child = boost::get<1>(tup);
      auto expectedChild = boost::get<0>(tup);

      recursive_box_tree_validation(child, curveIt, expectedChild, level + 1,
                                    childBoxIdx++);
    }
  }

  /* EXPECT_EQ(curveIt, box.eventEnd()); */
}
