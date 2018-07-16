#include <functional>
#include <vector>

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
  using LeafBoxVector =
      std::vector<std::reference_wrapper<MDBox<ND, IntT, MortonT>>>;
  using ZCurveIterator = typename MDEvent<ND>::ZCurve::const_iterator;

public:
  /**
   * Adds MD events from a Z-curve to the appropriate leaf boxes on a box
   * structure.
   *
   * @param leafBoxes Leaf boxes (must be sorted by Morton number)
   * @param curve Events on Z-curve (must be sorted by Morton number)
   */
  static void
  InsertEventsToLeafBoxes(LeafBoxVector &leafBoxes,
                          const typename MDEvent<ND>::ZCurve &curve) {
    auto boxIt = leafBoxes.begin();

    /* First box always begins at the first event (even if the first even does
     * not belong in this box) */
    boxIt->get().m_eventBegin = curve.cbegin();

    /* For each event on the Z-curve */
    for (auto eventIt = curve.cbegin(); eventIt != curve.cend(); ++eventIt) {
      /* Stop when we find the box this event fits in */
      while (!boxIt->get().contains(*eventIt)) {
        /* Terminate the previous box */
        boxIt->get().m_eventEnd = eventIt;

        /* End if we run out of boxes */
        if (++boxIt == leafBoxes.end()) {
          return;
        }

        /* Start the next box */
        boxIt->get().m_eventBegin = eventIt;
      }
    }

    /* If we run out of events then terminate the current box */
    (boxIt++)->get().m_eventEnd = curve.cend();

    /* If there are boxes remaining then mark them as empty */
    for (; boxIt != leafBoxes.end(); ++boxIt) {
      boxIt->get().m_eventBegin = curve.cend();
      boxIt->get().m_eventEnd = curve.cend();
    }
  }

public:
  /**
   * Construct a "root" MDBox, i.e. one that makes use of the full intermediate
   * integer space.
   */
  MDBox() {
    using intLimits = std::numeric_limits<IntT>;

    const auto axisMin = intLimits::min();
    const auto axisMax = intLimits::max();

    m_min = interleave<ND, IntT, MortonT>({axisMin, axisMin, axisMin, axisMin});
    m_max = interleave<ND, IntT, MortonT>({axisMax, axisMax, axisMax, axisMax});

    m_morton = calculate_centre<ND, IntT, MortonT>(m_min, m_max);
  }

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
    const MortonT eventMorton(event.spaceFillingCurveOrder());
    return eventMorton >= m_min && eventMorton < m_max;
  }

  /**
   * (Recursively) splits boxes into 2^N boxes by taking the midpoint
   * coordinate of each axis.
   *
   * @param depth Maximum depth to perform splitting to (defaults to 1 (i.e.
   * split only this box), must be greater than zero)
   */
  void split(size_t depth = 1) {
    const auto lower = deinterleave<ND, IntT, MortonT>(m_min);
    const auto upper = deinterleave<ND, IntT, MortonT>(m_max);

    const auto mid = (upper - lower) / 2;

    /* Iterate over permutations of coordinates (selected from bits of integers
     * up to 2^ND) */
    IntArray<ND, IntT> boxLower;
    IntArray<ND, IntT> boxUpper;
    for (size_t p = 0; p < std::pow(2, ND); p++) {
      /* Select coordinates based on set bits */
      for (size_t i = 0; i < ND; i++) {
        const bool select((p >> i) & 0x1);
        boxLower[i] = select ? lower[i] : mid[i];
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
   * Removes child boxes where the total event count is less than a threshold.
   *
   * @param threshold Minimum number of events in a single box
   */
  void prune(size_t threshold) {
    bool allChildBoxesBelowThreshold(true);

    /* If this box has no events then we can just remove all the child boxes and
     * exit */
    if (eventCount() == 0) {
      m_childBoxes.clear();
      return;
    }

    /* Prune children first */
    for (auto &child : m_childBoxes) {
      child.prune(threshold);

      /* Record if all child boxes have less events than the minimum threshold
       */
      if (child.eventCount() >= threshold) {
        allChildBoxesBelowThreshold = false;
      }
    }

    /* If all child boxes have less events than the minimum number of events per
     * box */
    if (allChildBoxesBelowThreshold) {
      /* "Merge" the boxes (essentially just delete the children as the start
       * and end iterators will remain unchanged in this box) */
      m_childBoxes.clear();
    }
  }

  /**
   * Gets all leaf boxes in a single vector.
   *
   * @param boxes Vector of references to child boxes
   */
  void getLeafBoxes(LeafBoxVector &boxes) {
    if (m_childBoxes.empty()) {
      boxes.push_back(*this);
    } else {
      for (auto &b : m_childBoxes) {
        b.getLeafBoxes(boxes);
      }
    }
  }

  /**
   * Updates the start and end event iterators that point to the Z-curve from
   * child boxes.
   *
   * Uses the start iterator of the child with the lowest Morton number and the
   * end iterator of the child with the highest Morton number.
   *
   * Assumes that the child box list is already sorted by Morton number.
   */
  void setIteratorsFromChildren() {
    if (m_childBoxes.size() > 0) {
      for (auto &child : m_childBoxes) {
        child.setIteratorsFromChildren();
      }

      m_eventBegin = m_childBoxes.front().m_eventBegin;
      m_eventEnd = m_childBoxes.back().m_eventEnd;
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
