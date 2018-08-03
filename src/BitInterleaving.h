#include <cinttypes>
#include <cstddef>

#include <boost/multiprecision/cpp_int.hpp>

#include "Types.h"

#pragma once

template <size_t N, typename Int> Int pad(Int x) {
  throw std::runtime_error("No pad() specialisation.");
}

template <size_t N, typename Int> Int compact(Int x) {
  throw std::runtime_error("No compact() specialisation.");
}

/**
 * Trait used to determine which pad and compact function we can use for a given
 * Morton number type (defaults to the Morton number type itself).
 */
template <typename T> struct morton_to_pad_compact_int { typedef T type; };

template <> struct morton_to_pad_compact_int<uint32_t> {
  typedef uint64_t type;
};

/**
 * Pad an integer with 1 padding bit between integer bits.
 * Maximum input width is 32 bit.
 */
template <> uint64_t pad<1, uint64_t>(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 8) & 0xff00ff;
  x = (x | x << 4) & 0xf0f0f0f;
  x = (x | x << 2) & 0x33333333;
  x = (x | x << 1) & 0x55555555;
  return x;
}

/**
 * Compacts (removes padding) an integer with 1 padding bit between integer
 * bits.
 * Maximum output bit width is 32 bit.
 */
template <> uint64_t compact<1, uint64_t>(uint64_t x) {
  x &= 0x55555555;
  x = (x | x >> 1) & 0x33333333;
  x = (x | x >> 2) & 0xf0f0f0f;
  x = (x | x >> 4) & 0xff00ff;
  x = (x | x >> 8) & 0xffff;
  return x;
}

/**
 * Pad an integer with 2 padding bits between integer bits.
 * Maximum input width is 16 bit.
 */
template <> uint64_t pad<2, uint64_t>(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 16) & 0xff0000ff;
  x = (x | x << 8) & 0xf00f00f00f;
  x = (x | x << 4) & 0xc30c30c30c3;
  x = (x | x << 2) & 0x249249249249;
  return x;
}

/**
 * Compacts (removes padding) an integer with 3 padding bit between integer
 * bits.
 * Maximum output bit width is 16 bit.
 */
template <> uint64_t compact<2, uint64_t>(uint64_t x) {
  x &= 0x249249249249;
  x = (x | x >> 2) & 0xc30c30c30c3;
  x = (x | x >> 4) & 0xf00f00f00f;
  x = (x | x >> 8) & 0xff0000ff;
  x = (x | x >> 16) & 0xffff;
  return x;
}

/**
 * Pad an integer with 3 padding bits between integer bits.
 * Maximum input width is 16 bit.
 */
template <> uint64_t pad<3, uint64_t>(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 32) & 0xf800000007ff;
  x = (x | x << 16) & 0xf80007c0003f;
  x = (x | x << 8) & 0xc0380700c03807;
  x = (x | x << 4) & 0x843084308430843;
  x = (x | x << 2) & 0x909090909090909;
  x = (x | x << 1) & 0x1111111111111111;
  return x;
}

/**
 * Compacts (removes padding) an integer with 3 padding bit between integer
 * bits.
 * Maximum output bit width is 16 bit.
 */
template <> uint64_t compact<3, uint64_t>(uint64_t x) {
  x &= 0x1111111111111111;
  x = (x | x >> 1) & 0x909090909090909;
  x = (x | x >> 2) & 0x843084308430843;
  x = (x | x >> 4) & 0xc0380700c03807;
  x = (x | x >> 8) & 0xf80007c0003f;
  x = (x | x >> 16) & 0xf800000007ff;
  x = (x | x >> 32) & 0xffff;
  return x;
}

using uint128_t = boost::multiprecision::uint128_t;

/**
 * Pad an integer with 3 padding bits between integer bits.
 * Maximum input width is 32 bit.
 */
template <> uint128_t pad<3, uint128_t>(uint128_t x) {
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
template <> uint128_t compact<3, uint128_t>(uint128_t x) {
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

template <size_t ND, typename IntT, typename MortonT>
MortonT interleave(const IntArray<ND, IntT> &coord) {
  MortonT retVal(0);
  for (size_t i = 0; i < ND; i++) {
    retVal |=
        pad<ND - 1, typename morton_to_pad_compact_int<MortonT>::type>(coord[i])
        << i;
  }
  return retVal;
}

template <size_t ND, typename IntT, typename MortonT>
IntArray<ND, IntT> deinterleave(const MortonT z) {
  IntArray<ND, IntT> retVal;
  for (size_t i = 0; i < ND; i++) {
    retVal[i] = (IntT)
        compact<ND - 1, typename morton_to_pad_compact_int<MortonT>::type>(z >>
                                                                           i);
  }
  return retVal;
}
