#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving128bit.h"
#include "BitInterleavingNaive.h"

TEST(BitInterleaving128BitTest, Interleave4) {
  /* A 0b10101010101010101010101010101010 */
  /* B 0b00000000000000001111111111111111 */
  /* C 0b11111111111111110000000000000000 */
  /* D 0b00000000111111111111111100000000 */
  /* Imsb 0b0101010001010100010101000101010011011100110111001101110011011100 */
  /* Ilsb 0b1011101010111010101110101011101000110010001100100011001000110010 */

  const uint32_t a(2863311530);
  const uint32_t b(65535);
  const uint32_t c(4294901760);
  const uint32_t d(16776960);

  uint64_t msb(0);
  uint64_t lsb(0);

  Interleave4_128Bit<uint32_t>(msb, lsb, a, b, c, d);

  EXPECT_EQ(6076574520689089756UL, msb);
  EXPECT_EQ(13455272145591611954UL, lsb);
}

TEST(BitInterleaving128BitTest, Deinterleave4) {
  /* A 0b10101010101010101010101010101010 */
  /* B 0b00000000000000001111111111111111 */
  /* C 0b11111111111111110000000000000000 */
  /* D 0b00000000111111111111111100000000 */
  /* Imsb 0b0101010001010100010101000101010011011100110111001101110011011100 */
  /* Ilsb 0b1011101010111010101110101011101000110010001100100011001000110010 */

  const uint64_t msb(6076574520689089756UL);
  const uint64_t lsb(13455272145591611954UL);

  uint32_t a(0), b(0), c(0), d(0);
  Deinterleave4_128Bit<uint32_t>(msb, lsb, a, b, c, d);

  EXPECT_EQ(2863311530, a);
  EXPECT_EQ(65535, b);
  EXPECT_EQ(4294901760, c);
  EXPECT_EQ(16776960, d);
}

TEST(BitInterleaving128BitTest, Interleave4_Boost) {
  /* A 0b10101010101010101010101010101010 */
  /* B 0b00000000000000001111111111111111 */
  /* C 0b11111111111111110000000000000000 */
  /* D 0b00000000111111111111111100000000 */
  /* Imsb 0b0101010001010100010101000101010011011100110111001101110011011100 */
  /* Ilsb 0b1011101010111010101110101011101000110010001100100011001000110010 */

  using namespace boost::multiprecision;

  const uint32_t a(2863311530);
  const uint32_t b(65535);
  const uint32_t c(4294901760);
  const uint32_t d(16776960);

  uint128_t res = Interleave4<uint128_t, uint32_t>(a, b, c, d);

  const uint64_t msb(6076574520689089756UL);
  const uint64_t lsb(13455272145591611954UL);

  uint128_t expected(lsb);
  expected |= uint128_t(msb) << 64;

  EXPECT_EQ(expected, res);
}

TEST(BitInterleaving128BitTest, Deinterleave4_Boost) {
  /* A 0b10101010101010101010101010101010 */
  /* B 0b00000000000000001111111111111111 */
  /* C 0b11111111111111110000000000000000 */
  /* D 0b00000000111111111111111100000000 */
  /* Imsb 0b0101010001010100010101000101010011011100110111001101110011011100 */
  /* Ilsb 0b1011101010111010101110101011101000110010001100100011001000110010 */

  using namespace boost::multiprecision;

  const uint64_t msb(6076574520689089756UL);
  const uint64_t lsb(13455272145591611954UL);

  uint128_t i(lsb);
  i |= uint128_t(msb) << 64;

  uint32_t a(0), b(0), c(0), d(0);
  Deinterleave4<uint128_t, uint32_t>(i, a, b, c, d);

  EXPECT_EQ(2863311530, a);
  EXPECT_EQ(65535, b);
  EXPECT_EQ(4294901760, c);
  EXPECT_EQ(16776960, d);
}
