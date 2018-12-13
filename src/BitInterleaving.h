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
#include <cstddef>

#include "Types.h"

#pragma once

/**
 * Pad an integer with a given number of padding bits.
 *
 * @param N Number of padding bits to add
 * @param IntT Integer type
 * @param MortonT Padded integer type
 * @param x Integer to pad
 * @return Padded integer
 */
template <size_t N, typename IntT, typename MortonT> MortonT pad(IntT x) {
  throw std::runtime_error("No pad() specialisation.");
}

/**
 * Compacts (removes padding from) an integer with a given number of padding
 * bits.
 *
 * @param N Number of padding bits to remove
 * @param IntT Integer type
 * @param MortonT Padded integer type
 * @param x Padded integer
 * @return Original integer
 */
template <size_t N, typename IntT, typename MortonT> IntT compact(MortonT x) {
  throw std::runtime_error("No compact() specialisation.");
}

/* Bit masks used for pad and compact operations are derived using
 * docs/bit_padding_generator.py. */
/* For more details see docs/bit_interleaving.md. */

template <> uint32_t pad<1, uint16_t, uint32_t>(uint16_t v) {
  uint32_t x(v);
  x &= 0xffff;
  x = (x | x << 8) & 0xff00ff;
  x = (x | x << 4) & 0xf0f0f0f;
  x = (x | x << 2) & 0x33333333;
  x = (x | x << 1) & 0x55555555;
  return x;
}

template <> uint16_t compact<1, uint16_t, uint32_t>(uint32_t x) {
  x &= 0x55555555;
  x = (x | x >> 1) & 0x33333333;
  x = (x | x >> 2) & 0xf0f0f0f;
  x = (x | x >> 4) & 0xff00ff;
  x = (x | x >> 8) & 0xffff;
  return (uint16_t)x;
}

template <> uint64_t pad<1, uint16_t, uint64_t>(uint16_t v) {
  uint64_t x(v);
  x &= 0xffff;
  x = (x | x << 8) & 0xff00ff;
  x = (x | x << 4) & 0xf0f0f0f;
  x = (x | x << 2) & 0x33333333;
  x = (x | x << 1) & 0x55555555;
  return x;
}

template <> uint16_t compact<1, uint16_t, uint64_t>(uint64_t x) {
  x &= 0x55555555;
  x = (x | x >> 1) & 0x33333333;
  x = (x | x >> 2) & 0xf0f0f0f;
  x = (x | x >> 4) & 0xff00ff;
  x = (x | x >> 8) & 0xffff;
  return (uint16_t)x;
}

template <> uint32_t pad<2, uint8_t, uint32_t>(uint8_t v) {
  uint32_t x(v);
  x &= 0xff;
  x = (x | x << 8) & 0xf00f;
  x = (x | x << 4) & 0xc30c3;
  x = (x | x << 2) & 0x249249;
  return x;
}

template <> uint8_t compact<2, uint8_t, uint32_t>(uint32_t x) {
  x &= 0x249249;
  x = (x | x >> 2) & 0xc30c3;
  x = (x | x >> 4) & 0xf00f;
  x = (x | x >> 8) & 0xff;
  return (uint8_t)x;
}

template <> uint64_t pad<2, uint16_t, uint64_t>(uint16_t v) {
  uint64_t x(v);
  x &= 0xffff;
  x = (x | x << 16) & 0xff0000ff;
  x = (x | x << 8) & 0xf00f00f00f;
  x = (x | x << 4) & 0xc30c30c30c3;
  x = (x | x << 2) & 0x249249249249;
  return x;
}

template <> uint16_t compact<2, uint16_t, uint64_t>(uint64_t x) {
  x &= 0x249249249249;
  x = (x | x >> 2) & 0xc30c30c30c3;
  x = (x | x >> 4) & 0xf00f00f00f;
  x = (x | x >> 8) & 0xff0000ff;
  x = (x | x >> 16) & 0xffff;
  return (uint16_t)x;
}

template <> uint64_t pad<3, uint16_t, uint64_t>(uint16_t v) {
  uint64_t x(v);
  x &= 0xffff;
  x = (x | x << 32) & 0xf800000007ff;
  x = (x | x << 16) & 0xf80007c0003f;
  x = (x | x << 8) & 0xc0380700c03807;
  x = (x | x << 4) & 0x843084308430843;
  x = (x | x << 2) & 0x909090909090909;
  x = (x | x << 1) & 0x1111111111111111;
  return x;
}

template <> uint16_t compact<3, uint16_t, uint64_t>(uint64_t x) {
  x &= 0x1111111111111111;
  x = (x | x >> 1) & 0x909090909090909;
  x = (x | x >> 2) & 0x843084308430843;
  x = (x | x >> 4) & 0xc0380700c03807;
  x = (x | x >> 8) & 0xf80007c0003f;
  x = (x | x >> 16) & 0xf800000007ff;
  x = (x | x >> 32) & 0xffff;
  return (uint16_t)x;
}

