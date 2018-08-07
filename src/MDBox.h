#include <functional>
#include <vector>

#include <omp.h>

#include "BitInterleaving.h"
#include "MDEvent.h"
#include "Types.h"

#pragma once

template <size_t ND, typename IntT, typename MortonT> class MDBox {
public:
  using Children = std::vector<MDBox<ND, IntT, MortonT>>;
  using ZCurveIterator = typename MDEvent<ND>::ZCurve::const_iterator;

private:
  using MortonTLimits = std::numeric_limits<MortonT>;

public:
  /**
   * Construct a "root" MDBox, i.e. one that makes use of the full intermediate
   * integer space.
   */
  MDBox(ZCurveIterator begin, ZCurveIterator end,
        MortonT mortonMin = MortonTLimits::min(),
        MortonT mortonMax = MortonTLimits::max())
      : m_min(mortonMin), m_max(mortonMax), m_eventBegin(begin),
        m_eventEnd(end) {}

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
   * Recursively splits this box into 2^N uniformly sized child boxes and
   * distributes its events within the child boxes.
   *
   * This assumes that the bounds of the Morton space (i.e. coordinate space
   * once converted to the intermediate integer type) are equal and the
   * dimensions are a power of 2.
   * For the way boxes are constructed and split these assumptions both hold.
   *
   * @param splitThreshold Number of events at which a box will be further split
   * @param maxDepth Maximum box tree depth (including root box)
   */
  void distributeEvents(size_t splitThreshold, size_t maxDepth) {
    /* Number of child boxes to split this box into */
    constexpr size_t numSplits = 1 << ND;

    /* Determine the "width" of this box in Morton number */
    const MortonT thisBoxWidth = m_max - m_min;

    /* Determine the "width" of the child boxes in Morton number */
    const MortonT childBoxWidth = thisBoxWidth / numSplits;

    auto eventIt = m_eventBegin;

    /* For each new child box */
    for (size_t i = 0; i < numSplits; i++) {
      /* Lower child box bound is parent box lower bound plus for each previous
       * child box; box width plus offset by one (such that lower bound of box
       * i+1 is one grater than upper bound of box i) */
      const auto boxLower = m_min + ((childBoxWidth + 1) * i);

      /* Upper child box bound is lower plus child box width */
      const auto boxUpper = boxLower + childBoxWidth;

      const auto boxEventStart = eventIt;

      /* Iterate over event list to find the first event that should not be in
       * the current child box */
      while (boxLower <= eventIt->spaceFillingCurveOrder() &&
             eventIt->spaceFillingCurveOrder() <= boxUpper &&
             eventIt != m_eventEnd) {
        /* Event was in the box, increment the event iterator */
        ++eventIt;
      }

      /* Add new child box. */
      /* As we are adding as we iterate over Morton numbers and parent events
       * child boxes are inserted in the correct sorted order. */
      m_childBoxes.emplace_back(boxEventStart, eventIt, boxLower, boxUpper);
    }

    /* Distribute events within child boxes */
    /* Check maximum tree depth has yet to be reached: first decrement the depth
     * counter (as we are checking for the children) and ensure it is greater
     * than 1 (the root box is included in the tree depth but does not take part
     * in the recursive step, therefore we stop recursion at 1). */
    if (--maxDepth > 1) {
/* See https://en.wikibooks.org/wiki/OpenMP/Tasks */
/* The parallel pragma enables execution of the following block by all worker
 * threads. */
#pragma omp parallel
/* The single nowait pragma disables execution on a single worker thread and
 * disables its barrier. */
#pragma omp single nowait
      {
        for (size_t i = 0; i < m_childBoxes.size(); i++) {
          /* Ensure there are enough events in the child to require splitting */
          if (m_childBoxes[i].eventCount() >= splitThreshold) {

/* Run each child box splitting as a separate task */
#pragma omp task
            m_childBoxes[i].distributeEvents(splitThreshold, maxDepth);
          }
        }

/* Wait for all tasks to be completed */
#pragma omp taskwait
      }
    }
  }

  MortonT min() const { return m_min; }
  MortonT max() const { return m_max; }

  Children &children() { return m_childBoxes; }
  const Children &children() const { return m_childBoxes; }

  size_t eventCount() const { return std::distance(m_eventBegin, m_eventEnd); }
  ZCurveIterator eventBegin() const { return m_eventBegin; };
  ZCurveIterator eventEnd() const { return m_eventEnd; };

  /**
   * Compares MDBox using their lower bound Morton number.
   *
   * Primarily used for sorting in Z-curve order.
   */
  bool operator<(const auto &other) const { return m_min < other.m_min; }

private:
  MortonT m_min;
  MortonT m_max;

  /**
   * Vector of child boxes.
   * Must be kept sorted by Morton number (either lower or upper will work) of
   * each box.
   */
  Children m_childBoxes;

  ZCurveIterator m_eventBegin;
  ZCurveIterator m_eventEnd;
};
