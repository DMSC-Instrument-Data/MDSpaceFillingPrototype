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

#include "BitInterleaving.h"
#include "BitInterleaving128bit.h"

const uint32_t integerA =
    bit_string_to_int<uint32_t>("10101010101010101010101010101010");
const uint32_t integerB =
    bit_string_to_int<uint32_t>("00000000000000001111111111111111");
const uint32_t integerC =
    bit_string_to_int<uint32_t>("11111111111111110000000000000000");
const uint32_t integerD =
    bit_string_to_int<uint32_t>("00000000111111111111111100000000");

const uint64_t interleavedMsb = bit_string_to_int<uint64_t>(
    "0101010001010100010101000101010011011100110111001101110011011100");
const uint64_t interleavedLsb = bit_string_to_int<uint64_t>(
    "1011101010111010101110101011101000110010001100100011001000110010");

TEST(BitInterleaving128BitTest, Interleave_4_32_128) {
  uint64_t msb(0);
  uint64_t lsb(0);

  Interleave_4_32_128(msb, lsb, integerA, integerB, integerC, integerD);

  EXPECT_EQ(interleavedMsb, msb);
  EXPECT_EQ(interleavedLsb, lsb);
}

TEST(BitInterleaving128BitTest, Deinterleave_4_32_128) {
  uint32_t a(0), b(0), c(0), d(0);
  Deinterleave_4_32_128(interleavedMsb, interleavedLsb, a, b, c, d);

  EXPECT_EQ(integerA, a);
  EXPECT_EQ(integerB, b);
  EXPECT_EQ(integerC, c);
  EXPECT_EQ(integerD, d);
}

TEST(BitInterleaving128BitTest, Interleave_4_32_128_Boost) {
  uint128_t res = interleave<4, uint32_t, uint128_t>(
      {integerA, integerB, integerC, integerD});

  uint128_t interleaved(interleavedLsb);
  interleaved |= ((uint128_t)interleavedMsb) << 64;

  EXPECT_EQ(interleaved, res);
}

TEST(BitInterleaving128BitTest, Deinterleave_4_32_128_Boost) {
  uint128_t z(interleavedLsb);
  z |= ((uint128_t)interleavedMsb) << 64;

  const auto result = deinterleave<4, uint32_t, uint128_t>(z);

  EXPECT_EQ(integerA, result[0]);
  EXPECT_EQ(integerB, result[1]);
  EXPECT_EQ(integerC, result[2]);
  EXPECT_EQ(integerD, result[3]);
}