template <> uint128_t pad<1, uint32_t, uint128_t>(uint32_t v) {
  using namespace boost::multiprecision::literals;
  uint128_t x(v);
  x &= 0xffffffff_cppui128;
  x = (x | x << 16) & 0xffff0000ffff_cppui128;
  x = (x | x << 8) & 0xff00ff00ff00ff_cppui128;
  x = (x | x << 4) & 0xf0f0f0f0f0f0f0f_cppui128;
  x = (x | x << 2) & 0x3333333333333333_cppui128;
  x = (x | x << 1) & 0x5555555555555555_cppui128;
  return x;
}

template <> uint32_t compact<1, uint32_t, uint128_t>(uint128_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0x5555555555555555_cppui128;
  x = (x | x >> 1) & 0x3333333333333333_cppui128;
  x = (x | x >> 2) & 0xf0f0f0f0f0f0f0f_cppui128;
  x = (x | x >> 4) & 0xff00ff00ff00ff_cppui128;
  x = (x | x >> 8) & 0xffff0000ffff_cppui128;
  x = (x | x >> 16) & 0xffffffff_cppui128;
  return (uint32_t)x;
}

template <> uint128_t pad<2, uint32_t, uint128_t>(uint32_t v) {
  using namespace boost::multiprecision::literals;
  uint128_t x(v);
  x &= 0xffffffff_cppui128;
  x = (x | x << 32) & 0xffff00000000ffff_cppui128;
  x = (x | x << 16) & 0xff0000ff0000ff0000ff_cppui128;
  x = (x | x << 8) & 0xf00f00f00f00f00f00f00f_cppui128;
  x = (x | x << 4) & 0xc30c30c30c30c30c30c30c3_cppui128;
  x = (x | x << 2) & 0x249249249249249249249249_cppui128;
  return x;
}

template <> uint32_t compact<2, uint32_t, uint128_t>(uint128_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0x249249249249249249249249_cppui128;
  x = (x | x >> 2) & 0xc30c30c30c30c30c30c30c3_cppui128;
  x = (x | x >> 4) & 0xf00f00f00f00f00f00f00f_cppui128;
  x = (x | x >> 8) & 0xff0000ff0000ff0000ff_cppui128;
  x = (x | x >> 16) & 0xffff00000000ffff_cppui128;
  x = (x | x >> 32) & 0xffffffff_cppui128;
  return (uint32_t)x;
}

