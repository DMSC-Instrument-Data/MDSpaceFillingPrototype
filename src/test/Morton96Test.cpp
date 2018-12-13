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

#include "Types.h"
#include "TestUtil.h"

#include <gtest/gtest.h>
#include <iostream>
TEST(Morton96Test, To128BitTest) {

  Morton96 mrt{0};
  mrt.upper = 0xFFFF; // 0000'0000'0000'0000'1111'1111'1111'1111
  mrt.lower =0xFFFF00F0FFFF; // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111
  uint128_t check("0xFFFF0000FFFF00F0FFFF"); // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111
//  std::cerr << int_to_bit_string<uint128_t >(check) << "\n";
//  std::cerr << "00000000000000000000000000000000" << int_to_bit_string<uint32_t>(mrt.upper) << int_to_bit_string<uint64_t>(mrt.lower) << "\n";
//  std::cerr << int_to_bit_string<uint128_t >(mrt.to_uint128_t()) << "\n";
  EXPECT_EQ(check, mrt.to_uint128_t());

  mrt.upper = 0xFABC;
  mrt.lower =0xDCBA1234006075AF; // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111
  check = uint128_t("0xFABCDCBA1234006075AF");
  EXPECT_EQ(check, mrt.to_uint128_t());
}

TEST(Morton96Test, From128BitTest) {
  Morton96 mrt{0};
  mrt.upper = 0xFFFF; // 0000'0000'0000'0000'1111'1111'1111'1111
  mrt.lower =0xFFFF00F0FFFF; // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111
  Morton96 check(uint128_t("0xFFFF0000FFFF00F0FFFF")); // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111

  EXPECT_EQ(check.upper, mrt.upper);
  EXPECT_EQ(check.lower, mrt.lower);

  mrt.upper = 0xFABC;
  mrt.lower =0xDCBA1234006075AF; // 0000'0000'0000'0000'1111'1111'1111'1111"0000'0000'1111'0000'1111'1111'1111'1111
  check = Morton96(uint128_t("0xFABCDCBA1234006075AF"));
  EXPECT_EQ(check.upper, mrt.upper);
  EXPECT_EQ(check.lower, mrt.lower);
}
