/*
 * Space filling curve prototype for MD event data structure
 * Copyright (C) 2018 European Spallation Source
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <functional>
#include <vector>
#include <iterator>
#include <iostream>
#include <atomic>

#include <omp.h>

#include "BitInterleaving.h"
#include "Constants.h"
#include "MDEvent.h"
#include "Types.h"

#pragma once

//store events on tree leafs
//#define STORING_EVENTS

/**
 * Performs a dimension-wise comparison on two Morton numbers by masking the
 * bits of each interleaved integer.
 *
 * @param a Left hand operand
 * @param b Right hand operand
 * @return True if comparison passes for all dimensions
 */
template <size_t ND, typename IntT, typename MortonT>
bool masked_morton_lte(const MortonT a, const MortonT b) {
  using MMask = MortonMask<ND, IntT, MortonT>;

  /* For each dimension */
  for (size_t i = 0; i < ND; i++) {
    /* Perform comparison on masked morton numbers */
    if ((a | MMask::mask[i]) > (b | MMask::mask[i])) {
      /* Return on the first failed comparison */
      return false;
    }
  }

  /* All comparisons passed */
  return true;
}

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
  using EventRange = std::pair<ZCurveIterator, ZCurveIterator>;

private:
  MortonT CalculateDefaultBound(IntT intBound) {
    IntArray<ND, IntT> minCoord;
    minCoord.fill(intBound);
    return interleave<ND, IntT, MortonT>(minCoord);
  }

