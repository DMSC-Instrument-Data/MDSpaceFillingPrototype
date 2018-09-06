set(BENCHMARKS
  BitInterleaving128bitBenchmark
  BitInterleaving64bitBenchmark
  CoordinateConversionBenchmark
  EventCreationBenchmark
  MDBoxBenchmark
  SortBenchmark
)

foreach(BENCHMARK ${BENCHMARKS})
  Benchmark(
    NAME ${BENCHMARK}
    SOURCES ${BENCHMARK}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
  )
set_tests_properties(${BENCHMARK} PROPERTIES LABELS "Benchmark")
endforeach(BENCHMARK)

# Data dependant benchmark
Benchmark(
  NAME SortDatasetBenchmark
  SOURCES SortDatasetBenchmark.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
)
set_tests_properties(SortDatasetBenchmark PROPERTIES LABELS "DataBenchmark")

# Data and library depenadant benchmarks
Benchmark(
  NAME QConversionBenchmark
  SOURCES QConversionBenchmark.cpp
  HEADERS ${CMAKE_SOURCE_DIR}/src
  LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
)
set_tests_properties(QConversionBenchmark PROPERTIES LABELS "DataBenchmark")
