#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "TestUtil.h"

#include "BitInterleaving.h"
#include "BitInterleaving128bit.h"

TEST(BitInterleaving128BitTest, Interleave_4_32_128) {
  const uint32_t a =
      bit_string_to_int<uint32_t>("10101010101010101010101010101010");
  const uint32_t b =
      bit_string_to_int<uint32_t>("00000000000000001111111111111111");
  const uint32_t c =
      bit_string_to_int<uint32_t>("11111111111111110000000000000000");
  const uint32_t d =
      bit_string_to_int<uint32_t>("00000000111111111111111100000000");

  uint64_t msb(0);
  uint64_t lsb(0);

  Interleave_4_32_128(msb, lsb, a, b, c, d);

  const uint64_t expectedMsb = bit_string_to_int<uint64_t>(
      "0101010001010100010101000101010011011100110111001101110011011100");
  const uint64_t expectedLsb = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");

  EXPECT_EQ(expectedMsb, msb);
  EXPECT_EQ(expectedLsb, lsb);
}

TEST(BitInterleaving128BitTest, Deinterleave_4_32_128) {
  const uint64_t msb = bit_string_to_int<uint64_t>(
      "0101010001010100010101000101010011011100110111001101110011011100");
  const uint64_t lsb = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");

  uint32_t a(0), b(0), c(0), d(0);
  Deinterleave_4_32_128(msb, lsb, a, b, c, d);

  const uint32_t expectedA =
      bit_string_to_int<uint32_t>("10101010101010101010101010101010");
  const uint32_t expectedB =
      bit_string_to_int<uint32_t>("00000000000000001111111111111111");
  const uint32_t expectedC =
      bit_string_to_int<uint32_t>("11111111111111110000000000000000");
  const uint32_t expectedD =
      bit_string_to_int<uint32_t>("00000000111111111111111100000000");

  EXPECT_EQ(expectedA, a);
  EXPECT_EQ(expectedB, b);
  EXPECT_EQ(expectedC, c);
  EXPECT_EQ(expectedD, d);
}

TEST(BitInterleaving128BitTest, Interleave_4_32_128_Boost) {
  const uint128_t msb = bit_string_to_int<uint64_t>(
      "0101010001010100010101000101010011011100110111001101110011011100");
  const uint128_t lsb = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");

  const uint32_t a =
      bit_string_to_int<uint32_t>("10101010101010101010101010101010");
  const uint32_t b =
      bit_string_to_int<uint32_t>("00000000000000001111111111111111");
  const uint32_t c =
      bit_string_to_int<uint32_t>("11111111111111110000000000000000");
  const uint32_t d =
      bit_string_to_int<uint32_t>("00000000111111111111111100000000");

  uint128_t res = interleave<4, uint32_t, uint128_t>({a, b, c, d});

  uint128_t expected(lsb);
  expected |= msb << 64;

  EXPECT_EQ(expected, res);
}

TEST(BitInterleaving128BitTest, Deinterleave_4_32_128_Boost) {
  const uint128_t msb = bit_string_to_int<uint64_t>(
      "0101010001010100010101000101010011011100110111001101110011011100");
  const uint128_t lsb = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");

  uint128_t z(lsb);
  z |= msb << 64;

  const auto result = deinterleave<4, uint32_t, uint128_t>(z);

  const uint32_t expectedA =
      bit_string_to_int<uint32_t>("10101010101010101010101010101010");
  const uint32_t expectedB =
      bit_string_to_int<uint32_t>("00000000000000001111111111111111");
  const uint32_t expectedC =
      bit_string_to_int<uint32_t>("11111111111111110000000000000000");
  const uint32_t expectedD =
      bit_string_to_int<uint32_t>("00000000111111111111111100000000");

  EXPECT_EQ(expectedA, result[0]);
  EXPECT_EQ(expectedB, result[1]);
  EXPECT_EQ(expectedC, result[2]);
  EXPECT_EQ(expectedD, result[3]);
}
