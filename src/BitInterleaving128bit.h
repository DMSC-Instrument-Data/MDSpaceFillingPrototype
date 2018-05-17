#include <cinttypes>
#include <cstddef>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving.h"

#pragma once

void Interleave_4_32_128(uint64_t &msb, uint64_t &lsb, const uint32_t a,
                         const uint32_t b, const uint32_t c, const uint32_t d) {
  const size_t halfBitLen(sizeof(uint32_t) * 4);

  lsb = Interleave_4_16_64(a, b, c, d);
  msb = Interleave_4_16_64(a >> halfBitLen, b >> halfBitLen, c >> halfBitLen,
                           d >> halfBitLen);
}

void Deinterleave_4_32_128(const uint64_t msb, const uint64_t lsb, uint32_t &a,
                           uint32_t &b, uint32_t &c, uint32_t &d) {
  const size_t halfBitLen(sizeof(uint32_t) * 4);

  uint16_t aa, bb, cc, dd;

  Deinterleave_4_16_64(lsb, aa, bb, cc, dd);
  a = aa;
  b = bb;
  c = cc;
  d = dd;

  Deinterleave_4_16_64(msb, aa, bb, cc, dd);
  a |= aa << halfBitLen;
  b |= bb << halfBitLen;
  c |= cc << halfBitLen;
  d |= dd << halfBitLen;
}

using uint128_t = boost::multiprecision::uint128_t;

uint128_t Pad3_128(uint128_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0xffffffff_cppui128;
  x = (x | x << 64) & 0xffc0000000000000003fffff_cppui128;
  x = (x | x << 32) & 0xffc00000003ff800000007ff_cppui128;
  x = (x | x << 16) & 0xf80007c0003f0000f80007c0003f_cppui128;
  x = (x | x << 8) & 0xc0380700c0380700c0380700c03807_cppui128;
  x = (x | x << 4) & 0x8430843084308430843084308430843_cppui128;
  x = (x | x << 2) & 0x9090909090909090909090909090909_cppui128;
  x = (x | x << 1) & 0x11111111111111111111111111111111_cppui128;
  return x;
}

uint128_t Compact3_128(uint128_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0x11111111111111111111111111111111_cppui128;
  x = (x | x >> 1) & 0x9090909090909090909090909090909_cppui128;
  x = (x | x >> 2) & 0x8430843084308430843084308430843_cppui128;
  x = (x | x >> 4) & 0xc0380700c0380700c0380700c03807_cppui128;
  x = (x | x >> 8) & 0xf80007c0003f0000f80007c0003f_cppui128;
  x = (x | x >> 16) & 0xffc00000003ff800000007ff_cppui128;
  x = (x | x >> 32) & 0xffc0000000000000003fffff_cppui128;
  x = (x | x >> 64) & 0xffffffff_cppui128;
  return x;
}

uint128_t Interleave_4_32_128(uint128_t a, uint128_t b, uint128_t c,
                              uint128_t d) {
  return Pad3_128(a) | (Pad3_128(b) << 1) | (Pad3_128(c) << 2) |
         (Pad3_128(d) << 3);
}

void Deinterleave_4_32_128(uint128_t z, uint32_t &a, uint32_t &b, uint32_t &c,
                           uint32_t &d) {
  a = (uint32_t)Compact3_128(z);
  b = (uint32_t)Compact3_128(z >> 1);
  c = (uint32_t)Compact3_128(z >> 2);
  d = (uint32_t)Compact3_128(z >> 3);
}
