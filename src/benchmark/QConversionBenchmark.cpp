#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>

#include "BitInterleaving.h"
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

void load_instrument_and_data(Instrument &instrument,
                              std::vector<TofEvent> &tofEvents,
                              const std::string &instrumentFilename,
                              const std::string &dataFilename,
                              const std::string &dataPath) {
  /* Load instrument */
  load_instrument(instrument, instrumentFilename);

  EventNexusLoader loader(dataFilename, dataPath);

  /* Load spectrum to detector mapping */
  loader.loadSpectrumDetectorMapping(instrument.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<size_t> frameIdxs(loader.frameCount());
  std::iota(frameIdxs.begin(), frameIdxs.end(), 0);
  loader.loadFrames(tofEvents, frameIdxs);
}

template <size_t ND, typename IntT, typename MortonT>
void do_conversion(benchmark::State &state, const Instrument &inst,
                   const std::vector<TofEvent> &tofEventsRaw,
                   const MDSpaceBounds<ND> &mdSpace,
                   const ConversionInfo &convInfo, const size_t splitThreshold,
                   const size_t maxBoxTreeDepth) {
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
}

template <typename IntT, typename MortonT>
void BM_QConversion_WISH(benchmark::State &state) {
  constexpr size_t ND(3);

  /* Load instrument and data */
  Instrument inst;
  std::vector<TofEvent> tofEventsRaw;
  load_instrument_and_data(inst, tofEventsRaw, "/home/dan/wish.h5",
                           "/home/dan/WISH00034509.nxs",
                           "/raw_data_1/detector_1_events");

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -10.0f, 10.0f,
    -10.0f, 10.0f,
    -10.0f, 10.0f;
  // clang-format on

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, mdSpace,
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  /* Average duration counters (there are counter flags to do this in the latest
   * Google Benchmark) */
  for (const auto &timerName : {"q_conversion", "sort", "box_structure"}) {
    state.counters[timerName] /= state.iterations();
  }
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
