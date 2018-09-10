/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "TestUtil.h"

#include "BitInterleavingEigen.h"
#include "Types.h"

TEST(BitInterleavingEigenTest, Interleave2) {
  IntArray<2, uint8_t> a;
  // clang-format off
  a << bit_string_to_int<uint8_t>("10101010"),
       bit_string_to_int<uint8_t>("00001111");
  // clang-format on

  const uint32_t res = Interleave<uint32_t, uint8_t, 2>(a);

  EXPECT_EQ(bit_string_to_int<uint32_t>("0100010011101110"), res);
}

TEST(BitInterleavingEigenTest, Deinterleave2) {
  const uint32_t i = bit_string_to_int<uint32_t>("0100010011101110");

  auto res = Deinterleave<uint32_t, uint8_t, 2>(i);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), res[0]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), res[1]);
}

TEST(BitInterleavingEigenTest, Interleave3) {
  IntArray<3, uint8_t> a;
  // clang-format off
  a << bit_string_to_int<uint8_t>("10101010"),
       bit_string_to_int<uint8_t>("00001111"),
       bit_string_to_int<uint8_t>("11110000");
  // clang-format on

  const uint32_t res = Interleave<uint32_t, uint8_t, 3>(a);

  EXPECT_EQ(bit_string_to_int<uint32_t>("101100101100011010011010"), res);
}

TEST(BitInterleavingEigenTest, Deinterleave3) {
  const uint32_t i = bit_string_to_int<uint32_t>("101100101100011010011010");

  auto res = Deinterleave<uint32_t, uint8_t, 3>(i);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), res[0]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), res[1]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("11110000"), res[2]);
}

TEST(BitInterleavingEigenTest, Interleave4) {
  IntArray<4, uint8_t> a;
  // clang-format off
  a << bit_string_to_int<uint8_t>("10101010"),
       bit_string_to_int<uint8_t>("00001111"),
       bit_string_to_int<uint8_t>("11110000"),
       bit_string_to_int<uint8_t>("00111100");
  // clang-format on

  const uint32_t res = Interleave<uint32_t, uint8_t, 4>(a);

  EXPECT_EQ(bit_string_to_int<uint32_t>("01010100110111001011101000110010"),
            res);
}

TEST(BitInterleavingEigenTest, Deinterleave4) {
  const uint32_t i =
      bit_string_to_int<uint32_t>("01010100110111001011101000110010");

  auto res = Deinterleave<uint32_t, uint8_t, 4>(i);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), res[0]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), res[1]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("11110000"), res[2]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00111100"), res[3]);
}

TEST(BitInterleavingEigenTest, Deinterleave4_128Bit_Boost) {
  using namespace boost::multiprecision;

  const uint64_t msb = bit_string_to_int<uint64_t>(
      "0101010001010100010101000101010011011100110111001101110011011100");
  const uint64_t lsb = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");

  uint128_t i(lsb);
  i |= uint128_t(msb) << 64;

  auto res = Deinterleave<uint128_t, uint32_t, 4>(i);

  const uint32_t expectedA =
      bit_string_to_int<uint32_t>("10101010101010101010101010101010");
  const uint32_t expectedB =
      bit_string_to_int<uint32_t>("00000000000000001111111111111111");
  const uint32_t expectedC =
      bit_string_to_int<uint32_t>("11111111111111110000000000000000");
  const uint32_t expectedD =
      bit_string_to_int<uint32_t>("00000000111111111111111100000000");

  EXPECT_EQ(expectedA, res[0]);
  EXPECT_EQ(expectedB, res[1]);
  EXPECT_EQ(expectedC, res[2]);
  EXPECT_EQ(expectedD, res[3]);
}
