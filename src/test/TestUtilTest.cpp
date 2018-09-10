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

#include "TestUtil.h"

TEST(TestUtilTest, bit_string_to_int_8bit) {
  uint8_t res = bit_string_to_int<uint8_t>("10101010");
  EXPECT_EQ(170, res);
}

TEST(TestUtilTest, bit_string_to_int_64bit) {
  const auto res = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");
  EXPECT_EQ(13455272145591611954UL, res);
}

TEST(TestUtilTest, int_to_bit_string_64bit) {
  const auto res = int_to_bit_string<uint64_t>(13455272145591611954UL);
  EXPECT_EQ("1011101010111010101110101011101000110010001100100011001000110010",
            res);
}
