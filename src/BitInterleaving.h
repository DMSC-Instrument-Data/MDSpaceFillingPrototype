#include <cinttypes>
#include <cstddef>

#pragma once

uint64_t Pad1(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 8) & 0xff00ff;
  x = (x | x << 4) & 0xf0f0f0f;
  x = (x | x << 2) & 0x33333333;
  x = (x | x << 1) & 0x55555555;
  return x;
}

uint64_t Pad2(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 16) & 0xff0000ff;
  x = (x | x << 8) & 0xf00f00f00f;
  x = (x | x << 4) & 0xc30c30c30c3;
  x = (x | x << 2) & 0x249249249249;
  return x;
}

uint64_t Pad3(uint64_t x) {
  x &= 0xffff;
  x = (x | x << 32) & 0xf800000007ff;
  x = (x | x << 16) & 0xf80007c0003f;
  x = (x | x << 8) & 0xc0380700c03807;
  x = (x | x << 4) & 0x843084308430843;
  x = (x | x << 2) & 0x909090909090909;
  x = (x | x << 1) & 0x1111111111111111;
  return x;
}

uint64_t Compact3(uint64_t x) {
  x &= 0x1111111111111111;
  x = (x | x >> 1) & 0x909090909090909;
  x = (x | x >> 2) & 0x843084308430843;
  x = (x | x >> 4) & 0xc0380700c03807;
  x = (x | x >> 8) & 0xf80007c0003f;
  x = (x | x >> 16) & 0xf800000007ff;
  x = (x | x >> 32) & 0xffff;
  return x;
}

uint64_t Interleave_4_16_64(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
  return Pad3(a) | (Pad3(b) << 1) | (Pad3(c) << 2) | (Pad3(d) << 3);
}

void Deinterleave_4_16_64(uint64_t z, uint16_t &a, uint16_t &b, uint16_t &c,
                          uint16_t &d) {
  a = Compact3(z);
  b = Compact3(z >> 1);
  c = Compact3(z >> 2);
  d = Compact3(z >> 3);
}
