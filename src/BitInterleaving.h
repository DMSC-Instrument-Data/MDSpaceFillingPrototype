#include <cinttypes>
#include <cstddef>

#pragma once

/**
 * Pad an integer with 1 padding bit between integer bits.
 * Maximum input width is 32 bit.
 */
uint64_t Pad1_64(uint64_t x) {
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
uint64_t Compact1_64(uint64_t x) {
  x &= 0x55555555;
  x = (x | x >> 1) & 0x33333333;
  x = (x | x >> 2) & 0xf0f0f0f;
  x = (x | x >> 4) & 0xff00ff;
  x = (x | x >> 8) & 0xffff;
  return x;
}

/**
 * Pad an integer with 1 padding bit between integer bits.
 * Maximum input width is 16 bit.
 */
uint64_t Pad2_64(uint64_t x) {
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
uint64_t Compact2_64(uint64_t x) {
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
uint64_t Pad3_64(uint64_t x) {
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
uint64_t Compact3_64(uint64_t x) {
  x &= 0x1111111111111111;
  x = (x | x >> 1) & 0x909090909090909;
  x = (x | x >> 2) & 0x843084308430843;
  x = (x | x >> 4) & 0xc0380700c03807;
  x = (x | x >> 8) & 0xf80007c0003f;
  x = (x | x >> 16) & 0xf800000007ff;
  x = (x | x >> 32) & 0xffff;
  return x;
}

/**
 * Interleaves two 16 bit integers into a single 32 bit integer.
 */
uint32_t Interleave_2_16_32(uint32_t a, uint32_t b) {
  return Pad1_64(a) | (Pad1_64(b) << 1);
}

/**
 * Deinterleaves a 32 bit integer into two 16 bit integers.
 */
void Deinterleave_2_16_32(uint32_t z, uint16_t &a, uint16_t &b) {
  a = Compact1_64(z);
  b = Compact1_64(z >> 1);
}

/**
 * Interleaves three 16 bit integers into a single 64 bit integer.
 */
uint64_t Interleave_3_16_64(uint64_t a, uint64_t b, uint64_t c) {
  return Pad2_64(a) | (Pad2_64(b) << 1) | (Pad2_64(c) << 2);
}

/**
 * Deinterleaves a 64 bit integer into three 16 bit integers.
 */
void Deinterleave_3_16_64(uint64_t z, uint16_t &a, uint16_t &b, uint16_t &c) {
  a = Compact2_64(z);
  b = Compact2_64(z >> 1);
  c = Compact2_64(z >> 2);
}

/**
 * Interleaves four 16 bit integers into a single 64 bit integer.
 */
uint64_t Interleave_4_16_64(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
  return Pad3_64(a) | (Pad3_64(b) << 1) | (Pad3_64(c) << 2) | (Pad3_64(d) << 3);
}

/**
 * Deinterleaves a 64 bit integer into four 16 bit integers.
 */
void Deinterleave_4_16_64(uint64_t z, uint16_t &a, uint16_t &b, uint16_t &c,
                          uint16_t &d) {
  a = Compact3_64(z);
  b = Compact3_64(z >> 1);
  c = Compact3_64(z >> 2);
  d = Compact3_64(z >> 3);
}
