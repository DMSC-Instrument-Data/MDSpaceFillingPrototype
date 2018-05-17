#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleavingEigen.h"

TEST(BitInterleavingEigenTest, Interleave2) {
  Array<uint8_t, 2> a;
  a << 170, 15;
  const uint32_t res = Interleave<uint32_t, uint8_t, 2>(a);
  EXPECT_EQ(17646, res);
}

TEST(BitInterleavingEigenTest, Deinterleave2) {
  const uint32_t i(17646);
  auto res = Deinterleave<uint32_t, uint8_t, 2>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
}

TEST(BitInterleavingEigenTest, Interleave3) {
  Array<uint8_t, 3> a;
  a << 170, 15, 240;
  const uint32_t res = Interleave<uint32_t, uint8_t, 3>(a);
  EXPECT_EQ(11716250, res);
}

TEST(BitInterleavingEigenTest, Deinterleave3) {
  const uint32_t i(11716250);
  auto res = Deinterleave<uint32_t, uint8_t, 3>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
  EXPECT_EQ(240, res[2]);
}

TEST(BitInterleavingEigenTest, Interleave4) {
  Array<uint8_t, 4> a;
  a << 170, 15, 240, 60;
  const uint32_t res = Interleave<uint32_t, uint8_t, 4>(a);
  EXPECT_EQ(1423751730, res);
}

TEST(BitInterleavingEigenTest, Deinterleave4) {
  const uint32_t i(1423751730);
  auto res = Deinterleave<uint32_t, uint8_t, 4>(i);
  EXPECT_EQ(170, res[0]);
  EXPECT_EQ(15, res[1]);
  EXPECT_EQ(240, res[2]);
  EXPECT_EQ(60, res[3]);
}

TEST(BitInterleavingEigenTest, Deinterleave4_128Bit_Boost) {
  using namespace boost::multiprecision;

  const uint64_t msb(6076574520689089756UL);
  const uint64_t lsb(13455272145591611954UL);

  uint128_t i(lsb);
  i |= uint128_t(msb) << 64;

  auto res = Deinterleave<uint128_t, uint32_t, 4>(i);

  EXPECT_EQ(2863311530, res[0]);
  EXPECT_EQ(65535, res[1]);
  EXPECT_EQ(4294901760, res[2]);
  EXPECT_EQ(16776960, res[3]);
}
