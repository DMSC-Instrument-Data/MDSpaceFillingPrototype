#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>

#include "EventNexusLoader.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"
#include "MDBox.h"
#include "MDEvent.h"

class scoped_wallclock_timer {
public:
  using Clock = std::chrono::high_resolution_clock;

public:
  scoped_wallclock_timer(benchmark::State &state,
                         const std::string &counterName)
      : m_state(state), m_counterName(counterName), m_start(Clock::now()) {}

  ~scoped_wallclock_timer() {
    const auto duration = Clock::now() - m_start;
    m_state.PauseTiming();
    const auto durationSeconds =
        std::chrono::duration<double>(duration).count();
    m_state.counters[m_counterName] += durationSeconds;
    std::cout << m_counterName << " took " << durationSeconds << " seconds.\n";
    m_state.ResumeTiming();
  }

private:
  benchmark::State &m_state;
  const std::string m_counterName;
  const std::chrono::time_point<Clock> m_start;
};

void BM_QConversion_WISH(benchmark::State &state) {
  const size_t ND(3);
  using IntT = uint16_t;
  using MortonT = uint64_t;

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, "/home/dan/wish.h5");

  EventNexusLoader loader("/home/dan/WISH00034509.nxs",
                          "/raw_data_1/detector_1_events");

  /* Load spectrum to detector mapping */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<TofEvent> tofEventsRaw;
  {
    std::vector<size_t> frameIdxs(loader.frameCount());
    std::iota(frameIdxs.begin(), frameIdxs.end(), 0);
    loader.loadFrames(tofEventsRaw, frameIdxs);
  }

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    -10.0f, 10.0f;
  // clang-format on

  /* Define Q conversion parameters */
  ConversionInfo convInfo{false, Eigen::Matrix3f::Identity()};

  /* Define box structure construction parameters */
  constexpr size_t splitThreshold(1000);
  constexpr size_t maxBoxTreeDepth(20);

  for (auto _ : state) {
    /* Copy raw ToF events form loaded events (needed as convert_events() sorts
     * the vector) */
    state.PauseTiming();
    std::vector<TofEvent> tofEvents(tofEventsRaw);
    state.ResumeTiming();

    /* Convert to Q space */
    std::vector<MDEvent<ND, IntT, MortonT>> mdEvents;
    {
      scoped_wallclock_timer timer(state, "q_conversion");
      convert_events(mdEvents, tofEvents, convInfo, inst, mdSpace);
    }

    /* Sort events */
    {
      scoped_wallclock_timer timer(state, "sort");
      boost::sort::block_indirect_sort(mdEvents.begin(), mdEvents.end());
    }

    /* Construct box structure */
    MDBox<ND, IntT, MortonT> rootMdBox(mdEvents.cbegin(), mdEvents.cend());
    {
      scoped_wallclock_timer timer(state, "box_structure");
      rootMdBox.distributeEvents(splitThreshold, maxBoxTreeDepth);
    }

    benchmark::DoNotOptimize(mdEvents);
    benchmark::DoNotOptimize(rootMdBox);
  }

  /* Average duration counters (there are counter flags to do this in the latest
   * Google Benchmark) */
  for (const auto &timerName : {"q_conversion", "sort", "box_structure"}) {
    state.counters[timerName] /= state.iterations();
  }
}
BENCHMARK(BM_QConversion_WISH)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
