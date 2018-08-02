#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "TestUtil.h"

#include "BitInterleaving.h"

const uint64_t integerA = bit_string_to_int<uint64_t>(
    "1010101010101010101010101010101010101010101010101010101010101010");
const uint64_t integerB = bit_string_to_int<uint64_t>(
    "0000000000000000000000000000000011111111111111111111111111111111");
const uint64_t integerC = bit_string_to_int<uint64_t>(
    "1111111111111111111111111111111100000000000000000000000000000000");
const uint64_t integerD = bit_string_to_int<uint64_t>(
    "0000000000000000111111111111111111111111111111110000000000000000");

const uint256_t interleavedBitsD = bit_string_to_int<uint64_t>(
    "0101010001010100010101000101010001010100010101000101010001010100");
const uint256_t interleavedBitsC = bit_string_to_int<uint64_t>(
    "1101110011011100110111001101110011011100110111001101110011011100");
const uint256_t interleavedBitsB = bit_string_to_int<uint64_t>(
    "1011101010111010101110101011101010111010101110101011101010111010");
const uint256_t interleavedBitsA = bit_string_to_int<uint64_t>(
    "0011001000110010001100100011001000110010001100100011001000110010");

TEST(BitInterleaving256BitTest, Interleave_4_64_256_Boost) {

  uint256_t res = interleave<4, uint64_t, uint256_t>(
      {integerA, integerB, integerC, integerD});

  uint256_t expected(interleavedBitsA);
  expected |= interleavedBitsB << 64;
  expected |= interleavedBitsC << 128;
  expected |= interleavedBitsD << 192;

  EXPECT_EQ(expected, res);
}

TEST(BitInterleaving256BitTest, Deinterleave_4_64_256_Boost) {
  uint256_t z(interleavedBitsA);
  z |= interleavedBitsB << 64;
  z |= interleavedBitsC << 128;
  z |= interleavedBitsD << 192;

  const auto result = deinterleave<4, uint64_t, uint256_t>(z);

  EXPECT_EQ(integerA, result[0]);
  EXPECT_EQ(integerB, result[1]);
  EXPECT_EQ(integerC, result[2]);
  EXPECT_EQ(integerD, result[3]);
}
