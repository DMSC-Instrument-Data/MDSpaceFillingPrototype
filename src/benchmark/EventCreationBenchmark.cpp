#include <benchmark/benchmark.h>

#include "MDEvent.h"

static void BM_CreateEvent_N4_100000000(benchmark::State &state) {
  MDSpace<4> space;
  // clang-format off
  space <<
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f,
    0.0f, 10.0f;
  // clang-format on

  MDCoordinate<4> coord;
  coord << 5.0f, 6.0f, 7.0f, 3.0f;

  for (auto _ : state) {
    for (size_t i = 0; i < 100000000; i++) {
      MDEvent<4> event(coord, space);
      benchmark::DoNotOptimize(event);
    }
  }
}
BENCHMARK(BM_CreateEvent_N4_100000000)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
