#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "TestUtil.h"

#include "BitInterleaving.h"

TEST(BitInterleavingTest, Interleave2) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");

  const uint32_t res = Interleave_2_16_32(a, b);

  EXPECT_EQ(bit_string_to_int<uint32_t>("0100010011101110"), res);
}

TEST(BitInterleavingTest, Deinterleave2) {
  const uint32_t i = bit_string_to_int<uint32_t>("0100010011101110");

  uint16_t a(0), b(0);
  Deinterleave_2_16_32(i, a, b);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), b);
}

TEST(BitInterleavingTest, Interleave3) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");
  const uint16_t c = bit_string_to_int<uint8_t>("11110000");

  const uint64_t res = Interleave_3_16_64(a, b, c);

  EXPECT_EQ(bit_string_to_int<uint32_t>("101100101100011010011010"), res);
}

TEST(BitInterleavingTest, Deinterleave3) {
  const uint32_t i = bit_string_to_int<uint32_t>("101100101100011010011010");

  uint16_t a(0), b(0), c(0);
  Deinterleave_3_16_64(i, a, b, c);

  EXPECT_EQ(bit_string_to_int<uint16_t>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00001111"), b);
  EXPECT_EQ(bit_string_to_int<uint16_t>("11110000"), c);
}

TEST(BitInterleavingTest, Interleave4) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");
  const uint16_t c = bit_string_to_int<uint8_t>("11110000");
  const uint16_t d = bit_string_to_int<uint8_t>("00111100");

  const uint64_t res = Interleave_4_16_64(a, b, c, d);

  EXPECT_EQ(bit_string_to_int<uint32_t>("01010100110111001011101000110010"),
            res);
}

TEST(BitInterleavingTest, Deinterleave4) {
  const uint64_t i =
      bit_string_to_int<uint32_t>("01010100110111001011101000110010");

  uint16_t a(0), b(0), c(0), d(0);
  Deinterleave_4_16_64(i, a, b, c, d);

  EXPECT_EQ(bit_string_to_int<uint16_t>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00001111"), b);
  EXPECT_EQ(bit_string_to_int<uint16_t>("11110000"), c);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00111100"), d);
}