public:
  MDBox(ZCurveIterator begin, ZCurveIterator end, MortonT mortonMin,
        MortonT mortonMax)
      : m_lowerBound(mortonMin), m_upperBound(mortonMax), m_eventBegin(begin),
        m_eventEnd(end) {}

  /**
   * Construct a "root" MDBox, i.e. one that makes use of the full intermediate
   * integer space.
   */
  MDBox(ZCurveIterator begin, ZCurveIterator end)
      : m_lowerBound(CalculateDefaultBound(std::numeric_limits<IntT>::min())),
        m_upperBound(CalculateDefaultBound(std::numeric_limits<IntT>::max())),
        m_eventBegin(begin), m_eventEnd(end) {}

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
    /* For some reason using ChildBoxCount directly caused a build failure in
     * debug mode with GCC 5.4. Oddly only on the line that assigns
     * childBoxWidth. Assigning it to a local variable works. */
    const auto childBoxCount(ChildBoxCount);

    /* Stop iteration if we reach the maximum tree depth or have too few events
     * in the box to split again. */
    /* We check for maxDepth == 1 as maximum depth includes the root node, which
     * did not decrement the max depth counter. */
    if (maxDepth-- == 1 || eventCount() < splitThreshold) {
#ifdef STORING_EVENTS
      if(m_events.begin() != m_eventBegin) {
        m_events.insert(m_events.end(), m_eventBegin, m_eventEnd);
      }
      m_eventBegin = m_events.begin();
      m_eventEnd = m_events.end();
#endif // STORING_EVENTS
      return;
    }

    /* Reserve storage for child boxes */
    m_childBoxes.reserve(childBoxCount);

    /* Determine the "width" of this box in Morton number */
    const MortonT thisBoxWidth = m_upperBound - m_lowerBound;

    /* Determine the "width" of the child boxes in Morton number */
    const MortonT childBoxWidth = thisBoxWidth / childBoxCount;

    auto eventIt = m_eventBegin;

    /* For each new child box */
    for (size_t i = 0; i < childBoxCount; i++) {
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

  /**
   * Gets events that are within a defined bounding box.
   *
   * @param eventRanges Ranges of events that are withing the bounding box
   * @param lower Lower bound in Morton number
   * @param upper Upper bound in Morton number
   */
  void getEventsInBoundingBox(std::vector<EventRange> &eventRanges,
                              const MortonT lower, const MortonT upper) const {
    const auto less_than_equal = masked_morton_lte<ND, IntT, MortonT>;

    /* Test for full intersection. */
    if (less_than_equal(lower, m_lowerBound) &&
        less_than_equal(m_upperBound, upper)) {
      /* All events in this box are relevant. */
      eventRanges.emplace_back(m_eventBegin, m_eventEnd);
    }
    /* Test for partial intersection */
    else if (less_than_equal(m_lowerBound, upper) &&
             less_than_equal(lower, m_upperBound)) {
      /* There is partial intersection with this box. */
      if (m_childBoxes.empty()) {
        /* This MDBox has no children. Need to resolve based on Morton number of
         * individual events. */
        ZCurveIterator startOfCurrentRange(m_eventBegin);
        bool currentlyInARange(false);
        for (auto eventIt = m_eventBegin; eventIt != m_eventEnd; ++eventIt) {
          /* Check if the event is within the bounding box */
          bool eventInBoundingBox(
              less_than_equal(lower, eventIt->mortonNumber()) &&
              less_than_equal(eventIt->mortonNumber(), upper));

          if (eventInBoundingBox && !currentlyInARange) {
            /* Record the start event iterator for this range of events that
             * fall within the bounding box */
            startOfCurrentRange = eventIt;
            currentlyInARange = true;
          } else if (!eventInBoundingBox && currentlyInARange) {
            /* Record the end of the current range of events that fall within a
             * bounding box */
            eventRanges.emplace_back(startOfCurrentRange, eventIt);
            currentlyInARange = false;
          }
        }

        /* Handle an unfinished event range if we run out of events in the box
         */
        if (currentlyInARange) {
          eventRanges.emplace_back(startOfCurrentRange, m_eventEnd);
        }
      } else {
        /* This MDBox has children. They can resolve the intersection. */
        for (auto &child : m_childBoxes) {
          child.getEventsInBoundingBox(eventRanges, lower, upper);
        }
      }
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

  struct Leaf {
    unsigned level;
    MDBox<ND, IntT, MortonT >& box;
  };

  void leafs(std::vector<Leaf>& lf, unsigned& level) {
    if(m_childBoxes.empty())
      lf.emplace_back(Leaf{level, *this});
    else {
      ++level;
      for(auto& child: m_childBoxes)
        child.leafs(lf, level);
      --level;
    }
  }

  std::vector<Leaf> leafs() {
    std::vector<Leaf> leafBoxes;
    unsigned level = 0;
    leafs(leafBoxes, level);
    return leafBoxes;
  }

#ifdef STORING_EVENTS
  void distributeEventsSingleThread(const size_t splitThreshold, size_t maxDepth) {
    const auto childBoxCount(ChildBoxCount);

    if (maxDepth-- == 1 || eventCount() < splitThreshold) {
      if(m_events.begin() != m_eventBegin) {
        m_events.insert(m_events.end(), m_eventBegin, m_eventEnd);
      }
      m_eventBegin = m_events.begin();
      m_eventEnd = m_events.end();
      return;
    }

    m_childBoxes.reserve(childBoxCount);
    const MortonT thisBoxWidth = m_upperBound - m_lowerBound;
    const MortonT childBoxWidth = thisBoxWidth / childBoxCount;
    auto eventIt = m_eventBegin;

    for (size_t i = 0; i < childBoxCount; i++) {
      const auto boxLower = m_lowerBound + ((childBoxWidth + 1) * i);

      const auto boxUpper = boxLower + childBoxWidth;

      const auto boxEventStart = eventIt;

      while (morton_contains<MortonT>(boxLower, boxUpper,
                                      eventIt->mortonNumber()) &&
          eventIt != m_eventEnd) {
        ++eventIt;
      }

      m_childBoxes.emplace_back(boxEventStart, eventIt, boxLower, boxUpper);
    }

    for (size_t i = 0; i < m_childBoxes.size(); i++) {
      m_childBoxes[i].distributeEventsSingleThread(splitThreshold, maxDepth);
    }
  }

  size_t totalEvents() {
    std::size_t count = 0;
    for(auto& leaf: leafs())
      count += leaf.box.m_events.size();
    return count;
  }

  void appendEvents(ZCurveIterator from, ZCurveIterator to, const size_t splitThreshold, size_t maxDepth) {

    auto oldSz = m_events.size();
    m_events.insert(m_events.cend(), from, to);
    auto middle = m_events.begin() + oldSz;

    std::inplace_merge(m_events.begin(), middle, m_events.end());

    m_eventBegin = m_events.begin();
    m_eventEnd = m_events.end();
    distributeEventsSingleThread(splitThreshold, maxDepth);
    if(!m_childBoxes.empty()) {
      m_events.resize(0);
      m_events.shrink_to_fit();
    }
  }

  void appendEvents(typename MDEvent<ND, IntT, MortonT>::ZCurve newEvents, const size_t splitThreshold, size_t maxDepth) {
    auto leafBoxes = leafs();
    ZCurveIterator it1 = newEvents.begin();
    ZCurveIterator it2 = newEvents.begin();
    struct task {
      ZCurveIterator it1;
      ZCurveIterator it2;
      const Leaf& leaf;
    };
    std::vector<task> tasks;
    for(auto& lbox: leafBoxes) {
      while(it2->mortonNumber() <= lbox.box.m_upperBound && it2 != newEvents.end())
        ++it2;

      if(it1 != it2) {
        tasks.emplace_back(task{it1, it2, lbox});
        it1 = it2;
      }
      else
        continue;

      if(it2 == newEvents.end())
        break;
    }
#pragma omp parallel for
    for(unsigned i = 0; i < tasks.size(); ++i) {
      const task& tsk = tasks[i];
      tsk.leaf.box.appendEvents(tsk.it1, tsk.it2, splitThreshold, maxDepth - tsk.leaf.level);
    }
  }
#endif // STORING_EVENTS

  struct BoxStructure {
    friend std::ostream &operator<<(std::ostream &os, const BoxStructure &structure) {
      os << structure.lowerBound << " " << structure.upperBound << " " << structure.count;
      return os;
    }
    MortonT lowerBound;
    MortonT upperBound;
    std::size_t count;
  };

  using Structure = std::vector<BoxStructure>;
  static void print(std::ostream &os, const Structure &structure) {
    for(auto& bstruct: structure)
      os << bstruct << "\n";
  }

  void structure(Structure& res) {
#ifdef STORING_EVENTS
    res.emplace_back(BoxStructure{m_lowerBound, m_upperBound, totalEvents()});
#else // STORING_EVENTS
    res.emplace_back(BoxStructure{m_lowerBound, m_upperBound, m_eventEnd - m_eventBegin});
#endif //STORING_EVENTS
    for(auto& ch: m_childBoxes)
      structure(res);
  }

  Structure structure() {
    Structure res;
    structure(res);
    return res;
  }

private:
  /* Lower box bound, the smallest Morton number an event can have an be
   * contained in this box. */
  const MortonT m_lowerBound;

  /* Upper box bound, the greatest Morton number an event can have and be
   * contained in this box. */
  const MortonT m_upperBound;

  ZCurveIterator m_eventBegin;
  ZCurveIterator m_eventEnd;

#ifdef STORING_EVENTS
  using EventType = typename std::iterator_traits<ZCurveIterator >::value_type;
  std::vector<EventType> m_events;
#endif //STORING_EVENTS
  /**
   * Vector of child boxes.
   * Must be kept sorted by Morton number (either lower or upper will work) of
   * each box.
   */
  Children m_childBoxes;
};
