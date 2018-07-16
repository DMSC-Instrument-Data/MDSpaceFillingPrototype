#include <benchmark/benchmark.h>

#include <random>

#include <boost/sort/sort.hpp>

#include "MDBox.h"
#include "MDEvent.h"

const size_t ND(4);
using IntT = uint16_t;
using MortonT = uint64_t;

using Box = MDBox<ND, IntT, MortonT>;
using Event = MDEvent<ND>;

static void tree_depths(benchmark::internal::Benchmark *b) {
  for (int i = 1; i < 6; ++i) {
    b->Arg(i);
  }
}

void BM_MDBox_split(benchmark::State &state) {
  for (auto _ : state) {
    /* Create box */
    state.PauseTiming();
    Box root;
    state.ResumeTiming();

    /* Do splitting */
    root.split(state.range(0));

    benchmark::DoNotOptimize(root);
  }
}
BENCHMARK(BM_MDBox_split)->Apply(tree_depths)->Unit(benchmark::kMicrosecond);

void BM_MDBox_gather_leaf_boxes(benchmark::State &state) {
  for (auto _ : state) {
    /* Create box and do splitting */
    state.PauseTiming();
    Box root;
    root.split(state.range(0));
    state.ResumeTiming();

    /* Gather leaf boxes */
    Box::LeafBoxVector leafBoxes;
    root.getLeafBoxes(leafBoxes);

    benchmark::DoNotOptimize(root);
    benchmark::DoNotOptimize(leafBoxes);
  }
}
BENCHMARK(BM_MDBox_gather_leaf_boxes)
    ->Apply(tree_depths)
    ->Unit(benchmark::kMicrosecond);

void BM_MDBox_prune(benchmark::State &state) {
  std::mt19937 gen;
  std::uniform_int_distribution<MortonT> dist;

  for (auto _ : state) {
    state.PauseTiming();

    const size_t depth(state.range(0));
    const size_t numEvents(state.range(1));
    const size_t pruneThreshold(state.range(2));

    /* Generate test data */
    Box root;
    {
      root.split(depth);

      /* Generate event Z-curve */
      Event::ZCurve curve;
      for (size_t i = 0; i < numEvents; i++) {
        curve.emplace_back(dist(gen));
      }
      boost::sort::block_indirect_sort(curve.begin(), curve.end());

      /* Add events to leaf MDBoxes */
      {
        Box::LeafBoxVector leafBoxes;
        root.getLeafBoxes(leafBoxes);
        boost::sort::block_indirect_sort(leafBoxes.begin(), leafBoxes.end(),
                                         [](const auto &a, const auto &b) {
                                           return a.get().morton() <
                                                  b.get().morton();
                                         });

        Box::InsertEventsToLeafBoxes(leafBoxes, curve);

        root.setIteratorsFromChildren();
      }
    }

    state.ResumeTiming();

    root.prune(pruneThreshold);

    benchmark::DoNotOptimize(root);
  }
}
BENCHMARK(BM_MDBox_prune)
    ->Args({1, 100, 10})
    ->Args({2, 100, 10})
    ->Args({3, 100, 10})
    ->Args({4, 100, 10})
    ->Args({5, 100, 10})
    ->Args({1, 1000, 10})
    ->Args({2, 1000, 10})
    ->Args({3, 1000, 10})
    ->Args({4, 1000, 10})
    ->Args({5, 1000, 10})
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
