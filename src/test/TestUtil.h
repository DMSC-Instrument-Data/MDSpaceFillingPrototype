#pragma once

#include <string>

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
