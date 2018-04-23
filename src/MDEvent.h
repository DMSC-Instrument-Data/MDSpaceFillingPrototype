#include <cstring>

#pragma once

template <size_t ND, typename OrderT> class MDEvent {
public:
  MDEvent(float *coords, float signal = 1.0f) : m_signal(signal) {
    std::memcpy(m_coords, coords, 4 * sizeof(float));
  }

  float coord(size_t i) const { return m_coords[i]; }
  OrderT spaceFillingCurveOrder() const { return m_spaceFillingCurveOrder; }

protected:
  float m_signal;
  float m_coords[ND];
  OrderT m_spaceFillingCurveOrder;
};
