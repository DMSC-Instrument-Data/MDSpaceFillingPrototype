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

TEST(BitInterleavingTest, Interleave2) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");

  const uint32_t res = interleave<2, uint16_t, uint32_t>({a, b});

  EXPECT_EQ(bit_string_to_int<uint32_t>("0100010011101110"), res);
}

TEST(BitInterleavingTest, Deinterleave2) {
  const uint32_t i = bit_string_to_int<uint32_t>("0100010011101110");

  const auto result = deinterleave<2, uint16_t, uint32_t>(i);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), result[0]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), result[1]);
}

TEST(BitInterleavingTest, Interleave3_32) {
  const uint8_t a = bit_string_to_int<uint8_t>("10101010");
  const uint8_t b = bit_string_to_int<uint8_t>("00001111");
  const uint8_t c = bit_string_to_int<uint8_t>("11110000");

  const uint32_t res = interleave<3, uint8_t, uint32_t>({a, b, c});

  EXPECT_EQ(bit_string_to_int<uint32_t>("101100101100011010011010"), res);
}

TEST(BitInterleavingTest, Deinterleave3_32) {
  const uint32_t i = bit_string_to_int<uint32_t>("101100101100011010011010");

  const auto result = deinterleave<3, uint8_t, uint32_t>(i);

  EXPECT_EQ(bit_string_to_int<uint8_t>("10101010"), result[0]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("00001111"), result[1]);
  EXPECT_EQ(bit_string_to_int<uint8_t>("11110000"), result[2]);
}

TEST(BitInterleavingTest, Interleave3_64) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");
  const uint16_t c = bit_string_to_int<uint8_t>("11110000");

  const uint64_t res = interleave<3, uint16_t, uint64_t>({a, b, c});

  EXPECT_EQ(bit_string_to_int<uint32_t>("101100101100011010011010"), res);
}

TEST(BitInterleavingTest, Deinterleave3_64) {
  const uint32_t i = bit_string_to_int<uint32_t>("101100101100011010011010");

  const auto result = deinterleave<3, uint16_t, uint64_t>(i);

  EXPECT_EQ(bit_string_to_int<uint16_t>("10101010"), result[0]);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00001111"), result[1]);
  EXPECT_EQ(bit_string_to_int<uint16_t>("11110000"), result[2]);
}

TEST(BitInterleavingTest, Interleave4) {
  const uint16_t a = bit_string_to_int<uint8_t>("10101010");
  const uint16_t b = bit_string_to_int<uint8_t>("00001111");
  const uint16_t c = bit_string_to_int<uint8_t>("11110000");
  const uint16_t d = bit_string_to_int<uint8_t>("00111100");

  const uint64_t res = interleave<4, uint16_t, uint64_t>({a, b, c, d});

  EXPECT_EQ(bit_string_to_int<uint32_t>("01010100110111001011101000110010"),
            res);
}

TEST(BitInterleavingTest, Deinterleave4) {
  const uint64_t i =
      bit_string_to_int<uint32_t>("01010100110111001011101000110010");

  const auto result = deinterleave<4, uint16_t, uint64_t>(i);

  EXPECT_EQ(bit_string_to_int<uint16_t>("10101010"), result[0]);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00001111"), result[1]);
  EXPECT_EQ(bit_string_to_int<uint16_t>("11110000"), result[2]);
  EXPECT_EQ(bit_string_to_int<uint16_t>("00111100"), result[3]);
}
