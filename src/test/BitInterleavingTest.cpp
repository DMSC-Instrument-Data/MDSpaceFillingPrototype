#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving.h"

TEST(BitInterleavingTest, Interleave2) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b0100010011101110 */

  const uint8_t a(170);
  const uint8_t b(15);
  const uint32_t res = Interleave2<uint32_t, uint8_t>(a, b);
  EXPECT_EQ(17646, res);
}

TEST(BitInterleavingTest, Interleave_N2) {
  Array<uint8_t, 2> a;
  a << 170, 15;
  const uint32_t res = Interleave<uint32_t, uint8_t, 2>(a);
  EXPECT_EQ(17646, res);
}

TEST(BitInterleavingTest, Deinterleave2) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b0100010011101110 */

  const uint32_t i(17646);
  uint8_t a(0), b(0);
  Deinterleave2<uint32_t, uint8_t>(i, a, b);
  EXPECT_EQ(170, a);
  EXPECT_EQ(15, b);
}

TEST(BitInterleavingTest, Deinterleave_N2) {
  const uint32_t i(17646);
  auto res = Deinterleave<uint32_t, uint8_t, 2>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
}

TEST(BitInterleavingTest, Interleave2_RoundTrip_Max_8Bit) {
  uint8_t a(255);
  uint8_t b(255);
  const uint16_t res = Interleave2<uint16_t, uint8_t>(a, b);
  EXPECT_EQ(65535, res);

  a = 0;
  b = 0;
  Deinterleave2<uint16_t, uint8_t>(res, a, b);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
}

TEST(BitInterleavingTest, Interleave2_RoundTrip_MinMax_8Bit) {
  uint8_t a(0);
  uint8_t b(255);
  const uint16_t res = Interleave2<uint16_t, uint8_t>(a, b);
  EXPECT_EQ(43690, res);

  a = 0;
  b = 0;
  Deinterleave2<uint16_t, uint8_t>(res, a, b);
  EXPECT_EQ(0, a);
  EXPECT_EQ(255, b);
}

TEST(BitInterleavingTest, Interleave2_RoundTrip_MaxMin_8Bit) {
  uint8_t a(255);
  uint8_t b(0);
  const uint16_t res = Interleave2<uint16_t, uint8_t>(a, b);
  EXPECT_EQ(21845, res);

  a = 0;
  b = 0;
  Deinterleave2<uint16_t, uint8_t>(res, a, b);
  EXPECT_EQ(255, a);
  EXPECT_EQ(0, b);
}

TEST(BitInterleavingTest, Interleave3) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b101100101100011010011010 */

  const uint8_t a(170);
  const uint8_t b(15);
  const uint8_t c(240);
  const uint32_t res = Interleave3<uint32_t, uint8_t>(a, b, c);
  EXPECT_EQ(11716250, res);
}

TEST(BitInterleavingTest, Interleave_N3) {
  Array<uint8_t, 3> a;
  a << 170, 15, 240;
  const uint32_t res = Interleave<uint32_t, uint8_t, 3>(a);
  EXPECT_EQ(11716250, res);
}

TEST(BitInterleavingTest, Deinterleave3) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b101100101100011010011010 */

  const uint32_t i(11716250);
  uint8_t a(0), b(0), c(0);
  Deinterleave3<uint32_t, uint8_t>(i, a, b, c);
  EXPECT_EQ(170, a);
  EXPECT_EQ(15, b);
  EXPECT_EQ(240, c);
}

TEST(BitInterleavingTest, Deinterleave_N3) {
  const uint32_t i(11716250);
  auto res = Deinterleave<uint32_t, uint8_t, 3>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
  EXPECT_EQ(240, res[2]);
}

TEST(BitInterleavingTest, Interleave3_RoundTrip_Max_8Bit) {
  uint8_t a(255);
  uint8_t b(255);
  uint8_t c(255);

  const uint32_t res = Interleave3<uint32_t, uint8_t>(a, b, c);
  EXPECT_EQ(16777215, res);

  a = 0;
  b = 0;
  c = 0;
  Deinterleave3<uint32_t, uint8_t>(res, a, b, c);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
  EXPECT_EQ(255, c);
}

TEST(BitInterleavingTest, Interleave4) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b00111100 */
  /* 0b01010100110111001011101000110010 */

  const uint8_t a(170);
  const uint8_t b(15);
  const uint8_t c(240);
  const uint8_t d(60);
  const uint32_t res = Interleave4<uint32_t, uint8_t>(a, b, c, d);
  EXPECT_EQ(1423751730, res);
}

TEST(BitInterleavingTest, Interleave_N4) {
  Array<uint8_t, 4> a;
  a << 170, 15, 240, 60;
  const uint32_t res = Interleave<uint32_t, uint8_t, 4>(a);
  EXPECT_EQ(1423751730, res);
}

TEST(BitInterleavingTest, Deinterleave4) {
  /* 0b10101010 */
  /* 0b00001111 */
  /* 0b11110000 */
  /* 0b00111100 */
  /* 0b01010100110111001011101000110010 */

  const uint32_t i(1423751730);
  uint8_t a(0), b(0), c(0), d(0);
  Deinterleave4<uint32_t, uint8_t>(i, a, b, c, d);
  EXPECT_EQ(170, a);
  EXPECT_EQ(15, b);
  EXPECT_EQ(240, c);
  EXPECT_EQ(60, d);
}

TEST(BitInterleavingTest, Deinterleave_N4) {
  const uint32_t i(1423751730);
  auto res = Deinterleave<uint32_t, uint8_t, 4>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
  EXPECT_EQ(240, res[2]);
  EXPECT_EQ(60, res[3]);
}

TEST(BitInterleavingTest, Interleave4_RoundTrip_Max_8Bit) {
  uint8_t a(255);
  uint8_t b(255);
  uint8_t c(255);
  uint8_t d(255);

  const uint32_t res = Interleave4<uint32_t, uint8_t>(a, b, c, d);
  EXPECT_EQ(4294967295, res);

  a = 0;
  b = 0;
  c = 0;
  d = 0;
  Deinterleave4<uint32_t, uint8_t>(res, a, b, c, d);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
  EXPECT_EQ(255, c);
  EXPECT_EQ(255, d);
}

TEST(BitInterleavingTest, Interleave4_128Bit) {
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

TEST(BitInterleavingTest, Deinterleave4_128Bit) {
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

TEST(BitInterleavingTest, Interleave4_128Bit_Boost) {
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

TEST(BitInterleavingTest, Deinterleave4_128Bit_Boost) {
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

TEST(BitInterleavingTest, Deinterleave_N4_128Bit_Boost) {
  using namespace boost::multiprecision;

  const uint64_t msb(6076574520689089756UL);
  const uint64_t lsb(13455272145591611954UL);

  uint128_t i(lsb);
  i |= uint128_t(msb) << 64;

  auto res = Deinterleave<uint128_t, uint32_t, 4>(i);

  EXPECT_EQ(2863311530, res[0]);
  EXPECT_EQ(65535, res[1]);
  EXPECT_EQ(4294901760, res[2]);
  EXPECT_EQ(16776960, res[3]);
}
