#include <cinttypes>
#include <cstddef>

#include <Eigen/Dense>

#pragma once

template <typename T, size_t N> using Array = Eigen::Array<T, N, 1>;

template <typename InterleavedT, typename IntegerT, size_t N>
InterleavedT Interleave(const Array<IntegerT, N> &data) {
  InterleavedT out(0);
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    for (int j = 0; j < N; j++) {
      out |= (data[j] & (IntegerT)1 << i) << (i * (N - 1)) + j;
    }
  }
  return out;
}

template <typename InterleavedT, typename IntegerT, size_t N>
Array<IntegerT, N> Deinterleave(const InterleavedT in) {
  Array<IntegerT, N> res;
  res.setZero();
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    for (int j = 0; j < N; j++) {
      res[j] |= (IntegerT)((in & (InterleavedT)1 << ((i * N) + j)) >>
                           ((i * (N - 1)) + j));
    }
  }
  return res;
}

template <typename InterleavedT, typename IntegerT>
InterleavedT Interleave2(IntegerT a, IntegerT b) {
  InterleavedT out(0);
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    out |= (a & (IntegerT)1 << i) << i;
    out |= (b & (IntegerT)1 << i) << (i + 1);
  }
  return out;
}

template <typename InterleavedT, typename IntegerT>
void Deinterleave2(InterleavedT in, IntegerT &a, IntegerT &b) {
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    a |= (IntegerT)((in & (InterleavedT)1 << (2 * i)) >> i);
    b |= (IntegerT)((in & (InterleavedT)1 << ((2 * i) + 1)) >> (i + 1));
  }
}

template <typename InterleavedT, typename IntegerT>
InterleavedT Interleave3(IntegerT a, IntegerT b, IntegerT c) {
  InterleavedT out(0);
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    out |= (a & (IntegerT)1 << i) << (i * 2);
    out |= (b & (IntegerT)1 << i) << ((i * 2) + 1);
    out |= (c & (IntegerT)1 << i) << ((i * 2) + 2);
  }
  return out;
}

template <typename InterleavedT, typename IntegerT>
void Deinterleave3(InterleavedT in, IntegerT &a, IntegerT &b, IntegerT &c) {
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    a |= (IntegerT)((in & (InterleavedT)1 << i * 3) >> (i * 2));
    b |= (IntegerT)((in & (InterleavedT)1 << (i * 3) + 1) >> ((i * 2) + 1));
    c |= (IntegerT)((in & (InterleavedT)1 << (i * 3) + 2) >> ((i * 2) + 2));
  }
}

template <typename InterleavedT, typename IntegerT>
InterleavedT Interleave4(IntegerT a, IntegerT b, IntegerT c, IntegerT d) {
  InterleavedT out(0);
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    out |= ((InterleavedT)(a & (IntegerT)1 << i)) << (i * 3);
    out |= ((InterleavedT)(b & (IntegerT)1 << i)) << ((i * 3) + 1);
    out |= ((InterleavedT)(c & (IntegerT)1 << i)) << ((i * 3) + 2);
    out |= ((InterleavedT)(d & (IntegerT)1 << i)) << ((i * 3) + 3);
  }
  return out;
}

template <typename InterleavedT, typename IntegerT>
void Deinterleave4(InterleavedT in, IntegerT &a, IntegerT &b, IntegerT &c,
                   IntegerT &d) {
  for (int i = 0; i < sizeof(IntegerT) * 8; i++) {
    a |= (IntegerT)((in & (InterleavedT)1 << (i * 4)) >> (i * 3));
    b |= (IntegerT)((in & (InterleavedT)1 << ((i * 4) + 1)) >> ((i * 3) + 1));
    c |= (IntegerT)((in & (InterleavedT)1 << ((i * 4) + 2)) >> ((i * 3) + 2));
    d |= (IntegerT)((in & (InterleavedT)1 << ((i * 4) + 3)) >> ((i * 3) + 3));
  }
}

template <typename IntegerT>
void Interleave4_128Bit(uint64_t &msb, uint64_t &lsb, const IntegerT a,
                        const IntegerT b, const IntegerT c, const IntegerT d) {
  const size_t halfInputBitLength(sizeof(IntegerT) * 4);

  for (int i = 0; i < halfInputBitLength; i++) {
    /* LSB is identical to Interleave4 */
    lsb |= (a & (uint64_t)1 << i) << (i * 3);
    lsb |= (b & (uint64_t)1 << i) << ((i * 3) + 1);
    lsb |= (c & (uint64_t)1 << i) << ((i * 3) + 2);
    lsb |= (d & (uint64_t)1 << i) << ((i * 3) + 3);

    /* MSB is as Interleave4 but operating on shifted integers */
    msb |= ((a >> halfInputBitLength) & (uint64_t)1 << i) << (i * 3);
    msb |= ((b >> halfInputBitLength) & (uint64_t)1 << i) << ((i * 3) + 1);
    msb |= ((c >> halfInputBitLength) & (uint64_t)1 << i) << ((i * 3) + 2);
    msb |= ((d >> halfInputBitLength) & (uint64_t)1 << i) << ((i * 3) + 3);
  }
}

template <typename IntegerT>
void Deinterleave4_128Bit(const uint64_t msb, const uint64_t lsb, IntegerT &a,
                          IntegerT &b, IntegerT &c, IntegerT &d) {
  const size_t halfInputBitLength(sizeof(IntegerT) * 4);

  for (int i = 0; i < halfInputBitLength; i++) {
    /* LSB is identical to Interleave4 */
    a |= ((lsb & (uint64_t)1 << (i * 4)) >> (i * 3));
    b |= ((lsb & (uint64_t)1 << ((i * 4) + 1)) >> ((i * 3) + 1));
    c |= ((lsb & (uint64_t)1 << ((i * 4) + 2)) >> ((i * 3) + 2));
    d |= ((lsb & (uint64_t)1 << ((i * 4) + 3)) >> ((i * 3) + 3));

    /* MSB is as Interleave4 but shifting by half input integer width */
    a |= ((msb & (uint64_t)1 << (i * 4)) >> ((i * 3))) << halfInputBitLength;
    b |= ((msb & (uint64_t)1 << ((i * 4) + 1)) >> ((i * 3) + 1))
         << halfInputBitLength;
    c |= ((msb & (uint64_t)1 << ((i * 4) + 2)) >> ((i * 3) + 2))
         << halfInputBitLength;
    d |= ((msb & (uint64_t)1 << ((i * 4) + 3)) >> ((i * 3) + 3))
         << halfInputBitLength;
  }
}

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
