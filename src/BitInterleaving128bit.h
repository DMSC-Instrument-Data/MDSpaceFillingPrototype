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
