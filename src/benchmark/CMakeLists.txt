set(BENCHMARKS
  BitInterleaving128bitBenchmark
  BitInterleaving64bitBenchmark
  BitInterleaving64bitBoostGMPBenchmark
  CoordinateConversionBenchmark
  EventCreationBenchmark
  SortBenchmark
  SortDatasetBenchmark
  SortEventBenchmark
)

foreach(BENCHMARK ${BENCHMARKS})
  Benchmark(
    NAME ${BENCHMARK}
    SOURCES ${BENCHMARK}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
    LIBRARIES ${GMP_LIBRARIES}
  )
endforeach(BENCHMARK)
