set(BENCHMARKS
  BitInterleaving128bitBenchmark
  BitInterleaving64bitBenchmark
  CoordinateConversionBenchmark
  EventCreationBenchmark
  SortBenchmark
  SortBenchmarkLarge
  SortEventBenchmark
)

foreach(BENCHMARK ${BENCHMARKS})
  Benchmark(
    NAME ${BENCHMARK}
    SOURCES ${BENCHMARK}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
  )
endforeach(BENCHMARK)
