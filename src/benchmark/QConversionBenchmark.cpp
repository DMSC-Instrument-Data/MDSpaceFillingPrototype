#include <benchmark/benchmark.h>

#include <boost/sort/sort.hpp>

#include "BitInterleaving.h"
#include "EventToMDEventConversion.h"
#include "Instrument.h"
#include "IsisEventNexusLoader.h"
#include "MDBox.h"
#include "MDEvent.h"
#include "MantidEventNexusLoader.h"

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
    m_state.ResumeTiming();
  }

private:
  benchmark::State &m_state;
  const std::string m_counterName;
  const std::chrono::time_point<Clock> m_start;
};

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

  state.counters["md_events"] += mdEvents.size();

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

/**
 * Average counters (there are counter flags to do this in the latest Google
 * Benchmark)
 */
void average_counters(benchmark::State &state) {
  for (const auto &name :
       {"q_conversion", "sort", "box_structure", "md_events"}) {
    state.counters[name] /= state.iterations();
  }
}

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_34509(benchmark::State &state) {
  constexpr size_t ND(3);

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, "/home/dan/wish.h5");

  IsisEventNexusLoader loader("/home/dan/WISH00034509.nxs",
                              "/raw_data_1/detector_1_events");

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<size_t> frameIdxs(loader.frameCount());
  std::iota(frameIdxs.begin(), frameIdxs.end(), 0);
  std::vector<TofEvent> tofEventsRaw;
  loader.loadFrames(tofEventsRaw, frameIdxs);

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -12.0f, 12.0f,
    -3.0f, 3.0f,
    0.0f, 23.0f;
  // clang-format on

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, mdSpace,
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_34509, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_WISH_38423(benchmark::State &state) {
  constexpr size_t ND(3);

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, "/home/dan/wish.h5");

  MantidEventNexusLoader loader("/home/dan/WISH00038423_event.nxs");

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<TofEvent> tofEventsRaw;
  loader.loadAllEvents(tofEventsRaw);

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -12.0f, 12.0f,
    -3.0f, 3.0f,
    0.0f, 23.0f;
  // clang-format on

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, mdSpace,
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_WISH_38423, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_TOPAZ_3132(benchmark::State &state) {
  constexpr size_t ND(3);

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, "/home/dan/topaz.h5");

  MantidEventNexusLoader loader("/home/dan/TOPAZ_3132_event.nxs");

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<TofEvent> tofEventsRaw;
  loader.loadAllEvents(tofEventsRaw);

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    0.0f, 60.0f,
    -40.0f, 40.0f,
    0.0f, 110.0f;
  // clang-format on

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, mdSpace,
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_TOPAZ_3132, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);

template <typename IntT, typename MortonT>
void BM_QConversion_SXD_23767(benchmark::State &state) {
  constexpr size_t ND(3);

  /* Load instrument */
  Instrument inst;
  load_instrument(inst, "/home/dan/sxd.h5");

  MantidEventNexusLoader loader("/home/dan/SXD23767_event.nxs");

  /* Load a mapping from the NeXus file */
  loader.loadSpectrumDetectorMapping(inst.spectrum_detector_mapping);

  /* Load ToF events */
  std::vector<TofEvent> tofEventsRaw;
  loader.loadAllEvents(tofEventsRaw);

  /* Define MD space extents */
  MDSpaceBounds<ND> mdSpace;
  // clang-format off
  mdSpace <<
    -18.0f, 18.0f,
    -7.0f, 17.0f,
    0.0f, 34.0f;
  // clang-format on

  for (auto _ : state) {
    do_conversion<ND, IntT, MortonT>(state, inst, tofEventsRaw, mdSpace,
                                     {false, Eigen::Matrix3f::Identity()}, 1000,
                                     20);
  }

  average_counters(state);
}
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint16_t, uint64_t)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_QConversion_SXD_23767, uint32_t, uint128_t)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
