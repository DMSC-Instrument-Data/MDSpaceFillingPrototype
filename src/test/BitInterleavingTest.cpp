#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving.h"

TEST(BitInterleavingTest, Interleave4) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b00111100 */
  /* 0b01010100110111001011101000110010 */

  const uint16_t a(170);
  const uint16_t b(15);
  const uint16_t c(240);
  const uint16_t d(60);
  const uint64_t res = Interleave_4_16_64(a, b, c, d);
  EXPECT_EQ(1423751730, res);
}

TEST(BitInterleavingTest, Deinterleave4) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b00111100 */
  /* 0b01010100110111001011101000110010 */

  const uint64_t i(1423751730);
  uint16_t a(0), b(0), c(0), d(0);
  Deinterleave_4_16_64(i, a, b, c, d);
  EXPECT_EQ(170, a);
  EXPECT_EQ(15, b);
  EXPECT_EQ(240, c);
  EXPECT_EQ(60, d);
}
