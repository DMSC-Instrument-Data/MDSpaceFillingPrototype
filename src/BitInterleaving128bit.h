#include <cinttypes>
#include <climits>
#include <cstddef>

#include <boost/multiprecision/cpp_int.hpp>

#include "BitInterleaving.h"

#pragma once

/**
 * Interleaves four 32 bit integers into a single 128 bit integer (represented
 * by 64 bit LSB and MSB).
 */
void Interleave_4_32_128(uint64_t &msb, uint64_t &lsb, const uint32_t a,
                         const uint32_t b, const uint32_t c, const uint32_t d) {
  const size_t halfBitLen(sizeof(uint32_t) * CHAR_BIT / 2);

  lsb = interleave<4, uint16_t, uint64_t>(
      {(uint16_t)a, (uint16_t)b, (uint16_t)c, (uint16_t)d});

  msb = interleave<4, uint16_t, uint64_t>(
      {(uint16_t)(a >> halfBitLen), (uint16_t)(b >> halfBitLen),
       (uint16_t)(c >> halfBitLen), (uint16_t)(d >> halfBitLen)});
}

/**
 * Deinterleaves a 128 bit integer (represented by 64 bit MSB and LSB) into four
 * 32 bit integers.
 */
void Deinterleave_4_32_128(const uint64_t msb, const uint64_t lsb, uint32_t &a,
                           uint32_t &b, uint32_t &c, uint32_t &d) {
  const size_t halfBitLen(sizeof(uint32_t) * CHAR_BIT / 2);

  const auto coordLsb = deinterleave<4, uint16_t, uint64_t>(lsb);
  const auto coordMsb = deinterleave<4, uint16_t, uint64_t>(msb);

  a = coordLsb[0] | (coordMsb[0] << halfBitLen);
  b = coordLsb[1] | (coordMsb[1] << halfBitLen);
  c = coordLsb[2] | (coordMsb[2] << halfBitLen);
  d = coordLsb[3] | (coordMsb[3] << halfBitLen);
}

using uint128_t = boost::multiprecision::uint128_t;

/**
 * Pad an integer with 3 padding bits between integer bits.
 * Maximum input width is 32 bit.
 */
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

/**
 * Compacts (removes padding) an integer with 3 padding bits between integer
 * bits.
 * Maximum output bit width is 32 bit.
 */
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

/**
 * Interleaves four 32 bit integers into a single 128 bit integer.
 */
uint128_t Interleave_4_32_128(uint128_t a, uint128_t b, uint128_t c,
                              uint128_t d) {
  return Pad3_128(a) | (Pad3_128(b) << 1) | (Pad3_128(c) << 2) |
         (Pad3_128(d) << 3);
}

/**
 * Deinterleaves a 128 bit integer into four 32 bit integers.
 */
void Deinterleave_4_32_128(uint128_t z, uint32_t &a, uint32_t &b, uint32_t &c,
                           uint32_t &d) {
  a = (uint32_t)Compact3_128(z);
  b = (uint32_t)Compact3_128(z >> 1);
  c = (uint32_t)Compact3_128(z >> 2);
  d = (uint32_t)Compact3_128(z >> 3);
}
