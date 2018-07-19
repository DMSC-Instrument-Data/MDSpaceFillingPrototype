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

/* TODO */

BENCHMARK_MAIN();
