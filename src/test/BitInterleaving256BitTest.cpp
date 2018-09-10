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

const uint256_t interleavedBits3C = bit_string_to_int<uint64_t>(
    "1011001011001011001011001011001011001011001011001011001011001011");
const uint256_t interleavedBits3B = bit_string_to_int<uint64_t>(
    "0010110010110010110010110010110001101001101001101001101001101001");
const uint256_t interleavedBits3A = bit_string_to_int<uint64_t>(
    "1010011010011010011010011010011010011010011010011010011010011010");

const uint256_t interleavedBits4D = bit_string_to_int<uint64_t>(
    "0101010001010100010101000101010001010100010101000101010001010100");
const uint256_t interleavedBits4C = bit_string_to_int<uint64_t>(
    "1101110011011100110111001101110011011100110111001101110011011100");
const uint256_t interleavedBits4B = bit_string_to_int<uint64_t>(
    "1011101010111010101110101011101010111010101110101011101010111010");
const uint256_t interleavedBits4A = bit_string_to_int<uint64_t>(
    "0011001000110010001100100011001000110010001100100011001000110010");

TEST(BitInterleaving256BitTest, Interleave_3_64_256_Boost) {
  uint256_t res =
      interleave<3, uint64_t, uint256_t>({integerA, integerB, integerC});

  uint256_t expected(interleavedBits3A);
  expected |= interleavedBits3B << 64;
  expected |= interleavedBits3C << 128;

  EXPECT_EQ(expected, res);
}

TEST(BitInterleaving256BitTest, Deinterleave_3_64_256_Boost) {
  uint256_t z(interleavedBits3A);
  z |= interleavedBits3B << 64;
  z |= interleavedBits3C << 128;

  const auto result = deinterleave<3, uint64_t, uint256_t>(z);

  EXPECT_EQ(integerA, result[0]);
  EXPECT_EQ(integerB, result[1]);
  EXPECT_EQ(integerC, result[2]);
}

TEST(BitInterleaving256BitTest, Interleave_4_64_256_Boost) {
  uint256_t res = interleave<4, uint64_t, uint256_t>(
      {integerA, integerB, integerC, integerD});

  uint256_t expected(interleavedBits4A);
  expected |= interleavedBits4B << 64;
  expected |= interleavedBits4C << 128;
  expected |= interleavedBits4D << 192;

  EXPECT_EQ(expected, res);
}

TEST(BitInterleaving256BitTest, Deinterleave_4_64_256_Boost) {
  uint256_t z(interleavedBits4A);
  z |= interleavedBits4B << 64;
  z |= interleavedBits4C << 128;
  z |= interleavedBits4D << 192;

  const auto result = deinterleave<4, uint64_t, uint256_t>(z);

  EXPECT_EQ(integerA, result[0]);
  EXPECT_EQ(integerB, result[1]);
  EXPECT_EQ(integerC, result[2]);
  EXPECT_EQ(integerD, result[3]);
}
