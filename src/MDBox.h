#include <functional>
#include <vector>

#include <omp.h>

#include "BitInterleaving.h"
#include "MDEvent.h"
#include "Types.h"

#pragma once

template <size_t ND, typename IntT, typename MortonT>
MortonT calculate_centre(const MortonT min, const MortonT max) {
  const auto lower = deinterleave<ND, IntT, MortonT>(min);
  const auto upper = deinterleave<ND, IntT, MortonT>(max);

  const auto mid = (upper - lower) / 2;

  return interleave<ND, IntT, MortonT>(mid);
}

template <size_t ND, typename IntT, typename MortonT> class MDBox {
public:
  using Children = std::vector<MDBox<ND, IntT, MortonT>>;
  using ZCurveIterator = typename MDEvent<ND>::ZCurve::const_iterator;

public:
  /**
   * Construct a "root" MDBox, i.e. one that makes use of the full intermediate
   * integer space.
   */
  MDBox(ZCurveIterator begin, ZCurveIterator end)
      : m_min(std::numeric_limits<MortonT>::min()),
        m_max(std::numeric_limits<MortonT>::max()),
        m_morton(calculate_centre<ND, IntT, MortonT>(m_min, m_max)),
        m_eventBegin(begin), m_eventEnd(end) {}

  /**
   * Construct a "root" MDBox, i.e. one that makes use of the full intermediate
   * integer space.
   */
  MDBox()
      : m_min(std::numeric_limits<MortonT>::min()),
        m_max(std::numeric_limits<MortonT>::max()),
        m_morton(calculate_centre<ND, IntT, MortonT>(m_min, m_max)) {}

  /**
   * Construct an MDBox with given integer bounds.
   *
   * @param min Lower bounds
   * @param max Upper bounds
   */
  MDBox(IntArray<ND, IntT> min, IntArray<ND, IntT> max)
      : m_min(interleave<ND, IntT, MortonT>(min)),
        m_max(interleave<ND, IntT, MortonT>(max)),
        m_morton(calculate_centre<ND, IntT, MortonT>(m_min, m_max)) {}

  /**
   * Performs a check if an MDEvent falls within the bounds of this box.
   *
   * Tests based on event Morton number and box bounds, does NOT check if the
   * event is within the iterator range for stored Z-curve.
   *
   * This assumes that the root MDBox was created with the same MDSpaceBounds as
   * the MDEvent being tested (as only the integer coordinates are used for the
   * comparison).
   *
   * @param event MDEvent to test
   * @return True if event falls within bounds of this box
   */
  bool contains(const MDEvent<ND> &event) const {
    return contains(event.spaceFillingCurveOrder());
  }

  /**
   * Tests if a given Morton number is inside this box.
   *
   * @see MDBox::contains(MDEvent)
   *
   * @param morton Morton number to test
   * @return True if Morton number falls within bounds of this box
   */
  bool contains(const MortonT morton) const {
    return m_min <= morton && morton <= m_max;
  }

  /**
   * Recursively splits boxes into 2^N boxes by taking the midpoint
   * coordinate of each axis.
   *
   * @param depth Maximum depth to perform splitting to (defaults to 1 (i.e.
   * split only this box), must be greater than zero)
   */
  void split(size_t depth = 1) {
    const auto lower = deinterleave<ND, IntT, MortonT>(m_min);
    const auto upper = deinterleave<ND, IntT, MortonT>(m_max);

    const auto mid = lower + ((upper - lower) / 2);

    /* Iterate over permutations of coordinates (selected from bits of integers
     * up to 2^ND) */
    IntArray<ND, IntT> boxLower;
    IntArray<ND, IntT> boxUpper;
    for (size_t p = 0; p < std::pow(2, ND); p++) {
      /* Select coordinates based on set bits */
      for (size_t i = 0; i < ND; i++) {
        const bool select((p >> i) & 0x1);
        boxLower[i] = select ? lower[i] : mid[i] + 1;
        boxUpper[i] = select ? mid[i] : upper[i];
      }

      m_childBoxes.emplace_back(boxLower, boxUpper);
    }

    /* If we are yet to reach the required depth */
    if (--depth > 0) {
      /* Continue to split child boxes */
      for (auto &box : m_childBoxes) {
        box.split(depth);
      }
    }

    /* Sort child boxes by Morton number */
    std::sort(m_childBoxes.begin(), m_childBoxes.end());
  }

  /**
   * Recursively splits this box into 2^N uniformly sized child boxes and
   * distributes its events within the child boxes.
   *
   * @param splitThreshold Number of events at which a box will be further split
   * @param maxDepth Maximum box tree depth (including root box)
   */
  void distributeEvents(size_t splitThreshold, size_t maxDepth) {
    /* If this box has less events than the splitting threshold or we have
     * reached the maximum tree depth then exit */
    if (eventCount() < splitThreshold || maxDepth-- == 0) {
      return;
    }

    /* Split this box */
    split();

    auto childIt = m_childBoxes.begin();
    auto eventIt = m_eventBegin;

    /* First child always has same event begin iterator as this box */
    childIt->m_eventBegin = m_eventBegin;

    /* Iterate over all child boxes except the last */
    for (; childIt != m_childBoxes.end() - 1;) {
      /* Iterate over event list to find the first event that should not be in
       * the current child box */
      while (childIt->contains(*eventIt) && eventIt != m_eventEnd) {
        ++eventIt;
      }

      /* Set the end event iterator of the current child box */
      childIt->m_eventEnd = eventIt;

      /* Set the start event iterator of the next child box */
      (++childIt)->m_eventBegin = eventIt;
    }

    /* Last child always has same event end iterator as this box */
    childIt->m_eventEnd = m_eventEnd;

/* Distribute events within child boxes */
#pragma omp parallel
#pragma omp single nowait
    {
      for (size_t i = 0; i < m_childBoxes.size(); i++) {
#pragma omp task
        m_childBoxes[i].distributeEvents(splitThreshold, maxDepth);
      }

#pragma omp taskwait
    }
  }

  MortonT min() const { return m_min; }
  MortonT max() const { return m_max; }
  MortonT morton() const { return m_morton; }

  Children &children() { return m_childBoxes; }
  const Children &children() const { return m_childBoxes; }

  size_t eventCount() const { return std::distance(m_eventBegin, m_eventEnd); }
  ZCurveIterator eventBegin() const { return m_eventBegin; };
  ZCurveIterator eventEnd() const { return m_eventEnd; };

  /**
   * Compares MDBox using their central Morton number.
   *
   * Primarily used for sorting in Z-curve order.
   */
  bool operator<(const auto &other) const { return m_morton < other.m_morton; }

private:
  MortonT m_min;
  MortonT m_max;

  MortonT m_morton;

  /**
   * Vector of child boxes.
   * Must be kept sorted by the central Morton number of each box.
   */
  Children m_childBoxes;

  ZCurveIterator m_eventBegin;
  ZCurveIterator m_eventEnd;
};
