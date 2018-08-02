#include <cinttypes>
#include <climits>
#include <cstddef>

#include "Types.h"

#pragma once

template <typename InterleavedT, typename IntT, size_t N>
InterleavedT Interleave(const IntArray<N, IntT> &data) {
  InterleavedT out(0);
  /* For each bit in the input integer width */
  for (int i = 0; i < sizeof(IntT) * CHAR_BIT; i++) {
    /* For each input integer */
    for (int b = 0; b < N; b++) {
      out |=
          /* Select the bit in the input integer we are interested in, the
           * result of this is either an integer with bit b being set or 0 */
          (data[b] & ((IntT)1 << i))
          /* Offset for the correct amount of padding, minus 1 input integer
           * width (as the bit will already be in shifted for one integer width)
           */
          << ((i * (N - 1))
              /* Offset b bits (for the position of the integer in the
               * coordinate
               * point) */
              + b);
    }
  }
  return out;
}

template <typename InterleavedT, typename IntT, size_t N>
IntArray<N, IntT> Deinterleave(const InterleavedT in) {
  IntArray<N, IntT> res;
  res.setZero();
  /* For each bit in the input integer width */
  for (int i = 0; i < sizeof(IntT) * CHAR_BIT; i++) {
    /* For each input integer */
    for (int b = 0; b < N; b++) {
      /* Select the bit in the interleaved number, shift it back to the
       * appropriate position in the result coordinate integer (this is
       * essentially the opposite of what happens in Interleave()) */
      res[b] |= (IntT)((in & (InterleavedT)1 << ((i * N) + b)) >>
                       ((i * (N - 1)) + b));
    }
  }
  return res;
}
