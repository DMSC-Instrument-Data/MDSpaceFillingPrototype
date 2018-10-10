set(BENCHMARKS
  BitInterleaving128bitBenchmark
  BitInterleaving256bitBenchmark
  BitInterleaving64bitBenchmark
  CoordinateConversionBenchmark
  EventCreationBenchmark
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
set(DATA_BENCHMARKS
  EventAccessBenchmark
  MDBoxBenchmark
  QConversionBenchmark
)

foreach(BENCHMARK ${DATA_BENCHMARKS})
  Benchmark(
    NAME ${BENCHMARK}
    SOURCES ${BENCHMARK}.cpp
    HEADERS ${CMAKE_SOURCE_DIR}/src
    LIBRARIES ${CONAN_LIBS} MDSpaceFillingPrototype
  )
  set_tests_properties(${BENCHMARK} PROPERTIES LABELS "DataBenchmark")
endforeach(BENCHMARK)
