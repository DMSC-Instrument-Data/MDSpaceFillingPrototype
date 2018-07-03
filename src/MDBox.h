#include <functional>
#include <vector>

#include <omp.h>

#include "BitInterleaving.h"
#include "MDEvent.h"
#include "Types.h"

#pragma once

/**
 * Checks if a point defined by a Morton number is within the box bounds (as
 * defined by an upper and lower Morton number).
 *
 * A box contains a Morton number if it is within the upper and lower bounds,
 * inclusive of the box bounds themselves.
 *
 * @param lower Lower bound
 * @param upper Upper bound
 * @param value Morton value to test
 */
template <typename MortonT>
bool morton_contains(const MortonT lower, const MortonT upper,
                     const MortonT value) {
  return lower <= value && value <= upper;
}

/**
 * @class MDBox
 *
 * A single box in an MD structure.
 *
 * Boxes are defined by an upper and lower bound given as Morton numbers, Morton
 * numbers of events that fall within the box fall within these bounds.
 *
 * Bounds of adjacent boxes i and i+1 are such that the lower bound of i+1 is
 * one less than the upper bound of i. As such containment checking is performed
 * by an inclusive range check of the box's Morton number bounds.
 *
 * MDBox is templated as follows:
 *  - ND: number of dimensions in the MD space
 *  - IntT: intermediate integer type
 *  - MortonT: Morton number type
 *
 * The bit width of MortonT should be at least ND times greater than IntT.
 */
template <size_t ND, typename IntT, typename MortonT> class MDBox {
public:
  /**
   * The number of child boxes a box can have.
   *
   * This is fixed and determined by the dimensionality of the box (2^N).
   * A box may have either zero or ChildBoxCount child boxes.
   */
  static constexpr size_t ChildBoxCount = 1 << ND;

public:
  using Children = std::vector<MDBox<ND, IntT, MortonT>>;
  using ZCurveIterator =
      typename MDEvent<ND, IntT, MortonT>::ZCurve::const_iterator;

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
      : m_lowerBound(mortonMin), m_upperBound(mortonMax), m_eventBegin(begin),
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
  bool contains(const MDEvent<ND, IntT, MortonT> &event) const {
    return contains(event.mortonNumber());
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
    return morton_contains(m_lowerBound, m_upperBound, morton);
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
  void distributeEvents(const size_t splitThreshold, size_t maxDepth) {
    /* Stop iteration if we reach the maximum tree depth or have too few events
     * in the box to split again. */
    /* We check for maxDepth == 1 as maximum depth includes the root node, which
     * did not decrement the max depth counter. */
    if (maxDepth-- == 1 || eventCount() < splitThreshold) {
      return;
    }

    /* Reserve storage for child boxes */
    m_childBoxes.reserve(ChildBoxCount);

    /* Determine the "width" of this box in Morton number */
    const MortonT thisBoxWidth = m_upperBound - m_lowerBound;

    /* Determine the "width" of the child boxes in Morton number */
    const MortonT childBoxWidth = thisBoxWidth / ChildBoxCount;

    auto eventIt = m_eventBegin;

    /* For each new child box */
    for (size_t i = 0; i < ChildBoxCount; i++) {
      /* Lower child box bound is parent box lower bound plus for each previous
       * child box; box width plus offset by one (such that lower bound of box
       * i+1 is one grater than upper bound of box i) */
      const auto boxLower = m_lowerBound + ((childBoxWidth + 1) * i);

      /* Upper child box bound is lower plus child box width */
      const auto boxUpper = boxLower + childBoxWidth;

      const auto boxEventStart = eventIt;

      /* Iterate over event list to find the first event that should not be in
       * the current child box */
      while (morton_contains<MortonT>(boxLower, boxUpper,
                                      eventIt->mortonNumber()) &&
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
/* See https://en.wikibooks.org/wiki/OpenMP/Tasks */
/* The parallel pragma enables execution of the following block by all worker
 * threads. */
#pragma omp parallel
/* The single nowait pragma disables execution on a single worker thread and
 * disables its barrier. */
#pragma omp single nowait
    {
      for (size_t i = 0; i < m_childBoxes.size(); i++) {

/* Run each child box splitting as a separate task */
#pragma omp task
        m_childBoxes[i].distributeEvents(splitThreshold, maxDepth);
      }

/* Wait for all tasks to be completed */
#pragma omp taskwait
    }
  }

  MortonT min() const { return m_lowerBound; }
  MortonT max() const { return m_upperBound; }

  size_t eventCount() const { return std::distance(m_eventBegin, m_eventEnd); }
  ZCurveIterator eventBegin() const { return m_eventBegin; };
  ZCurveIterator eventEnd() const { return m_eventEnd; };

  Children &children() { return m_childBoxes; }
  const Children &children() const { return m_childBoxes; }

  /**
   * Compares MDBox using their lower bound Morton number.
   *
   * Primarily used for sorting in Z-curve order.
   */
  bool operator<(const auto &other) const {
    return m_lowerBound < other.m_lowerBound;
  }

private:
  /* Lower box bound, the smallest Morton number an event can have an be
   * contained in this box. */
  const MortonT m_lowerBound;

  /* Upper box bound, the greatest Morton number an event can have and be
   * contained in this box. */
  const MortonT m_upperBound;

  const ZCurveIterator m_eventBegin;
  const ZCurveIterator m_eventEnd;

  /**
   * Vector of child boxes.
   * Must be kept sorted by Morton number (either lower or upper will work) of
   * each box.
   */
  Children m_childBoxes;
};
