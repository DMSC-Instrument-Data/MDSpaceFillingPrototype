#include <cinttypes>
#include <cstddef>

#pragma once

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
