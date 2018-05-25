#include <gtest/gtest.h>

#include <boost/multiprecision/gmp.hpp>

#include "TestUtil.h"

#include "BitInterleavingBoostGMP.h"

using mpz_int = boost::multiprecision::mpz_int;

TEST(BitInterleavingBoostGMPTest, Interleave2) {
  const mpz_int a = bit_string_to_int<uint8_t>("10101010");
  const mpz_int b = bit_string_to_int<uint8_t>("00001111");

  const mpz_int res = Interleave_2_16_32(a, b);

  EXPECT_EQ(bit_string_to_int<mpz_int>("0100010011101110"), res);
}

TEST(BitInterleavingBoostGMPTest, Deinterleave2) {
  const mpz_int i = bit_string_to_int<mpz_int>("0100010011101110");

  mpz_int a(0), b(0);
  Deinterleave_2_16_32(i, a, b);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), b);
}

TEST(BitInterleavingBoostGMPTest, Interleave3) {
  const mpz_int a = bit_string_to_int<uint8_t>("10101010");
  const mpz_int b = bit_string_to_int<uint8_t>("00001111");
  const mpz_int c = bit_string_to_int<uint8_t>("11110000");

  const mpz_int res = Interleave_3_16_64(a, b, c);

  EXPECT_EQ(bit_string_to_int<mpz_int>("101100101100011010011010"), res);
}

TEST(BitInterleavingBoostGMPTest, Deinterleave3) {
  const mpz_int i = bit_string_to_int<mpz_int>("101100101100011010011010");

  mpz_int a(0), b(0), c(0);
  Deinterleave_3_16_64(i, a, b, c);

  EXPECT_EQ(bit_string_to_int<mpz_int>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<mpz_int>("00001111"), b);
  EXPECT_EQ(bit_string_to_int<mpz_int>("11110000"), c);
}

TEST(BitInterleavingBoostGMPTest, Interleave4) {
  const mpz_int a = bit_string_to_int<uint8_t>("10101010");
  const mpz_int b = bit_string_to_int<uint8_t>("00001111");
  const mpz_int c = bit_string_to_int<uint8_t>("11110000");
  const mpz_int d = bit_string_to_int<uint8_t>("00111100");

  const mpz_int res = Interleave_4_16_64(a, b, c, d);

  EXPECT_EQ(bit_string_to_int<mpz_int>("01010100110111001011101000110010"),
            res);
}

TEST(BitInterleavingBoostGMPTest, Deinterleave4) {
  const mpz_int i =
      bit_string_to_int<mpz_int>("01010100110111001011101000110010");

  mpz_int a(0), b(0), c(0), d(0);
  Deinterleave_4_16_64(i, a, b, c, d);

  EXPECT_EQ(bit_string_to_int<mpz_int>("10101010"), a);
  EXPECT_EQ(bit_string_to_int<mpz_int>("00001111"), b);
  EXPECT_EQ(bit_string_to_int<mpz_int>("11110000"), c);
  EXPECT_EQ(bit_string_to_int<mpz_int>("00111100"), d);
}
