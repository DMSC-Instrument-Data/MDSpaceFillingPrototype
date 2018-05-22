#include <gtest/gtest.h>

#include "TestUtil.h"

TEST(TestUtilTest, bit_string_to_int_8bit) {
  uint8_t res = bit_string_to_int<uint8_t>("10101010");
  EXPECT_EQ(170, res);
}

TEST(TestUtilTest, bit_string_to_int_64bit) {
  uint64_t res = bit_string_to_int<uint64_t>(
      "1011101010111010101110101011101000110010001100100011001000110010");
  EXPECT_EQ(13455272145591611954UL, res);
}
