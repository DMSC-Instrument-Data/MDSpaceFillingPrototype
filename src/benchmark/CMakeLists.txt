set(BENCHMARKS
  BitInterleaving128bitBenchmark
  BitInterleaving64bitBenchmark
  CoordinateConversionBenchmark
  EventCreationBenchmark
  MDBoxBenchmark
  SortBenchmark
  SortDatasetBenchmark
  SortEventBenchmark
)

foreach(BENCHMARK ${BENCHMARKS})
  Benchmark(
    NAME ${BENCHMARK}
    SOURCES ${BENCHMARK}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
  )
endforeach(BENCHMARK)

Benchmark(
  NAME QConversionBenchmark
  SOURCES QConversionBenchmark.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
