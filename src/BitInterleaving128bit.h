#include <cinttypes>
#include <cstddef>

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
