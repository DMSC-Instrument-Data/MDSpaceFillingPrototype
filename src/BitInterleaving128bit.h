#include <cinttypes>
#include <climits>
#include <cstddef>

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