template <> uint128_t pad<3, uint32_t, uint128_t>(uint32_t v) {
  using namespace boost::multiprecision::literals;
  uint128_t x(v);
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

template <> uint32_t compact<3, uint32_t, uint128_t>(uint128_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0x11111111111111111111111111111111_cppui128;
  x = (x | x >> 1) & 0x9090909090909090909090909090909_cppui128;
  x = (x | x >> 2) & 0x8430843084308430843084308430843_cppui128;
  x = (x | x >> 4) & 0xc0380700c0380700c0380700c03807_cppui128;
  x = (x | x >> 8) & 0xf80007c0003f0000f80007c0003f_cppui128;
  x = (x | x >> 16) & 0xffc00000003ff800000007ff_cppui128;
  x = (x | x >> 32) & 0xffc0000000000000003fffff_cppui128;
  x = (x | x >> 64) & 0xffffffff_cppui128;
  return (uint32_t)x;
}

using uint256_t = boost::multiprecision::uint256_t;

template <> uint256_t pad<2, uint64_t, uint256_t>(uint64_t v) {
  using namespace boost::multiprecision::literals;
  uint256_t x(v);
  x &= 0xffffffffffffffff_cppui256;
  x = (x | x << 64) & 0xffffffff0000000000000000ffffffff_cppui256;
  x = (x | x << 32) & 0xffff00000000ffff00000000ffff00000000ffff_cppui256;
  x = (x | x << 16) & 0xff0000ff0000ff0000ff0000ff0000ff0000ff0000ff_cppui256;
  x = (x | x << 8) & 0xf00f00f00f00f00f00f00f00f00f00f00f00f00f00f00f_cppui256;
  x = (x | x << 4) & 0xc30c30c30c30c30c30c30c30c30c30c30c30c30c30c30c3_cppui256;
  x = (x | x << 2) &
      0x249249249249249249249249249249249249249249249249_cppui256;
  return x;
}

template <> uint64_t compact<2, uint64_t, uint256_t>(uint256_t x) {
  using namespace boost::multiprecision::literals;
  x &= 0x249249249249249249249249249249249249249249249249_cppui256;
  x = (x | x >> 2) & 0xc30c30c30c30c30c30c30c30c30c30c30c30c30c30c30c3_cppui256;
  x = (x | x >> 4) & 0xf00f00f00f00f00f00f00f00f00f00f00f00f00f00f00f_cppui256;
  x = (x | x >> 8) & 0xff0000ff0000ff0000ff0000ff0000ff0000ff0000ff_cppui256;
  x = (x | x >> 16) & 0xffff00000000ffff00000000ffff00000000ffff_cppui256;
  x = (x | x >> 32) & 0xffffffff0000000000000000ffffffff_cppui256;
  x = (x | x >> 64) & 0xffffffffffffffff_cppui256;
  return (uint64_t)x;
}

template <> uint256_t pad<3, uint64_t, uint256_t>(uint64_t v) {
  using namespace boost::multiprecision::literals;
  uint256_t x(v);
  x &= 0xffffffffffffffff_cppui256;
  x = (x | x << 128) &
      0xfffff800000000000000000000000000000007ffffffffff_cppui256;
  x = (x | x << 64) &
      0xfffff80000000000000007ffffc0000000000000003fffff_cppui256;
  x = (x | x << 32) &
      0xffc00000003ff800000007ff00000000ffc00000003ff800000007ff_cppui256;
  x = (x | x << 16) &
      0xf80007c0003f0000f80007c0003f0000f80007c0003f0000f80007c0003f_cppui256;
  x = (x | x << 8) &
      0xc0380700c0380700c0380700c0380700c0380700c0380700c0380700c03807_cppui256;
  x = (x | x << 4) &
      0x843084308430843084308430843084308430843084308430843084308430843_cppui256;
  x = (x | x << 2) &
      0x909090909090909090909090909090909090909090909090909090909090909_cppui256;
  x = (x | x << 1) &
      0x1111111111111111111111111111111111111111111111111111111111111111_cppui256;
  return x;
}

template <> uint64_t compact<3, uint64_t, uint256_t>(uint256_t x) {
  using namespace boost::multiprecision::literals;
  x &=
      0x1111111111111111111111111111111111111111111111111111111111111111_cppui256;
  x = (x | x >> 1) &
      0x909090909090909090909090909090909090909090909090909090909090909_cppui256;
  x = (x | x >> 2) &
      0x843084308430843084308430843084308430843084308430843084308430843_cppui256;
  x = (x | x >> 4) &
      0xc0380700c0380700c0380700c0380700c0380700c0380700c0380700c03807_cppui256;
  x = (x | x >> 8) &
      0xf80007c0003f0000f80007c0003f0000f80007c0003f0000f80007c0003f_cppui256;
  x = (x | x >> 16) &
      0xffc00000003ff800000007ff00000000ffc00000003ff800000007ff_cppui256;
  x = (x | x >> 32) &
      0xfffff80000000000000007ffffc0000000000000003fffff_cppui256;
  x = (x | x >> 64) &
      0xfffff800000000000000000000000000000007ffffffffff_cppui256;
  x = (x | x >> 128) & 0xffffffffffffffff_cppui256;
  return (uint64_t)x;
}

/**
 * Interleaves an integer coordinate.
 *
 * @param ND Number of dimensions
 * @param IntT Intermediate integer type
 * @param MortonT Morton number type
 * @param coord Coordinate in intermediate integer space
 * @return Interleaved integer (Morton number)
 */
template <size_t ND, typename IntT, typename MortonT>
MortonT interleave(const IntArray<ND, IntT> &coord) {
  MortonT retVal(0);
  for (size_t i = 0; i < ND; i++) {
    retVal |= pad<ND - 1, IntT, MortonT>(coord[i]) << i;
  }
  return retVal;
}

template <size_t ND, typename IntT>
Morton96 interleave(const IntArray<3, uint32_t> &coord) {
  return Morton96(interleave<ND, IntT, uint128_t>(coord));
}
/**
 * Deinterleaves a Morton number into an integer coordinate.
 *
 * @param ND Number of dimensions
 * @param IntT Intermediate integer type
 * @param MortonT Morton number type
 * @param z Morton number
 * @return Integer coordinate
 */
template <size_t ND, typename IntT, typename MortonT>
IntArray<ND, IntT> deinterleave(const MortonT z) {
  IntArray<ND, IntT> retVal;
  for (size_t i = 0; i < ND; i++) {
    retVal[i] = (IntT)compact<ND - 1, IntT, MortonT>(z >> i);
  }
  return retVal;
}

template <size_t ND, typename IntT>
IntArray<ND, IntT> deinterleave(const Morton96& z) {
  return deinterleave<ND, IntT, uint128_t >(z.to_uint128_t());
}