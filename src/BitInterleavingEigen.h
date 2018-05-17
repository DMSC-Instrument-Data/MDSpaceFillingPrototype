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
